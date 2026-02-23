#include <unistd.h>
#include <stdlib.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_StreamWriter.h>

static const VPS_TYPE_SIZE PRIVATE_VPS_STREAMWRITER_BUFFER_SIZE = 8192;

char VPS_StreamWriter_Allocate
(
	struct VPS_StreamWriter **item
)
{
	struct VPS_StreamWriter *writer;

	if (!item)
	{
		return 0;
	}

	writer = calloc(1, sizeof(struct VPS_StreamWriter));
	if (!writer)
	{
		return 0;
	}

	if
	(
		!VPS_Data_Allocate
		(
			&writer->write_buffer
			, PRIVATE_VPS_STREAMWRITER_BUFFER_SIZE
			, 0
		)
	)
	{
		free(writer);
		return 0;
	}

	writer->file_handle = -1;
	*item = writer;

	return 1;
}

char VPS_StreamWriter_Construct
(
	struct VPS_StreamWriter *item
	, int file_handle
)
{
	if (!item)
	{
		return 0;
	}

	if (!VPS_Data_Construct(item->write_buffer))
	{
		return 0;
	}

	item->file_handle = file_handle;

	return 1;
}

char VPS_StreamWriter_Deconstruct
(
	struct VPS_StreamWriter *item
)
{
	if (!item)
	{
		return 0;
	}

	VPS_StreamWriter_Flush(item);
	VPS_Data_Deconstruct(item->write_buffer);
	item->file_handle = -1;

	return 1;
}

char VPS_StreamWriter_Release
(
	struct VPS_StreamWriter *item
)
{
	if (item)
	{
		VPS_StreamWriter_Deconstruct(item);
		VPS_Data_Release(item->write_buffer);
		free(item);
	}

	return 1;
}

char VPS_StreamWriter_Flush
(
	struct VPS_StreamWriter *item
)
{
	VPS_TYPE_SIZE total_written = 0;

	if (!item || item->file_handle < 0 || !item->write_buffer)
	{
		return 0;
	}

	while (total_written < item->write_buffer->limit)
	{
		ssize_t result = write
		(
			item->file_handle
			, item->write_buffer->bytes + total_written
			, item->write_buffer->limit - total_written
		);

		if (result < 0)
		{
			return 0;
		}

		total_written += (VPS_TYPE_SIZE)result;
	}

	item->write_buffer->limit = 0;

	return 1;
}

char VPS_StreamWriter_Write
(
	struct VPS_StreamWriter *item
	, const unsigned char *data
	, VPS_TYPE_SIZE size
)
{
	VPS_TYPE_SIZE offset = 0;

	if (!item || (!data && size > 0) || item->file_handle < 0)
	{
		return 0;
	}

	while (offset < size)
	{
		VPS_TYPE_SIZE available = item->write_buffer->size - item->write_buffer->limit;
		VPS_TYPE_SIZE chunk = size - offset;

		if (chunk > available)
		{
			chunk = available;
		}

		if (chunk > 0)
		{
			memcpy
			(
				item->write_buffer->bytes + item->write_buffer->limit
				, data + offset
				, chunk
			);
			item->write_buffer->limit += chunk;
			offset += chunk;
		}

		if (item->write_buffer->limit >= item->write_buffer->size)
		{
			if (!VPS_StreamWriter_Flush(item))
			{
				return 0;
			}
		}
	}

	return 1;
}
