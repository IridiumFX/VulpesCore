#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Size.h>

VPS_TYPE_RESULT VPS_Size_Init
(
	struct VPS_Size *size
	, VPS_TYPE_32U width
	, VPS_TYPE_32U height
)
{
	if (!size)
	{
		return VPS_FAIL;
	}

	size->width = width;
	size->height = height;

	return VPS_OK;
}
