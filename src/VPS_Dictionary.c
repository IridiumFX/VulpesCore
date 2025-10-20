#include <stdlib.h>
#include <vulpes/VPS_Types.h>

#include <vulpes/VPS_List.h>

#include <vulpes/VPS_Dictionary.h>

#define DEFAULT_BUCKETS_COUNT		17

/**
 * @brief A static helper function compatible with VPS_List's node_data_release.
 *        It knows how to properly release a VPS_Dictionary_Entry and its contents.
 */
static char _dictionary_entry_releaser(void *entry_data)
{
	if (!entry_data) return 1;

	struct VPS_Dictionary_Entry *entry = entry_data;
	// The dictionary context is stored in the parent list's generic data pointer.
	struct VPS_Dictionary *dict = entry->owner_node->parent->data;

	if (dict)
	{
		if (dict->key_release) dict->key_release(entry->key);
		if (dict->data_release) dict->data_release(entry->data);
	}

	free(entry);

	return 1;
}

static char VPS_Dictionary_PRIVATE_FindEntry
(
	struct VPS_Dictionary *item,
	void *key,
	struct VPS_Dictionary_Entry **entry,
	VPS_TYPE_SIZE *key_hash_output,
	struct VPS_List **bucket_output
)
{
	struct VPS_Dictionary_Entry *bucket_entry;
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
	*bucket_output = bucket;
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

static char VPS_Dictionary_PRIVATE_Rehash
(
	struct VPS_Dictionary *item
)
{
	struct VPS_Dictionary_Entry *entry;
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

		VPS_List_Construct(new_bucket_vector[i], item, 0, _dictionary_entry_releaser);
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

char VPS_Dictionary_Allocate
(
	struct VPS_Dictionary **item,
	VPS_TYPE_SIZE buckets
)
{
	struct VPS_Dictionary *subject;
	VPS_TYPE_SIZE i;
	char result;

	if (!buckets)
	{
		buckets = DEFAULT_BUCKETS_COUNT;
	}

	subject = calloc(1, sizeof(struct VPS_Dictionary));
	if (!subject)
	{
		return 0;
	}

	subject->bucket_vector = calloc(buckets, sizeof(struct VPS_List *));
	if (!subject->bucket_vector)
	{
		free(subject);
		return 0;
	}
	subject->buckets = buckets;

	for (i = 0; i < buckets; i++)
	{
		result = VPS_List_Allocate(&subject->bucket_vector[i]);
		if (!result)
		{
			goto cleanup;
		}
		VPS_List_Construct(subject->bucket_vector[i], subject, 0, _dictionary_entry_releaser);
	}

	*item = subject;

	return 1;

cleanup:

	VPS_Dictionary_Release(subject);
	*item = 0;

	return 0;
}

char VPS_Dictionary_Construct
(
	struct VPS_Dictionary *item,
	char (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
	char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
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
	item->key_release = key_release;
	item->data_release = data_release;

	return 1;
}

char VPS_Dictionary_Deconstruct
(
	struct VPS_Dictionary *item
)
{
	if (!item) return 0;

	if (item->bucket_vector)
	{
		for (VPS_TYPE_SIZE i = 0; i < item->buckets; i++)
		{
			if (item->bucket_vector[i])
			{
				VPS_List_Deconstruct(item->bucket_vector[i]);
			}
		}
	}
	item->total_entries = 0;

	return 1;
}

char VPS_Dictionary_Release
(
	struct VPS_Dictionary *item
)
{
	VPS_TYPE_SIZE i;

	if (item)
	{
		if (item->bucket_vector)
		{
			for (i = 0; i < item->buckets; i++)
			{
				VPS_List_Release(item->bucket_vector[i]);
			}

			free(item->bucket_vector);
		}

		free(item);
	}

	return 1;
}

char VPS_Dictionary_Find
(
	struct VPS_Dictionary *item,
	void *key,
	void **data
)
{
	struct VPS_Dictionary_Entry *entry;
	struct VPS_List *bucket;
	VPS_TYPE_SIZE key_hash;
	char result;

	if (!item || !key)
	{
		return 0;
	}

	result = VPS_Dictionary_PRIVATE_FindEntry(item, key, &entry, &key_hash, &bucket);
	if (!result)
	{
		return 0;
	}

	if (data)
	{
		*data = entry->data;
	}

	return 1;
}

char VPS_Dictionary_Add
(
	struct VPS_Dictionary *item,
	void *key,
	void *data
)
{
	struct VPS_List *bucket;
	struct VPS_Dictionary_Entry *entry;
	VPS_TYPE_SIZE key_hash;
	char found;

	if (!item || !key)
	{
		return 0;
	}

	found = VPS_Dictionary_PRIVATE_FindEntry(item, key, &entry, &key_hash, &bucket);

	if (found)
	{
		// Entry found, update data.
		if (item->data_release)
		{
			item->data_release(entry->data);
		}
		entry->data = data;
	}
	else
	{
		// Entry not found, create a new one.
		struct VPS_List_Node *owner_node;

		entry = calloc(1, sizeof(struct VPS_Dictionary_Entry));
		if (!entry) return 0;

		entry->key = key;
		entry->data = data;
		entry->hash = key_hash;

		VPS_List_Node_Allocate(&owner_node);
		VPS_List_Node_Construct(owner_node, entry);
		VPS_List_AddTail(bucket, owner_node);

		entry->owner_node = owner_node;

		item->total_entries++;
	}

	if ((item->total_entries * 100 > item->load_percent_threshold * item->buckets) ||
	    (bucket->count > item->single_bucket_threshold))
	{
		VPS_Dictionary_PRIVATE_Rehash(item);
	}

	return 1;
}

char VPS_Dictionary_Remove
(
	struct VPS_Dictionary *item,
	void *key
)
{
	struct VPS_Dictionary_Entry *entry;
	struct VPS_List *bucket;
	VPS_TYPE_SIZE key_hash;

	if (!item || !key)
	{
		return 0;
	}

	// Find the entry. If it doesn't exist, the operation is idempotent.
	if
	(
		!VPS_Dictionary_PRIVATE_FindEntry
		(
			item
			, key
			, &entry
			, &key_hash
			, &bucket
		)
	)
	{
		return 1;
	}

	// Save the node pointer before we release the entry it contains.
	struct VPS_List_Node *node_to_remove = entry->owner_node;

	// The node is still linked, so its parent is valid.
	// This allows the releaser to find the dictionary context and free the key/data.
	if (bucket->node_data_release)
	{
		// This call will free the 'entry' struct itself.
		bucket->node_data_release(entry); 
	}

	// Now that the entry's contents are released, remove the node from the list.
	VPS_List_Node_Remove(node_to_remove);

	// Now deconstruct and release the empty node.
	VPS_List_Node_Deconstruct(node_to_remove);
	VPS_List_Node_Release(node_to_remove);

	item->total_entries--;

	return 1;
}
