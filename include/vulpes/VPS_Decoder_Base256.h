// Forward declarations
struct VPS_Decoder;

/**
 * @brief Constructs a pre-allocated decoder instance to be a Base256 (pass-through) decoder.
 *
 * This decoder simply copies data from the source to the destination without
 * any transformation.
 *
 * This follows the alloc/construct pattern. The caller should first allocate
 * a generic VPS_Decoder using VPS_Decoder_Allocate().
 * @param decoder The pre-allocated decoder instance to construct.
 * @return 1 on success, 0 on failure.
 */
char VPS_Decoder_Base256_Construct
(
	struct VPS_Decoder* decoder
);
