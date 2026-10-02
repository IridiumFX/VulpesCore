#pragma once
#include <vulpes/VPS_Types.h>

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
 * @return VPS_OK on success, non-zero on failure.
 */
VPS_TYPE_RESULT VPS_Decoder_Base256_Construct
(
	struct VPS_Decoder* decoder
);
