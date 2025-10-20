struct VPS_DataReader
{
	struct VPS_Data* source;
};

char VPS_DataReader_Allocate
(
	struct VPS_DataReader** item
);

char VPS_DataReader_Construct
(
	struct VPS_DataReader* item,
	struct VPS_Data* source
);

char VPS_DataReader_Deconstruct
(
	struct VPS_DataReader* item
);

char VPS_DataReader_Release
(
	struct VPS_DataReader* item
);

char VPS_DataReader_Remaining
(
	struct VPS_DataReader* reader,
	VPS_TYPE_SIZE *remaining
);

char VPS_DataReader_Read8U
(
	struct VPS_DataReader* reader,
	VPS_TYPE_8U* value
);

char VPS_DataReader_Read16UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16U* value
);

char VPS_DataReader_Read16ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16U* value
);

char VPS_DataReader_Read16SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16S* value
);

char VPS_DataReader_Read16SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16S* value
);

char VPS_DataReader_Read32UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32U* value
);

char VPS_DataReader_Read32ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32U* value
);

char VPS_DataReader_Read32SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32S* value
);

char VPS_DataReader_Read32SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32S* value
);

char VPS_DataReader_Read64UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64U* value
);

char VPS_DataReader_Read64ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64U* value
);

char VPS_DataReader_Read64SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64S* value
);

char VPS_DataReader_Read64SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64S* value
);

char VPS_DataReader_ReadBytes
(
	struct VPS_DataReader* reader,
	unsigned char* buffer,
	VPS_TYPE_SIZE size
);
