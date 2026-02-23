/**
 * @brief Provides buffered output to a file handle via write() syscall.
 *
 * This is the write-side twin of VPS_StreamReader. Data is accumulated
 * in an internal buffer and flushed to the file handle when the buffer
 * is full or when explicitly requested.
 */
struct VPS_StreamWriter
{
	struct VPS_Data *write_buffer;
	int file_handle;
};

char VPS_StreamWriter_Allocate
(
	struct VPS_StreamWriter **item
);

char VPS_StreamWriter_Construct
(
	struct VPS_StreamWriter *item
	, int file_handle
);

char VPS_StreamWriter_Deconstruct
(
	struct VPS_StreamWriter *item
);

char VPS_StreamWriter_Release
(
	struct VPS_StreamWriter *item
);

char VPS_StreamWriter_Write
(
	struct VPS_StreamWriter *item
	, const unsigned char *data
	, VPS_TYPE_SIZE size
);

char VPS_StreamWriter_Flush
(
	struct VPS_StreamWriter *item
);
