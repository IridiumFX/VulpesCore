#include <stdio.h>
#include <stdlib.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_List.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

static int g_release_counter = 0;
static char _test_releaser(void *data) {
    if (data) {
        g_release_counter++;
        free(data);
    }
    return 1;
}

/**
 * @test test_list_lifecycle
 * @brief Verifies the basic Allocate, Construct, Deconstruct, and Release cycle.
 */
static char test_list_lifecycle() {
    struct VPS_List *list = 0;
    TEST_ASSERT(VPS_List_Allocate(&list));
    TEST_ASSERT(list != 0);

    TEST_ASSERT(VPS_List_Construct(list, 0, 0, 0));
    TEST_ASSERT(list->head == 0);
    TEST_ASSERT(list->tail == 0);
    TEST_ASSERT(list->count == 0);

    TEST_ASSERT(VPS_List_Deconstruct(list));
    VPS_List_Release(list);
    return 1;
}

/**
 * @test test_list_add_remove
 * @brief Verifies adding and removing nodes from the head and tail.
 */
static char test_list_add_remove() {
    struct VPS_List *list = 0;
    struct VPS_List_Node *n1 = 0, *n2 = 0, *n3 = 0, *removed = 0;

    VPS_List_Allocate(&list);
    VPS_List_Construct(list, 0, 0, 0);

    VPS_List_Node_Allocate(&n1);
    VPS_List_Node_Allocate(&n2);
    VPS_List_Node_Allocate(&n3);

    VPS_List_Node_Construct(n1, (void*)1);
    VPS_List_Node_Construct(n2, (void*)2);
    VPS_List_Node_Construct(n3, (void*)3);

    // Add 2, 1, 3 -> List should be [1, 2, 3]
    VPS_List_AddTail(list, n2);
    VPS_List_AddHead(list, n1);
    VPS_List_AddTail(list, n3);

    TEST_ASSERT(list->count == 3);
    TEST_ASSERT(list->head == n1);
    TEST_ASSERT(list->tail == n3);
    TEST_ASSERT(list->head->next == n2);
    TEST_ASSERT(list->tail->back == n2);

    // Remove head (1) -> List should be [2, 3]
    TEST_ASSERT(VPS_List_RemoveHead(list, &removed));
    TEST_ASSERT(removed == n1);
    TEST_ASSERT(list->count == 2);
    TEST_ASSERT(list->head == n2);

    // Remove tail (3) -> List should be [2]
    TEST_ASSERT(VPS_List_RemoveTail(list, &removed));
    TEST_ASSERT(removed == n3);
    TEST_ASSERT(list->count == 1);
    TEST_ASSERT(list->head == n2);
    TEST_ASSERT(list->tail == n2);

    // Remove last item (2) -> List should be empty
    TEST_ASSERT(VPS_List_RemoveHead(list, &removed));
    TEST_ASSERT(removed == n2);
    TEST_ASSERT(list->count == 0);
    TEST_ASSERT(list->head == 0);
    TEST_ASSERT(list->tail == 0);

    VPS_List_Node_Release(n1);
    VPS_List_Node_Release(n2);
    VPS_List_Node_Release(n3);
    VPS_List_Release(list);
    return 1;
}

/**
 * @test test_list_clear_with_releaser
 * @brief Verifies that VPS_List_Clear correctly invokes the node_data_release callback.
 */
static char test_list_clear_with_releaser() {
    struct VPS_List *list = 0;
    struct VPS_List_Node *n1 = 0, *n2 = 0;

    VPS_List_Allocate(&list);
    // Construct the list with our test releaser function
    VPS_List_Construct(list, 0, 0, _test_releaser);

    VPS_List_Node_Allocate(&n1);
    VPS_List_Node_Allocate(&n2);

    // Allocate some dummy data on the heap
    VPS_List_Node_Construct(n1, malloc(1));
    VPS_List_Node_Construct(n2, malloc(1));

    VPS_List_AddTail(list, n1);
    VPS_List_AddTail(list, n2);

    g_release_counter = 0; // Reset global counter
    TEST_ASSERT(list->count == 2);

    // Clear the list. This should trigger the releaser for each node's data.
    VPS_List_Clear(list);

    TEST_ASSERT(list->count == 0);
    TEST_ASSERT(list->head == 0);
    TEST_ASSERT(g_release_counter == 2); // Verify the releaser was called twice

    VPS_List_Release(list);
    return 1;
}

void test_suite_VPS_List() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_list_lifecycle);
    RUN_TEST(test_list_add_remove);
    RUN_TEST(test_list_clear_with_releaser);

    printf("  ----------------------------------\n");
    printf("  VPS_List Summary: %d passed, %d failed\n", success_count, failure_count);
}