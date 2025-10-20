#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Endian.h>

VPS_TYPE_16U VPS_Endian_Read16UBE
(
	const unsigned char* buffer
)
{
	return ((VPS_TYPE_16U)buffer[0] << 8) | buffer[1];
}

VPS_TYPE_16U VPS_Endian_Read16ULE
(
	const unsigned char* buffer
)
{
	return buffer[0] | ((VPS_TYPE_16U)buffer[1] << 8);
}

VPS_TYPE_32U VPS_Endian_Read32UBE
(
	const unsigned char* buffer
)
{
	return ((VPS_TYPE_32U)buffer[0] << 24) |
		   ((VPS_TYPE_32U)buffer[1] << 16) |
		   ((VPS_TYPE_32U)buffer[2] << 8)  |
		   buffer[3];
}

VPS_TYPE_32U VPS_Endian_Read32ULE
(
	const unsigned char* buffer
)
{
	return buffer[0] |
		   ((VPS_TYPE_32U)buffer[1] << 8)  |
		   ((VPS_TYPE_32U)buffer[2] << 16) |
		   ((VPS_TYPE_32U)buffer[3] << 24);
}

VPS_TYPE_64U VPS_Endian_Read64UBE
(
	const unsigned char* buffer
)
{
	return ((VPS_TYPE_64U)buffer[0] << 56) |
		   ((VPS_TYPE_64U)buffer[1] << 48) |
		   ((VPS_TYPE_64U)buffer[2] << 40) |
		   ((VPS_TYPE_64U)buffer[3] << 32) |
		   ((VPS_TYPE_64U)buffer[4] << 24) |
		   ((VPS_TYPE_64U)buffer[5] << 16) |
		   ((VPS_TYPE_64U)buffer[6] << 8)  |
		   buffer[7];
}

VPS_TYPE_64U VPS_Endian_Read64ULE
(
	const unsigned char* buffer
)
{
	return (VPS_TYPE_64U)buffer[0] |
		   ((VPS_TYPE_64U)buffer[1] << 8)  |
		   ((VPS_TYPE_64U)buffer[2] << 16) |
		   ((VPS_TYPE_64U)buffer[3] << 24) |
		   ((VPS_TYPE_64U)buffer[4] << 32) |
		   ((VPS_TYPE_64U)buffer[5] << 40) |
		   ((VPS_TYPE_64U)buffer[6] << 48) |
		   ((VPS_TYPE_64U)buffer[7] << 56);
}