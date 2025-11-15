#include <unistd.h>
#include <stdlib.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Decoder.h>
#include <vulpes/VPS_StreamReader.h>

static const VPS_TYPE_SIZE PRIVATE_VPS_STREAMREADER_BUFFER_SIZE = 4096;

char VPS_StreamReader_Allocate
(
	struct VPS_StreamReader **item
)
{
	if (!item)
	{
		return 0;
	}

	*item = calloc
	(
		1
		, sizeof(struct VPS_StreamReader)
	);
	if (!*item)
	{
		return 0;
	}

	return 1;
}

char VPS_StreamReader_Construct
(
	struct VPS_StreamReader *item
	, struct VPS_Data *destination_buffer
	, int file_handle
	, struct VPS_Data *raw_buffer
	, char take_ownership
)
{
	if (!item || !destination_buffer)
	{
		return 0;
	}

	if (raw_buffer)
	{
		// Use the user-provided buffer.
		item->raw_buffer = raw_buffer;
		item->own_raw_buffer = take_ownership;
	}
	else
	{
		// No buffer provided, so allocate and construct our own internal one.
		if
		(
			!VPS_Data_Allocate
			(
				&item->raw_buffer
				, PRIVATE_VPS_STREAMREADER_BUFFER_SIZE
				, 0
			)
		)
		{
			return 0;
		}

		if
		(
			!VPS_Data_Construct
			(
				item->raw_buffer
			)
		)
		{
			VPS_Data_Release
			(
				item->raw_buffer
			);
			item->raw_buffer = 0;

			return 0;
		}
		item->own_raw_buffer = 1;
	}

	item->decoded_buffer = destination_buffer;
	item->file_handle = file_handle;

	return 1;
}

char VPS_StreamReader_Deconstruct
(
	struct VPS_StreamReader *item
)
{
	if (!item)
	{
		return 0;
	}

	if (item->own_raw_buffer)
	{
		VPS_Data_Deconstruct
		(
			item->raw_buffer
		);
	}

	item->decoded_buffer = 0;
	item->raw_buffer = 0;
	item->file_handle = -1;

	return 1;
}

char VPS_StreamReader_Release
(
	struct VPS_StreamReader *item
)
{
	if (item)
	{
		VPS_StreamReader_Deconstruct
		(
			item
		);

		if (item->own_raw_buffer)
		{
			VPS_Data_Release
			(
				item->raw_buffer
			);
		}
		free
		(
			item
		);
	}

	return 1;
}

char VPS_StreamReader_Read
(
	struct VPS_StreamReader *item
	, VPS_TYPE_SIZE new_bytes_to_read
	, VPS_TYPE_SIZE *bytes_read_from_source
	, VPS_TYPE_SIZE *bytes_consumed_by_decoder
	, VPS_TYPE_SIZE *bytes_produced_by_decoder
	, VPS_TYPE_SIZE *bytes_available_to_use
	, struct VPS_Decoder *decoder
	, void *decoder_context
)
{
	ssize_t read_result;
	VPS_TYPE_SIZE bytes_consumed;

	if (!item || !item->decoded_buffer || !item->raw_buffer || item->file_handle < 0 || !decoder || !decoder->decode)
	{
		return 0;
	}

	// 1. Compact the internal buffer. This moves any unprocessed data from the
	// previous read to the beginning of the buffer, making room for new data.
	VPS_Data_Compact
	(
		item->raw_buffer
	);

	// 2. Determine how many bytes we *still* need to read from the OS to
	//    satisfy the caller's request, after accounting for what's already buffered.
	VPS_TYPE_SIZE bytes_still_needed = 0;
	if (new_bytes_to_read > item->raw_buffer->limit)
	{
		bytes_still_needed = new_bytes_to_read - item->raw_buffer->limit;
	}

	// 3. Ensure our internal raw buffer has enough capacity for the new data.
	VPS_TYPE_SIZE required_raw_size = item->raw_buffer->limit + bytes_still_needed;
	if (item->raw_buffer->size < required_raw_size)
	{
		if (!VPS_Data_Resize(item->raw_buffer, required_raw_size))
		{
			// Resize failed
			return 0;
		}
	}

	// 4. Read the required number of new bytes from the file handle.
	read_result = 0;
	if (bytes_still_needed > 0)
	{
		read_result = read
		(
			item->file_handle
			, item->raw_buffer->bytes + item->raw_buffer->limit
			, bytes_still_needed
		);
	}

	if (read_result < 0)
	{
		// Read error
		return 0;
	}

	// 5. Update the raw buffer's limit to include the newly read data.
	item->raw_buffer->limit += (VPS_TYPE_SIZE) read_result;
	if (bytes_read_from_source)
	{
		*bytes_read_from_source = (VPS_TYPE_SIZE) read_result;
	}

	// 6. Process the data.
	// Use the stateless decoder to transform from our raw_buffer to the user's destination buffer.
	bytes_consumed = 0;
	// Capture the state of the destination buffer *before* decoding to accurately measure what's new.
	VPS_TYPE_SIZE initial_decoded_bytes = item->decoded_buffer->limit - item->decoded_buffer->position;

	if
	(
		!decoder->decode
		(
			item->raw_buffer
			, item->decoded_buffer
			, decoder_context
			, &bytes_consumed
		)
	)
	{
		return 0;
	}

	// 7. Advance the raw buffer's position by the amount the decoder consumed.
	// The unconsumed data will be handled by Compact() on the next call, creating
	// a "sliding window" effect.
	item->raw_buffer->position = bytes_consumed;

	if (bytes_consumed_by_decoder)
	{
		*bytes_consumed_by_decoder = bytes_consumed;
	}

	if (bytes_produced_by_decoder)
	{
		// The number of bytes produced is the difference in the destination buffer's
		// using the difference between available now vs before decoding
		*bytes_produced_by_decoder = (item->decoded_buffer->limit - item->decoded_buffer->position) - initial_decoded_bytes;
	}

	if (bytes_available_to_use)
	{
		*bytes_available_to_use = item->decoded_buffer->limit - item->decoded_buffer->position;
	}

	return 1;
}

char VPS_StreamReader_Seek
(
	struct VPS_StreamReader *item
	, VPS_TYPE_64S offset
	, int whence
)
{
	if (!item || item->file_handle < 0)
	{
		return 0;
	}

	// Invalidate internal buffers since we are moving the file pointer.
	if (item->raw_buffer)
	{
		item->raw_buffer->position = 0;
		item->raw_buffer->limit = 0;
	}

	// Attempt a true seek first. This will succeed for files.
	if (lseek(item->file_handle, offset, whence) != -1)
	{
		return 1;
	}

	// If lseek fails (e.g., on a pipe or socket) and it's a forward skip,
	// fall back to reading and discarding data.
	if (whence == SEEK_CUR && offset > 0)
	{
		VPS_TYPE_64U remaining_to_skip = offset;
		// TODO: move the fixed size temp buffer to the reader itself
		char temp_buf[4096];
		while (remaining_to_skip > 0)
		{
			ssize_t bytes_to_read = (remaining_to_skip > sizeof(temp_buf)) ? sizeof(temp_buf) : remaining_to_skip;
			ssize_t bytes_read = read(item->file_handle, temp_buf, bytes_to_read);
			if (bytes_read <= 0)
			{
				// Error or EOF before we finished skipping.
				return 0;
			}
			remaining_to_skip -= bytes_read;
		}
		return 1;
	}

	// Seek failed for a reason we don't support (e.g., seeking backwards on a stream).
	return 0;
}
