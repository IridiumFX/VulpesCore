/** @brief A generic string hashing function (FNV-1a). */
char VPS_Hash_Utils_String
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

char VPS_Hash_Utils_8U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

char VPS_Hash_Utils_16U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

char VPS_Hash_Utils_32U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);

char VPS_Hash_Utils_64U
(
	void *key
	, VPS_TYPE_SIZE *key_hash
);
