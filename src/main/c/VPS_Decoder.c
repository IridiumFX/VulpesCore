#include <stdlib.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Decoder.h>

VPS_TYPE_RESULT VPS_Decoder_Allocate
(
	struct VPS_Decoder **item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	*item = calloc(1, sizeof(struct VPS_Decoder));
	if (!*item)
	{
		return VPS_FAIL;
	}

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_Decoder_Construct
(
	struct VPS_Decoder *item,
	VPS_TYPE_RESULT (*decode)
	(
		struct VPS_Data* source,
		struct VPS_Data* destination,
		void* context,
		VPS_TYPE_SIZE* bytes_consumed
	)
)
{
	// The decode callback is invoked unguarded by the stream readers.
	if (!item || !decode)
	{
		return VPS_FAIL;
	}

	item->decode = decode;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_Decoder_Deconstruct
(
	struct VPS_Decoder *item
)
{
	if (!item)
	{
		return VPS_FAIL;
	}

	item->decode = NULL;

	return VPS_OK;
}

VPS_TYPE_RESULT VPS_Decoder_Release
(
	struct VPS_Decoder *item
)
{
	if (item)
	{
		VPS_Decoder_Deconstruct(item);
		free(item);
	}

	return VPS_OK;
}