struct VPS_Decoder
{
	/**
	 * @brief Decodes the source data buffer into the destination buffer.
	 * @param source The VPS_Data buffer containing the encoded data.
	 * @param destination The VPS_Data buffer that will receive the decoded data.
	 * @param context An optional, implementation-specific context pointer.
	 * @return 1 on success, 0 on failure.
	 */
	char (*decode)
	(
		struct VPS_Data* source,
		struct VPS_Data* destination,
		void* context,
		VPS_TYPE_SIZE* bytes_consumed // Out-param: How many bytes were read from source.
	);
};

char VPS_Decoder_Allocate
(
	struct VPS_Decoder **item
);

char VPS_Decoder_Construct
(
	struct VPS_Decoder *item,
	char (*decode)
	(
		struct VPS_Data* source,
		struct VPS_Data* destination,
		void* context,
		VPS_TYPE_SIZE* bytes_consumed
	)
);

char VPS_Decoder_Deconstruct
(
	struct VPS_Decoder *item
);

char VPS_Decoder_Release
(
	struct VPS_Decoder *item
);
