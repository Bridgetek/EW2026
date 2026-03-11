/**
 * @file utils_vertex_adapter.h
 * @brief Header file for vertex adapter functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_VERTEX_ADAPTER_H
#define UTILS_VERTEX_ADAPTER_H

#include "eve.h"

/**
 * @brief Function pointer to scaled VERTEX2F emitter.
 */
extern void (*g_utils_adapter_vertex2f)(EVE_HalContext *phost, int32_t x, int32_t y);

/**
 * @brief Set vertex format and install corresponding scaling function.
 * @param phost EVE context
 * @param format Vertex format (0, 1, 2, 3, 4)
 */
void utils_vertex_format(EVE_HalContext *phost, uint32_t format);

#endif
