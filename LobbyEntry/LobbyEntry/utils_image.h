/**
 * @file utils_image.h
 * @brief Image processing utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_IMAGE_H
#define UTILS_IMAGE_H

#include "eve.h"

#define Load_PNG Load_Image
#define BMHL_AUTO 99

typedef enum { IMG_FMT_PNG = 0, IMG_FMT_JPEG, IMG_FMT_RAW } Image_Format;

typedef struct {
    uint32_t tagval;
    uint8_t *sd_path;
    uint16_t x, y, w, h;
    Image_Format fmt;
    uint16_t bitmap_format;
    int16_t bitmap_handler;
    uint16_t opt;
    bool is_loaded;
    uint32_t ptr;
    bool isOnFlash;
    uint32_t scale;
    char *name;
} Image;

uint32_t utils_load_image_from_sd_card(EVE_HalContext *phost, Image *image);
uint32_t utils_draw_image_xy(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y);
uint32_t utils_draw_image_xy_tag(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y, uint32_t tag);
uint32_t utils_draw_image(EVE_HalContext *phost, Image *image);
uint32_t utils_draw_image_center(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void utils_draw_back_button();

#endif
