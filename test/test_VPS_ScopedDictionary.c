#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_ScopedDictionary.h>
#include <vulpes/VPS_Hash_Utils.h>
#include <vulpes/VPS_Compare_Utils.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

static int g_scoped_key_release_count = 0;
static int g_scoped_data_release_count = 0;

static char _test_scoped_dict_key_releaser(void *key) {
    g_scoped_key_release_count++;
    free(key);
    return 1;
}

static char _test_scoped_dict_data_releaser(void *data) {
    // The data can be NULL for logical removes, so we must check.
    if (data) {
        g_scoped_data_release_count++;
        free(data);
    }
    return 1;
}

/**
 * @test test_scoped_dict_scoping
 * @brief Verifies the core scoping, shadowing, and value reversion logic.
 */
static char test_scoped_dict_scoping() {
    struct VPS_ScopedDictionary *dict = 0;
    void *found_data = 0;

    VPS_ScopedDictionary_Allocate(&dict, 17);
    VPS_ScopedDictionary_Construct(dict,
        VPS_Hash_Utils_String,
        VPS_Compare_Utils_String,
        0, // No data compare needed
        _test_scoped_dict_key_releaser,
        _test_scoped_dict_data_releaser,
        0, 0, 0);

    // --- Root Scope ---
    char *key_a = strdup("key_a");
    int *data_a1 = malloc(sizeof(int)); *data_a1 = 100;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, key_a, data_a1));
    TEST_ASSERT(dict->total_entries == 1);

    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 100);

    // --- Enter Child Scope ---
    TEST_ASSERT(VPS_ScopedDictionary_EnterScope(dict));

    // Value from parent scope should still be visible
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 100);

    // Add a new key in the child scope
    char *key_b = strdup("key_b");
    int *data_b = malloc(sizeof(int)); *data_b = 200;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, key_b, data_b));
    TEST_ASSERT(dict->total_entries == 2);

    // Shadow the parent's key
    int *data_a2 = malloc(sizeof(int)); *data_a2 = 101;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, "key_a", data_a2));
    TEST_ASSERT(dict->total_entries == 2); // Total entries should not change

    // Verify the new, shadowed value is found
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 101);

    // --- Leave Child Scope ---
    g_scoped_key_release_count = 0;
    g_scoped_data_release_count = 0;
    TEST_ASSERT(VPS_ScopedDictionary_LeaveScope(dict));

    // Verify that the shadowed value and the child-scope key were released.
    // key_b is released, data_b is released.
    // The shadowed data_a2 is released. The original key_a is NOT released.
    TEST_ASSERT(g_scoped_key_release_count == 1);
    TEST_ASSERT(g_scoped_data_release_count == 2);
    TEST_ASSERT(dict->total_entries == 1);

    // Verify that the original value for key_a is restored
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 100);

    // Verify that the child-scope key is no longer findable
    TEST_ASSERT(!VPS_ScopedDictionary_Find(dict, "key_b", &found_data));

    // --- Deconstruct ---
    // This should clean up the final root-scope items.
    g_scoped_key_release_count = 0;
    g_scoped_data_release_count = 0;
    VPS_ScopedDictionary_Deconstruct(dict);
    TEST_ASSERT(g_scoped_key_release_count == 1);
    TEST_ASSERT(g_scoped_data_release_count == 1);

    VPS_ScopedDictionary_Release(dict);
    return 1;
}

/**
 * @test test_scoped_dict_remove
 * @brief Verifies the logical remove functionality.
 */
static char test_scoped_dict_remove() {
    struct VPS_ScopedDictionary *dict = 0;
    void *found_data = (void*)1; // Non-null start value

    VPS_ScopedDictionary_Allocate(&dict, 17);
    VPS_ScopedDictionary_Construct(dict, VPS_Hash_Utils_String, VPS_Compare_Utils_String, 0, 0, 0, 0, 0, 0);

    // Add a value in the root scope
    VPS_ScopedDictionary_Add(dict, "key_c", (void*)300);

    // Enter a new scope and logically remove the key
    VPS_ScopedDictionary_EnterScope(dict);
    TEST_ASSERT(VPS_ScopedDictionary_Remove(dict, "key_c"));

    // Find should succeed, but the data pointer should be NULL
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_c", &found_data));
    TEST_ASSERT(found_data == 0);

    // Leave the scope
    VPS_ScopedDictionary_LeaveScope(dict);

    // The original value should be visible again
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_c", &found_data));
    TEST_ASSERT((intptr_t)found_data == 300);

    VPS_ScopedDictionary_Release(dict);
    return 1;
}

/**
 * @test test_scoped_dict_multi_level
 * @brief Verifies the dictionary's behavior across multiple nested scopes,
 *        including shadowing keys at different levels and ensuring correct
 *        state reversion and memory management.
 */
static char test_scoped_dict_multi_level() {
    struct VPS_ScopedDictionary *dict = 0;
    void *found_data = 0;

    VPS_ScopedDictionary_Allocate(&dict, 17);
    VPS_ScopedDictionary_Construct(dict,
        VPS_Hash_Utils_String,
        VPS_Compare_Utils_String,
        0,
        _test_scoped_dict_key_releaser,
        _test_scoped_dict_data_releaser,
        0, 0, 0);

    // --- Scope 0 (Root) ---
    char *key_a = strdup("key_a");
    int *data_a0 = malloc(sizeof(int)); *data_a0 = 100;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, key_a, data_a0));
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 100);

    // --- Enter Scope 1 ---
    TEST_ASSERT(VPS_ScopedDictionary_EnterScope(dict));
    char *key_b = strdup("key_b");
    int *data_b1 = malloc(sizeof(int)); *data_b1 = 200;
    int *data_a1 = malloc(sizeof(int)); *data_a1 = 101;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, key_b, data_b1));    // New key in scope 1
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, "key_a", data_a1));   // Shadow key_a in scope 1

    // Verify values in scope 1
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 101);
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_b", &found_data));
    TEST_ASSERT(*(int*)found_data == 200);

    // --- Enter Scope 2 ---
    TEST_ASSERT(VPS_ScopedDictionary_EnterScope(dict));
    char *key_c = strdup("key_c");
    int *data_c2 = malloc(sizeof(int)); *data_c2 = 300;
    int *data_b2 = malloc(sizeof(int)); *data_b2 = 202;
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, key_c, data_c2));    // New key in scope 2
    TEST_ASSERT(VPS_ScopedDictionary_Add(dict, "key_b", data_b2));   // Shadow key_b in scope 2

    // Verify values in scope 2
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 101); // Should see scope 1's value
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_b", &found_data));
    TEST_ASSERT(*(int*)found_data == 202); // Should see scope 2's value
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_c", &found_data));
    TEST_ASSERT(*(int*)found_data == 300); // Should see scope 2's value

    // --- Leave Scope 2 ---
    g_scoped_key_release_count = 0;
    g_scoped_data_release_count = 0;
    TEST_ASSERT(VPS_ScopedDictionary_LeaveScope(dict));
    // key_c and its data are released. data_b2 is released.
    TEST_ASSERT(g_scoped_key_release_count == 1);
    TEST_ASSERT(g_scoped_data_release_count == 2);

    // Verify state has reverted to Scope 1
    TEST_ASSERT(!VPS_ScopedDictionary_Find(dict, "key_c", &found_data));
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_b", &found_data));
    TEST_ASSERT(*(int*)found_data == 200); // Reverted to scope 1's value
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 101); // Still scope 1's value

    // --- Leave Scope 1 ---
    g_scoped_key_release_count = 0;
    g_scoped_data_release_count = 0;
    TEST_ASSERT(VPS_ScopedDictionary_LeaveScope(dict));
    // key_b and data_b1 are released. data_a1 is released.
    TEST_ASSERT(g_scoped_key_release_count == 1);
    TEST_ASSERT(g_scoped_data_release_count == 2);

    // Verify state has reverted to Root Scope
    TEST_ASSERT(!VPS_ScopedDictionary_Find(dict, "key_b", &found_data));
    TEST_ASSERT(VPS_ScopedDictionary_Find(dict, "key_a", &found_data));
    TEST_ASSERT(*(int*)found_data == 100); // Reverted to original root value

    VPS_ScopedDictionary_Release(dict);
    return 1;
}

void test_suite_VPS_ScopedDictionary() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_scoped_dict_scoping);
    RUN_TEST(test_scoped_dict_remove);
    RUN_TEST(test_scoped_dict_multi_level);

    printf("  ----------------------------------\n");
    printf("  VPS_ScopedDictionary Summary: %d passed, %d failed\n", success_count, failure_count);
}