#include <string.h>
#include <limits.h>
#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Hash_Utils.h>
#include <vulpes/VPS_Data.h>

// The FNV-1a hash algorithm has different standard "magic" constants for 32-bit
// and 64-bit hashes.
// A 32 bit compiler will most likely have to typedef or define an ULLONG_MAX, however, in order to support 32 bits

// FNV-1a constants
// Check if unsigned long long is > 32-bit (i.e., 64-bit)
#if ULLONG_MAX > 4294967295UL
	#define FNV_OFFSET_BASIS 14695981039346656037ULL
	#define FNV_PRIME 1099511628211ULL
#else
	#define FNV_OFFSET_BASIS 2166136261U
	#define FNV_PRIME 16777619U
#endif

char VPS_Hash_Utils_String
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	VPS_TYPE_SIZE hash = FNV_OFFSET_BASIS;
	const unsigned char* str = (const unsigned char*)key;

	while (*str)
	{
		hash ^= (VPS_TYPE_SIZE)(*str++);
		hash *= FNV_PRIME;
	}

	*key_hash = hash;
	return 1;
}

char VPS_Hash_Utils_Data
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	struct VPS_Data* data = (struct VPS_Data*)key;
	VPS_TYPE_SIZE hash = FNV_OFFSET_BASIS;
	VPS_TYPE_SIZE i;

	// Hash the readable portion of the buffer, from position to limit.
	for (i = data->position; i < data->limit; ++i)
	{
		hash ^= (VPS_TYPE_SIZE)(data->bytes[i]);
		hash *= FNV_PRIME;
	}

	*key_hash = hash;

	return 1;
}

char VPS_Hash_Utils_8U
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	// Promote to 32-bit and use the 32-bit integer hash.
	VPS_TYPE_32U h = *(VPS_TYPE_8U*)key;

	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	*key_hash = (VPS_TYPE_SIZE)h;
	return 1;
}

char VPS_Hash_Utils_16U
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	// Promote to 32-bit and use the 32-bit integer hash.
	VPS_TYPE_32U h = *(VPS_TYPE_16U*)key;

	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	*key_hash = (VPS_TYPE_SIZE)h;
	return 1;
}

char VPS_Hash_Utils_32U
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	VPS_TYPE_32U h = *(VPS_TYPE_32U*)key;

	// MurmurHash3 finalizer
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	*key_hash = (VPS_TYPE_SIZE)h;
	return 1;
}

char VPS_Hash_Utils_64U
(
	void* key,
	VPS_TYPE_SIZE* key_hash
)
{
	if (!key || !key_hash)
	{
		return 0;
	}

	VPS_TYPE_64U h = *(VPS_TYPE_64U*)key;

	// MurmurHash3 finalizer
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccdULL;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53ULL;
	h ^= h >> 33;

	*key_hash = (VPS_TYPE_SIZE)h;
	return 1;
}
