#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_SwissDictionary.h>
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

/**
 * @test test_swiss_dictionary_basic
 * @brief Tests basic add, find, remove operations on the swiss dictionary.
 */
static char test_swiss_dictionary_basic()
{
    struct VPS_SwissDictionary *dict = 0;
    char *key1 = strdup("hello");
    char *key2 = strdup("world");
    int *val1 = (int*)malloc(sizeof(int)); *val1 = 100;
    int *val2 = (int*)malloc(sizeof(int)); *val2 = 200;
    void *found_val = NULL;

    VPS_SwissDictionary_Allocate(&dict, 16);
    TEST_ASSERT(dict != NULL);

    VPS_SwissDictionary_Construct(dict, hash_string, compare_string, release_string, release_int, 2, 75);

    // Add
    TEST_ASSERT(VPS_SwissDictionary_Add(dict, key1, val1));
    TEST_ASSERT(VPS_SwissDictionary_Add(dict, key2, val2));

    // Find
    TEST_ASSERT(VPS_SwissDictionary_Find(dict, "hello", &found_val));
    TEST_ASSERT(*((int*)found_val) == 100);

    TEST_ASSERT(VPS_SwissDictionary_Find(dict, "world", &found_val));
    TEST_ASSERT(*((int*)found_val) == 200);

    // Try to find non-existent key
    TEST_ASSERT(!VPS_SwissDictionary_Find(dict, "nonexistent", &found_val));

    // Remove
    TEST_ASSERT(VPS_SwissDictionary_Remove(dict, "hello"));
    TEST_ASSERT(!VPS_SwissDictionary_Find(dict, "hello", &found_val)); // Should not be found after removal

    // Add again
    char *key3 = strdup("newkey");
    int *val3 = (int*)malloc(sizeof(int)); *val3 = 300;
    TEST_ASSERT(VPS_SwissDictionary_Add(dict, key3, val3));
    TEST_ASSERT(VPS_SwissDictionary_Find(dict, "newkey", &found_val));
    TEST_ASSERT(*((int*)found_val) == 300);

    VPS_SwissDictionary_Release(dict);

    return 1;
}

/**
 * @test test_swiss_dictionary_resize
 * @brief Tests the resize functionality of the swiss dictionary.
 */
static char test_swiss_dictionary_resize()
{
    struct VPS_SwissDictionary *dict = 0;

    // Use a small initial capacity to trigger resize quickly
    VPS_SwissDictionary_Allocate(&dict, 4);
    TEST_ASSERT(dict != NULL);

    VPS_SwissDictionary_Construct(dict, hash_string, compare_string, release_string, release_int, 2, 75);

    // Add items to trigger a resize (load factor > 75%)
    for (int i = 0; i < 10; ++i) {
        char *key = (char *)malloc(16);
        sprintf(key, "key%d", i);
        int *val = (int *)malloc(sizeof(int));
        *val = i;
        TEST_ASSERT(VPS_SwissDictionary_Add(dict, key, val));
    }

    // Verify all items are still present after resizing
    for (int i = 0; i < 10; ++i) {
        char key_buffer[16];
        sprintf(key_buffer, "key%d", i);
        void *found_val = NULL;
        TEST_ASSERT(VPS_SwissDictionary_Find(dict, key_buffer, &found_val));
        TEST_ASSERT(*((int*)found_val) == i);
    }

    VPS_SwissDictionary_Release(dict);

    return 1;
}


void test_suite_VPS_SwissDictionary() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_swiss_dictionary_basic);
    RUN_TEST(test_swiss_dictionary_resize);

    printf("  ----------------------------------\n");
    printf("  VPS_SwissDictionary Summary: %d passed, %d failed\n", success_count, failure_count);
}
