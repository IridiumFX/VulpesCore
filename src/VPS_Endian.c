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

void VPS_Endian_Write16UBE
(
	unsigned char* buffer
	, VPS_TYPE_16U value
)
{
	buffer[0] = (unsigned char)(value >> 8);
	buffer[1] = (unsigned char)(value);
}

void VPS_Endian_Write16ULE
(
	unsigned char* buffer
	, VPS_TYPE_16U value
)
{
	buffer[0] = (unsigned char)(value);
	buffer[1] = (unsigned char)(value >> 8);
}

void VPS_Endian_Write32UBE
(
	unsigned char* buffer
	, VPS_TYPE_32U value
)
{
	buffer[0] = (unsigned char)(value >> 24);
	buffer[1] = (unsigned char)(value >> 16);
	buffer[2] = (unsigned char)(value >> 8);
	buffer[3] = (unsigned char)(value);
}

void VPS_Endian_Write32ULE
(
	unsigned char* buffer
	, VPS_TYPE_32U value
)
{
	buffer[0] = (unsigned char)(value);
	buffer[1] = (unsigned char)(value >> 8);
	buffer[2] = (unsigned char)(value >> 16);
	buffer[3] = (unsigned char)(value >> 24);
}

void VPS_Endian_Write64UBE
(
	unsigned char* buffer
	, VPS_TYPE_64U value
)
{
	buffer[0] = (unsigned char)(value >> 56);
	buffer[1] = (unsigned char)(value >> 48);
	buffer[2] = (unsigned char)(value >> 40);
	buffer[3] = (unsigned char)(value >> 32);
	buffer[4] = (unsigned char)(value >> 24);
	buffer[5] = (unsigned char)(value >> 16);
	buffer[6] = (unsigned char)(value >> 8);
	buffer[7] = (unsigned char)(value);
}

void VPS_Endian_Write64ULE
(
	unsigned char* buffer
	, VPS_TYPE_64U value
)
{
	buffer[0] = (unsigned char)(value);
	buffer[1] = (unsigned char)(value >> 8);
	buffer[2] = (unsigned char)(value >> 16);
	buffer[3] = (unsigned char)(value >> 24);
	buffer[4] = (unsigned char)(value >> 32);
	buffer[5] = (unsigned char)(value >> 40);
	buffer[6] = (unsigned char)(value >> 48);
	buffer[7] = (unsigned char)(value >> 56);
}