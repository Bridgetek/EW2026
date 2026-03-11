/**
 * @file utils_bitmap_handler.c
 * @brief Bitmap handler module to manage bitmap handles in BT820 EVE GUI Project
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "utils_bitmap_handler.h"

/* Bitmap handle counter. Valid range is MIN_HANDLER to MAX_HANDLER */
static uint32_t bitmap_handler_inc = MIN_HANDLER;

/**
 * utils_bitmap_handler_get() - Get next available bitmap handler
 *
 * Allocates and returns the next available bitmap handler ID.
 * Skips ROM font range (16-34) automatically.
 *
 * Returns: Next available handler ID, or BMHL_NONE if no more available
 */
uint32_t utils_bitmap_handler_get(void)
{
	if (bitmap_handler_inc > MAX_HANDLER)
		return BMHL_NONE;

	if (bitmap_handler_inc == ROM_FONT_START)
		bitmap_handler_inc = ROM_FONT_END + 1;

	bitmap_handler_inc++;

	return bitmap_handler_inc - 1;
}
