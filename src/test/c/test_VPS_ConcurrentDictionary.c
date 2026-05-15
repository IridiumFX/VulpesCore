#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_ConcurrentDictionary.h>
#include <vulpes/VPS_Hash_Utils.h>
#include <vulpes/VPS_Compare_Utils.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

// --- Helper functions for dictionary tests ---
static char hash_string(void *key, VPS_TYPE_SIZE *key_hash)
{
    // Use the public API for string hashing
    return VPS_Hash_Utils_String(key, key_hash);
}

static char compare_string(void *key_1, void *key_2, VPS_TYPE_16S *ordering)
{
    *ordering = (VPS_TYPE_16S)strcmp((const char*)key_1, (const char*)key_2);
    return 1;
}

static char release_string(void *key)
{
    free(key);
    return 1;
}

static char release_int(void *data)
{
    free(data);
    return 1;
}

// --- Concurrent Test Data ---
#define NUM_THREADS 4
#define NUM_OPERATIONS_PER_THREAD 1000

struct ThreadData {
    struct VPS_ConcurrentDictionary *dict;
    int thread_id;
};

void *concurrent_operations(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    struct VPS_ConcurrentDictionary *dict = data->dict;
    int thread_id = data->thread_id;

    for (int i = 0; i < NUM_OPERATIONS_PER_THREAD; ++i) {
        char *key = (char *)malloc(32);
        sprintf(key, "key_%d_%d", thread_id, i);
        int *value = (int *)malloc(sizeof(int));
        *value = thread_id * 1000 + i;

        // Add operation
        VPS_ConcurrentDictionary_Add(dict, key, value);

        // Find operation
        void *found_value = NULL;
        if (VPS_ConcurrentDictionary_Find(dict, key, &found_value)) {
            // Check if the found value matches
            if (*((int*)found_value) != *value) {
                fprintf(stderr, "Thread %d: Found value mismatch for key %s\n", thread_id, key);
                // This is a failure, but we don't want to stop the test
            }
        } else {
            fprintf(stderr, "Thread %d: Failed to find key %s after adding\n", thread_id, key);
        }

        // Remove operation (only remove some to keep others for final check)
        if (i % 2 == 0) {
            if (!VPS_ConcurrentDictionary_Remove(dict, key)) {
                // If remove failed (e.g. not found), we might need to free key/value if we still own them?
                // But here we just added it, so it should be there unless another thread removed it (which shouldn't happen with unique keys per thread)
            }
            // Dictionary handles freeing key/value on removal.
        } else {
            // If not removed, the dictionary will own the key and value
        }
    }
    return NULL;
}


/**
 * @test test_concurrent_dictionary_basic
 * @brief Tests basic add, find, remove operations on the concurrent dictionary.
 */
static char test_concurrent_dictionary_basic()
{
    struct VPS_ConcurrentDictionary *dict = 0;
    char *key1 = strdup("hello");
    char *key2 = strdup("world");
    int *val1 = (int*)malloc(sizeof(int)); *val1 = 100;
    int *val2 = (int*)malloc(sizeof(int)); *val2 = 200;
    void *found_val = NULL;

    VPS_ConcurrentDictionary_Allocate(&dict, 10);
    TEST_ASSERT(dict != NULL);

    VPS_ConcurrentDictionary_Construct(dict, hash_string, compare_string, release_string, release_int, 2, 75, 5);

    // Add
    TEST_ASSERT(VPS_ConcurrentDictionary_Add(dict, key1, val1));
    TEST_ASSERT(VPS_ConcurrentDictionary_Add(dict, key2, val2));

    // Find
    TEST_ASSERT(VPS_ConcurrentDictionary_Find(dict, "hello", &found_val));
    TEST_ASSERT(*((int*)found_val) == 100);

    TEST_ASSERT(VPS_ConcurrentDictionary_Find(dict, "world", &found_val));
    TEST_ASSERT(*((int*)found_val) == 200);

    // Try to find non-existent key
    TEST_ASSERT(!VPS_ConcurrentDictionary_Find(dict, "nonexistent", &found_val));

    // Remove
    TEST_ASSERT(VPS_ConcurrentDictionary_Remove(dict, "hello"));
    TEST_ASSERT(!VPS_ConcurrentDictionary_Find(dict, "hello", &found_val)); // Should not be found after removal

    // Add again
    char *key3 = strdup("newkey");
    int *val3 = (int*)malloc(sizeof(int)); *val3 = 300;
    TEST_ASSERT(VPS_ConcurrentDictionary_Add(dict, key3, val3));
    TEST_ASSERT(VPS_ConcurrentDictionary_Find(dict, "newkey", &found_val));
    TEST_ASSERT(*((int*)found_val) == 300);

    VPS_ConcurrentDictionary_Release(dict);

    return 1;
}

/**
 * @test test_concurrent_dictionary_multithreaded
 * @brief Tests concurrent add, find, remove operations from multiple threads.
 */
static char test_concurrent_dictionary_multithreaded()
{
    struct VPS_ConcurrentDictionary *dict = 0;
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    VPS_ConcurrentDictionary_Allocate(&dict, 100); // Larger initial size for concurrent ops
    TEST_ASSERT(dict != NULL);

    VPS_ConcurrentDictionary_Construct(dict, hash_string, compare_string, release_string, release_int, 2, 75, 5);

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].dict = dict;
        thread_data[i].thread_id = i;
        pthread_create(&threads[i], NULL, concurrent_operations, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Verify some elements after concurrent operations
    void *found_value = NULL;
    for (int i = 0; i < NUM_THREADS; ++i) {
        for (int j = 0; j < NUM_OPERATIONS_PER_THREAD; ++j) {
            if (j % 2 != 0) { // Only check keys that were NOT removed
                char key_buffer[32];
                sprintf(key_buffer, "key_%d_%d", i, j);
                if (VPS_ConcurrentDictionary_Find(dict, key_buffer, &found_value)) {
                    TEST_ASSERT(*((int*)found_value) == i * 1000 + j);
                } else {
                    fprintf(stderr, "Failed to find key %s after concurrent ops (expected to be present)\n", key_buffer);
                    TEST_ASSERT(0); // This will mark the test as failed
                }
            }
        }
    }

    VPS_ConcurrentDictionary_Release(dict);

    return 1;
}


void test_suite_VPS_ConcurrentDictionary() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_concurrent_dictionary_basic);
    RUN_TEST(test_concurrent_dictionary_multithreaded);

    printf("  ----------------------------------\n");
    printf("  VPS_ConcurrentDictionary Summary: %d passed, %d failed\n", success_count, failure_count);
}
