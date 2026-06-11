#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Size.h>

char VPS_Size_Init
(
	struct VPS_Size *size
	, VPS_TYPE_32U width
	, VPS_TYPE_32U height
)
{
	if (!size)
	{
		return 0;
	}

	size->width = width;
	size->height = height;

	return 1;
}
