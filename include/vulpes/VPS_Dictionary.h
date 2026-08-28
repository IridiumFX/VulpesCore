#pragma once
#include <vulpes/VPS_Types.h>

struct VPS_Dictionary_Entry
{
	void *key;
	void *data;

	VPS_TYPE_SIZE hash;

	// Backlink to the node that owns this entry in the parent bucket's list.
	struct VPS_List_Node *owner_node;
};

struct VPS_Dictionary
{
	// bucket lists: Each node->data points to a VPS_Dictionary_Entry.
	struct VPS_List **bucket_vector;
	VPS_TYPE_SIZE buckets;

	VPS_TYPE_SIZE total_entries;
	VPS_TYPE_SIZE growth_multiplier;
	VPS_TYPE_SIZE load_percent_threshold;
	VPS_TYPE_SIZE single_bucket_threshold;

	VPS_TYPE_RESULT (*hash)(void *key, VPS_TYPE_SIZE *key_hash);
	VPS_TYPE_RESULT (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering);
	VPS_TYPE_RESULT (*key_release)(void *key);
	VPS_TYPE_RESULT (*data_release)(void *data);
};

VPS_TYPE_RESULT VPS_Dictionary_Allocate
(
	struct VPS_Dictionary **item,
	VPS_TYPE_SIZE buckets
);
VPS_TYPE_RESULT VPS_Dictionary_Construct
(
	struct VPS_Dictionary *item,
	VPS_TYPE_RESULT (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
	VPS_TYPE_RESULT (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
	VPS_TYPE_RESULT (*key_release)(void *key),
	VPS_TYPE_RESULT (*data_release)(void *data),
	VPS_TYPE_SIZE growth_multiplier,
	VPS_TYPE_SIZE load_percent_threshold,
	VPS_TYPE_SIZE single_bucket_threshold
);
VPS_TYPE_RESULT VPS_Dictionary_Deconstruct
(
	struct VPS_Dictionary *item
);
VPS_TYPE_RESULT VPS_Dictionary_Release
(
	struct VPS_Dictionary *item
);

char VPS_Dictionary_Find
(
	struct VPS_Dictionary *item,
	void *key,
	void **data
);
VPS_TYPE_RESULT VPS_Dictionary_Add
(
	struct VPS_Dictionary *item,
	void *key,
	void *data
);
VPS_TYPE_RESULT VPS_Dictionary_Remove
(
	struct VPS_Dictionary *item,
	void *key
);

