#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Rect.h>

char VPS_Rect_Init
(
	struct VPS_Rect *rect
	, struct VPS_Point position
	, struct VPS_Size size
)
{
	if (!rect)
	{
		return 0;
	}

	rect->position = position;
	rect->size = size;

	return 1;
}
