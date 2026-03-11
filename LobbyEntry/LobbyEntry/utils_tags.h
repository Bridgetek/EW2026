/**
 * @file utils_tags.h
 * @brief Header file for tag allocation and management
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTIL_TAGS_H
#define UTIL_TAGS_H

#include <stdint.h>

#define TAG_MIN  1
#define TAG_MAX  16777215
#define TAG_INC  (TAG_MAX + 1)
#define TAG_SAME (TAG_INC + 1)
#define TAG_NONE 0

int utils_tag_alloc(uint32_t val);
uint32_t utils_tag_next(void);

#endif