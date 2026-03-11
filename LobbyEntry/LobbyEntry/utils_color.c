/**
 * @file utils_color.c
 * @brief Color helpers for EVE display lists
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "eve.h"
#include "utils_color.h"

/**
 * @brief Set the color of the next EVE primitive using a packed 32-bit integer with reversed byte order (BGR).
 *        This function is a minimal wrapper for the COLOR_RGB EVE command.
 * @param phost The EVE HAL context.
 * @param color_int32_t The color value as a packed 32-bit integer (0xBGR), where B is the blue component, G is the green component, and R is the red component.
void set_color_reverse(EVE_HalContext *phost, uint32_t color_int32_t)
{
    EVE_Cmd_wr32(phost, COLOR_RGB(
                            (color_int32_t) & 0xFF,
                            ((color_int32_t) >> 8) & 0xFF,
                            ((color_int32_t) >> 16) & 0xFF));
}

/**
 * @brief Set the color of the next EVE primitive using RGB.
 *
 * @param phost The EVE HAL context.
 * @param color_int32_t The color value as a packed 32-bit integer (0xRRGGBB).
 *
 * This function is a minimal wrapper for the COLOR_RGB EVE command. It extracts
 * the red, green, and blue components from the input color value and passes them
 * directly to the COLOR_RGB command. The color value should be a packed 32-bit
 * integer in the format 0xRRGGBB, where RR is the red component, GG is the green
 * component, and BB is the blue component.
 */
void set_color_rgb(EVE_HalContext *phost, uint32_t color_int32_t)
{
    EVE_Cmd_wr32(phost, COLOR_RGB(
                            ((color_int32_t) >> 16) & 0xFF,
                            ((color_int32_t) >> 8) & 0xFF,
                            (color_int32_t) & 0xFF));
}
