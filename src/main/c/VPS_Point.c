#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Point.h>

VPS_TYPE_RESULT VPS_Point_Init
(
	struct VPS_Point *point
	, VPS_TYPE_32S x
	, VPS_TYPE_32S y
)
{
	if (!point)
	{
		return VPS_FAIL;
	}

	point->x = x;
	point->y = y;

	return VPS_OK;
}
