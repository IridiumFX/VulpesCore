#include <stdlib.h>
#include <pthread.h>
#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_ConcurrentDictionary.h>

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Allocate
(
    struct VPS_ConcurrentDictionary **item,
    VPS_TYPE_SIZE buckets
)
{
    struct VPS_ConcurrentDictionary *subject;

    if (!item)
    {
        return VPS_FAIL;
    }

    subject = calloc(1, sizeof(struct VPS_ConcurrentDictionary));
    if (!subject)
    {
        return VPS_FAIL;
    }

    if (VPS_Dictionary_Allocate(&subject->dictionary, buckets))
    {
        free(subject);
        return VPS_FAIL;
    }

    if (pthread_mutex_init(&subject->mutex, NULL) != 0)
    {
        VPS_Dictionary_Release(subject->dictionary);
        free(subject);
        return VPS_FAIL;
    }

    *item = subject;
    return VPS_OK;
}

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Construct
(
    struct VPS_ConcurrentDictionary *item,
    VPS_TYPE_RESULT (*hash)(void *key, VPS_TYPE_SIZE *key_hash),
    VPS_TYPE_RESULT (*key_compare)(void *key_1, void *key_2, VPS_TYPE_16S *ordering),
    VPS_TYPE_RESULT (*key_release)(void *key),
    VPS_TYPE_RESULT (*data_release)(void *data),
    VPS_TYPE_SIZE growth_multiplier,
    VPS_TYPE_SIZE load_percent_threshold,
    VPS_TYPE_SIZE single_bucket_threshold
)
{
    if (!item)
    {
        return VPS_FAIL;
    }

    return VPS_Dictionary_Construct(
        item->dictionary,
        hash,
        key_compare,
        key_release,
        data_release,
        growth_multiplier,
        load_percent_threshold,
        single_bucket_threshold
    );
}

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Deconstruct
(
    struct VPS_ConcurrentDictionary *item
)
{
    if (!item)
    {
        return VPS_FAIL;
    }

    // The mutex must stay valid here: Release re-runs Deconstruct, so the
    // mutex is destroyed exactly once, in Release.
    pthread_mutex_lock(&item->mutex);
    VPS_Dictionary_Deconstruct(item->dictionary);
    pthread_mutex_unlock(&item->mutex);

    return VPS_OK;
}

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Release
(
    struct VPS_ConcurrentDictionary *item
)
{
    if (item)
    {
        VPS_ConcurrentDictionary_Deconstruct(item);
        pthread_mutex_destroy(&item->mutex);
        VPS_Dictionary_Release(item->dictionary);
        free(item);
    }
    return VPS_OK;
}

char VPS_ConcurrentDictionary_Find
(
    struct VPS_ConcurrentDictionary *item,
    void *key,
    void **data
)
{
    char result;

    if (!item)
    {
        return 0;
    }

    pthread_mutex_lock(&item->mutex);
    result = VPS_Dictionary_Find(item->dictionary, key, data);
    pthread_mutex_unlock(&item->mutex);

    return result;
}

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Add
(
    struct VPS_ConcurrentDictionary *item,
    void *key,
    void *data
)
{
    VPS_TYPE_RESULT result;

    if (!item)
    {
        return VPS_FAIL;
    }

    pthread_mutex_lock(&item->mutex);
    result = VPS_Dictionary_Add(item->dictionary, key, data);
    pthread_mutex_unlock(&item->mutex);

    return result;
}

VPS_TYPE_RESULT VPS_ConcurrentDictionary_Remove
(
    struct VPS_ConcurrentDictionary *item,
    void *key
)
{
    VPS_TYPE_RESULT result;

    if (!item)
    {
        return VPS_FAIL;
    }

    pthread_mutex_lock(&item->mutex);
    result = VPS_Dictionary_Remove(item->dictionary, key);
    pthread_mutex_unlock(&item->mutex);

    return result;
}
