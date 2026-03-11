/**
 * @file app_hal_events.c
 * @brief HAL event callback implementations for DCMI events
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "dcmi.h"
#include "i2c.h"
#include "eve.h"

static void (*fp_HAL_DCMI_FrameEventCallback)(DCMI_HandleTypeDef *hdcmi) = NULL;
static void (*fp_HAL_DCMI_LineEventCallback)(DCMI_HandleTypeDef *hdcmi) = NULL;
static void (*fp_HAL_DCMI_DMAHalfCpltCallback)(DCMI_HandleTypeDef *hdcmi) = NULL;
static void (*fp_HAL_DCMI_DMACpltCallback)(DCMI_HandleTypeDef *hdcmi) = NULL;

/**
 * @brief Sets the callback function to be called when a frame event occurs
 * @param fp Pointer to the callback function
 */
void set_HAL_DCMI_FrameEventCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi))
{
    fp_HAL_DCMI_FrameEventCallback = fp;
}

/**
 * @brief Sets the callback function to be called when a line event occurs
 * @param fp Pointer to the callback function
 */
void set_HAL_DCMI_LineEventCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi))
{
    fp_HAL_DCMI_LineEventCallback = fp;
}

/**
 * @brief Sets the callback function to be called when an DMA half-completion occurs
 * @param fp Pointer to the callback function
 */
void set_HAL_DCMI_DMAHalfCpltCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi))
{
    fp_HAL_DCMI_DMAHalfCpltCallback = fp;
}

/**
 * @brief Sets the callback function to be called when an DMA completion occurs
 * @param fp Pointer to the callback function
 */
void set_HAL_DCMI_DMACpltCallback(void (*fp)(DCMI_HandleTypeDef *hdcmi))
{
    fp_HAL_DCMI_DMACpltCallback = fp;
}

/**
 * @brief This function is a callback for the DCMI HAL driver. It gets called when a frame event occurs.
 * @param hdcmi Pointer to the DCMI handle structure
 * @return Nothing
 * @note This function is only a wrapper for the user-provided callback function.
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (fp_HAL_DCMI_FrameEventCallback)
        return fp_HAL_DCMI_FrameEventCallback(hdcmi);
}

/**
 * @brief This function is a callback for the DCMI HAL driver. It gets called when a line event occurs.
 * @param hdcmi Pointer to the DCMI handle structure
 * @return Nothing
 * @note This function is only a wrapper for the user-provided callback function.
 */
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (fp_HAL_DCMI_LineEventCallback)
        return fp_HAL_DCMI_LineEventCallback(hdcmi);
}

/**
 * @brief This function is a callback for the DCMI HAL driver. It gets called when a DMA half-completion occurs.
 * @param hdcmi Pointer to the DCMI handle structure
 * @return Nothing
 * @note This function is only a wrapper for the user-provided callback function.
 */
void HAL_DCMI_DMAHalfCpltCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (fp_HAL_DCMI_DMAHalfCpltCallback)
        return fp_HAL_DCMI_DMAHalfCpltCallback(hdcmi);
}

/**
 * @brief This function is a callback for the DCMI HAL driver. It gets called when an DMA completion occurs.
 * @param hdcmi Pointer to the DCMI handle structure
 * @return Nothing
 * @note This function is only a wrapper for the user-provided callback function.
 */
void HAL_DCMI_DMACpltCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (fp_HAL_DCMI_DMACpltCallback)
        return fp_HAL_DCMI_DMACpltCallback(hdcmi);
}

/**
 * @brief This function is a callback for the DCMI HAL driver. It gets called when a DCMI error occurs.
 * @param hdcmi Pointer to the DCMI handle structure
 * @return Nothing
 * @note This function is only a wrapper for the user-provided callback function.
 */
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
    printf("DCMI ERROR\n");
}
