#include <stdio.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_DataReader.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_datareader_reads
 * @brief Verifies that the DataReader can correctly parse multi-byte integers
 *        with different endianness from a source buffer.
 */
static char test_datareader_reads() {
    struct VPS_Data *source_data = 0;
    struct VPS_DataReader *reader = 0;

    // Prepare a source buffer with known byte patterns
    unsigned char buffer[] = {
        0x12, 0x34, // 16-bit Big Endian
        0x78, 0x56, // 16-bit Little Endian
        0x1A, 0x2B, 0x3C, 0x4D, // 32-bit Big Endian
        0xBC, 0x9A, 0x78, 0x56  // 32-bit Little Endian
    };

    VPS_Data_Allocate(&source_data, 0, 0);
    VPS_Data_Wrap(source_data, buffer, sizeof(buffer));

    VPS_DataReader_Allocate(&reader);
    VPS_DataReader_Construct(reader, source_data);

    // --- Perform Reads and Assertions ---
    VPS_TYPE_16U val16_be, val16_le;
    VPS_TYPE_32U val32_be, val32_le;

    // Read 16-bit BE (0x1234)
    TEST_ASSERT(VPS_DataReader_Read16UBE(reader, &val16_be));
    TEST_ASSERT(val16_be == 0x1234);
    TEST_ASSERT(source_data->position == 2);

    // Read 16-bit LE (0x5678)
    TEST_ASSERT(VPS_DataReader_Read16ULE(reader, &val16_le));
    TEST_ASSERT(val16_le == 0x5678);
    TEST_ASSERT(source_data->position == 4);

    // Read 32-bit BE (0x1A2B3C4D)
    TEST_ASSERT(VPS_DataReader_Read32UBE(reader, &val32_be));
    TEST_ASSERT(val32_be == 0x1A2B3C4D);
    TEST_ASSERT(source_data->position == 8);

    // Read 32-bit LE (0x56789ABC)
    TEST_ASSERT(VPS_DataReader_Read32ULE(reader, &val32_le));
    TEST_ASSERT(val32_le == 0x56789ABC);
    TEST_ASSERT(source_data->position == 12);

    // Check remaining bytes
    VPS_TYPE_SIZE remaining;
    VPS_DataReader_Remaining(reader, &remaining);
    TEST_ASSERT(remaining == 0);

    // --- Cleanup ---
    VPS_DataReader_Release(reader);
    VPS_Data_Release(source_data);

    return 1;
}

void test_suite_VPS_DataReader() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_datareader_reads);

    printf("  ----------------------------------\n");
    printf("  VPS_DataReader Summary: %d passed, %d failed\n", success_count, failure_count);
}