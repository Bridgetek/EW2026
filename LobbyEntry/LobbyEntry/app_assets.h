/**
 * @file app_assets.h
 * @brief Application assets header with image metadata definitions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef APP_ASSETS_H
#define APP_ASSETS_H

#include "utils_image.h"

#define IMG_DIR "assets_sdcard_eve/Lobby-entry-ew2026/img/"

extern Image demo_images[];
#define IMG_COUNT 26
#define ICON_BACKGROUND (&demo_images[0])
#define ICON_DIGIT_0 (&demo_images[1])
#define ICON_DIGIT_1 (&demo_images[2])
#define ICON_DIGIT_2 (&demo_images[3])
#define ICON_DIGIT_3 (&demo_images[4])
#define ICON_DIGIT_4 (&demo_images[5])
#define ICON_DIGIT_5 (&demo_images[6])
#define ICON_DIGIT_6 (&demo_images[7])
#define ICON_DIGIT_7 (&demo_images[8])
#define ICON_DIGIT_8 (&demo_images[9])
#define ICON_DIGIT_9 (&demo_images[10])
#define ICON_GREEN_CALL (&demo_images[11])
#define ICON_RED_HANGUP (&demo_images[12])
#define ICON_DIALER (&demo_images[13])
#define ICON_HELP (&demo_images[14])
#define ICON_QR_CODE_SCANNER (&demo_images[15])
#define ICON_DIRECTORY (&demo_images[16])
#define ICON_BACKWARD (&demo_images[17])
#define ICON_BACKSPACE (&demo_images[18])
#define ICON_ARROW_CLOCKWISE_EN (&demo_images[19])
#define ICON_ARROW_LEFT_RIGHT_EN (&demo_images[20])
#define ICON_ARROWS_ANGLE_EXPAND_EN (&demo_images[21])
#define ICON_CAMERA_VIDEO_FILL_EN (&demo_images[22])
#define ICON_CAMERA_VIDEO_OFF_FILL_EN (&demo_images[23])
#define ICON_ZOOM_IN_EN (&demo_images[24])
#define ICON_ZOOM_OUT_EN (&demo_images[25])

#endif // APP_ASSETS_H
