#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Point.h>

char VPS_Point_Init
(
	struct VPS_Point *point
	, VPS_TYPE_32S x
	, VPS_TYPE_32S y
)
{
	if (!point)
	{
		return 0;
	}

	point->x = x;
	point->y = y;

	return 1;
}
