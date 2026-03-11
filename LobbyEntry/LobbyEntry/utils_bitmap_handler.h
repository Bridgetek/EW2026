/**
 * @file utils_bitmap_handler.h
 * @brief Interface for bitmap handler management
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_BITMAP_HANDLER_H
#define UTILS_BITMAP_HANDLER_H

#include "stdint.h"
/* Auto handler allocation - only a few handlers available 0-15 */
#define BMHL_AUTO	99
/* No handler allocated */
#define BMHL_NONE	98

/* Handler range definitions */
#define MAX_HANDLER	63	/* Vertex2II uses 0-31 handle only */
#define MIN_HANDLER	6
#define ROM_FONT_START	15	/* ROM fonts use handlers 16-34, Scatch uses 15 */
#define ROM_FONT_END	34

/**
 * IS_BITMAP_HANDLER_VALID() - Check if bitmap handler value is valid
 * @val: Handler value to check
 *
 * Returns: true if valid handler, false otherwise
 */
#define IS_BITMAP_HANDLER_VALID(val) \
	(((val) >= 0 && (val) <= MAX_HANDLER) || \
	 ((val) >= ROM_FONT_START && (val) <= ROM_FONT_END))

/**
 * SET_BITMAP_HANDLE() - Set bitmap handle if valid
 * @phost: EVE host context
 * @val: Handler value to set
 *
 * Only sets the bitmap handle if the value is valid.
 */
#define SET_BITMAP_HANDLE(phost, val)		\
	do {					\
		if (IS_BITMAP_HANDLER_VALID(val))	\
			EVE_Cmd_wr32(phost, BITMAP_HANDLE(val));	\
	} while (0)

/**
 * utils_bitmap_handler_get() - Get next available bitmap handler
 *
 * Returns: Next available handler ID, or BMHL_NONE if none available
 */
uint32_t utils_bitmap_handler_get(void);

#endif /* UTILS_BITMAP_HANDLER_H */
