#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>

char VPS_Data_Allocate
(
	struct VPS_Data **item
	, VPS_TYPE_SIZE size
	, VPS_TYPE_SIZE limit
)
{
	struct VPS_Data *data;

	if (!item)
	{
		return 0;
	}

	data = calloc
	(
		1
		, sizeof(struct VPS_Data)
	);
	if (!data)
	{
		return 0;
	}

	if (size > 0)
	{
		data->bytes = calloc
		(
			1
			, size
		);
		if (!data->bytes && size > 0)
		{
			goto cleanup;
		}
		data->own_bytes = 1;
	}

	data->size = size;
	data->position = 0;
	data->limit = limit;

	*item = data;

	return 1;

	cleanup:

	VPS_Data_Release(data);

	return 0;
}

char VPS_Data_Construct
(
	struct VPS_Data *item
)
{
	if (!item)
	{
		return 0;
	}

	return 1;
}

char VPS_Data_Deconstruct
(
	struct VPS_Data *item
)
{
	if (!item)
	{
		return 0;
	}

	return 1;
}

char VPS_Data_Release
(
	struct VPS_Data *item
)
{
	if (item)
	{
		VPS_Data_Deconstruct(item);

		// If we own the buffer, we are responsible for freeing it.
		if (item->own_bytes && item->bytes)
		{
			free(item->bytes);
			item->bytes = 0;
		}

		free(item);
	}

	return 1;
}

char VPS_Data_Copy
(
	struct VPS_Data *item,
	struct VPS_Data *destination,
	VPS_TYPE_SIZE from,
	VPS_TYPE_SIZE size,
	VPS_TYPE_SIZE to
)
{
	if (!item || !destination || !item->bytes || !destination->bytes)
	{
		return 0;
	}

	// Bounds checking
	if (size > item->limit || from > item->limit - size || // Check source bounds safely
		size > destination->size || to > destination->size - size // Check destination bounds safely
	)
	{
		return 0;
	}

	memcpy(destination->bytes + to, item->bytes + from, size);

	if ((to + size) > destination->limit)
	{
		destination->limit = to + size;
	}

	return 1;
}

char VPS_Data_Clone
(
	struct VPS_Data **item,
	struct VPS_Data *source,
	VPS_TYPE_SIZE from,
	VPS_TYPE_SIZE size
)
{
	struct VPS_Data *clone = 0;

	if (!item || !source || !source->bytes)
	{
		return 0;
	}

	// Bounds checking
	if ((from + size) > source->limit)
	{
		return 0;
	}

	if (!VPS_Data_Allocate(&clone, size, size))
	{
		return 0;
	}

	if (!VPS_Data_Construct(clone))
	{
		VPS_Data_Release(clone);
		return 0;
	}

	memcpy(clone->bytes, source->bytes + from, size);
	*item = clone;

	return 1;
}

char VPS_Data_Resize
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE new_size
)
{
	unsigned char *new_bytes;

	if (!item)
	{
		return 0;
	}

	// Cannot resize a buffer we don't own
	if (!item->own_bytes)
	{
		return 0;
	}

	new_bytes = realloc(item->bytes, new_size);
	if (!new_bytes && new_size > 0)
	{
		// realloc failed, original block is untouched
		return 0;
	}

	item->bytes = new_bytes;
	item->size = new_size;

	// Adjust pointers to stay within new bounds
	if (item->limit > new_size)
	{
		item->limit = new_size;
	}
	if (item->position > item->limit)
	{
		item->position = item->limit;
	}

	return 1;
}

char VPS_Data_Expand
(
	struct VPS_Data *item,
	VPS_TYPE_SIZE delta
)
{
	VPS_TYPE_SIZE new_size;

	if (!item)
	{
		return 0;
	}

	new_size = item->size + delta;
	if (new_size < item->size)
	{
		// Overflow
		return 0;
	}

	return VPS_Data_Resize(item, new_size);
}

char VPS_Data_Compact
(
	struct VPS_Data *item
)
{
	VPS_TYPE_SIZE length;

	if (!item || !item->bytes)
	{
		return 0;
	}

	if (item->position == 0)
	{
		return 1; // Nothing to do
	}

	length = item->limit - item->position;
	if (length > 0)
	{
		// Use memmove as regions are guaranteed to overlap
		memmove(item->bytes, item->bytes + item->position, length);
	}

	// Update pointers relative to the new start
	item->limit = length;
	item->position = 0;

	return 1;
}

char VPS_Data_Wrap
(
	struct VPS_Data *item,
	unsigned char *bytes,
	VPS_TYPE_SIZE size
)
{
	if (!item)
	{
		return 0;
	}

	VPS_Data_Deconstruct(item);
	if (item->own_bytes && item->bytes)
	{
		free(item->bytes);
		item->bytes = 0;
	}

	item->bytes = bytes;
	item->own_bytes = 0; // We do not own this memory
	item->size = size;
	item->position = 0;
	item->limit = size;

	return 1;
}

char VPS_Data_Unwrap
(
	struct VPS_Data *item,
	unsigned char **str,
	VPS_TYPE_SIZE *size
)
{
	if (!item || !str || !size)
	{
		return 0;
	}

	// Cannot unwrap a buffer we own, as the caller would
	// not know whether to free it or not.
	if (item->own_bytes)
	{
		return 0;
	}

	*str = item->bytes;
	*size = item->size;

	// Reset the struct to a safe, empty state
	VPS_Data_Deconstruct(item);
	item->bytes = 0;

	return 1;
}

char VPS_Data_Seek
(
	struct VPS_Data *item,
	VPS_TYPE_SPAN offset,
	int whence
)
{
	VPS_TYPE_SPAN new_pos;

	if (!item)
	{
		return 0;
	}

	switch (whence)
	{
		case SEEK_SET:
			new_pos = offset; // offset should be positive
			break;
		case SEEK_CUR:
			new_pos = (VPS_TYPE_SPAN)item->position + offset;
			break;
		case SEEK_END:
			new_pos = (VPS_TYPE_SPAN)item->limit + offset; // offset is typically negative here
			break;
		default:
			return 0; // Invalid 'whence'
	}

	// Bounds check: position cannot go past the limit
	if (new_pos < 0 || (VPS_TYPE_SIZE)new_pos > item->limit)
	{
		return 0;
	}

	item->position = (VPS_TYPE_SIZE)new_pos;
	return 1;
}

char VPS_Data_Attach
(
	struct VPS_Data *item,
	unsigned char **bytes,
	VPS_TYPE_SIZE *size
)
{
	if (!item || !bytes || !size)
	{
		return 0;
	}

	// Return a pointer to the current write position
	*bytes = item->bytes + item->position;
	// Return the remaining writable space in the buffer
	*size = item->limit - item->position;

	return 1;
}