/**
    @file eve_example.h
 **/
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 @brief Definitions of handles for custom fonts and bitmaps.
 */
//@{
#define FONT_LARGE 1
#define FONT_MEDIUM 2
#define FONT_SMALL 3
#define BITMAP_HEART 4
#define BITMAP_GRAPH 5
#define BITMAP_GRAD 6
#define BITMAP_FINGER 7
#define BITMAP_SETTINGS 8
#define BITMAP_ROTATE 9
#define BITMAP_VOLUME 10

//@}

/**
 @brief Definitions of TAG values for tagged buttons.
 */
//@{
#define SETTINGS_TAG 1
#define BRIGHTNESS_TAG 2
#define SCREEN_ROTATE_TAG 3
#define MUTE_VOLUME_TAG 4
//@}

/* Globals available within the eve_example code */

/* Functions called within the eve_example code */
uint32_t eve_display_load_assets(void);
uint32_t platform_get_time(void);

/* Entry point to the example code */
void eve_example(void);

#include "SparkFun_Bio_Sensor_Hub_Library.h"
#include "touch.h"
#include "maths/trig_furman.h"

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
