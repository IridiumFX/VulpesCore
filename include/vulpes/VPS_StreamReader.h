/**
 * @brief Provides a high-level, buffered reader for streaming data from a source.
 *
 * This component orchestrates reading from a source (like a file handle),
 * buffering the raw data, and optionally passing it through a decoder
 * before making it available in a final data buffer.
 */
struct VPS_StreamReader
{
	// The final destination buffer for the processed data.
	struct VPS_Data *decoded_buffer;

	// An internal buffer for staging raw data read from the file.
	struct VPS_Data *raw_buffer;

	// Flag indicating if this component owns and should release the raw_buffer.
	char own_raw_buffer;

	// The input source, typically a file handle.
	int file_handle;
};

char VPS_StreamReader_Allocate
(
	struct VPS_StreamReader **item
);

char VPS_StreamReader_Construct
(
	struct VPS_StreamReader *item
	, struct VPS_Data *destination_buffer
	, int file_handle
	, struct VPS_Data *raw_buffer
	, char take_ownership
);

char VPS_StreamReader_Deconstruct
(
	struct VPS_StreamReader *item
);

char VPS_StreamReader_Release
(
	struct VPS_StreamReader *item
);

char VPS_StreamReader_Read
(
	struct VPS_StreamReader *item
	, VPS_TYPE_SIZE new_bytes_to_read
	, VPS_TYPE_SIZE *bytes_read_from_source
	, VPS_TYPE_SIZE *bytes_consumed_by_decoder
	, VPS_TYPE_SIZE *bytes_produced_by_decoder
	, VPS_TYPE_SIZE *bytes_available_to_use
	, struct VPS_Decoder *decoder
	, void *decoder_context
);

char VPS_StreamReader_Seek
(
	struct VPS_StreamReader *item
	, VPS_TYPE_64S offset
	, int whence
);
