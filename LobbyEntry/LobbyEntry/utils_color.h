/**
 * @file utils_color.h
 * @brief Header file for color utility functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_COLOR_H
#define UTILS_COLOR_H

#include <stdint.h>
#include "eve.h"

/**
 * @brief Set COLOR_RGB with reversed byte order (B,G,R).
 */
void set_color_reverse(EVE_HalContext *phost, uint32_t color_int32_t);

/**
 * @brief Set COLOR_RGB from 0xRRGGBB.
 */
void set_color_rgb(EVE_HalContext *phost, uint32_t color_int32_t);

/* Color macros - Standard web colors */
#define SET_COLOR_BLACK(phost) set_color_rgb(phost, 0x000000)
#define SET_COLOR_SILVER(phost) set_color_rgb(phost, 0xC0C0C0)
#define SET_COLOR_GRAY(phost) set_color_rgb(phost, 0x808080)
#define SET_COLOR_WHITE(phost) set_color_rgb(phost, 0xFFFFFF)
#define SET_COLOR_MAROON(phost) set_color_rgb(phost, 0x800000)
#define SET_COLOR_RED(phost) set_color_rgb(phost, 0xFF0000)
#define SET_COLOR_PURPLE(phost) set_color_rgb(phost, 0x800080)
#define SET_COLOR_FUCHSIA(phost) set_color_rgb(phost, 0xFF00FF)
#define SET_COLOR_GREEN(phost) set_color_rgb(phost, 0x008000)
#define SET_COLOR_LIME(phost) set_color_rgb(phost, 0x00FF00)
#define SET_COLOR_OLIVE(phost) set_color_rgb(phost, 0x808000)
#define SET_COLOR_YELLOW(phost) set_color_rgb(phost, 0xFFFF00)
#define SET_COLOR_NAVY(phost) set_color_rgb(phost, 0x000080)
#define SET_COLOR_BLUE(phost) set_color_rgb(phost, 0x0000FF)
#define SET_COLOR_TEAL(phost) set_color_rgb(phost, 0x008080)
#define SET_COLOR_AQUA(phost) set_color_rgb(phost, 0x00FFFF)
#define SET_COLOR_ORANGE(phost) set_color_rgb(phost, 0xFFA500)
#define SET_COLOR_ALICEBLUE(phost) set_color_rgb(phost, 0xF0F8FF)
#define SET_COLOR_ANTIQUEWHITE(phost) set_color_rgb(phost, 0xFAEBD7)
#define SET_COLOR_AQUAMARINE(phost) set_color_rgb(phost, 0x7FFFD4)
#define SET_COLOR_AZURE(phost) set_color_rgb(phost, 0xF0FFFF)
#define SET_COLOR_BEIGE(phost) set_color_rgb(phost, 0xF5F5DC)
#define SET_COLOR_BISQUE(phost) set_color_rgb(phost, 0xFFE4C4)
#define SET_COLOR_BLANCHEDALMOND(phost) set_color_rgb(phost, 0xFFEBCD)
#define SET_COLOR_BLUEVIOLET(phost) set_color_rgb(phost, 0x8A2BE2)
#define SET_COLOR_BROWN(phost) set_color_rgb(phost, 0xA52A2A)
#define SET_COLOR_BURLYWOOD(phost) set_color_rgb(phost, 0xDEB887)
#define SET_COLOR_CADETBLUE(phost) set_color_rgb(phost, 0x5F9EA0)
#define SET_COLOR_CHARTREUSE(phost) set_color_rgb(phost, 0x7FFF00)
#define SET_COLOR_CHOCOLATE(phost) set_color_rgb(phost, 0xD2691E)

#endif // UTILS_COLOR_H
