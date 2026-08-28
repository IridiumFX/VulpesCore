#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Endian.h>
#include <vulpes/VPS_DataWriter.h>

static VPS_TYPE_RESULT VPS_DataWriter_PRIVATE_EnsureCapacity
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_SIZE bytes_needed
)
{
	// Subtraction form: limit <= size always holds, and limit + bytes_needed could wrap
	if (bytes_needed > writer->target->size - writer->target->limit)
	{
		if (VPS_Data_Expand(writer->target, bytes_needed))
		{
			return VPS_FAIL;
		}
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Allocate
(
	struct VPS_DataWriter **item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	*item = calloc(1, sizeof(struct VPS_DataWriter));
	if (!*item)
	{
		return VPS_FAIL;
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Construct
(
	struct VPS_DataWriter *item
	, struct VPS_Data *target
)
{
	if (!item || !target)
	{
		return VPS_FAIL;
	}

	item->target = target;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Deconstruct
(
	struct VPS_DataWriter *item
)
{
	if (item)
	{
		item->target = 0;
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Release
(
	struct VPS_DataWriter *item
)
{
	if (item)
	{
		VPS_DataWriter_Deconstruct(item);
		free(item);
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write8U
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_8U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 1))
	{
		return VPS_FAIL;
	}

	writer->target->bytes[writer->target->limit] = value;
	writer->target->limit += 1;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write16UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 2))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write16UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 2;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write16ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 2))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write16ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 2;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write16SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
)
{
	return VPS_DataWriter_Write16UBE(writer, (VPS_TYPE_16U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_Write16SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
)
{
	return VPS_DataWriter_Write16ULE(writer, (VPS_TYPE_16U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_Write32UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 4))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write32UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 4;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write32ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 4))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write32ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 4;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write32SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
)
{
	return VPS_DataWriter_Write32UBE(writer, (VPS_TYPE_32U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_Write32SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
)
{
	return VPS_DataWriter_Write32ULE(writer, (VPS_TYPE_32U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_Write64UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 8))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write64UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 8;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write64ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
)
{
	if (!writer || !writer->target)
	{
		return VPS_FAIL;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 8))
	{
		return VPS_FAIL;
	}

	VPS_Endian_Write64ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 8;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_DataWriter_Write64SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
)
{
	return VPS_DataWriter_Write64UBE(writer, (VPS_TYPE_64U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_Write64SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
)
{
	return VPS_DataWriter_Write64ULE(writer, (VPS_TYPE_64U)value);
}

VPS_TYPE_RESULT VPS_DataWriter_WriteBytes
(
	struct VPS_DataWriter *writer
	, const unsigned char *buffer
	, VPS_TYPE_SIZE size
)
{
	if (!writer || !writer->target || (!buffer && size > 0))
	{
		return VPS_FAIL;
	}

	if (size == 0)
	{
		return VPS_OK;
	}

	if (VPS_DataWriter_PRIVATE_EnsureCapacity(writer, size))
	{
		return VPS_FAIL;
	}

	memcpy(writer->target->bytes + writer->target->limit, buffer, size);
	writer->target->limit += size;

	return VPS_OK;
}
