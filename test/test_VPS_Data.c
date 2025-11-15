#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_data_lifecycle
 * @brief Verifies the basic Allocate, Construct, Deconstruct, and Release cycle.
 */
static char test_data_lifecycle() {
    struct VPS_Data *data = 0;

    TEST_ASSERT(VPS_Data_Allocate(&data, 128, 0));
    TEST_ASSERT(data != 0);

    TEST_ASSERT(VPS_Data_Construct(data));
    TEST_ASSERT(data->bytes != 0);
    TEST_ASSERT(data->size == 128);
    TEST_ASSERT(data->limit == 0);
    TEST_ASSERT(data->own_bytes == 1);

    TEST_ASSERT(VPS_Data_Deconstruct(data));

    VPS_Data_Release(data);
    return 1;
}

/**
 * @test test_data_wrap_unwrap
 * @brief Verifies that VPS_Data can correctly wrap and unwrap an external buffer
 *        without taking ownership of its memory.
 */
static char test_data_wrap_unwrap() {
    struct VPS_Data *data = 0;
    unsigned char my_buffer[32];
    unsigned char *unwrapped_buffer = 0;
    VPS_TYPE_SIZE unwrapped_size = 0;

    VPS_Data_Allocate(&data, 10, 0);

    // Wrap the external buffer
    TEST_ASSERT(VPS_Data_Wrap(data, my_buffer, 32));
    TEST_ASSERT(data->bytes == my_buffer);
    TEST_ASSERT(data->size == 32);
    TEST_ASSERT(data->limit == 32);
    TEST_ASSERT(data->own_bytes == 0); // Critical check

    TEST_ASSERT(VPS_Data_Deconstruct(data));

    // Re-wrap to test unwrap
    VPS_Data_Wrap(data, my_buffer, 32);
    TEST_ASSERT(VPS_Data_Unwrap(data, &unwrapped_buffer, &unwrapped_size));
    TEST_ASSERT(unwrapped_buffer == my_buffer);
    TEST_ASSERT(unwrapped_size == 32);
    TEST_ASSERT(data->bytes == 0); // Unwrap should clear the struct

    VPS_Data_Release(data);
    return 1;
}

/**
 * @test test_data_compact
 * @brief Verifies the "sliding window" behavior of the Compact function.
 */
static char test_data_compact() {
    struct VPS_Data *data = 0;
    VPS_Data_Allocate(&data, 16, 10);
    VPS_Data_Construct(data);

    // Write some data to simulate a partial read
    memcpy(data->bytes, "0123456789ABCDEF", 10);
    data->position = 4; // Pretend we've processed the first 4 bytes

    // Before compact: "0123[456789]ABCDEF" (position at '4', limit at 'A')
    TEST_ASSERT(data->bytes[0] == '0');
    TEST_ASSERT(data->position == 4);
    TEST_ASSERT(data->limit == 10);

    VPS_Data_Compact(data);

    // After compact: "[456789]......" (position at '4', limit at '6')
    // The unprocessed data should be at the start of the buffer.
    TEST_ASSERT(data->bytes[0] == '4');
    TEST_ASSERT(data->bytes[5] == '9');
    TEST_ASSERT(data->position == 0); // Position is reset
    TEST_ASSERT(data->limit == 6);    // Limit is adjusted

    VPS_Data_Release(data);
    return 1;
}

/**
 * @test test_data_copy_and_seek
 * @brief Verifies that data can be copied between buffers and that seeking works as expected.
 */
static char test_data_copy_and_seek() {
    struct VPS_Data *src = 0, *dest = 0;
    VPS_Data_Allocate(&src, 32, 16);
    VPS_Data_Allocate(&dest, 32, 0);

    // 1. Setup source and destination buffers
    VPS_Data_Construct(src); // size=32, limit=16
    VPS_Data_Construct(dest); // size=32, limit=0 (initially empty)
    memcpy(src->bytes, "0123456789ABCDEF", 16);

    // 2. Test Copy
    // Copy 4 bytes from source's position 8 ("89AB") to destination's position 0
    TEST_ASSERT(VPS_Data_Copy(src, dest, 8, 4, 0));

    // Verify destination state
    TEST_ASSERT(dest->limit == 4); // Limit should be updated to the end of the copied data
    TEST_ASSERT(strncmp((char*)dest->bytes, "89AB", 4) == 0);

    // Copy another 4 bytes from source's position 0 ("0123") to destination's position 4
    TEST_ASSERT(VPS_Data_Copy(src, dest, 0, 4, 4));

    // Verify destination state again
    TEST_ASSERT(dest->limit == 8); // Limit should be updated again
    TEST_ASSERT(strncmp((char*)dest->bytes + 4, "0123", 4) == 0);

    // 3. Test Seek
    // Seek to position 6 in the destination buffer
    TEST_ASSERT(VPS_Data_Seek(dest, 6, SEEK_SET));
    TEST_ASSERT(dest->position == 6);

    // Seek forward by 1
    TEST_ASSERT(VPS_Data_Seek(dest, 1, SEEK_CUR));
    TEST_ASSERT(dest->position == 7);

    // Seek to 3 bytes from the end
    TEST_ASSERT(VPS_Data_Seek(dest, (VPS_TYPE_SPAN)-3, SEEK_END));
    TEST_ASSERT(dest->position == 5);

    // Seek backward by 2
    TEST_ASSERT(VPS_Data_Seek(dest, (VPS_TYPE_SPAN)-2, SEEK_CUR));
    TEST_ASSERT(dest->position == 3);

    // 4. Test Boundary and Invalid Seeks
    // Seek to the very end (the limit)
    TEST_ASSERT(VPS_Data_Seek(dest, 0, SEEK_END));
    TEST_ASSERT(dest->position == 8);

    // Seek to the very beginning
    TEST_ASSERT(VPS_Data_Seek(dest, (VPS_TYPE_SPAN)-8, SEEK_CUR));
    TEST_ASSERT(dest->position == 0);

    // Test invalid seek (past limit from current position)
    TEST_ASSERT(!VPS_Data_Seek(dest, 9, SEEK_CUR));
    TEST_ASSERT(dest->position == 0); // Position should be unchanged

    // Test invalid seek (past limit from start)
    TEST_ASSERT(!VPS_Data_Seek(dest, 9, SEEK_SET));
    TEST_ASSERT(dest->position == 0); // Position should be unchanged

    // Test invalid seek (before start from current position)
    TEST_ASSERT(!VPS_Data_Seek(dest, (VPS_TYPE_SPAN)-1, SEEK_CUR));
    TEST_ASSERT(dest->position == 0); // Position should be unchanged

    VPS_Data_Release(src);
    VPS_Data_Release(dest);
    return 1;
}

/**
 * @test test_data_resize
 * @brief Verifies that resizing a buffer works correctly and adjusts pointers.
 */
static char test_data_resize() {
    struct VPS_Data *data = 0;
    VPS_Data_Allocate(&data, 10, 10);
    VPS_Data_Construct(data);
    data->position = 8;

    // Shrink the buffer. The limit and position should be adjusted.
    TEST_ASSERT(VPS_Data_Resize(data, 5));
    TEST_ASSERT(data->size == 5);
    TEST_ASSERT(data->limit == 5); // Limit is capped at new size
    TEST_ASSERT(data->position == 5); // Position is capped at new limit

    // Expand the buffer
    TEST_ASSERT(VPS_Data_Expand(data, 15));
    TEST_ASSERT(data->size == 20);
    TEST_ASSERT(data->limit == 5); // Limit and position should be unchanged by expansion
    TEST_ASSERT(data->position == 5);

    VPS_Data_Release(data);
    return 1;
}

void test_suite_VPS_Data() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_data_lifecycle);
    RUN_TEST(test_data_wrap_unwrap);
    RUN_TEST(test_data_compact);
    RUN_TEST(test_data_copy_and_seek);
    RUN_TEST(test_data_resize);

    printf("  ----------------------------------\n");
    printf("  VPS_Data Summary: %d passed, %d failed\n", success_count, failure_count);
}