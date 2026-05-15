#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Endian.h>
#include <vulpes/VPS_DataWriter.h>

static char VPS_DataWriter_PRIVATE_EnsureCapacity
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_SIZE bytes_needed
)
{
	if (writer->target->limit + bytes_needed > writer->target->size)
	{
		if (!VPS_Data_Expand(writer->target, bytes_needed))
		{
			return 0;
		}
	}

	return 1;
}

char VPS_DataWriter_Allocate
(
	struct VPS_DataWriter **item
)
{
	if (!item)
	{
		return 0;
	}

	*item = calloc(1, sizeof(struct VPS_DataWriter));

	return (*item != 0);
}

char VPS_DataWriter_Construct
(
	struct VPS_DataWriter *item
	, struct VPS_Data *target
)
{
	if (!item || !target)
	{
		return 0;
	}

	item->target = target;

	return 1;
}

char VPS_DataWriter_Deconstruct
(
	struct VPS_DataWriter *item
)
{
	if (item)
	{
		item->target = 0;
	}

	return 1;
}

char VPS_DataWriter_Release
(
	struct VPS_DataWriter *item
)
{
	if (item)
	{
		VPS_DataWriter_Deconstruct(item);
		free(item);
	}

	return 1;
}

char VPS_DataWriter_Write8U
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_8U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 1))
	{
		return 0;
	}

	writer->target->bytes[writer->target->limit] = value;
	writer->target->limit += 1;

	return 1;
}

char VPS_DataWriter_Write16UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 2))
	{
		return 0;
	}

	VPS_Endian_Write16UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 2;

	return 1;
}

char VPS_DataWriter_Write16ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 2))
	{
		return 0;
	}

	VPS_Endian_Write16ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 2;

	return 1;
}

char VPS_DataWriter_Write16SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
)
{
	return VPS_DataWriter_Write16UBE(writer, (VPS_TYPE_16U)value);
}

char VPS_DataWriter_Write16SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_16S value
)
{
	return VPS_DataWriter_Write16ULE(writer, (VPS_TYPE_16U)value);
}

char VPS_DataWriter_Write32UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 4))
	{
		return 0;
	}

	VPS_Endian_Write32UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 4;

	return 1;
}

char VPS_DataWriter_Write32ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 4))
	{
		return 0;
	}

	VPS_Endian_Write32ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 4;

	return 1;
}

char VPS_DataWriter_Write32SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
)
{
	return VPS_DataWriter_Write32UBE(writer, (VPS_TYPE_32U)value);
}

char VPS_DataWriter_Write32SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_32S value
)
{
	return VPS_DataWriter_Write32ULE(writer, (VPS_TYPE_32U)value);
}

char VPS_DataWriter_Write64UBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 8))
	{
		return 0;
	}

	VPS_Endian_Write64UBE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 8;

	return 1;
}

char VPS_DataWriter_Write64ULE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64U value
)
{
	if (!writer || !writer->target)
	{
		return 0;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, 8))
	{
		return 0;
	}

	VPS_Endian_Write64ULE(writer->target->bytes + writer->target->limit, value);
	writer->target->limit += 8;

	return 1;
}

char VPS_DataWriter_Write64SBE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
)
{
	return VPS_DataWriter_Write64UBE(writer, (VPS_TYPE_64U)value);
}

char VPS_DataWriter_Write64SLE
(
	struct VPS_DataWriter *writer
	, VPS_TYPE_64S value
)
{
	return VPS_DataWriter_Write64ULE(writer, (VPS_TYPE_64U)value);
}

char VPS_DataWriter_WriteBytes
(
	struct VPS_DataWriter *writer
	, const unsigned char *buffer
	, VPS_TYPE_SIZE size
)
{
	if (!writer || !writer->target || (!buffer && size > 0))
	{
		return 0;
	}

	if (size == 0)
	{
		return 1;
	}

	if (!VPS_DataWriter_PRIVATE_EnsureCapacity(writer, size))
	{
		return 0;
	}

	memcpy(writer->target->bytes + writer->target->limit, buffer, size);
	writer->target->limit += size;

	return 1;
}
