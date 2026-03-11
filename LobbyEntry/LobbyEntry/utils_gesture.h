/**
 * @file utils_gesture.h
 * @brief Header file for gesture functions.
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_GESTURE_H
#define UTILS_GESTURE_H

#include <stdint.h>
#include "eve.h"
#include "utils_scrolling.h"

#define MAX_FINGER 5
#define SIGN(x) (((x) > 0) - ((x) < 0))

typedef struct
{
	uint16_t touchX;
	uint16_t touchY;
} Gesture_Finger_t;

typedef struct Gesture_Touch
{
	uint32_t tagTrackTouched;
	uint32_t tagFirstTap;
	uint32_t tagPressed;
	uint32_t tagPressed1;
	uint32_t tagReleased;
	uint32_t tagVelocity;
	uint32_t tagDragStart;
	uint32_t trackValLine;
	uint32_t trackValCircle;
	uint32_t longTapMilisecond;
	uint32_t doubleTapMilisecond;
	uint8_t isTouch;
	int8_t isTouch3x; // touch 3x continue
	uint8_t isSwipeX;
	uint8_t isSwipeY;
	uint8_t isSwipe;
	uint8_t isDoubleTapTag;
	uint8_t isDoubleTapXY;
	uint8_t isLongTap;
	uint16_t touchX;
	uint16_t touchY;
	uint16_t touchXStart;
	uint16_t touchYStart;
	int velocityX;
	int velocityY;
	int distance;
	int distanceX;
	int distanceY;

	int velocityX_total;
	int velocityY_total;

	uint8_t velocityStopRequest;

	Gesture_Finger_t finger[MAX_FINGER];
} Gesture_Touch_t;

Gesture_Touch_t *utils_gesture_renew(EVE_HalContext *phost);
Gesture_Touch_t *utils_gesture_get();

#endif /* UTILS_GESTURE_H */
