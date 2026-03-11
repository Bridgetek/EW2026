/**
 * @file utils_vertex_adapter.c
 * @brief Vertex adapter functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "eve.h"
#include "utils_vertex_adapter.h"

void (*g_utils_adapter_vertex2f)(EVE_HalContext *phost, int32_t x, int32_t y);

static void vertex2f_1(EVE_HalContext *phost, int32_t x, int32_t y) { EVE_Cmd_wr32(phost, VERTEX2F(x, y)); }
static void vertex2f_2(EVE_HalContext *phost, int32_t x, int32_t y) { EVE_Cmd_wr32(phost, VERTEX2F(x * 2, y * 2)); }
static void vertex2f_4(EVE_HalContext *phost, int32_t x, int32_t y) { EVE_Cmd_wr32(phost, VERTEX2F(x * 4, y * 4)); }
static void vertex2f_8(EVE_HalContext *phost, int32_t x, int32_t y) { EVE_Cmd_wr32(phost, VERTEX2F(x * 8, y * 8)); }
static void vertex2f_16(EVE_HalContext *phost, int32_t x, int32_t y) { EVE_Cmd_wr32(phost, VERTEX2F(x * 16, y * 16)); }

/**
 * @brief Set vertex format and install corresponding scaling function.
 */
void utils_vertex_format(EVE_HalContext *phost, uint32_t format)
{
    static void (*g_vertex_functions[])(EVE_HalContext *, int32_t, int32_t) = {
        vertex2f_1, vertex2f_2, vertex2f_4, vertex2f_8, vertex2f_16};
    if (!phost || format >= 5)
        return;

    g_utils_adapter_vertex2f = g_vertex_functions[format];
    EVE_Cmd_wr32(phost, VERTEX_FORMAT(format));
}
