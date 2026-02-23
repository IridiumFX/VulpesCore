struct VPS_DataWriter
{
	struct VPS_Data *target;
};

char VPS_DataWriter_Allocate
(
	struct VPS_DataWriter **item
);

char VPS_DataWriter_Construct
(
	struct VPS_DataWriter *item
	, struct VPS_Data *target
);

char VPS_DataWriter_Deconstruct
(
	struct VPS_DataWriter *item
);

char VPS_DataWriter_Release
(
	struct VPS_DataWriter *item
);

char VPS_DataWriter_Write8U
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_8U value
);

char VPS_DataWriter_Write16UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
);

char VPS_DataWriter_Write16ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
);

char VPS_DataWriter_Write16SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
);

char VPS_DataWriter_Write16SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
);

char VPS_DataWriter_Write32UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
);

char VPS_DataWriter_Write32ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
);

char VPS_DataWriter_Write32SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
);

char VPS_DataWriter_Write32SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
);

char VPS_DataWriter_Write64UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
);

char VPS_DataWriter_Write64ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
);

char VPS_DataWriter_Write64SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
);

char VPS_DataWriter_Write64SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
);

char VPS_DataWriter_WriteBytes
(
	struct VPS_DataWriter *writer
	, const unsigned char *buffer
	, VPS_TYPE_SIZE size
);
