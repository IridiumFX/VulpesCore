#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Set.h>
#include <vulpes/VPS_Hash_Utils.h>
#include <vulpes/VPS_Compare_Utils.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

static int g_item_release_count = 0;

// Simple releaser for string items used in tests
static char _test_set_item_releaser(void *item) {
    g_item_release_count++;
    free(item);
    return 1;
}

/**
 * @test test_set_add_contains_remove
 * @brief Verifies the core Add, Contains, and Remove functionality.
 */
static char test_set_add_contains_remove() {
    struct VPS_Set *set = 0;
    char *item1 = strdup("item1");
    char *item2 = strdup("item2");

    // 1. Allocate and Construct
    TEST_ASSERT(VPS_Set_Allocate(&set, 17));
    TEST_ASSERT(set != 0);
    TEST_ASSERT(VPS_Set_Construct(set,
        VPS_Hash_Utils_String,
        VPS_Compare_Utils_String,
        _test_set_item_releaser,
        0, 0, 0));

    // 2. Add initial items
    TEST_ASSERT(VPS_Set_Add(set, item1));
    TEST_ASSERT(VPS_Set_Add(set, item2));
    TEST_ASSERT(set->total_entries == 2);

    // 3. Check for existence
    TEST_ASSERT(VPS_Set_Contains(set, "item1"));
    TEST_ASSERT(VPS_Set_Contains(set, "item2"));
    TEST_ASSERT(!VPS_Set_Contains(set, "non-existent-item"));

    // 4. Add a duplicate item - should be a no-op
    char *item1_dup = strdup("item1"); // Use a different pointer to the same string content
    TEST_ASSERT(VPS_Set_Add(set, item1_dup));
    TEST_ASSERT(set->total_entries == 2); // Count should not change
    // The set should not take ownership of the duplicate, so we must free it.
    free(item1_dup);

    // 5. Remove an item
    g_item_release_count = 0;
    TEST_ASSERT(VPS_Set_Remove(set, "item1"));
    TEST_ASSERT(set->total_entries == 1);
    TEST_ASSERT(g_item_release_count == 1); // Item should be released

    // Verify it's gone
    TEST_ASSERT(!VPS_Set_Contains(set, "item1"));

    // 6. Deconstruct should clean up the remaining item (item2)
    g_item_release_count = 0;
    VPS_Set_Deconstruct(set);
    TEST_ASSERT(g_item_release_count == 1);

    VPS_Set_Release(set);
    return 1;
}

/**
 * @test test_set_rehash
 * @brief Verifies that the set correctly rehashes when it gets too full.
 */
static char test_set_rehash() {
    struct VPS_Set *set = 0;
    // Use a small initial size and high load factor to force a rehash
    VPS_Set_Allocate(&set, 5);
    // No item releaser needed as we are using string literals
    VPS_Set_Construct(set, VPS_Hash_Utils_String, VPS_Compare_Utils_String, 0, 2, 50, 8);

    TEST_ASSERT(set->buckets == 5);

    // Add items to trigger rehash (3 items is > 50% of 5 buckets)
    VPS_Set_Add(set, "item1");
    VPS_Set_Add(set, "item2");
    VPS_Set_Add(set, "item3"); // Rehash should happen here

    TEST_ASSERT(set->buckets > 5); // Buckets should have grown
    TEST_ASSERT(set->total_entries == 3);

    // Verify all items are still findable after rehash
    TEST_ASSERT(VPS_Set_Contains(set, "item1"));
    TEST_ASSERT(VPS_Set_Contains(set, "item2"));
    TEST_ASSERT(VPS_Set_Contains(set, "item3"));

    VPS_Set_Release(set);
    return 1;
}

void test_suite_VPS_Set() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_set_add_contains_remove);
    RUN_TEST(test_set_rehash);

    printf("  ----------------------------------\n");
    printf("  VPS_Set Summary: %d passed, %d failed\n", success_count, failure_count);
}