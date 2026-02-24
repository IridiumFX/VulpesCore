#include <stdlib.h>
#include <string.h>
#include <vulpes/VPS_SwissDictionary.h>

#define DEFAULT_CAPACITY 16
// Use top 7 bits for H1. Assuming 64-bit hash.
#define H1_SAFE(hash) ((unsigned char)((hash >> (sizeof(VPS_TYPE_SIZE) * 8 - 7)) & 0x7F))

static char VPS_SwissDictionary_Resize(struct VPS_SwissDictionary *item);

char VPS_SwissDictionary_Allocate
(
    struct VPS_SwissDictionary **item,
    VPS_TYPE_SIZE initial_capacity
)
{
    struct VPS_SwissDictionary *subject;

    if (!item) return 0;

    if (initial_capacity == 0) initial_capacity = DEFAULT_CAPACITY;

    subject = calloc(1, sizeof(struct VPS_SwissDictionary));
    if (!subject) return 0;

    subject->control_bytes = malloc(initial_capacity * sizeof(unsigned char));
    if (!subject->control_bytes)
    {
        free(subject);
        return 0;
    }
    // Initialize control bytes to EMPTY
    memset(subject->control_bytes, VPS_SWISS_CTRL_EMPTY, initial_capacity);

    subject->entries = calloc(initial_capacity, sizeof(struct VPS_SwissDictionary_Entry));
    if (!subject->entries)
    {
        free(subject->control_bytes);
        free(subject);
        return 0;
    }

    subject->capacity = initial_capacity;
    subject->count = 0;

    *item = subject;
    return 1;
}

char VPS_SwissDictionary_Construct
(
    struct VPS_SwissDictionary *item,
    char (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
    char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
    char (*key_release)(void *key),
    char (*data_release)(void *data),
    VPS_TYPE_SIZE growth_factor,
    VPS_TYPE_SIZE load_percent_threshold
)
{
    if (!item || !hash || !key_compare) return 0;

    item->hash = hash;
    item->key_compare = key_compare;
    item->key_release = key_release;
    item->data_release = data_release;
    item->growth_factor = growth_factor > 1 ? growth_factor : 2;
    item->load_percent_threshold = load_percent_threshold > 0 ? load_percent_threshold : 75;

    return 1;
}

char VPS_SwissDictionary_Deconstruct
(
    struct VPS_SwissDictionary *item
)
{
    if (!item) return 0;

    for (VPS_TYPE_SIZE i = 0; i < item->capacity; ++i)
    {
        if (item->control_bytes[i] != VPS_SWISS_CTRL_EMPTY && item->control_bytes[i] != VPS_SWISS_CTRL_DELETED)
        {
            if (item->key_release) item->key_release(item->entries[i].key);
            if (item->data_release) item->data_release(item->entries[i].data);
        }
    }

    return 1;
}

char VPS_SwissDictionary_Release
(
    struct VPS_SwissDictionary *item
)
{
    if (item)
    {
        VPS_SwissDictionary_Deconstruct(item);
        free(item->control_bytes);
        free(item->entries);
        free(item);
    }
    return 1;
}

static char VPS_SwissDictionary_Resize(struct VPS_SwissDictionary *item)
{
    VPS_TYPE_SIZE new_capacity = item->capacity * item->growth_factor;
    unsigned char *new_control_bytes = malloc(new_capacity * sizeof(unsigned char));
    struct VPS_SwissDictionary_Entry *new_entries = calloc(new_capacity, sizeof(struct VPS_SwissDictionary_Entry));

    if (!new_control_bytes || !new_entries)
    {
        free(new_control_bytes);
        free(new_entries);
        return 0;
    }

    memset(new_control_bytes, VPS_SWISS_CTRL_EMPTY, new_capacity);

    for (VPS_TYPE_SIZE i = 0; i < item->capacity; ++i)
    {
        if (item->control_bytes[i] != VPS_SWISS_CTRL_EMPTY && item->control_bytes[i] != VPS_SWISS_CTRL_DELETED)
        {
            // Re-insert into new arrays
            VPS_TYPE_SIZE hash = item->entries[i].hash;
            unsigned char h1 = H1_SAFE(hash);
            VPS_TYPE_SIZE index = hash % new_capacity;

            while (new_control_bytes[index] != VPS_SWISS_CTRL_EMPTY)
            {
                index = (index + 1) % new_capacity;
            }

            new_control_bytes[index] = h1;
            new_entries[index] = item->entries[i];
        }
    }

    free(item->control_bytes);
    free(item->entries);

    item->control_bytes = new_control_bytes;
    item->entries = new_entries;
    item->capacity = new_capacity;

    return 1;
}

char VPS_SwissDictionary_Find
(
    struct VPS_SwissDictionary *item,
    void *key,
    void **data
)
{
    if (!item || !key) return 0;

    VPS_TYPE_SIZE hash;
    if (!item->hash(key, &hash)) return 0;

    unsigned char h1 = H1_SAFE(hash);
    VPS_TYPE_SIZE index = hash % item->capacity;
    VPS_TYPE_SIZE start_index = index;

    do
    {
        unsigned char ctrl = item->control_bytes[index];

        if (ctrl == VPS_SWISS_CTRL_EMPTY)
        {
            return 0;
        }
        else if (ctrl == h1)
        {
            if (item->entries[index].hash == hash)
            {
                VPS_TYPE_16S ordering;
                if (item->key_compare(key, item->entries[index].key, &ordering) && ordering == 0)
                {
                    if (data) *data = item->entries[index].data;
                    return 1;
                }
            }
        }

        index = (index + 1) % item->capacity;

    } while (index != start_index);

    return 0;
}

char VPS_SwissDictionary_Add
(
    struct VPS_SwissDictionary *item,
    void *key,
    void *data
)
{
    if (!item || !key) return 0;

    // Check load factor and resize if needed
    if ((item->count + 1) * 100 >= item->capacity * item->load_percent_threshold)
    {
        if (!VPS_SwissDictionary_Resize(item)) return 0;
    }

    VPS_TYPE_SIZE hash;
    if (!item->hash(key, &hash)) return 0;

    unsigned char h1 = H1_SAFE(hash);
    VPS_TYPE_SIZE index = hash % item->capacity;
    VPS_TYPE_SIZE start_index = index;
    VPS_TYPE_SIZE insert_index = (VPS_TYPE_SIZE)-1;

    do
    {
        unsigned char ctrl = item->control_bytes[index];

        if (ctrl == VPS_SWISS_CTRL_EMPTY)
        {
            if (insert_index == (VPS_TYPE_SIZE)-1) insert_index = index;
            break; // Found empty slot, key not present
        }
        else if (ctrl == VPS_SWISS_CTRL_DELETED)
        {
            if (insert_index == (VPS_TYPE_SIZE)-1) insert_index = index;
            // Continue searching to ensure key is not present later
        }
        else if (ctrl == h1)
        {
            if (item->entries[index].hash == hash)
            {
                VPS_TYPE_16S ordering;
                if (item->key_compare(key, item->entries[index].key, &ordering) && ordering == 0)
                {
                    // Update existing
                    if (item->data_release) item->data_release(item->entries[index].data);
                    item->entries[index].data = data;
                    return 1;
                }
            }
        }

        index = (index + 1) % item->capacity;

    } while (index != start_index);

    if (insert_index != (VPS_TYPE_SIZE)-1)
    {
        item->control_bytes[insert_index] = h1;
        item->entries[insert_index].key = key;
        item->entries[insert_index].data = data;
        item->entries[insert_index].hash = hash;
        item->count++;
        return 1;
    }

    // Should not happen if resize logic is correct
    return 0;
}

char VPS_SwissDictionary_Remove
(
    struct VPS_SwissDictionary *item,
    void *key
)
{
    if (!item || !key) return 0;

    VPS_TYPE_SIZE hash;
    if (!item->hash(key, &hash)) return 0;

    unsigned char h1 = H1_SAFE(hash);
    VPS_TYPE_SIZE index = hash % item->capacity;
    VPS_TYPE_SIZE start_index = index;

    do
    {
        unsigned char ctrl = item->control_bytes[index];

        if (ctrl == VPS_SWISS_CTRL_EMPTY)
        {
            return 0; // Not found
        }
        else if (ctrl == h1)
        {
            if (item->entries[index].hash == hash)
            {
                VPS_TYPE_16S ordering;
                if (item->key_compare(key, item->entries[index].key, &ordering) && ordering == 0)
                {
                    // Found, remove it
                    if (item->key_release) item->key_release(item->entries[index].key);
                    if (item->data_release) item->data_release(item->entries[index].data);

                    item->control_bytes[index] = VPS_SWISS_CTRL_DELETED;
                    item->entries[index].key = 0;
                    item->entries[index].data = 0;
                    item->entries[index].hash = 0;
                    item->count--;
                    return 1;
                }
            }
        }

        index = (index + 1) % item->capacity;

    } while (index != start_index);

    return 0;
}
