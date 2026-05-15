#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_StreamWriter.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_streamwriter_pipeline
 * @brief Tests the StreamWriter by writing data to a file and verifying the content.
 */
static char test_streamwriter_pipeline()
{
    const char* test_filename = "stream_write_test.tmp";
    const char* test_data_1 = "Hello, ";
    const char* test_data_2 = "World!";
    int fh = -1;

    // 1. --- Create a temporary test file ---
    fh = open(test_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    TEST_ASSERT(fh != -1);

    struct VPS_StreamWriter *stream_writer = 0;

    VPS_StreamWriter_Allocate(&stream_writer);
    VPS_StreamWriter_Construct(stream_writer, fh);

    // 2. --- Write data ---
    TEST_ASSERT(VPS_StreamWriter_Write(stream_writer, (const unsigned char*)test_data_1, strlen(test_data_1)));
    TEST_ASSERT(VPS_StreamWriter_Write(stream_writer, (const unsigned char*)test_data_2, strlen(test_data_2)));

    // 3. --- Flush and Cleanup ---
    TEST_ASSERT(VPS_StreamWriter_Flush(stream_writer));

    VPS_StreamWriter_Release(stream_writer);
    close(fh);

    // 4. --- Verify the file content ---
    fh = open(test_filename, O_RDONLY);
    TEST_ASSERT(fh != -1);

    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    read(fh, buffer, sizeof(buffer) - 1);
    close(fh);

    TEST_ASSERT(strcmp(buffer, "Hello, World!") == 0);

    remove(test_filename);

    return 1;
}

void test_suite_VPS_StreamWriter() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_streamwriter_pipeline);

    printf("  ----------------------------------\n");
    printf("  VPS_StreamWriter Summary: %d passed, %d failed\n", success_count, failure_count);
}
