#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Decoder.h>
#include <vulpes/VPS_Decoder_Base256.h>

/**
 * @brief The actual implementation of the Base256 decode function.
 *
 * This function copies the available data from the source buffer to the
 * destination buffer, resizing the destination if necessary.
 */
static char vps_base256_decode
(
	struct VPS_Data* source,
	struct VPS_Data* destination,
	void* context,
	VPS_TYPE_SIZE* bytes_consumed
)
{
	VPS_TYPE_SIZE bytes_to_copy;

	(void)context; // Unused for this simple decoder

	if (!source || !destination || !bytes_consumed)
	{
		return 0;
	}

	// 1. Compact the destination buffer to preserve any unconsumed data.
	VPS_Data_Compact(destination);

	bytes_to_copy = source->limit - source->position;
	VPS_TYPE_SIZE write_pos = destination->limit;

	// Ensure destination has enough capacity
	if (destination->size < write_pos + bytes_to_copy)
	{
		// Attempt to expand the buffer to make room for the new data.
		if (!VPS_Data_Resize(destination, write_pos + bytes_to_copy))
		{
			return 0; // Resize failed
		}
	}

	// Copy the new data to the end of the existing valid data in the destination.
	// The now-robust VPS_Data_Copy will handle updating the destination's limit correctly.
	VPS_Data_Copy(source, destination, source->position, bytes_to_copy, write_pos);

	// Report that we consumed all available bytes from the source
	*bytes_consumed = bytes_to_copy;

	return 1;
}

char VPS_Decoder_Base256_Construct
(
	struct VPS_Decoder* decoder
)
{
	if (!decoder)
	{
		return 0;
	}

	// Use the generic construction function, providing our private decode implementation.
	return VPS_Decoder_Construct(decoder, vps_base256_decode);
}