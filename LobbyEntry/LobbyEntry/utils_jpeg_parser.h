/**
 * @file utils_jpeg_parser.h
 * @brief Header for JPEG validation utility
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef utils_jpeg_parser_H
#define utils_jpeg_parser_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief JPEG check result codes
 */
typedef enum {
    JPEG_CHECK_OK = 0,                  /**< Valid baseline JPEG with supported subsampling */
    JPEG_CHECK_INVALID_BUFFER,          /**< Buffer is NULL or too small */
    JPEG_CHECK_NO_SOI,                  /**< No SOI marker - not a JPEG */
    JPEG_CHECK_NO_SOF,                  /**< No SOF marker found */
    JPEG_CHECK_NOT_BASELINE,            /**< Not baseline JPEG (progressive/extended) */
    JPEG_CHECK_UNSUPPORTED_SUBSAMPLING, /**< Subsampling not 4:2:0, 4:2:2, or 4:4:4 */
    JPEG_CHECK_PARSE_ERROR,             /**< Failed to parse JPEG structure */
} jpeg_check_result_t;

/**
 * @brief Chroma subsampling types
 */
typedef enum {
    JPEG_SUBSAMPLING_UNKNOWN = 0,
    JPEG_SUBSAMPLING_444,   /**< 4:4:4 - No subsampling (Y:Cb:Cr = 1:1:1) */
    JPEG_SUBSAMPLING_422,   /**< 4:2:2 - Horizontal subsampling (Y:Cb:Cr = 2:1:1 horizontal) */
    JPEG_SUBSAMPLING_420,   /**< 4:2:0 - Both H&V subsampling (Y:Cb:Cr = 2:1:1 both) */
    JPEG_SUBSAMPLING_440,   /**< 4:4:0 - Vertical only (rare, not supported by EVE) */
    JPEG_SUBSAMPLING_411,   /**< 4:1:1 - Heavy horizontal (not supported by EVE) */
} jpeg_subsampling_t;

/**
 * @brief JPEG image information structure
 */
typedef struct {
    int32_t data_start;
    int32_t data_size;
    uint16_t width;                 /**< Image width in pixels */
    uint16_t height;                /**< Image height in pixels */
    uint8_t  precision;             /**< Bits per sample (usually 8) */
    uint8_t  num_components;        /**< Number of color components (1=grayscale, 3=YCbCr) */
    bool     is_baseline;           /**< true if baseline DCT (SOF0) */
    bool     is_jfif;               /**< true if JFIF marker present */
    jpeg_subsampling_t subsampling; /**< Chroma subsampling type */
} jpeg_info_t;

/**
 * @brief Check if a buffer contains a valid baseline JPEG for EVE
 *
 * Validates that the buffer contains:
 * - Valid JPEG structure with SOI marker
 * - Baseline DCT encoding (SOF0)
 * - Supported chroma subsampling (4:2:0, 4:2:2, or 4:4:4)
 *
 * @param buffer Pointer to JPEG data buffer
 * @param size Size of buffer in bytes
 * @param info Optional pointer to receive JPEG info (can be NULL)
 * @return JPEG_CHECK_OK if valid, error code otherwise
 *
 * @example
 * @code
 * jpeg_info_t info;
 * jpeg_check_result_t result = jpeg_check_buffer(jpeg_data, jpeg_size, &info);
 * if (result == JPEG_CHECK_OK) {
 *     printf("Valid JPEG: %dx%d, %s\n", info.width, info.height,
 *            jpeg_subsampling_str(info.subsampling));
 * } else {
 *     printf("Invalid: %s\n", jpeg_check_result_str(result));
 * }
 * @endcode
 */
jpeg_check_result_t jpeg_check_buffer(const uint8_t *buffer, uint32_t size, jpeg_info_t *info);

/**
 * @brief Quick check if JPEG is valid for EVE GPU
 *
 * Convenience function that returns true/false without detailed info.
 *
 * @param buffer Pointer to JPEG data buffer
 * @param size Size of buffer in bytes
 * @return true if valid baseline JPEG with supported subsampling
 */
bool jpeg_is_valid_for_eve(const uint8_t *buffer, uint32_t size);

/**
 * @brief Get human-readable string for check result
 * @param result Check result code
 * @return Descriptive string
 */
const char *jpeg_check_result_str(jpeg_check_result_t result);

/**
 * @brief Get human-readable string for subsampling type
 * @param subsampling Subsampling enum value
 * @return String like "4:2:0", "4:2:2", etc.
 */
const char *jpeg_subsampling_str(jpeg_subsampling_t subsampling);

/**
 * @brief Print JPEG info to stdout (for debugging)
 * @param info Pointer to jpeg_info_t structure
 */
void jpeg_print_info(const jpeg_info_t *info);

/**
 * @brief Comprehensive JPEG validation for EVE GPU with detailed statistics
 *
 * Combines features from validate_jpeg_buffer and jpeg_check_buffer:
 * - Finds SOI marker with tolerance (searches up to 10KB)
 * - Performs comprehensive structure validation
 * - Validates Huffman and quantization tables
 * - Checks subsampling compatibility for EVE
 * - Provides detailed statistics and image info
 *
 * @param buffer Pointer to JPEG data buffer
 * @param size Size of buffer in bytes
 * @param info Optional pointer to receive JPEG info (dimensions, subsampling) - can be NULL
 * @param verbose If true, prints validation details to stdout
 * @return JPEG_CHECK_OK if valid for EVE, error code otherwise
 *
 * @example
 * @code
 * jpeg_info_t info;
 * jpeg_check_result_t result = eve_validate_jpeg(jpeg_data, jpeg_size, &info, false);
 * if (result == JPEG_CHECK_OK) {
 *     printf("Valid: %dx%d, %s\n", info.width, info.height,
 *            jpeg_subsampling_str(info.subsampling));
 * }
 * @endcode
 */
jpeg_check_result_t eve_validate_jpeg(const uint8_t *buffer, uint32_t size, 
                                       jpeg_info_t *info, bool verbose);

#ifdef __cplusplus
}
#endif

#endif /* utils_jpeg_parser_H */
