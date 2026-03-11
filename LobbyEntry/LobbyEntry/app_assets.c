/**
 * @file app_assets.c
 * @brief Application assets table and image metadata
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include "utils_bitmap_handler.h"
#include "utils_tags.h"
#include "utils_image.h"
#include "app_assets.h"

#define im00 "00_background_1200x1920_ASTC_8X8.raw"
#define im01 "01_digit_0_256x256_ASTC_4X4.raw"
#define im02 "02_digit_1_256x256_ASTC_4X4.raw"
#define im03 "03_digit_2_256x256_ASTC_4X4.raw"
#define im04 "04_digit_3_256x256_ASTC_4X4.raw"
#define im05 "05_digit_4_256x256_ASTC_4X4.raw"
#define im06 "06_digit_5_256x256_ASTC_4X4.raw"
#define im07 "07_digit_6_256x256_ASTC_4X4.raw"
#define im08 "08_digit_7_256x256_ASTC_4X4.raw"
#define im09 "09_digit_8_256x256_ASTC_4X4.raw"
#define im10 "10_digit_9_256x256_ASTC_4X4.raw"
#define im11 "11_green_Call_256x256_ASTC_4X4.raw"
#define im12 "12_red_Hangup_200x200_ASTC_4X4.raw"
#define im13 "13_dialer_256x256_ASTC_4X4.raw"
#define im14 "14_help_256x256_ASTC_4X4.raw"
#define im15 "15_qr_code_scanner_256x256_ASTC_4X4.raw"
#define im16 "16_directory_256x256_ASTC_4X4.raw"
#define im17 "17_backward_128x128_ASTC_4X4.raw"
#define im18 "18_backspace_128x128_ASTC_4X4.raw"
#define im19 "19_arrow-clockwise_en_72x72_ASTC_4X4.raw"
#define im20 "20_arrow-left-right_en_72x72_ASTC_4X4.raw"
#define im21 "21_arrows-angle-expand_en_72x72_ASTC_4X4.raw"
#define im22 "22_camera-video-fill_en_72x72_ASTC_4X4.raw"
#define im23 "23_camera-video-off-fill_en_72x72_ASTC_4X4.raw"
#define im24 "24_zoom-in_en_72x72_ASTC_4X4.raw"
#define im25 "25_zoom-out_en_72x72_ASTC_4X4.raw"

Image demo_images[IMG_COUNT] = {
    /* 00 */ {.sd_path = IMG_DIR im00, .w = 1200, .h = 1920, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_8x8_KHR, .fmt = IMG_FMT_RAW},
    /* 01 */ {.sd_path = IMG_DIR im01, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 02 */ {.sd_path = IMG_DIR im02, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 03 */ {.sd_path = IMG_DIR im03, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 04 */ {.sd_path = IMG_DIR im04, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 05 */ {.sd_path = IMG_DIR im05, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 06 */ {.sd_path = IMG_DIR im06, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 07 */ {.sd_path = IMG_DIR im07, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 08 */ {.sd_path = IMG_DIR im08, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 09 */ {.sd_path = IMG_DIR im09, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 10 */ {.sd_path = IMG_DIR im10, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 11 */ {.sd_path = IMG_DIR im11, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 12 */ {.sd_path = IMG_DIR im12, .w = 200, .h = 200, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 13 */ {.sd_path = IMG_DIR im13, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 14 */ {.sd_path = IMG_DIR im14, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 15 */ {.sd_path = IMG_DIR im15, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 16 */ {.sd_path = IMG_DIR im16, .w = 256, .h = 256, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 17 */ {.sd_path = IMG_DIR im17, .w = 128, .h = 128, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 18 */ {.sd_path = IMG_DIR im18, .w = 128, .h = 128, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 19 */ {.sd_path = IMG_DIR im19, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 20 */ {.sd_path = IMG_DIR im20, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 21 */ {.sd_path = IMG_DIR im21, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 22 */ {.sd_path = IMG_DIR im22, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 23 */ {.sd_path = IMG_DIR im23, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 24 */ {.sd_path = IMG_DIR im24, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
    /* 25 */ {.sd_path = IMG_DIR im25, .w = 72, .h = 72, .bitmap_handler = BMHL_NONE, .tagval = TAG_INC, .bitmap_format = COMPRESSED_RGBA_ASTC_4x4_KHR, .fmt = IMG_FMT_RAW},
};
