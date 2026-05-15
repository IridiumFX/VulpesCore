#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <vulpes/VPS_Types.h>
#include <vulpes/VPS_Data.h>
#include <vulpes/VPS_Decoder.h>
#include <vulpes/VPS_StreamReader.h>
#include <vulpes/VPS_Decoder_Base256.h>

#include "Test.h"

static int success_count = 0;
static int failure_count = 0;

/**
 * @test test_streamreader_pipeline
 * @brief This is the key test to visualize the data pipeline. It creates a
 *        temporary file, writes known data to it, and then uses the
 *        StreamReader to read it back, demonstrating the interaction
 *        between the file, the stream reader, and the destination buffer.
 */
static char test_streamreader_pipeline()
{
    const char* test_filename = "stream_test.tmp";
    const char* test_data = "This is a test of the streaming data pipeline.";
    int fh = -1;

    // 1. --- Create a temporary test file ---
    fh = open(test_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    TEST_ASSERT(fh != -1);
    write(fh, test_data, strlen(test_data));
    close(fh);

    // 2. --- Set up the pipeline components ---
    fh = open(test_filename, O_RDONLY);
    TEST_ASSERT(fh != -1);

    struct VPS_Data *dest_buffer = 0;
    struct VPS_StreamReader *stream_reader = 0;
    struct VPS_Decoder *pass_through_decoder = 0; // A simple decoder that does nothing

    VPS_Data_Allocate(&dest_buffer, 128, 0);
    VPS_Data_Construct(dest_buffer);

    VPS_StreamReader_Allocate(&stream_reader);
    VPS_StreamReader_Construct(stream_reader, dest_buffer, fh, 0, 0);

    // For this test, we use a simple "pass-through" decoder.
    // In the IFF parser, this would be the Base256 decoder.
    VPS_Decoder_Allocate(&pass_through_decoder);
    VPS_Decoder_Base256_Construct(pass_through_decoder);

    // 3. --- Perform a read ---
    // Ask the stream reader to read 10 bytes from the file.
    // It will read into its internal raw_buffer, then use the decoder
    // to process that data into our dest_buffer.
    VPS_TYPE_SIZE bytes_from_source = 0, bytes_consumed = 0, bytes_produced = 0, bytes_available = 0;
    TEST_ASSERT(VPS_StreamReader_Read(stream_reader, 10, &bytes_from_source, &bytes_consumed, &bytes_produced, &bytes_available, pass_through_decoder, 0));

    // Verify the results
    TEST_ASSERT(bytes_from_source == 10);
    TEST_ASSERT(bytes_produced == 10);
    TEST_ASSERT(dest_buffer->limit == 10); // The destination buffer now holds 10 valid bytes
    TEST_ASSERT(strncmp((char*)dest_buffer->bytes, "This is a ", 10) == 0);

    // 4. --- Perform a seek (skip) and another read ---
    // Skip the next 5 bytes ("test ")
    TEST_ASSERT(VPS_StreamReader_Seek(stream_reader, 5, SEEK_CUR));

    // To test the sliding window, we'll manually "consume" the first 5 bytes
    // of the dest_buffer, leaving "is a " as unread data.
    dest_buffer->position = 5;

    // Read the next 2 bytes ("of")
    TEST_ASSERT(VPS_StreamReader_Read(stream_reader, 2, &bytes_from_source, &bytes_consumed, &bytes_produced, &bytes_available, pass_through_decoder, 0));
    TEST_ASSERT(bytes_produced == 2); // The decoder should report it produced 2 new bytes
    // The buffer should now contain the 5 unread bytes ("is a ") plus the 2 new bytes ("of")
    TEST_ASSERT(bytes_available == 7);
    TEST_ASSERT(strncmp((char*)dest_buffer->bytes, "is a of", 7) == 0); // Verify the final content

    // 5. --- Cleanup ---
    close(fh);
    remove(test_filename);

    VPS_Data_Release(dest_buffer);
    VPS_StreamReader_Release(stream_reader);
    VPS_Decoder_Release(pass_through_decoder);

    return 1;
}

void test_suite_VPS_StreamReader() {
    success_count = 0;
    failure_count = 0;

    RUN_TEST(test_streamreader_pipeline);

    printf("  ----------------------------------\n");
    printf("  VPS_StreamReader Summary: %d passed, %d failed\n", success_count, failure_count);
}