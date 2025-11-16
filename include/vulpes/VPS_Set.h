struct VPS_Set_Entry
{
	void *item;
	VPS_TYPE_SIZE hash;

	struct VPS_List_Node *owner_node;
};

struct VPS_Set
{
	struct VPS_List **bucket_vector;
	VPS_TYPE_SIZE buckets;
	VPS_TYPE_SIZE total_entries;

	// Callbacks for item management
	char (*hash)(void *item, VPS_TYPE_SIZE *item_hash);
	char (*item_compare)(void *item_1, void *item_2, VPS_TYPE_16S *ordering);
	char (*item_release)(void *item);

	// Rehashing policy
	VPS_TYPE_SIZE growth_multiplier;
	VPS_TYPE_SIZE load_percent_threshold;
	VPS_TYPE_SIZE single_bucket_threshold;
};

char VPS_Set_Allocate
(
	struct VPS_Set **item,
	VPS_TYPE_SIZE buckets
);

char VPS_Set_Construct
(
	struct VPS_Set *item,
	char (*hash)(void *item, VPS_TYPE_SIZE *item_hash),
	char (*item_compare)(void *item_1, void *item_2, VPS_TYPE_16S *ordering),
	char (*item_release)(void *item),
	VPS_TYPE_SIZE growth_multiplier,
	VPS_TYPE_SIZE load_percent_threshold,
	VPS_TYPE_SIZE single_bucket_threshold
);

char VPS_Set_Deconstruct
(
	struct VPS_Set *item
);

char VPS_Set_Release
(
	struct VPS_Set *item
);

char VPS_Set_Contains
(
	struct VPS_Set *item,
	void *value
);

char VPS_Set_Add
(
	struct VPS_Set *item,
	void *value
);

char VPS_Set_Remove
(
	struct VPS_Set *item,
	void *value
);
