#include <stdio.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Endian.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_endian_read_write_16
 * @brief Verifies 16-bit endian read/write operations.
 */
static int test_endian_read_write_16()
{
    unsigned char buffer[2];
    VPS_TYPE_16U value = 0x1234;

    // Big Endian Write
    VPS_Endian_Write16UBE(buffer, value);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read16UBE(buffer) == value);

    // Little Endian Write
    VPS_Endian_Write16ULE(buffer, value);
    TEST_ASSERT(buffer[0] == 0x34);
    TEST_ASSERT(buffer[1] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read16ULE(buffer) == value);

    return 1;
}

/**
 * @test test_endian_read_write_32
 * @brief Verifies 32-bit endian read/write operations.
 */
static int test_endian_read_write_32()
{
    unsigned char buffer[4];
    VPS_TYPE_32U value = 0x12345678;

    // Big Endian Write
    VPS_Endian_Write32UBE(buffer, value);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);
    TEST_ASSERT(buffer[2] == 0x56);
    TEST_ASSERT(buffer[3] == 0x78);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read32UBE(buffer) == value);

    // Little Endian Write
    VPS_Endian_Write32ULE(buffer, value);
    TEST_ASSERT(buffer[0] == 0x78);
    TEST_ASSERT(buffer[1] == 0x56);
    TEST_ASSERT(buffer[2] == 0x34);
    TEST_ASSERT(buffer[3] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read32ULE(buffer) == value);

    return 1;
}

/**
 * @test test_endian_read_write_64
 * @brief Verifies 64-bit endian read/write operations.
 */
static int test_endian_read_write_64()
{
    unsigned char buffer[8];
    VPS_TYPE_64U value = 0x123456789ABCDEF0ULL;

    // Big Endian Write
    VPS_Endian_Write64UBE(buffer, value);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);
    TEST_ASSERT(buffer[2] == 0x56);
    TEST_ASSERT(buffer[3] == 0x78);
    TEST_ASSERT(buffer[4] == 0x9A);
    TEST_ASSERT(buffer[5] == 0xBC);
    TEST_ASSERT(buffer[6] == 0xDE);
    TEST_ASSERT(buffer[7] == 0xF0);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read64UBE(buffer) == value);

    // Little Endian Write
    VPS_Endian_Write64ULE(buffer, value);
    TEST_ASSERT(buffer[0] == 0xF0);
    TEST_ASSERT(buffer[1] == 0xDE);
    TEST_ASSERT(buffer[2] == 0xBC);
    TEST_ASSERT(buffer[3] == 0x9A);
    TEST_ASSERT(buffer[4] == 0x78);
    TEST_ASSERT(buffer[5] == 0x56);
    TEST_ASSERT(buffer[6] == 0x34);
    TEST_ASSERT(buffer[7] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read64ULE(buffer) == value);

    return 1;
}

void test_suite_VPS_Endian() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_endian_read_write_16);
    RUN_TEST(test_endian_read_write_32);
    RUN_TEST(test_endian_read_write_64);

    printf("  ----------------------------------\n");
    printf("  VPS_Endian Summary: %d passed, %d failed\n", success_count, failure_count);
}
