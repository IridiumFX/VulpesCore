#include <stdlib.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Decoder.h>

char VPS_Decoder_Allocate
(
	struct VPS_Decoder **item
)
{
	if (!item)
	{
		return 0;
	}

	*item = calloc(1, sizeof(struct VPS_Decoder));
	if (!*item)
	{
		return 0;
	}

	return 1;
}

char VPS_Decoder_Construct
(
	struct VPS_Decoder *item,
	char (*decode)
	(
		struct VPS_Data* source,
		struct VPS_Data* destination,
		void* context,
		VPS_TYPE_SIZE* bytes_consumed
	)
)
{
	if (!item)
	{
		return 0;
	}

	item->decode = decode;

	return 1;
}

char VPS_Decoder_Deconstruct
(
	struct VPS_Decoder *item
)
{
	if (!item)
	{
		return 0;
	}

	item->decode = NULL;

	return 1;
}

char VPS_Decoder_Release
(
	struct VPS_Decoder *item
)
{
	if (item)
	{
		VPS_Decoder_Deconstruct(item);
		free(item);
	}

	return 1;
}