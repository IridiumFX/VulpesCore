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
 * @test test_streamwriter_basic
 * @brief Verifies that the StreamWriter can write data to a file.
 */
static int test_streamwriter_basic()
{
    const char* test_filename = "stream_write_test.tmp";
    const char* test_data = "Hello, StreamWriter!";
    int fh = -1;

    // 1. --- Create a temporary test file ---
    fh = open(test_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    TEST_ASSERT(fh != -1);

    // 2. --- Set up the StreamWriter ---
    struct VPS_StreamWriter *stream_writer = 0;
    VPS_StreamWriter_Allocate(&stream_writer);
    VPS_StreamWriter_Construct(stream_writer, fh);

    // 3. --- Write data ---
    TEST_ASSERT(VPS_StreamWriter_Write(stream_writer, (const unsigned char*)test_data, strlen(test_data)));

    // 4. --- Flush and Cleanup ---
    TEST_ASSERT(VPS_StreamWriter_Flush(stream_writer));

    VPS_StreamWriter_Release(stream_writer); // This should also close the internal buffer, but not the file handle?
                                             // Looking at the header, it doesn't say it closes the file handle.
                                             // Usually stream writers don't own the handle unless specified.
                                             // I'll assume it doesn't close the handle based on Construct taking it.
    close(fh);

    // 5. --- Verify content ---
    fh = open(test_filename, O_RDONLY);
    TEST_ASSERT(fh != -1);

    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    read(fh, buffer, sizeof(buffer));
    close(fh);
    remove(test_filename);

    TEST_ASSERT(strncmp(buffer, test_data, strlen(test_data)) == 0);

    return 1;
}

void test_suite_VPS_StreamWriter() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_streamwriter_basic);

    printf("  ----------------------------------\n");
    printf("  VPS_StreamWriter Summary: %d passed, %d failed\n", success_count, failure_count);
}
