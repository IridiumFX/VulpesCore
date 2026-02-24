#ifndef VPS_SWISS_DICTIONARY_H
#define VPS_SWISS_DICTIONARY_H

#include <vulpes/VPS_Types.h>

// Control byte values
// Empty slot
#define VPS_SWISS_CTRL_EMPTY 0x80
// Deleted slot (tombstone)
#define VPS_SWISS_CTRL_DELETED 0xFE
// Sentinel at the end of the group (if using groups, but here we use linear probing)
// For simplicity in this implementation, we'll use 0-127 for valid hash prefixes (H1)
// and special values for empty/deleted.

struct VPS_SwissDictionary_Entry
{
    void *key;
    void *data;
    VPS_TYPE_SIZE hash; // Full hash
};

struct VPS_SwissDictionary
{
    // Control bytes array. One byte per entry.
    // Stores the top 7 bits of the hash (H1) or special values (EMPTY, DELETED).
    unsigned char *control_bytes;

    struct VPS_SwissDictionary_Entry *entries;

    VPS_TYPE_SIZE capacity;
    VPS_TYPE_SIZE count;

    VPS_TYPE_SIZE growth_factor;
    VPS_TYPE_SIZE load_percent_threshold;

    char (*hash)(void *key, VPS_TYPE_SIZE *key_hash);
    char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering);
    char (*key_release)(void *key);
    char (*data_release)(void *data);
};

char VPS_SwissDictionary_Allocate
(
    struct VPS_SwissDictionary **item,
    VPS_TYPE_SIZE initial_capacity
);

char VPS_SwissDictionary_Construct
(
    struct VPS_SwissDictionary *item,
    char (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
    char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
    char (*key_release)(void *key),
    char (*data_release)(void *data),
    VPS_TYPE_SIZE growth_factor,
    VPS_TYPE_SIZE load_percent_threshold
);

char VPS_SwissDictionary_Deconstruct
(
    struct VPS_SwissDictionary *item
);

char VPS_SwissDictionary_Release
(
    struct VPS_SwissDictionary *item
);

char VPS_SwissDictionary_Find
(
    struct VPS_SwissDictionary *item,
    void *key,
    void **data
);

char VPS_SwissDictionary_Add
(
    struct VPS_SwissDictionary *item,
    void *key,
    void *data
);

char VPS_SwissDictionary_Remove
(
    struct VPS_SwissDictionary *item,
    void *key
);

#endif
