/**
 * @file app_camera_control.h
 * @brief Camera control functions for starting, stopping, pausing, and resuming video capture
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef APP_CAMERA_CONTROL_H
#define APP_CAMERA_CONTROL_H
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32h7xx_hal.h"
#include "dcmi.h"
#include "dma.h"
#include "tim.h"
#include "camera.h"

void Camera_On(framesize_t frame_size, pixformat_t format, uint32_t mode, uint32_t buffer_addr, uint32_t buffer_size);
void Camera_Off(void);
void Camera_Pause(void);
void Camera_Resume(void);
void Camera_Start_Next(void);

#endif /* APP_CAMERA_CONTROL_H */
