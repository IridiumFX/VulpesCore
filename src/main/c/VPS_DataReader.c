#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Endian.h>
#include <vulpes/VPS_DataReader.h>

VPS_TYPE_RESULT VPS_DataReader_Allocate
(
	struct VPS_DataReader** item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}
	*item = calloc(1, sizeof(struct VPS_DataReader));

	if (!*item)
	{
		return VPS_FAIL;
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Construct
(
	struct VPS_DataReader* item,
	struct VPS_Data* source
)
{
	if (!item || !source)
	{
		return VPS_FAIL;
	}
	item->source = source;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Deconstruct
(
	struct VPS_DataReader* item
)
{
	if (item)
	{
		item->source = 0;
	}
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Release
(
	struct VPS_DataReader* item
)
{
	if (item)
	{
		VPS_DataReader_Deconstruct(item);
		free(item);
	}
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Remaining
(
	struct VPS_DataReader* reader,
	VPS_TYPE_SIZE *remaining
)
{
	if (!reader || !reader->source || !remaining)
	{
		return VPS_FAIL;
	}

	*remaining = reader->source->limit - reader->source->position;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read8U
(
	struct VPS_DataReader* reader,
	VPS_TYPE_8U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 1)
	{
		return VPS_FAIL;
	}

	*value = reader->source->bytes[reader->source->position];
	reader->source->position += 1;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read16UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 2)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read16UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 2;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read16ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 2)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read16ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 2;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read16SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 2)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_16S)VPS_Endian_Read16UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 2;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read16SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_16S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 2)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_16S)VPS_Endian_Read16ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 2;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read32UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 4)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read32UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 4;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read32ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 4)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read32ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 4;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read32SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 4)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_32S)VPS_Endian_Read32UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 4;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read32SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_32S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 4)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_32S)VPS_Endian_Read32ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 4;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read64UBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 8)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read64UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 8;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read64ULE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64U* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 8)
	{
		return VPS_FAIL;
	}

	*value = VPS_Endian_Read64ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 8;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read64SBE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 8)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_64S)VPS_Endian_Read64UBE(reader->source->bytes + reader->source->position);
	reader->source->position += 8;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_Read64SLE
(
	struct VPS_DataReader* reader,
	VPS_TYPE_64S* value
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || !value || remaining < 8)
	{
		return VPS_FAIL;
	}

	*value = (VPS_TYPE_64S)VPS_Endian_Read64ULE(reader->source->bytes + reader->source->position);
	reader->source->position += 8;
	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataReader_ReadBytes
(
	struct VPS_DataReader* reader,
	unsigned char* buffer,
	VPS_TYPE_SIZE size
)
{
	VPS_TYPE_SIZE remaining;

	if (VPS_DataReader_Remaining(reader, &remaining) || (!buffer && size > 0) || remaining < size)
	{
		return VPS_FAIL;
	}

	if (size == 0)
	{
		return VPS_OK;
	}

	memcpy(buffer, reader->source->bytes + reader->source->position, size);
	reader->source->position += size;
	return VPS_OK;
}
