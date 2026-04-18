#pragma once
#include <vulpes/VPS_Types.h>

struct VPS_Size
{
    VPS_TYPE_32U width;
    VPS_TYPE_32U height;
};

char VPS_Size_Init(struct VPS_Size *size, VPS_TYPE_32U width, VPS_TYPE_32U height);
