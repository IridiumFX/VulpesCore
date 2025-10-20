#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Dictionary.h>
#include <vulpes/VPS_Hash_Utils.h>
#include <vulpes/VPS_Compare_Utils.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

static int g_key_release_count = 0;
static int g_data_release_count = 0;

static char _test_dict_key_releaser(void *key) {
    g_key_release_count++;
    free(key);
    return 1;
}

static char _test_dict_data_releaser(void *data) {
    g_data_release_count++;
    free(data);
    return 1;
}

/**
 * @test test_dict_add_find_remove
 * @brief Verifies the core Add, Find, and Remove functionality.
 */
static char test_dict_add_find_remove() {
    struct VPS_Dictionary *dict = 0;
    char *key1 = strdup("key1");
    char *key2 = strdup("key2");
    int *data1 = malloc(sizeof(int));
    int *data2 = malloc(sizeof(int));
    *data1 = 100;
    *data2 = 200;

    VPS_Dictionary_Allocate(&dict, 17);
    VPS_Dictionary_Construct(dict,
        VPS_Hash_Utils_String,
        VPS_Compare_Utils_String,
        _test_dict_key_releaser,
        _test_dict_data_releaser,
        0, 0, 0);

    // 1. Add initial items
    TEST_ASSERT(VPS_Dictionary_Add(dict, key1, data1));
    TEST_ASSERT(VPS_Dictionary_Add(dict, key2, data2));
    TEST_ASSERT(dict->total_entries == 2);

    // 2. Find items
    void *found_data = 0;
    TEST_ASSERT(VPS_Dictionary_Find(dict, "key1", &found_data));
    TEST_ASSERT(found_data != 0);
    TEST_ASSERT(*(int*)found_data == 100);

    TEST_ASSERT(VPS_Dictionary_Find(dict, "key2", &found_data));
    TEST_ASSERT(*(int*)found_data == 200);

    // 3. Update an item
    int *data1_updated = malloc(sizeof(int));
    *data1_updated = 101;
    g_data_release_count = 0;
    TEST_ASSERT(VPS_Dictionary_Add(dict, "key1", data1_updated));
    TEST_ASSERT(dict->total_entries == 2); // Count should not change
    TEST_ASSERT(g_data_release_count == 1); // Old data should have been released

    TEST_ASSERT(VPS_Dictionary_Find(dict, "key1", &found_data));
    TEST_ASSERT(*(int*)found_data == 101);

    // 4. Remove an item
    g_key_release_count = 0;
    g_data_release_count = 0;
    TEST_ASSERT(VPS_Dictionary_Remove(dict, "key1"));
    TEST_ASSERT(dict->total_entries == 1);
    TEST_ASSERT(g_key_release_count == 1); // Key should be released
    TEST_ASSERT(g_data_release_count == 1); // Data should be released

    // Verify it's gone
    TEST_ASSERT(!VPS_Dictionary_Find(dict, "key1", &found_data));

    // 5. Deconstruct should clean up the remaining item (key2)
    g_key_release_count = 0;
    g_data_release_count = 0;
    VPS_Dictionary_Deconstruct(dict);
    TEST_ASSERT(g_key_release_count == 1);
    TEST_ASSERT(g_data_release_count == 1);

    VPS_Dictionary_Release(dict);
    return 1;
}

/**
 * @test test_dict_rehash
 * @brief Verifies that the dictionary correctly rehashes when it gets too full.
 */
static char test_dict_rehash() {
    struct VPS_Dictionary *dict = 0;
    // Use a small initial size and high load factor to force a rehash
    VPS_Dictionary_Allocate(&dict, 5);
    VPS_Dictionary_Construct(dict, VPS_Hash_Utils_String, VPS_Compare_Utils_String, 0, 0, 2, 50, 8);

    TEST_ASSERT(dict->buckets == 5);

    // Add items to trigger rehash (3 items is > 50% of 5 buckets)
    VPS_Dictionary_Add(dict, "key1", (void*)1);
    VPS_Dictionary_Add(dict, "key2", (void*)2);
    VPS_Dictionary_Add(dict, "key3", (void*)3); // Rehash should happen here

    TEST_ASSERT(dict->buckets > 5); // Buckets should have grown
    TEST_ASSERT(dict->total_entries == 3);

    // Verify all items are still findable after rehash
    void* data = 0;
    TEST_ASSERT(VPS_Dictionary_Find(dict, "key1", &data) && (VPS_TYPE_SIZE)data == 1);
    TEST_ASSERT(VPS_Dictionary_Find(dict, "key2", &data) && (VPS_TYPE_SIZE)data == 2);
    TEST_ASSERT(VPS_Dictionary_Find(dict, "key3", &data) && (VPS_TYPE_SIZE)data == 3);

    VPS_Dictionary_Release(dict);
    return 1;
}

void test_suite_VPS_Dictionary() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_dict_add_find_remove);
    RUN_TEST(test_dict_rehash);

    printf("  ----------------------------------\n");
    printf("  VPS_Dictionary Summary: %d passed, %d failed\n", success_count, failure_count);
}