#include <math.h>
#include <stdlib.h>
#include <vulpes/VPS_Types.h>

#include <vulpes/VPS_List.h>

#include <vulpes/VPS_ScopedDictionary.h>

/**
 * @brief A static helper function compatible with VPS_List's node_data_release.
 *        It knows how to properly release a VPS_ScopedDictionary_Entry and its contents.
 */
static char _scoped_dictionary_entry_releaser(void *entry_data)
{
	if (!entry_data) return 1;

	struct VPS_ScopedDictionary_Entry *entry = entry_data;
	// The dictionary context is stored in the parent list's generic data pointer.
	struct VPS_ScopedDictionary *dict = entry->owner_node->parent->data;

	if (dict)
	{
		// Note: The versions list is managed by the scope log, so we don't release its data here.
		// We only release the key, as it's owned by the entry.
		if (dict->key_release) dict->key_release(entry->key);
	}

	free(entry);

	return 1;
}

#define DEFAULT_BUCKETS_COUNT		17

static char VPS_ScopedDictionary_PRIVATE_FindEntry
(
	struct VPS_ScopedDictionary *item,
	void *key,
	struct VPS_ScopedDictionary_Entry **entry,
	VPS_TYPE_SIZE *key_hash_output
)
{
	struct VPS_ScopedDictionary_Entry *bucket_entry;
	struct VPS_List *bucket;
	struct VPS_List_Node *bucket_node;
	VPS_TYPE_SIZE hash;
	VPS_TYPE_SIZE bucket_index;
	VPS_TYPE_16S ordering;
	char result;

	result = item->hash(key, &hash);
	if (!result)
	{
		return 0;
	}

	*key_hash_output = hash;
	bucket_index = hash % item->buckets;
	bucket = item->bucket_vector[bucket_index];
	if (!bucket)
	{
		return 0;
	}

	bucket_node = bucket->head;
	while (bucket_node)
	{
		bucket_entry = bucket_node->data;
		if (bucket_entry->hash == hash)
		{
			result = item->key_compare(key, bucket_entry->key, &ordering);
			if (result && ordering == 0)
			{
				*entry = bucket_entry;

				return 1;
			}
		}

		bucket_node = bucket_node->next;
	}

	return 0;
}

static char VPS_ScopedDictionary_PRIVATE_Rehash
(
	struct VPS_ScopedDictionary *item
)
{
	struct VPS_ScopedDictionary_Entry *entry;
	struct VPS_List **new_bucket_vector;
	struct VPS_List **old_bucket_vector;
	struct VPS_List *old_bucket;
	struct VPS_List *new_bucket;
	struct VPS_List_Node *current_node;
	struct VPS_List_Node *next_node;
	VPS_TYPE_SIZE new_bucket_count;
	VPS_TYPE_SIZE old_bucket_count;
	VPS_TYPE_SIZE new_index;
	VPS_TYPE_SIZE i;
	VPS_TYPE_SIZE j;

	new_bucket_count = item->buckets * item->growth_multiplier;
	if (new_bucket_count <= item->buckets)
	{
		return 0;
	}

	new_bucket_vector = calloc(new_bucket_count, sizeof(struct VPS_List *));
	if (!new_bucket_vector)
	{
		return 0;
	}

	for (i = 0; i < new_bucket_count; ++i)
	{
		VPS_List_Allocate(&new_bucket_vector[i]);
		if (!new_bucket_vector[i])
		{
			for (j = 0; j < i; ++j)
			{
				VPS_List_Release(new_bucket_vector[j]);
			}
			free(new_bucket_vector);

			return 0;
		}

		// Construct the new bucket list, providing the dictionary context and the releaser.
		VPS_List_Construct(new_bucket_vector[i], item, 0, _scoped_dictionary_entry_releaser);
	}

	old_bucket_vector = item->bucket_vector;
	old_bucket_count = item->buckets;

	item->bucket_vector = new_bucket_vector;
	item->buckets = new_bucket_count;

	for (i = 0; i < old_bucket_count; ++i)
	{
		old_bucket = old_bucket_vector[i];
		current_node = old_bucket->head;

		while (current_node)
		{
			next_node = current_node->next;
			entry = current_node->data;

			new_index = entry->hash % new_bucket_count;
			new_bucket = new_bucket_vector[new_index];

			VPS_List_Node_Remove(entry->owner_node);
			VPS_List_AddTail(new_bucket, entry->owner_node);

			current_node = next_node;
		}
	}

	for (i = 0; i < old_bucket_count; ++i) {
		VPS_List_Deconstruct(old_bucket_vector[i]);
		VPS_List_Release(old_bucket_vector[i]);
	}
	free(old_bucket_vector);

	return 1;
}

char VPS_ScopedDictionary_Allocate
(
	struct VPS_ScopedDictionary **item,
	VPS_TYPE_SIZE buckets
)
{
	struct VPS_ScopedDictionary *subject;
	VPS_TYPE_SIZE bucket;
	char result;

	if (!buckets)
	{
		buckets = DEFAULT_BUCKETS_COUNT;
	}

	subject = calloc(1, sizeof(struct VPS_ScopedDictionary));
	if (!subject)
	{
		return 0;
	}

	subject->bucket_vector = calloc(buckets, sizeof(struct VPS_List *));
	if (!subject->bucket_vector)
	{
		goto cleanup;
	}
	subject->buckets = buckets;

	for (bucket = 0; bucket < buckets; bucket++)
	{
		result = VPS_List_Allocate(&subject->bucket_vector[bucket]);
		if (!result)
		{
			goto cleanup;
		}
		// Construct the bucket list, providing the dictionary context and the releaser.
		VPS_List_Construct(subject->bucket_vector[bucket], subject, 0, _scoped_dictionary_entry_releaser);
	}

	result = VPS_List_Allocate(&subject->scopeLog);
	if (!result)
	{
		goto cleanup;
	}

	*item = subject;

	return 1;

cleanup:

	VPS_ScopedDictionary_Deconstruct(subject);
	VPS_ScopedDictionary_Release(subject);

	return 0;
}

char VPS_ScopedDictionary_Construct
(
	struct VPS_ScopedDictionary *item,
	char (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
	char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
	char (*data_compare)(void *data_1, void *data_2, VPS_TYPE_16S *ordering),
	char (*key_release)(void *key),
	char (*data_release)(void *data),
	VPS_TYPE_SIZE growth_multiplier,
	VPS_TYPE_SIZE load_percent_threshold,
	VPS_TYPE_SIZE single_bucket_threshold
)
{
	if (!item || !hash || !key_compare)
	{
		return 0;
	}

	item->growth_multiplier = growth_multiplier > 1 ? growth_multiplier : 2;
	item->load_percent_threshold = load_percent_threshold > 0 ? load_percent_threshold : 75;
	item->single_bucket_threshold = single_bucket_threshold > 0 ? single_bucket_threshold : 8;

	item->hash = hash;
	item->key_compare = key_compare;
	item->data_compare = data_compare;
	item->key_release = key_release;
	item->data_release = data_release;

	return VPS_ScopedDictionary_EnterScope(item);
}

char VPS_ScopedDictionary_Deconstruct
(
	struct VPS_ScopedDictionary *item
)
{
	VPS_TYPE_SIZE bucket;
	char result;

	if (!item || !item->scopeLog)
	{
		return 0;
	}

	while (item->scopeLog->head)
	{
		result = VPS_ScopedDictionary_LeaveScope(item);
		if (!result)
		{
			return 0;
		}
	}

	if (item->bucket_vector)
	{
		for (bucket = 0; bucket < item->buckets; bucket ++)
		{
			VPS_List_Deconstruct(item->bucket_vector[bucket]);
		}
	}

	return 1;
}

char VPS_ScopedDictionary_Release
(
	struct VPS_ScopedDictionary *item
)
{
	VPS_TYPE_SIZE bucket;

	if (item)
	{
		if (item->bucket_vector)
		{
			for (bucket = 0; bucket < item->buckets; bucket ++)
			{
				VPS_List_Release(item->bucket_vector[bucket]);
			}

			free(item->bucket_vector);
		}

		VPS_List_Release(item->scopeLog);

		free(item);
	}

	return 1;
}

char VPS_ScopedDictionary_EnterScope
(
	struct VPS_ScopedDictionary *item
)
{
	struct VPS_List *scopeLog;
	struct VPS_List_Node *scopeLog_node;
	char result;

	if (!item)
	{
		return 0;
	}

	result = VPS_List_Allocate(&scopeLog);
	if (!result)
	{
		return 0;
	}
	VPS_List_Construct(scopeLog, 0, 0, 0);

	result = VPS_List_Node_Allocate(&scopeLog_node);
	if (!result)
	{
		goto cleanup;
	}

	VPS_List_Node_Construct(scopeLog_node, scopeLog);
	VPS_List_AddHead(item->scopeLog, scopeLog_node);

	return 1;

cleanup:

	VPS_List_Release(scopeLog);

	return 0;
}

char VPS_ScopedDictionary_LeaveScope
(
	struct VPS_ScopedDictionary *item
)
{
	struct VPS_List *changes;
	struct VPS_List_Node *scope_node;
	struct VPS_List_Node *change_node;
	struct VPS_List_Node *version_node;
	struct VPS_ScopedDictionary_Entry *entry;

	char result;

	if (!item)
	{
		return 0;
	}

	result = VPS_List_RemoveHead(item->scopeLog, &scope_node);
	if (!result)
	{
		return 0;
	}

	changes = scope_node->data;
	while (changes->head)
	{
		VPS_List_RemoveHead(changes, &change_node);
		entry = change_node->data;

		result = VPS_List_RemoveHead(entry->versions, &version_node);
		if (result)
		{
			if (item->data_release)
			{
				item->data_release(version_node->data);
			}

			VPS_List_Node_Deconstruct(version_node);
			VPS_List_Node_Release(version_node);
		}

		if (!entry->versions->head)
		{
            struct VPS_List* bucket = entry->owner_node->parent;

            bucket->node_data_release(entry);

            VPS_List_Node_Remove(entry->owner_node);
            VPS_List_Node_Deconstruct(entry->owner_node);
            VPS_List_Node_Release(entry->owner_node);
			item->total_entries --;
		}

		VPS_List_Node_Deconstruct(change_node);
		VPS_List_Node_Release(change_node);
	}
	VPS_List_Deconstruct(changes);
	VPS_List_Release(changes);

	VPS_List_Node_Deconstruct(scope_node);
	VPS_List_Node_Release(scope_node);

	return 1;
}

char VPS_ScopedDictionary_Find
(
	struct VPS_ScopedDictionary *item,
	void *key,
	void **data
)
{
	struct VPS_ScopedDictionary_Entry *entry;
	VPS_TYPE_SIZE key_hash;
	char result;

	if (!item || !key)
	{
		return 0;
	}

	result = VPS_ScopedDictionary_PRIVATE_FindEntry(item, key, &entry, &key_hash);
	if (!result)
	{
		return 0;
	}

	if (data)
	{
		if (!entry->versions->head)
		{
			return 0;
		}

		*data = entry->versions->head->data;
	}

	return 1;
}

char VPS_ScopedDictionary_Add
(
	struct VPS_ScopedDictionary *item,
	void *key,
	void *data
)
{
	struct VPS_ScopedDictionary_Entry *entry = 0;
	struct VPS_List *current_changes_list;
	struct VPS_List_Node *version_node;
	struct VPS_List_Node *change_node;
	VPS_TYPE_SIZE key_hash;
	VPS_TYPE_16S ordering;
	char found;

	// 1. --- Validate Parameters & Get Current Scope ---
	if (!item || !key || !item->scopeLog || !item->scopeLog->head)
	{
		return 0;
	}
	current_changes_list = item->scopeLog->head->data;

	// 2. --- Find if an entry for this key already exists ---
	found = VPS_ScopedDictionary_PRIVATE_FindEntry(item, key, &entry, &key_hash);

	// 3. --- Handle the two paths: Entry Found vs. Entry Not Found ---
	if (found)
	{
		// --- PATH A: Entry Found ---
		// An entry for this key already exists, so we are adding a new version.

		// --- Idempotency Check ---
		// If a data comparison function is available, check if the new data is
		// identical to the current version. If so, do nothing.
		if (item->data_compare && entry->versions->head)
		{
			if (item->data_compare(data, entry->versions->head->data, &ordering) && ordering == 0)
			{
				// The new data is the same as the current version.
				// The operation is a success, as the desired state is already achieved.
				return 1;
			}
		}

		// Add the new version.
		VPS_List_Node_Allocate(&version_node);
		VPS_List_Node_Construct(version_node, data);
		VPS_List_AddHead(entry->versions, version_node);

		// Log this change in the current scope.
		VPS_List_Node_Allocate(&change_node);
		VPS_List_Node_Construct(change_node, entry);
		VPS_List_AddHead(current_changes_list, change_node);
	}
	else
	{
		// --- PATH B: Entry Not Found ---
		// This is the first time we've seen this key, so we must create a new entry.
		struct VPS_List *bucket;
		struct VPS_List_Node *owner_node;

		// a. Create the new entry object.
		entry = calloc(1, sizeof(struct VPS_ScopedDictionary_Entry));
		if (!entry) return 0;

		// b. Create and populate its versions list.
		VPS_List_Allocate(&entry->versions);
		VPS_List_Construct(entry->versions, 0, 0, 0);
		VPS_List_Node_Allocate(&version_node);
		VPS_List_Node_Construct(version_node, data);
		VPS_List_AddHead(entry->versions, version_node);

		// c. Set the entry's key and cached hash.
		entry->key = key;
		entry->hash = key_hash;

		// d. Add the new entry to the correct bucket.
		bucket = item->bucket_vector[key_hash % item->buckets];
		VPS_List_Node_Allocate(&owner_node);
		VPS_List_Node_Construct(owner_node, entry);
		VPS_List_AddTail(bucket, owner_node);

		// e. Set the backlink from the entry to the node that owns it.
		entry->owner_node = owner_node;

		// f. Log this change in the current scope.
		VPS_List_Node_Allocate(&change_node);
		VPS_List_Node_Construct(change_node, entry);
		VPS_List_AddHead(current_changes_list, change_node);

		// g. Update the dictionary's total entry count.
		item->total_entries++;
	}

	// 4. --- Trigger Rehash if Necessary ---
	// Check if the dictionary has become too crowded.
	struct VPS_List *bucket = item->bucket_vector[key_hash % item->buckets];
	if ((item->total_entries * 100 > item->load_percent_threshold * item->buckets) ||
	    (bucket->count > item->single_bucket_threshold))
	{
		VPS_ScopedDictionary_PRIVATE_Rehash(item);
	}

	return 1;
}

char VPS_ScopedDictionary_Remove
(
	struct VPS_ScopedDictionary *item,
	void *key
)
{
	void *current_data;

	// --- Idempotency Check ---
	// First, check if the key exists and is already logically removed (i.e., its
	// current version is NULL). If so, there's nothing to do.
	if (VPS_ScopedDictionary_Find(item, key, &current_data) && current_data == NULL)
	{
		return 1; // Success, the desired state is already achieved.
	}

	// If the key is visible (or doesn't exist), "remove" it by adding a
	// NULL version. This logically hides the entry for the current scope.
	return VPS_ScopedDictionary_Add(item, key, NULL);
}
