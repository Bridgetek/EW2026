/**
 * @file utils_eve.h
 * @brief Header file for EVE display controller utility functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef UTILS_EVE_H
#define UTILS_EVE_H

#include <stdint.h>

#include "eve.h"

#include "utils_bitmap_handler.h"
#include "utils_ddr.h"
#include "utils_gesture.h"
#include "utils_image.h"
#include "utils_paging.h"
#include "utils_phost.h"
#include "utils_tags.h"

/* Min/Max macros */
#ifndef MIN
/** @brief Return minimum of two values */
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif
#ifndef MAX
/** @brief Return maximum of two values */
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

void utils_eve_init(EVE_HalContext *phost);
void utils_eve_release(EVE_HalContext *phost);

void utils_eve_scale_and_translate_to(int32_t im_w, int32_t im_h, int32_t target_w, int32_t target_h, int32_t translate_x, int32_t translate_y, uint32_t format);
void utils_eve_scale_to(int32_t im_w, int32_t im_h, int32_t target_w, int32_t target_h, uint32_t format, int32_t *translated_x, int32_t *translated_y);
void utils_eve_scale(int percent);
bool utils_eve_calibrate(EVE_HalContext *phost);
void utils_display_start(EVE_HalContext *phost);
void utils_display_start_color(EVE_HalContext *phost, uint8_t *bgColor, uint8_t *textColor);
void utils_display_end(EVE_HalContext *phost);

void draw_circle_with_text(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
										 const char *txt, int32_t font, uint16_t opt, uint32_t txtcolor);
void draw_circle_with_text_tag(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
    const char *txt, int32_t font, uint16_t opt, uint32_t txtcolor, uint8_t tagval);
void draw_line(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h);
void draw_line_xy(EVE_HalContext *phost, int32_t x, int32_t y, int32_t x2, int32_t y2);

#endif // UTILS_EVE_H
