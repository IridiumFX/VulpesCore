
/**
 * @brief Reads a 16-bit unsigned integer from a Big Endian byte buffer.
 */
VPS_TYPE_16U VPS_Endian_Read16UBE
(
	const unsigned char* buffer
);

/**
 * @brief Reads a 16-bit unsigned integer from a Little Endian byte buffer.
 */
VPS_TYPE_16U VPS_Endian_Read16ULE
(
	const unsigned char* buffer
);

/**
 * @brief Reads a 32-bit unsigned integer from a Big Endian byte buffer.
 */
VPS_TYPE_32U VPS_Endian_Read32UBE
(
	const unsigned char* buffer
);

/**
 * @brief Reads a 32-bit unsigned integer from a Little Endian byte buffer.
 */
VPS_TYPE_32U VPS_Endian_Read32ULE
(
	const unsigned char* buffer
);

/**
 * @brief Reads a 64-bit unsigned integer from a Big Endian byte buffer.
 */
VPS_TYPE_64U VPS_Endian_Read64UBE
(
	const unsigned char* buffer
);

/**
 * @brief Reads a 64-bit unsigned integer from a Little Endian byte buffer.
 */
VPS_TYPE_64U VPS_Endian_Read64ULE
(
	const unsigned char* buffer
);

/**
 * @brief Writes a 16-bit unsigned integer into a Big Endian byte buffer.
 */
void VPS_Endian_Write16UBE
(
	unsigned char* buffer
	, VPS_TYPE_16U value
);

/**
 * @brief Writes a 16-bit unsigned integer into a Little Endian byte buffer.
 */
void VPS_Endian_Write16ULE
(
	unsigned char* buffer
	, VPS_TYPE_16U value
);

/**
 * @brief Writes a 32-bit unsigned integer into a Big Endian byte buffer.
 */
void VPS_Endian_Write32UBE
(
	unsigned char* buffer
	, VPS_TYPE_32U value
);

/**
 * @brief Writes a 32-bit unsigned integer into a Little Endian byte buffer.
 */
void VPS_Endian_Write32ULE
(
	unsigned char* buffer
	, VPS_TYPE_32U value
);

/**
 * @brief Writes a 64-bit unsigned integer into a Big Endian byte buffer.
 */
void VPS_Endian_Write64UBE
(
	unsigned char* buffer
	, VPS_TYPE_64U value
);

/**
 * @brief Writes a 64-bit unsigned integer into a Little Endian byte buffer.
 */
void VPS_Endian_Write64ULE
(
	unsigned char* buffer
	, VPS_TYPE_64U value
);
