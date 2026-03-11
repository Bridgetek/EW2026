/**
 * @file utils_phost.h
 * @brief Header file for EVE HAL context management utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef UTIL_PHOST_H_
#define UTIL_PHOST_H_

#include "eve.h"

extern EVE_HalContext g_hal_context;
extern EVE_HalContext *g_p_hal_context;

#define PHOST g_p_hal_context

void utils_host_init(EVE_HalContext *phost);
void utils_host_deinit(void);

#endif