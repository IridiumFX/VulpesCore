#include <stdio.h>
#include <string.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Endian.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_endian_read_write
 * @brief Tests reading and writing of 16, 32, and 64-bit integers in both Big and Little Endian formats.
 */
static char test_endian_read_write()
{
    unsigned char buffer[16];

    // --- 16-bit ---
    VPS_TYPE_16U val16 = 0x1234;

    // Big Endian Write
    VPS_Endian_Write16UBE(buffer, val16);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read16UBE(buffer) == val16);

    // Little Endian Write
    VPS_Endian_Write16ULE(buffer, val16);
    TEST_ASSERT(buffer[0] == 0x34);
    TEST_ASSERT(buffer[1] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read16ULE(buffer) == val16);

    // --- 32-bit ---
    VPS_TYPE_32U val32 = 0x12345678;

    // Big Endian Write
    VPS_Endian_Write32UBE(buffer, val32);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);
    TEST_ASSERT(buffer[2] == 0x56);
    TEST_ASSERT(buffer[3] == 0x78);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read32UBE(buffer) == val32);

    // Little Endian Write
    VPS_Endian_Write32ULE(buffer, val32);
    TEST_ASSERT(buffer[0] == 0x78);
    TEST_ASSERT(buffer[1] == 0x56);
    TEST_ASSERT(buffer[2] == 0x34);
    TEST_ASSERT(buffer[3] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read32ULE(buffer) == val32);

    // --- 64-bit ---
    VPS_TYPE_64U val64 = 0x123456789ABCDEF0ULL;

    // Big Endian Write
    VPS_Endian_Write64UBE(buffer, val64);
    TEST_ASSERT(buffer[0] == 0x12);
    TEST_ASSERT(buffer[1] == 0x34);
    TEST_ASSERT(buffer[2] == 0x56);
    TEST_ASSERT(buffer[3] == 0x78);
    TEST_ASSERT(buffer[4] == 0x9A);
    TEST_ASSERT(buffer[5] == 0xBC);
    TEST_ASSERT(buffer[6] == 0xDE);
    TEST_ASSERT(buffer[7] == 0xF0);

    // Big Endian Read
    TEST_ASSERT(VPS_Endian_Read64UBE(buffer) == val64);

    // Little Endian Write
    VPS_Endian_Write64ULE(buffer, val64);
    TEST_ASSERT(buffer[0] == 0xF0);
    TEST_ASSERT(buffer[1] == 0xDE);
    TEST_ASSERT(buffer[2] == 0xBC);
    TEST_ASSERT(buffer[3] == 0x9A);
    TEST_ASSERT(buffer[4] == 0x78);
    TEST_ASSERT(buffer[5] == 0x56);
    TEST_ASSERT(buffer[6] == 0x34);
    TEST_ASSERT(buffer[7] == 0x12);

    // Little Endian Read
    TEST_ASSERT(VPS_Endian_Read64ULE(buffer) == val64);

    return 1;
}

void test_suite_VPS_Endian() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_endian_read_write);

    printf("  ----------------------------------\n");
    printf("  VPS_Endian Summary: %d passed, %d failed\n", success_count, failure_count);
}
