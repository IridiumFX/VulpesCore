#pragma once
#include <vulpes/VPS_Types.h>

/** @brief A generic string hashing function (FNV-1a). */
VPS_TYPE_RESULT VPS_Hash_Utils_String
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

/** @brief A hashing function for VPS_Data objects (FNV-1a). */
VPS_TYPE_RESULT VPS_Hash_Utils_Data
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

VPS_TYPE_RESULT VPS_Hash_Utils_8U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

VPS_TYPE_RESULT VPS_Hash_Utils_16U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

VPS_TYPE_RESULT VPS_Hash_Utils_32U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

VPS_TYPE_RESULT VPS_Hash_Utils_64U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);
