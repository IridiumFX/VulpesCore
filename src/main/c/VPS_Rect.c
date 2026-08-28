#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Rect.h>

VPS_TYPE_RESULT VPS_Rect_Init
(
	struct VPS_Rect *rect
	, struct VPS_Point position
	, struct VPS_Size size
)
{
	if (!rect)
	{
		return VPS_FAIL;
	}

	rect->position = position;
	rect->size = size;

	return VPS_OK;
}
