/**
 * @file app_camera_control.c
 * @brief Camera control functions for starting, stopping, pausing, and resuming video capture
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32h7xx_hal.h"
#include "i2c.h"
#include "dcmi.h"
#include "dma.h"
#include "tim.h"
#include "camera.h"

extern DMA_HandleTypeDef hdma_dcmi;
static uint32_t s_mode = 0, s_buffer_addr = 0, s_buffer_size = 0;

static void Camera_XCLK_Enable(void)
{
    Camera_XCLK_Set(XCLK_TIM);
}

static void Camera_XCLK_Disable(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void Camera_On(framesize_t frame_size, pixformat_t format, uint32_t mode, uint32_t buffer_addr, uint32_t buffer_size)
{
    Camera_Off();
    Camera_Reset();
    s_mode = mode;
    s_buffer_addr = buffer_addr;
    s_buffer_size = buffer_size;

    MX_DCMI_Init();
    Camera_Init_Device(&hi2c1, frame_size, format);
    HAL_DCMI_Start_DMA(&hdcmi, s_mode, s_buffer_addr, s_buffer_size / 4);
}

void Camera_Off(void)
{
    HAL_DCMI_Stop(&hdcmi);
    HAL_DMA_Abort(&hdma_dcmi);
}

/**
 * @brief Pause the camera capture.
 *
 * This function stops the camera capture and aborts the DMA transfer.
 */
void Camera_Pause(void)
{
    HAL_DCMI_Stop(&hdcmi);
    HAL_DMA_Abort(&hdma_dcmi);
}

void Camera_Resume(void)
{
    Camera_Pause();
    HAL_DCMI_Start_DMA(&hdcmi, s_mode, s_buffer_addr, s_buffer_size / 4);
}
void Camera_Start_Next(void)
{
    HAL_DCMI_Start_DMA(&hdcmi, s_mode, s_buffer_addr, s_buffer_size / 4);
}
