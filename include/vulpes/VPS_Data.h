struct VPS_Data
{
	VPS_TYPE_SIZE size;
	VPS_TYPE_SIZE position;
	VPS_TYPE_SIZE limit;

	unsigned char *bytes;
	char own_bytes;
};

char VPS_Data_Allocate
(
	struct VPS_Data **item
);

char VPS_Data_Construct
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE size,
	VPS_TYPE_SIZE limit
);

char VPS_Data_Deconstruct
(
	struct VPS_Data *item
);

char VPS_Data_Release
(
	struct VPS_Data *item
);

char VPS_Data_Copy
(
	struct VPS_Data *item,
	struct VPS_Data *destination,
	VPS_TYPE_SIZE from,
	VPS_TYPE_SIZE size,
	VPS_TYPE_SIZE to
);

char VPS_Data_Clone
(
	struct VPS_Data **item,
	struct VPS_Data *source,
	VPS_TYPE_SIZE from,
	VPS_TYPE_SIZE size
);

char VPS_Data_Resize
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE size
);

char VPS_Data_Seek
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE offset,
	int whence
);

char VPS_Data_Expand
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE delta
);

char VPS_Data_Compact
(
	struct VPS_Data *item
);

char VPS_Data_Wrap
(
	struct VPS_Data *item,
	unsigned char *bytes,
	VPS_TYPE_SIZE size
);

char VPS_Data_Unwrap
(
	struct VPS_Data *item,
	unsigned char **str,
	VPS_TYPE_SIZE *size
);

char VPS_Data_Attach
(
	struct VPS_Data *item,
	unsigned char **bytes,
	VPS_TYPE_SIZE *size
);
