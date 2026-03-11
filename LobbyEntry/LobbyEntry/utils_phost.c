/**
 * @file utils_phost.c
 * @brief Provides utility functions for the EVE display host interface
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <string.h>
#include "eve.h"
#include "utils_eve.h"

#define MAX_HOST 10

EVE_HalContext g_hal_context;
EVE_HalContext *g_p_hal_context;
static EVE_HalContext *g_host_list[MAX_HOST] = {0};

/**
 * Initialize the EVE display host interface
 * @param phost EVE display host interface context to initialize
 *
 * Sets up the EVE display host interface with the specified context.
 * The context is stored in the g_host_list array and the latest
 * context is stored in g_p_hal_context.
 */
void utils_host_init(EVE_HalContext *phost)
{
	for (int i = 0; i < MAX_HOST; i++) {
		if (!g_host_list[i]) {
			g_host_list[i] = phost;
			break;
		}
	}
	g_p_hal_context = phost;
}

/**
 * @brief Deinitialize the EVE display host interface
 *
 * Resets the EVE display host interface by clearing the g_host_list array
 * and setting g_p_hal_context to NULL.
 */
void utils_host_deinit(void)
{
	memset(g_host_list, 0, sizeof(g_host_list));
	g_p_hal_context = NULL;
}
