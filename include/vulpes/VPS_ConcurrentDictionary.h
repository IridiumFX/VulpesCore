#include <pthread.h>
#include <vulpes/VPS_Dictionary.h>

struct VPS_ConcurrentDictionary
{
    struct VPS_Dictionary *dictionary;
    pthread_mutex_t mutex;
};

char VPS_ConcurrentDictionary_Allocate
(
    struct VPS_ConcurrentDictionary **item,
    VPS_TYPE_SIZE buckets
);

char VPS_ConcurrentDictionary_Construct
(
    struct VPS_ConcurrentDictionary *item,
    char (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
    char (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
    char (*key_release)(void *key),
    char (*data_release)(void *data),
    VPS_TYPE_SIZE growth_multiplier,
    VPS_TYPE_SIZE load_percent_threshold,
    VPS_TYPE_SIZE single_bucket_threshold
);

char VPS_ConcurrentDictionary_Deconstruct
(
    struct VPS_ConcurrentDictionary *item
);

char VPS_ConcurrentDictionary_Release
(
    struct VPS_ConcurrentDictionary *item
);

char VPS_ConcurrentDictionary_Find
(
    struct VPS_ConcurrentDictionary *item,
    void *key,
    void **data
);

char VPS_ConcurrentDictionary_Add
(
    struct VPS_ConcurrentDictionary *item,
    void *key,
    void *data
);

char VPS_ConcurrentDictionary_Remove
(
    struct VPS_ConcurrentDictionary *item,
    void *key
);
