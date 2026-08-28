#pragma once
#include <vulpes/VPS_Types.h>

struct VPS_Point
{
    VPS_TYPE_32S x;
    VPS_TYPE_32S y;
};

VPS_TYPE_RESULT VPS_Point_Init(struct VPS_Point *point, VPS_TYPE_32S x, VPS_TYPE_32S y);
