/**
 * @file utils_jpeg_parser.c
 * @brief Utility to validate baseline JPEG (JFIF) with supported chroma subsampling
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
*/

#include <string.h>
#include <stdio.h>
#include "utils_jpeg_parser.h"
#include "camera.h"

#define JPEG_MARKER_PREFIX  0xFF

/* Start/End markers */
#define JPEG_SOI            0xD8    /* Start of Image */
#define JPEG_EOI            0xD9    /* End of Image */

/* Frame markers (Start of Frame) */
#define JPEG_SOF0           0xC0    /* Baseline DCT */
#define JPEG_SOF1           0xC1    /* Extended sequential DCT */
#define JPEG_SOF2           0xC2    /* Progressive DCT */
#define JPEG_SOF3           0xC3    /* Lossless (sequential) */

/* Application markers */
#define JPEG_APP0           0xE0    /* JFIF marker */
#define JPEG_APP1           0xE1    /* EXIF marker */

/* Other markers */
#define JPEG_DHT            0xC4    /* Define Huffman Table */
#define JPEG_DQT            0xDB    /* Define Quantization Table */
#define JPEG_DRI            0xDD    /* Define Restart Interval */
#define JPEG_SOS            0xDA    /* Start of Scan */
#define JPEG_COM            0xFE    /* Comment */

/* Restart markers */
#define JPEG_RST0           0xD0
#define JPEG_RST7           0xD7

/* Sampling factor encoding: (H << 4) | V */
#define SAMPLING_1x1        0x11
#define SAMPLING_2x1        0x21
#define SAMPLING_1x2        0x12
#define SAMPLING_2x2        0x22

static bool is_valid_marker(uint16_t marker)
{
    if (marker == MARKER_SOI || marker == MARKER_EOI) return true;
    if (marker >= MARKER_RST0 && marker <= MARKER_RST7) return true;

    switch (marker) {
        case MARKER_SOF0: case MARKER_DHT: case MARKER_DQT:
        case MARKER_DRI: case MARKER_SOS: case MARKER_COM:
            return true;
        case MARKER_SOF1: case MARKER_SOF2: case 0xFFDF:
            return false;
    }

    if (marker >= 0xFFE0 && marker <= 0xFFEF) return true;
    if ((marker & 0xFF00) == 0xFF00) {
        uint8_t code = marker & 0xFF;
        if (code >= 0xC8 && code <= 0xCC) return false;
        if (code == 0xDC || (code >= 0xF0 && code <= 0xFD)) return false;
    }
    return false;
}

static bool is_standalone_marker(uint16_t marker)
{
    return (marker == MARKER_SOI || marker == MARKER_EOI ||
            (marker >= MARKER_RST0 && marker <= MARKER_RST7));
}

/* Validate Huffman table structure */
static bool validate_huffman_table(const uint8_t *data, uint32_t pos, uint16_t length, bool verbose)
{
    if (length < 17) {
        if (verbose) printf("        ERROR: DHT too short (%u bytes)\n", length);
        return false;
    }

    uint8_t ht_info = data[pos + 4];
    uint8_t ht_class = (ht_info >> 4) & 0x0F;
    uint8_t ht_id = ht_info & 0x0F;

    if (ht_class > 1) {
        if (verbose) printf("        ERROR: Invalid Huffman table class %u\n", ht_class);
        return false;
    }

    if (ht_id > 3) {
        if (verbose) printf("        ERROR: Invalid Huffman table ID %u\n", ht_id);
        return false;
    }

    /* Read the 16 counts */
    uint32_t total_codes = 0;
    for (int i = 0; i < 16; i++) {
        uint8_t count = data[pos + 5 + i];
        total_codes += count;
    }

    /* Check that we have enough data for all values */
    uint32_t required_length = 2 + 1 + 16 + total_codes;  /* marker + length + info + counts + values */
    if (length < required_length) {
        if (verbose) {
            printf("        ERROR: DHT length mismatch. Need %u bytes for %u codes, got %u\n",
                   required_length, total_codes, length);
        }
        return false;
    }

    /* Validate code lengths (JPEG allows max 16 bits per code) */
    if (total_codes > 256) {
        if (verbose) printf("        ERROR: Too many Huffman codes (%u > 256)\n", total_codes);
        return false;
    }

    /* Check for bogus patterns common in corrupted files */
    /* Pattern 1: All counts are 0 */
    if (total_codes == 0) {
        if (verbose) printf("        ERROR: Empty Huffman table (all counts zero)\n");
        return false;
    }

    /* Pattern 2: Check for impossible code distributions */
    uint32_t max_codes_at_length[17] = {0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
    uint32_t codes_used = 0;

    for (int i = 0; i < 16; i++) {
        uint8_t count = data[pos + 5 + i];
        uint32_t length_val = i + 1;

        /* Check if this length can accommodate the count */
        uint32_t max_at_this_length = max_codes_at_length[length_val];

        codes_used += count;

        /* Accumulate codes - each code at length L uses up 2^(16-L) codes at length 16 */
        uint32_t code_space_used = count * (1 << (16 - length_val));

        if (codes_used > 256) {
            if (verbose) {
                printf("        ERROR: Huffman table overflows at bit length %u (total=%u)\n",
                       length_val, codes_used);
            }
            return false;
        }
    }

    /* Validate Huffman code values */
    uint32_t value_start = pos + 5 + 16;
    for (uint32_t i = 0; i < total_codes && value_start + i < pos + 2 + length; i++) {
        uint8_t value = data[value_start + i];

        /* For DC tables, values should be 0-15 (encoding bit lengths) */
        if (ht_class == 0) {
            if (value > 15) {
                if (verbose) {
                    printf("        ERROR: Invalid DC Huffman value %u (must be 0-15)\n", value);
                }
                return false;
            }
        }
        /* For AC tables, values should be valid run-length/size pairs */
        else {
            uint8_t run = (value >> 4) & 0x0F;
            uint8_t size = value & 0x0F;

            /* Check for bogus values */
            if (run == 15 && size > 10) {
                if (verbose) {
                    printf("        ERROR: Invalid AC Huffman value 0x%02X (run=15, size=%u > 10)\n",
                           value, size);
                }
                return false;
            }

            if (value != 0x00 && size > 10) {  /* 0x00 is EOB, always valid */
                if (verbose) {
                    printf("        ERROR: Invalid AC Huffman value 0x%02X (size=%u > 10)\n",
                           value, size);
                }
                return false;
            }
        }
    }

    if (verbose) {
        printf("        -> Valid %s Huffman table %u (%u codes)\n",
               ht_class == 0 ? "DC" : "AC", ht_id, total_codes);
    }

    return true;
}

static const char* get_error_message(JpegValidationResult result)
{
    switch (result) {
        case JPEG_VALID: return "Valid baseline JFIF";
        case JPEG_ERROR_NO_SOI: return "Missing SOI marker";
        case JPEG_ERROR_NO_JFIF: return "Missing JFIF marker";
        case JPEG_ERROR_NO_SOF0: return "Not baseline JPEG";
        case JPEG_ERROR_INVALID_MARKER: return "Invalid markers present";
        case JPEG_ERROR_TRUNCATED: return "Truncated data";
        case JPEG_ERROR_NO_EOI: return "Missing EOI marker";
        case JPEG_ERROR_INVALID_DIMENSIONS: return "Invalid dimensions";
        case JPEG_ERROR_INVALID_COMPONENTS: return "Invalid components";
        case JPEG_ERROR_MISSING_TABLES: return "Missing required tables";
        case JPEG_ERROR_TABLE_MISMATCH: return "Table reference mismatch";
        case JPEG_ERROR_ENTROPY_CORRUPTION: return "Corrupted entropy data";
        case JPEG_ERROR_INVALID_SAMPLING: return "Invalid sampling factors";
        case JPEG_ERROR_BOGUS_HUFFMAN_TABLE: return "Bogus Huffman table definition";
        default: return "Unknown error";
    }
}

JpegValidationResult validate_jpeg_buffer(const uint8_t *picBuf,
                                          uint32_t bufSize,
                                          JpegValidationStats *stats,
                                          bool verbose)
{
    uint32_t pos = 0;
    uint16_t marker, length;

    memset(stats, 0, sizeof(JpegValidationStats));
    stats->first_invalid_pos = 0xFFFFFFFF;

    if (bufSize < 2) return JPEG_ERROR_TRUNCATED;

    marker = (picBuf[0] << 8) | picBuf[1];
    if (marker != MARKER_SOI) return JPEG_ERROR_NO_SOI;

    stats->has_soi = true;
    stats->total_markers++;
    if (verbose) printf("[%06u] SOI\n", pos);
    pos += 2;

    while (pos < bufSize - 1) {
        marker = (picBuf[pos] << 8) | picBuf[pos + 1];

        if ((marker & 0xFF00) != 0xFF00) return JPEG_ERROR_INVALID_MARKER;

        if (!is_valid_marker(marker)) {
            if (verbose) printf("INVALID MARKER at %u: 0x%04X\n", pos, marker);
            if (stats->invalid_markers == 0) {
                stats->first_invalid_pos = pos;
                stats->first_invalid_marker = marker;
            }
            stats->invalid_markers++;
            return JPEG_ERROR_INVALID_MARKER;
        }

        stats->total_markers++;

        if (marker == MARKER_APP0) {
            if (pos + 6 < bufSize) {
                length = (picBuf[pos + 2] << 8) | picBuf[pos + 3];
                if (memcmp(&picBuf[pos + 4], "JFIF\0", 5) == 0) {
                    stats->has_jfif = true;
                }
            }
        } else if (marker == MARKER_DQT) {
            length = (picBuf[pos + 2] << 8) | picBuf[pos + 3];
            if (pos + 4 < bufSize) {
                uint8_t qt_id = picBuf[pos + 4] & 0x0F;
                if (qt_id < 4) {
                    stats->dqt_defined[qt_id] = true;
                    stats->dqt_count++;
                }
            }
        } else if (marker == MARKER_DHT) {
            length = (picBuf[pos + 2] << 8) | picBuf[pos + 3];

            /* VALIDATE HUFFMAN TABLE */
            if (!validate_huffman_table(picBuf, pos, length, verbose)) {
                stats->huffman_errors++;
                return JPEG_ERROR_BOGUS_HUFFMAN_TABLE;
            }

            if (pos + 4 < bufSize) {
                uint8_t ht_info = picBuf[pos + 4];
                uint8_t ht_id = ht_info & 0x0F;
                uint8_t ht_class = (ht_info >> 4) & 0x0F;
                if (ht_id < 4) {
                    if (ht_class == 0) {
                        stats->dht_dc_defined[ht_id] = true;
                    } else {
                        stats->dht_ac_defined[ht_id] = true;
                    }
                    stats->dht_count++;
                }
            }
        } else if (marker == MARKER_SOF0) {
            stats->has_sof0 = true;
            if (pos + 9 < bufSize) {
                stats->precision = picBuf[pos + 4];
                stats->height = (picBuf[pos + 5] << 8) | picBuf[pos + 6];
                stats->width = (picBuf[pos + 7] << 8) | picBuf[pos + 8];
                stats->num_components = picBuf[pos + 9];

                if (stats->width == 0 || stats->height == 0) {
                    return JPEG_ERROR_INVALID_DIMENSIONS;
                }
                if (stats->num_components == 0 || stats->num_components > 4) {
                    return JPEG_ERROR_INVALID_COMPONENTS;
                }

                for (uint8_t i = 0; i < stats->num_components && pos + 10 + i * 3 + 2 < bufSize; i++) {
                    uint32_t cpos = pos + 10 + i * 3;
                    uint8_t sampling = picBuf[cpos + 1];
                    stats->comp_h_sample[i] = (sampling >> 4) & 0x0F;
                    stats->comp_v_sample[i] = sampling & 0x0F;
                    stats->comp_qt_table[i] = picBuf[cpos + 2];

                    if (stats->comp_h_sample[i] == 0 || stats->comp_v_sample[i] == 0) {
                        return JPEG_ERROR_INVALID_SAMPLING;
                    }
                }
            }
        } else if (marker == MARKER_SOS) {
            length = (picBuf[pos + 2] << 8) | picBuf[pos + 3];
            if (pos + 4 < bufSize) {
                uint8_t sos_comps = picBuf[pos + 4];
                for (uint8_t i = 0; i < sos_comps && pos + 5 + i * 2 + 1 < bufSize; i++) {
                    uint32_t cpos = pos + 5 + i * 2;
                    uint8_t ht_sel = picBuf[cpos + 1];
                    stats->sos_comp_dc_table[i] = (ht_sel >> 4) & 0x0F;
                    stats->sos_comp_ac_table[i] = ht_sel & 0x0F;
                }
            }
        } else if (marker == MARKER_EOI) {
            stats->has_eoi = true;
            break;
        }

        pos += 2;

        if (!is_standalone_marker(marker)) {
            if (pos + 1 >= bufSize) return JPEG_ERROR_TRUNCATED;
            length = (picBuf[pos] << 8) | picBuf[pos + 1];
            if (length < 2) return JPEG_ERROR_INVALID_MARKER;

            if (marker == MARKER_SOS) {
                pos += length;
                while (pos < bufSize) {
                    if (picBuf[pos] == 0xFF && pos + 1 < bufSize && picBuf[pos + 1] != 0x00) {
                        break;
                    }
                    pos++;
                }
            } else {
                pos += length;
            }
        }
    }

    if (!stats->has_eoi) return JPEG_ERROR_NO_EOI;
    if (!stats->has_jfif) return JPEG_ERROR_NO_JFIF;
    if (!stats->has_sof0) return JPEG_ERROR_NO_SOF0;

    /* Validate table references */
    for (uint8_t i = 0; i < stats->num_components; i++) {
        if (!stats->dqt_defined[stats->comp_qt_table[i]]) {
            if (verbose) printf("ERROR: Missing QT table %u\n", stats->comp_qt_table[i]);
            return JPEG_ERROR_MISSING_TABLES;
        }
        if (!stats->dht_dc_defined[stats->sos_comp_dc_table[i]]) {
            if (verbose) printf("ERROR: Missing DC Huffman table %u\n", stats->sos_comp_dc_table[i]);
            return JPEG_ERROR_MISSING_TABLES;
        }
        if (!stats->dht_ac_defined[stats->sos_comp_ac_table[i]]) {
            if (verbose) printf("ERROR: Missing AC Huffman table %u\n", stats->sos_comp_ac_table[i]);
            return JPEG_ERROR_MISSING_TABLES;
        }
    }

    if (stats->invalid_markers > 0) return JPEG_ERROR_INVALID_MARKER;

    return JPEG_VALID;
}

void print_validation_report(const JpegValidationStats *stats, JpegValidationResult result)
{
    printf("\n========================================\n");
    printf("JPEG Validation Report\n");
    printf("========================================\n");

    if (stats->has_sof0) {
        printf("Image: %ux%u, %u components, %u-bit\n",
               stats->width, stats->height, stats->num_components, stats->precision);
    }

    printf("Markers: %u total\n", stats->total_markers);
    printf("Tables: DQT=%u, DHT=%u\n", stats->dqt_count, stats->dht_count);
    if (stats->invalid_markers > 0) {
        printf("\nINVALID: %u markers\n", stats->invalid_markers);
        printf("First: 0x%04X at pos %u\n", stats->first_invalid_marker, stats->first_invalid_pos);
    }

    if (stats->huffman_errors > 0) {
        printf("\nHuffman errors: %u\n", stats->huffman_errors);
    }
    if (result == JPEG_VALID){

    }else{
        printf("\nResult: %s %s\n", "x", get_error_message(result));
    }
}

/**
 * @brief Read 16-bit big-endian value from buffer
 */
static uint16_t read_be16(const uint8_t *data)
{
    return ((uint16_t)data[0] << 8) | data[1];
}

/**
 * @brief Find next JPEG marker in buffer
 * @param data Buffer pointer
 * @param size Buffer size
 * @param offset Current offset (updated on return)
 * @return Marker value or 0 if not found
 */
static uint8_t find_next_marker(const uint8_t *data, uint32_t size, uint32_t *offset)
{
    while (*offset < size - 1)
    {
        if (data[*offset] == JPEG_MARKER_PREFIX)
        {
            uint8_t marker = data[*offset + 1];
            /* Skip padding bytes (0xFF followed by 0xFF) */
            if (marker != JPEG_MARKER_PREFIX && marker != 0x00)
            {
                *offset += 2;
                return marker;
            }
        }
        (*offset)++;
    }
    return 0;
}

/**
 * @brief Get segment length (excludes the 2-byte marker)
 */
static uint16_t get_segment_length(const uint8_t *data, uint32_t offset, uint32_t size)
{
    if (offset + 2 > size)
        return 0;
    return read_be16(&data[offset]);
}

/**
 * @brief Convert sampling factors to subsampling type
 * @param y_sampling Y component sampling factor (H<<4 | V)
 * @param cb_sampling Cb component sampling factor
 * @param cr_sampling Cr component sampling factor
 * @return Subsampling type enum
 */
static jpeg_subsampling_t get_subsampling_type(uint8_t y_sampling, uint8_t cb_sampling, uint8_t cr_sampling)
{
    /* For valid JPEG, Cb and Cr should have same sampling */
    if (cb_sampling != cr_sampling)
        return JPEG_SUBSAMPLING_UNKNOWN;

    /* Check Y component sampling relative to chroma */
    uint8_t y_h = (y_sampling >> 4) & 0x0F;
    uint8_t y_v = y_sampling & 0x0F;
    uint8_t c_h = (cb_sampling >> 4) & 0x0F;
    uint8_t c_v = cb_sampling & 0x0F;

    /* Calculate ratio */
    if (c_h == 0 || c_v == 0)
        return JPEG_SUBSAMPLING_UNKNOWN;

    uint8_t h_ratio = y_h / c_h;
    uint8_t v_ratio = y_v / c_v;

    if (h_ratio == 1 && v_ratio == 1)
        return JPEG_SUBSAMPLING_444;    /* 4:4:4 - no subsampling */
    else if (h_ratio == 2 && v_ratio == 1)
        return JPEG_SUBSAMPLING_422;    /* 4:2:2 - horizontal subsampling */
    else if (h_ratio == 2 && v_ratio == 2)
        return JPEG_SUBSAMPLING_420;    /* 4:2:0 - both H and V subsampling */
    else if (h_ratio == 1 && v_ratio == 2)
        return JPEG_SUBSAMPLING_440;    /* 4:4:0 - vertical only (rare) */
    else if (h_ratio == 4 && v_ratio == 1)
        return JPEG_SUBSAMPLING_411;    /* 4:1:1 */

    return JPEG_SUBSAMPLING_UNKNOWN;
}

/**
 * @brief Parse SOF0 (Start of Frame - Baseline) segment
 */
static bool parse_sof0_segment(const uint8_t *data, uint32_t offset, uint32_t size, jpeg_info_t *info)
{
    /* SOF0 structure:
     * - Length (2 bytes)
     * - Precision (1 byte) - bits per sample
     * - Height (2 bytes)
     * - Width (2 bytes)
     * - Number of components (1 byte)
     * - For each component:
     *   - Component ID (1 byte)
     *   - Sampling factors (1 byte): H in upper nibble, V in lower
     *   - Quantization table ID (1 byte)
     */

    uint16_t seg_len = get_segment_length(data, offset, size);
    if (seg_len < 8 || offset + seg_len > size)
        return false;

    info->precision = data[offset + 2];
    info->height = read_be16(&data[offset + 3]);
    info->width = read_be16(&data[offset + 5]);
    info->num_components = data[offset + 7];

    /* Validate component count */
    if (info->num_components < 1 || info->num_components > 4)
        return false;

    /* Check segment length matches expected size */
    uint16_t expected_len = 8 + (info->num_components * 3);
    if (seg_len < expected_len)
        return false;

    /* Parse component sampling factors */
    uint8_t y_sampling = 0;
    uint8_t cb_sampling = 0;
    uint8_t cr_sampling = 0;

    for (int i = 0; i < info->num_components && i < 4; i++)
    {
        uint32_t comp_offset = offset + 8 + (i * 3);
        uint8_t comp_id = data[comp_offset];
        uint8_t sampling = data[comp_offset + 1];
        /* uint8_t quant_table = data[comp_offset + 2]; */

        /* Component IDs: 1=Y, 2=Cb, 3=Cr (or 0,1,2 in some encoders) */
        switch (comp_id)
        {
        case 1:
        case 0:
            y_sampling = sampling;
            break;
        case 2:
            cb_sampling = sampling;
            break;
        case 3:
            cr_sampling = sampling;
            break;
        }
    }

    /* For grayscale (1 component), treat as 4:4:4 */
    if (info->num_components == 1)
    {
        info->subsampling = JPEG_SUBSAMPLING_444;
    }
    else
    {
        info->subsampling = get_subsampling_type(y_sampling, cb_sampling, cr_sampling);
    }

    return true;
}

/**
 * @brief Check JFIF APP0 segment
 */
static bool check_jfif_marker(const uint8_t *data, uint32_t offset, uint32_t size)
{
    uint16_t seg_len = get_segment_length(data, offset, size);
    if (seg_len < 14 || offset + seg_len > size)
        return false;

    /* Check "JFIF\0" identifier */
    if (memcmp(&data[offset + 2], "JFIF", 4) == 0 && data[offset + 6] == 0x00)
        return true;

    /* Also accept "JFXX" for JFIF extensions */
    if (memcmp(&data[offset + 2], "JFXX", 4) == 0)
        return true;

    return false;
}

jpeg_check_result_t jpeg_check_buffer(const uint8_t *buffer, uint32_t size, jpeg_info_t *info)
{
    jpeg_info_t local_info = {0};

    if (buffer == NULL || size < 4)
        return JPEG_CHECK_INVALID_BUFFER;

    uint32_t offset = 0;
    uint8_t found_soi = 0;
    local_info.data_start = 0;
    local_info.data_size = 0;

    /* find SOI marker */
    const int32_t MAX_SOI = 10*1024;
    while(offset < size - 1 && offset < MAX_SOI)
    {
        if (buffer[offset] == JPEG_MARKER_PREFIX && buffer[offset + 1] == JPEG_SOI)
        {
            local_info.data_start = offset;
            found_soi = 1;
            offset += 2;
            break;
        }
        offset++;
    }

    if(!found_soi){
        return JPEG_CHECK_NO_SOI;
    }

    bool found_jfif = false;
    bool found_sof0 = false;
    bool is_baseline = false;

    /* Parse JPEG segments */
    while (offset < size - 1)
    {
        uint8_t marker = find_next_marker(buffer, size, &offset);
        if (marker == 0)
            break;

        switch (marker)
        {
        case JPEG_APP0:
            if (check_jfif_marker(buffer, offset, size))
            {
                found_jfif = true;
            }
            /* Skip segment */
            offset += get_segment_length(buffer, offset, size);
            break;

        case JPEG_SOF0:
            /* Baseline DCT - this is what we want */
            is_baseline = true;
            found_sof0 = true;
            if (!parse_sof0_segment(buffer, offset, size, &local_info))
                return JPEG_CHECK_PARSE_ERROR;
            offset += get_segment_length(buffer, offset, size);
            break;

        case JPEG_SOF1:
        case JPEG_SOF2:
        case JPEG_SOF3:
            /* Extended/Progressive/Lossless - not supported */
            found_sof0 = true;
            is_baseline = false;
            offset += get_segment_length(buffer, offset, size);
            break;

        case JPEG_DHT:
        case JPEG_DQT:
        case JPEG_DRI:
        case JPEG_COM:
            /* Skip these segments */
            offset += get_segment_length(buffer, offset, size);
            break;
            
        case JPEG_EOI:
            /* End of Image */
            local_info.data_size = offset - local_info.data_start;
            goto parse_done;
            
        case JPEG_SOS:
        default:
            /* Skip APP1-APP15 and other markers */
            if (marker >= 0xE0 && marker <= 0xEF)
            {
                offset += get_segment_length(buffer, offset, size);
            }
            else if (marker >= JPEG_RST0 && marker <= JPEG_RST7)
            {
                /* Restart markers have no length */
            }
            else
            {
                /* Unknown marker with length */
                uint16_t len = get_segment_length(buffer, offset, size);
                if (len > 0)
                    offset += len;
            }
            break;
        }
    }

parse_done:
    /* Copy info if requested */
    local_info.is_jfif = found_jfif;
    local_info.is_baseline = is_baseline;

    if (info != NULL)
    {
        memcpy(info, &local_info, sizeof(jpeg_info_t));
    }

    /* Validate results */
    if (!found_sof0)
        return JPEG_CHECK_NO_SOF;

    if (!is_baseline)
        return JPEG_CHECK_NOT_BASELINE;

    if (!found_jfif)
    {
        /* Warning: not JFIF but might still work */
        /* Some cameras output JPEG without JFIF marker */
        return JPEG_CHECK_NOT_BASELINE;
    }

    /* Check subsampling compatibility */
    switch (local_info.subsampling)
    {
    case JPEG_SUBSAMPLING_444:
    case JPEG_SUBSAMPLING_422:
    case JPEG_SUBSAMPLING_420:
        //printf("found jpeg data start: %u, size: %u\n", (uint32_t)(local_info.data_start), local_info.data_size);
        return JPEG_CHECK_OK;

    case JPEG_SUBSAMPLING_440:
    case JPEG_SUBSAMPLING_411:
        return JPEG_CHECK_UNSUPPORTED_SUBSAMPLING;

    default:
        return JPEG_CHECK_UNSUPPORTED_SUBSAMPLING;
    }
}

const char *jpeg_check_result_str(jpeg_check_result_t result)
{
    switch (result)
    {
    case JPEG_CHECK_OK:
        return "OK - Valid baseline JPEG with supported subsampling";
    case JPEG_CHECK_INVALID_BUFFER:
        return "Error - Invalid buffer (NULL or too small)";
    case JPEG_CHECK_NO_SOI:
        return "Error - No SOI marker (not a JPEG)";
    case JPEG_CHECK_NO_SOF:
        return "Error - No SOF marker found";
    case JPEG_CHECK_NOT_BASELINE:
        return "Error - Not a baseline JPEG (progressive/extended not supported)";
    case JPEG_CHECK_UNSUPPORTED_SUBSAMPLING:
        return "Error - Unsupported chroma subsampling (need 4:2:0, 4:2:2, or 4:4:4)";
    case JPEG_CHECK_PARSE_ERROR:
        return "Error - Failed to parse JPEG structure";
    default:
        return "Error - Unknown error";
    }
}

const char *jpeg_subsampling_str(jpeg_subsampling_t subsampling)
{
    switch (subsampling)
    {
    case JPEG_SUBSAMPLING_444:
        return "4:4:4";
    case JPEG_SUBSAMPLING_422:
        return "4:2:2";
    case JPEG_SUBSAMPLING_420:
        return "4:2:0";
    case JPEG_SUBSAMPLING_440:
        return "4:4:0";
    case JPEG_SUBSAMPLING_411:
        return "4:1:1";
    default:
        return "Unknown";
    }
}

bool jpeg_is_valid_for_eve(const uint8_t *buffer, uint32_t size)
{
    return jpeg_check_buffer(buffer, size, NULL) == JPEG_CHECK_OK;
}

void jpeg_print_info(const jpeg_info_t *info)
{
    if (info == NULL)
        return;

    printf("JPEG Info:\n");
    printf("  Dimensions: %u x %u\n", info->width, info->height);
    printf("  Precision:  %u bits\n", info->precision);
    printf("  Components: %u\n", info->num_components);
    printf("  Baseline:   %s\n", info->is_baseline ? "Yes" : "No");
    printf("  JFIF:       %s\n", info->is_jfif ? "Yes" : "No");
    printf("  Subsampling: %s\n", jpeg_subsampling_str(info->subsampling));
}

jpeg_check_result_t eve_validate_jpeg(const uint8_t *buffer, uint32_t size,
                                       jpeg_info_t *info, bool verbose)
{
    if (buffer == NULL || size < 4) {
        if (verbose) printf("ERROR: Invalid buffer (NULL or size < 4)\n");
        return JPEG_CHECK_INVALID_BUFFER;
    }

    jpeg_info_t local_info = {0};
    uint32_t pos = 0;
    uint16_t marker, length;
    bool found_soi = false;

    /* Find SOI marker (with tolerance up to 10KB for headers before JPEG data) */
    const int32_t MAX_SOI = 10 * 1024;
    while (pos < size - 1 && pos < MAX_SOI) {
        if (buffer[pos] == JPEG_MARKER_PREFIX && buffer[pos + 1] == JPEG_SOI) {
            local_info.data_start = pos;
            found_soi = true;
            if (verbose) printf("[%06u] SOI\n", pos);
            pos += 2;
            break;
        }
        pos++;
    }

    if (!found_soi) {
        if (verbose) printf("ERROR: No SOI marker found\n");
        return JPEG_CHECK_NO_SOI;
    }

    /* Validation tracking */
    uint32_t total_markers = 1;
    uint32_t invalid_markers = 0;
    bool has_jfif = false;
    bool has_sof0 = false;
    bool is_baseline = false;
    bool dqt_defined[4] = {false};
    bool dht_dc_defined[4] = {false};
    bool dht_ac_defined[4] = {false};
    uint8_t dqt_count = 0;
    uint8_t dht_count = 0;
    uint8_t comp_qt_table[4] = {0};
    uint8_t sos_comp_dc_table[4] = {0};
    uint8_t sos_comp_ac_table[4] = {0};
    uint8_t comp_h_sample[4] = {0};
    uint8_t comp_v_sample[4] = {0};

    /* Parse JPEG segments */
    while (pos < size - 1) {
        marker = (buffer[pos] << 8) | buffer[pos + 1];

        /* Check for valid marker prefix */
        if ((marker & 0xFF00) != 0xFF00) {
            if (verbose) printf("ERROR: Invalid marker prefix at %u\n", pos);
            return JPEG_CHECK_PARSE_ERROR;
        }

        /* Validate marker */
        if (!is_valid_marker(marker)) {
            if (verbose) printf("ERROR: Invalid marker 0x%04X at %u\n", marker, pos);
            invalid_markers++;
            return JPEG_CHECK_PARSE_ERROR;
        }

        total_markers++;

        /* Handle specific markers */
        if (marker == MARKER_APP0) {
            if (pos + 6 < size) {
                length = (buffer[pos + 2] << 8) | buffer[pos + 3];
                if (memcmp(&buffer[pos + 4], "JFIF", 4) == 0 || 
                    (pos + 6 < size && buffer[pos + 6] == 0x00 && memcmp(&buffer[pos + 4], "JFIF", 4) == 0)) {
                    has_jfif = true;
                    if (verbose) printf("[%06u] APP0 (JFIF)\n", pos);
                }
            }
        } else if (marker == MARKER_DQT) {
            length = (buffer[pos + 2] << 8) | buffer[pos + 3];
            if (pos + 4 < size) {
                uint8_t qt_id = buffer[pos + 4] & 0x0F;
                if (qt_id < 4) {
                    dqt_defined[qt_id] = true;
                    dqt_count++;
                }
                if (verbose) printf("[%06u] DQT (table %u)\n", pos, qt_id);
            }
        } else if (marker == MARKER_DHT) {
            length = (buffer[pos + 2] << 8) | buffer[pos + 3];

            /* Validate Huffman table */
            if (!validate_huffman_table(buffer, pos, length, verbose)) {
                if (verbose) printf("ERROR: Invalid Huffman table at %u\n", pos);
                return JPEG_CHECK_PARSE_ERROR;
            }

            if (pos + 4 < size) {
                uint8_t ht_info = buffer[pos + 4];
                uint8_t ht_id = ht_info & 0x0F;
                uint8_t ht_class = (ht_info >> 4) & 0x0F;
                if (ht_id < 4) {
                    if (ht_class == 0) {
                        dht_dc_defined[ht_id] = true;
                    } else {
                        dht_ac_defined[ht_id] = true;
                    }
                    dht_count++;
                }
            }
        } else if (marker == MARKER_SOF0) {
            has_sof0 = true;
            is_baseline = true;
            if (pos + 9 < size) {
                length = (buffer[pos + 2] << 8) | buffer[pos + 3];
                local_info.precision = buffer[pos + 4];
                local_info.height = (buffer[pos + 5] << 8) | buffer[pos + 6];
                local_info.width = (buffer[pos + 7] << 8) | buffer[pos + 8];
                local_info.num_components = buffer[pos + 9];

                if (verbose) {
                    printf("[%06u] SOF0 (Baseline DCT)\n", pos);
                    printf("        Dimensions: %ux%u, %u components, %u-bit\n",
                           local_info.width, local_info.height, 
                           local_info.num_components, local_info.precision);
                }

                if (local_info.width == 0 || local_info.height == 0) {
                    if (verbose) printf("ERROR: Invalid dimensions\n");
                    return JPEG_CHECK_PARSE_ERROR;
                }
                if (local_info.num_components == 0 || local_info.num_components > 4) {
                    if (verbose) printf("ERROR: Invalid component count\n");
                    return JPEG_CHECK_PARSE_ERROR;
                }

                /* Parse component info for subsampling */
                for (uint8_t i = 0; i < local_info.num_components && pos + 10 + i * 3 + 2 < size; i++) {
                    uint32_t cpos = pos + 10 + i * 3;
                    uint8_t sampling = buffer[cpos + 1];
                    comp_h_sample[i] = (sampling >> 4) & 0x0F;
                    comp_v_sample[i] = sampling & 0x0F;
                    comp_qt_table[i] = buffer[cpos + 2];

                    if (comp_h_sample[i] == 0 || comp_v_sample[i] == 0) {
                        if (verbose) printf("ERROR: Invalid sampling factors\n");
                        return JPEG_CHECK_PARSE_ERROR;
                    }
                }

                /* Determine subsampling type */
                if (local_info.num_components == 1) {
                    local_info.subsampling = JPEG_SUBSAMPLING_444;
                } else {
                    uint8_t y_sampling = (comp_h_sample[0] << 4) | comp_v_sample[0];
                    uint8_t cb_sampling = (comp_h_sample[1] << 4) | comp_v_sample[1];
                    uint8_t cr_sampling = (comp_h_sample[2] << 4) | comp_v_sample[2];
                    local_info.subsampling = get_subsampling_type(y_sampling, cb_sampling, cr_sampling);
                }

                if (verbose) {
                    printf("        Subsampling: %s\n", jpeg_subsampling_str(local_info.subsampling));
                }
            }
        } else if (marker == MARKER_SOF1 || marker == MARKER_SOF2) {
            /* Progressive or extended - not baseline */
            has_sof0 = true;
            is_baseline = false;
            if (verbose) printf("[%06u] SOF%u (Not baseline)\n", pos, marker == MARKER_SOF1 ? 1 : 2);
        } else if (marker == MARKER_SOS) {
            length = (buffer[pos + 2] << 8) | buffer[pos + 3];
            if (pos + 4 < size) {
                uint8_t sos_comps = buffer[pos + 4];
                for (uint8_t i = 0; i < sos_comps && pos + 5 + i * 2 + 1 < size; i++) {
                    uint32_t cpos = pos + 5 + i * 2;
                    uint8_t ht_sel = buffer[cpos + 1];
                    sos_comp_dc_table[i] = (ht_sel >> 4) & 0x0F;
                    sos_comp_ac_table[i] = ht_sel & 0x0F;
                }
            }
            if (verbose) printf("[%06u] SOS\n", pos);
        } else if (marker == MARKER_EOI) {
            local_info.data_size = pos + 2 - local_info.data_start;
            if (verbose) printf("[%06u] EOI\n", pos);
            break;
        } else if (verbose && marker == MARKER_APP0) {
            printf("[%06u] APP0\n", pos);
        }

        /* Move to next marker */
        pos += 2;

        if (!is_standalone_marker(marker)) {
            if (pos + 1 >= size) {
                if (verbose) printf("ERROR: Truncated at %u\n", pos);
                return JPEG_CHECK_PARSE_ERROR;
            }
            length = (buffer[pos] << 8) | buffer[pos + 1];
            if (length < 2) {
                if (verbose) printf("ERROR: Invalid segment length at %u\n", pos);
                return JPEG_CHECK_PARSE_ERROR;
            }

            /* For SOS, skip entropy-coded data */
            if (marker == MARKER_SOS) {
                pos += length;
                while (pos < size) {
                    if (buffer[pos] == 0xFF && pos + 1 < size && buffer[pos + 1] != 0x00) {
                        break;
                    }
                    pos++;
                }
            } else {
                pos += length;
            }
        }
    }

    /* Set final info flags */
    local_info.is_jfif = has_jfif;
    local_info.is_baseline = is_baseline;

    /* Validate required markers and tables */
    if (!has_sof0) {
        if (verbose) printf("ERROR: No SOF marker found\n");
        return JPEG_CHECK_NO_SOF;
    }

    if (!is_baseline) {
        if (verbose) printf("ERROR: Not baseline JPEG\n");
        return JPEG_CHECK_NOT_BASELINE;
    }

    /* Validate table references */
    for (uint8_t i = 0; i < local_info.num_components; i++) {
        if (!dqt_defined[comp_qt_table[i]]) {
            if (verbose) printf("ERROR: Missing QT table %u\n", comp_qt_table[i]);
            return JPEG_CHECK_PARSE_ERROR;
        }
        if (!dht_dc_defined[sos_comp_dc_table[i]]) {
            if (verbose) printf("ERROR: Missing DC Huffman table %u\n", sos_comp_dc_table[i]);
            return JPEG_CHECK_PARSE_ERROR;
        }
        if (!dht_ac_defined[sos_comp_ac_table[i]]) {
            if (verbose) printf("ERROR: Missing AC Huffman table %u\n", sos_comp_ac_table[i]);
            return JPEG_CHECK_PARSE_ERROR;
        }
    }

    /* Check subsampling compatibility with EVE */
    switch (local_info.subsampling) {
    case JPEG_SUBSAMPLING_444:
    case JPEG_SUBSAMPLING_422:
    case JPEG_SUBSAMPLING_420:
        /* Valid for EVE */
        break;

    case JPEG_SUBSAMPLING_440:
    case JPEG_SUBSAMPLING_411:
    case JPEG_SUBSAMPLING_UNKNOWN:
    default:
        if (verbose) {
            printf("ERROR: Unsupported subsampling %s for EVE\n",
                   jpeg_subsampling_str(local_info.subsampling));
        }
        return JPEG_CHECK_UNSUPPORTED_SUBSAMPLING;
    }

    if (invalid_markers > 0) {
        if (verbose) printf("ERROR: Found %u invalid markers\n", invalid_markers);
        return JPEG_CHECK_PARSE_ERROR;
    }

    /* Copy info if requested */
    if (info != NULL) {
        memcpy(info, &local_info, sizeof(jpeg_info_t));
    }

    if (verbose) {
        printf("\n========================================\n");
        printf("VALIDATION: PASSED\n");
        printf("Image: %ux%u, %u components, %u-bit\n",
               local_info.width, local_info.height,
               local_info.num_components, local_info.precision);
        printf("Subsampling: %s (EVE compatible)\n", jpeg_subsampling_str(local_info.subsampling));
        printf("Tables: DQT=%u, DHT=%u\n", dqt_count, dht_count);
        printf("Markers: %u total\n", total_markers);
        printf("========================================\n");
    }

    return JPEG_CHECK_OK;
}
