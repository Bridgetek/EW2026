/**
 * @file utils_tags.c
 * @brief Tag allocation and management
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "eve.h"
#include "utils_tags.h"

static uint32_t g_tag_counter = 500;

/**
 * @brief Allocate a tag value for a widget or asset
 * @param val One of TAG_SAME, TAG_INC, or a custom value
 * @return Allocated tag value, 0 if TAG_MAX is reached
 *
 * TAG_SAME returns the current tag value, TAG_INC increments the tag value and returns the new one, and a custom value is returned directly.
 * This function is not thread-safe and should only be used during UI initialization.
 */
int utils_tag_alloc(uint32_t val)
{
    if (g_tag_counter >= TAG_MAX) return 0;
    if (val == TAG_SAME) return g_tag_counter;
    if (val == TAG_INC) return utils_tag_next();
    return (int)val;
}

/**
 * @brief Increment and return the current tag value
 *
 * This function increments the current tag value and returns the new one.
 * It is not thread-safe and should only be used during UI initialization.
 *
 * @return The next tag value, wrapping around to 0 if TAG_MAX is reached
 */
uint32_t utils_tag_next(void)
{
    return ++g_tag_counter;
}