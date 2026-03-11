/**
 * @file app_hal_events.h
 * @brief HAL event callback declarations for DCMI events
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef APP_HAL_EVENTS_H
#define APP_HAL_EVENTS_H
#include "dcmi.h"

void set_HAL_DCMI_FrameEventCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi));
void set_HAL_DCMI_LineEventCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi));
void set_HAL_DCMI_DMAHalfCpltCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi));
void set_HAL_DCMI_DMACpltCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi));

#endif