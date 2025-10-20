struct VPS_ScopedDictionary_Entry
{
	void *key;
	// The head of the list is the most recent version.
	struct VPS_List *versions;
	VPS_TYPE_SIZE hash;

	// Backlink to the node that owns this entry in the parent bucket's list.
	struct VPS_List_Node *owner_node;
};

struct VPS_ScopedDictionary
{
	// bucket lists: Each node->data points to a VPS_ScopedDictionary_Entry.
	struct VPS_List **bucket_vector;
	VPS_TYPE_SIZE buckets;

	// A list of VPS_Lists enclosed in VPS_List_Nodes, acting as a stack.
	// Node->data points to another VPS_List_Node whose data is a VPS_ScopedDictionary_Entry
	struct VPS_List *scopeLog;

	VPS_TYPE_SIZE total_entries;
	VPS_TYPE_SIZE growth_multiplier;
	VPS_TYPE_SIZE load_percent_threshold;
	VPS_TYPE_SIZE single_bucket_threshold;
	
	char (*hash)(void *key, VPS_TYPE_SIZE *key_hash);
	char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering);
	char (*data_compare)(void *data_1, void *data_2, VPS_TYPE_16S *ordering);
	char (*key_release)(void *key);
	char (*data_release)(void *data);
};

char VPS_ScopedDictionary_Allocate
(
	struct VPS_ScopedDictionary **item,
	VPS_TYPE_SIZE buckets
);
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
);
char VPS_ScopedDictionary_Deconstruct
(
	struct VPS_ScopedDictionary *item
);
char VPS_ScopedDictionary_Release
(
	struct VPS_ScopedDictionary *item
);

char VPS_ScopedDictionary_EnterScope
(
	struct VPS_ScopedDictionary *item
);
char VPS_ScopedDictionary_LeaveScope
(
	struct VPS_ScopedDictionary *item
);

char VPS_ScopedDictionary_Find
(
	struct VPS_ScopedDictionary *item,
	void *key,
	void **data
);
char VPS_ScopedDictionary_Add
(
	struct VPS_ScopedDictionary *item,
	void *key,
	void *data
);
char VPS_ScopedDictionary_Remove
(
	struct VPS_ScopedDictionary *item,
	void *key
);

