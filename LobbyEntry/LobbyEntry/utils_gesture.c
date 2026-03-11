/**
 * @file utils_gesture.c
 * @brief Gesture utility functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

/* Standard library includes */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

/* EVE platform and utility includes */
#include "eve.h"
#include "utils_gesture.h"
#include "utils_phost.h"
#include "utils_scrolling.h"

/* =============================================================================
 * Timing and distance threshold constants
 * =============================================================================
 */
#define MILLIS()                  (EVE_millis() / 1000)     /* Convert to milliseconds */
#define DOUBLE_TAP_THRESHOLD      500                       /* Max time between taps (ms) */
#define MAX_TAP_DISTANCE          50                        /* Max movement for tap (pixels) */
#define EVE_XY_RESET              0x80008000               /* EVE no-touch state */
#define SPEED_MULTIPLIER          5                         /* Velocity amplification factor */
#define MIN_VELOCITY_THRESHOLD    2                         /* Minimum velocity to maintain */
#define MIN_MOVE_DISTANCE         15                        /* Minimum movement for swipe */
#define MIN_TIME_MS               15                        /* Minimum time between calculations */
#define DEFAULT_DECELERATION      5                         /* Default deceleration percentage */
#define RESET_Y_THRESHOLD         200                       /* Y movement threshold for reset */
#define TOUCH_CONFIDENCE_MAX      3                         /* Maximum touch confidence level */
#define DOUBLE_TAP_DEFAULT_MS     200                       /* Default double tap threshold */
#define LONG_TAP_DEFAULT_MS       800                       /* Default long tap threshold */
#define FULL_CIRCLE_DEGREES       360                       /* Degrees in full circle */
#define TRACKER_SCALE             65535                     /* Tracker value scale */

/**
 * @brief Structure for tracking gesture axis data
 *
 * Contains velocity calculation state and configuration for a single axis
 * (X or Y) of touch movement. Manages velocity computation, deceleration,
 * swipe detection, and timing information for smooth gesture processing.
 */
typedef struct {
	int32_t  velocity;           /**< Current calculated velocity */
	int32_t  prevTouch;          /**< Previous touch position for delta calculation */
	uint8_t  *swipeFlag;         /**< Pointer to swipe detection flag */
	uint8_t  stopRequest;        /**< Flag to immediately stop velocity */
	int32_t  velocityTotal;      /**< Accumulated velocity for tracking */
	uint16_t delay_ms;           /**< Timing delay accumulator in milliseconds */
	int32_t  lastTouch;          /**< Last recorded touch position */
	uint32_t prev_time;          /**< Previous timestamp for timing calculations */
	int32_t  singleTouchCount;   /**< Counter for single touch events */
	uint32_t initialTag;         /**< Tag when gesture started */
	uint8_t  deceleration;       /**< Deceleration factor (0-100 percent) */
	int32_t  distance_velocity;  /**< Distance-based velocity accumulator */
} gesture_axis_t;

/* =============================================================================
 * Global state variables
 * =============================================================================
 */

/* Main gesture state structure */
static Gesture_Touch_t g_gesture;

/* X-axis gesture tracking */
static gesture_axis_t g_gestureX = {
	.deceleration = DEFAULT_DECELERATION,
	.swipeFlag = &g_gesture.isSwipeX
};

/* Y-axis gesture tracking */
static gesture_axis_t g_gestureY = {
	.deceleration = DEFAULT_DECELERATION,
	.swipeFlag = &g_gesture.isSwipeY
};

/**
 * @brief Calculate velocity for gesture axis with deceleration support
 *
 * Computes touch velocity based on position changes over time intervals.
 * Handles swipe initiation detection, velocity calculation during active
 * touch, and deceleration when touch is released. Implements timing-based
 * filtering to ensure smooth velocity calculations.
 *
 * @param axis Pointer to gesture axis structure containing state
 * @param touchPos Current touch position on the axis (pixels)
 * @return Calculated velocity value (velocity units)
 *
 * @note Function maintains internal timing state and applies deceleration
 *       when touch is released. Swipe detection requires minimum movement
 *       threshold to be exceeded.
 */
static int32_t get_velocity(gesture_axis_t *axis, int32_t touchPos)
{
	uint32_t current_time;
	int32_t moveDistance;

	current_time = EVE_millis();

	/* Update timing information and filter for minimum time interval */
	if (axis->prev_time != 0)
		axis->delay_ms += (current_time - axis->prev_time);
	axis->prev_time = current_time;

	/* Skip calculation if insufficient time has passed for accuracy */
	if (axis->delay_ms < MIN_TIME_MS)
		return axis->velocity;
	axis->delay_ms = 0;

	if (g_gesture.isTouch) {
		axis->stopRequest = 0;

		/* Handle swipe initiation detection */
		if (!*axis->swipeFlag) {
			if (!axis->lastTouch && g_gesture.tagPressed) {
				/* Record initial touch position and tag */
				axis->lastTouch = touchPos;
				axis->initialTag = g_gesture.tagPressed;
			} else if (axis->lastTouch) {
				moveDistance = touchPos - axis->lastTouch;
				g_gesture.tagVelocity = axis->initialTag;

				/* Detect swipe if movement exceeds threshold */
				if (abs(moveDistance) > MIN_MOVE_DISTANCE)
					*axis->swipeFlag = 1;
			}
		}
	} else {
		/* Reset state when touch is released */
		axis->lastTouch = 0;
		*axis->swipeFlag = 0;
	}

	/* Calculate velocity based on current state */
	if (*axis->swipeFlag) {
		/* Active swipe: calculate velocity from position delta */
		axis->velocity = (touchPos - axis->prevTouch) << SPEED_MULTIPLIER;
		axis->distance_velocity += abs(axis->velocity);
	} else if (axis->stopRequest) {
		/* Stop requested: zero velocity immediately */
		axis->velocity = 0;
	} else if (!g_gesture.isTouch) {
		/* Touch released: apply deceleration curve */
		if (axis->deceleration >= 100) {
			axis->velocity = 0;
		} else if (axis->deceleration > 0) {
			axis->velocity = (axis->velocity * (100 - axis->deceleration)) / 100;
			/* Apply minimum velocity threshold */
			if (abs(axis->velocity) < MIN_VELOCITY_THRESHOLD)
				axis->velocity = 0;
		}
		axis->velocityTotal += axis->velocity;
		axis->distance_velocity += abs(axis->velocity);
	} else {
		/* Touch present but no swipe: reset velocity counters */
		if (++axis->singleTouchCount > 0) {
			axis->singleTouchCount = 0;
			axis->velocity = 0;
			axis->velocityTotal = 0;
			axis->distance_velocity = 0;
		}
	}

	axis->prevTouch = touchPos;
	return axis->velocity;
}

/**
 * @brief Get X-axis velocity with current touch position
 *
 * Wrapper function to calculate X-axis velocity using current touch X coordinate.
 * Provides convenient access to horizontal velocity calculations.
 *
 * @return Current X-axis velocity value in velocity units
 */
static int32_t get_velocity_x(void)
{
	return get_velocity(&g_gestureX, g_gesture.touchX);
}

/**
 * @brief Get Y-axis velocity with current touch position
 *
 * Wrapper function to calculate Y-axis velocity using current touch Y coordinate.
 * Provides convenient access to vertical velocity calculations.
 *
 * @return Current Y-axis velocity value in velocity units
 */
static int32_t get_velocity_y(void)
{
	return get_velocity(&g_gestureY, g_gesture.touchY);
}

/**
 * @brief Detect tag release transition events
 *
 * Tracks tag state transitions to identify when a tag changes from
 * pressed to released state. Maintains internal state to detect the
 * exact moment when a tag is no longer being touched.
 *
 * @return Tag ID that was just released, 0 if no release detected
 *
 * @note Uses static variable to maintain state between calls.
 *       Returns tag only once per release event.
 */
static uint32_t get_tag_released(void)
{
	static uint32_t g_old_tag = 0;
	uint32_t new_tag;
	uint32_t released_tag;

	new_tag = g_gesture.tagPressed;
	released_tag = 0;

	/* Detect tag release transition: was pressed, now not pressed */
	if (!new_tag && g_old_tag) {
		released_tag = g_old_tag;
		g_old_tag = 0;
	} else if (new_tag) {
		g_old_tag = new_tag;
	}

	return released_tag;
}

/**
 * @brief Detect long tap gesture with movement tolerance
 *
 * Determines if the current touch qualifies as a long tap by checking
 * duration against threshold and ensuring movement stays within tolerance.
 * Implements stateful tracking to detect sustained touch events.
 *
 * @return true if long tap detected, false otherwise
 *
 * @note Long tap is detected when touch duration exceeds threshold and
 *       movement remains within MAX_TAP_DISTANCE from initial point.
 */
static bool is_long_tap(void)
{
	static uint32_t g_long_tap_start_time = 0;
	static uint32_t g_first_tap_point_x = 0;
	static uint32_t g_first_tap_point_y = 0;
	uint32_t time;
	uint32_t x, y, distance;
	bool long_tap_detected;

	time = MILLIS();
	long_tap_detected = false;

	if (g_gesture.isTouch) {
		x = g_gesture.touchX;
		y = g_gesture.touchY;

		/* Calculate squared distance from initial touch point */
		distance = (g_first_tap_point_x - x) * (g_first_tap_point_x - x) +
			   (g_first_tap_point_y - y) * (g_first_tap_point_y - y);

		/* Update current touch position for next calculation */
		g_first_tap_point_x = x;
		g_first_tap_point_y = y;

		if (!g_long_tap_start_time) {
			/* Start timing the tap */
			g_long_tap_start_time = time;
		} else if ((time - g_long_tap_start_time > g_gesture.longTapMilisecond) &&
			   (distance <= MAX_TAP_DISTANCE * MAX_TAP_DISTANCE)) {
			/* Long tap detected: sufficient time with minimal movement */
			long_tap_detected = true;
		}
	} else {
		/* Touch released: reset timing state */
		g_long_tap_start_time = 0;
	}

	return long_tap_detected;
}

/**
 * @brief Detect double tap gestures on the same tagged element
 *
 * Tracks tap sequences on tagged UI elements to detect double taps occurring
 * on the same tag within the specified time threshold. Implements state machine
 * logic to handle tap-release-tap sequences with proper timeout handling.
 *
 * @return true if double tap on same tag detected, false otherwise
 *
 * @note Requires two taps on the same tag within DOUBLE_TAP_THRESHOLD time.
 *       Automatically resets state on timeout or tag mismatch.
 */
static bool is_double_tap_tag(void)
{
	static bool g_first_tap_detected = false;
	static bool g_tap_and_release_detected = false;
	static uint32_t g_first_tap_time = 0;
	static uint32_t g_first_tap_tag = 0;
	uint32_t current_time;
	uint32_t tag;
	uint32_t time_since_first_tap;

	current_time = MILLIS();
	tag = g_gesture.tagPressed;

	if (g_gesture.isTouch && tag) {
		if (!g_first_tap_detected) {
			/* First tap detected on tagged element */
			g_first_tap_detected = true;
			g_tap_and_release_detected = false;
			g_first_tap_time = current_time;
			g_first_tap_tag = tag;
		} else if (g_tap_and_release_detected) {
			/* Second tap detected - validate timing and tag match */
			time_since_first_tap = current_time - g_first_tap_time;
			if ((time_since_first_tap <= DOUBLE_TAP_THRESHOLD) &&
			    (g_first_tap_tag == tag)) {
				/* Valid double tap detected */
				g_tap_and_release_detected = false;
				g_first_tap_detected = false;
				return true;
			} else if (time_since_first_tap > DOUBLE_TAP_THRESHOLD) {
				/* Timeout: reset state machine */
				g_tap_and_release_detected = false;
				g_first_tap_detected = false;
			}
		}
	} else {
		/* Touch released or no tag - handle timeout reset */
		if (g_first_tap_detected &&
		    ((current_time - g_first_tap_time) > DOUBLE_TAP_THRESHOLD)) {
			/* Timeout: reset state machine */
			g_first_tap_detected = false;
			g_tap_and_release_detected = false;
		}
		/* Mark release after first tap */
		if (g_first_tap_detected)
			g_tap_and_release_detected = true;
	}

	return false;
}

/**
 * @brief Detect double tap gestures at the same screen location
 *
 * Tracks tap sequences at specific coordinates to detect double taps occurring
 * at the same location within time and distance thresholds. Implements precise
 * location-based double tap detection independent of UI element tags.
 *
 * @return true if double tap at same location detected, false otherwise
 *
 * @note Requires two taps within DOUBLE_TAP_THRESHOLD time and MAX_TAP_DISTANCE
 *       from each other. Uses squared distance calculation for efficiency.
 */
static bool is_double_tap_xy(void)
{
	static bool g_first_tap_detected = false;
	static bool g_tap_and_release_detected = false;
	static uint32_t g_first_tap_time = 0;
	static uint32_t g_first_tap_point_x = 0;
	static uint32_t g_first_tap_point_y = 0;
	uint32_t current_time;
	uint32_t x, y;
	uint32_t time_since_first_tap, distance;

	current_time = MILLIS();

	if (g_gesture.isTouch) {
		x = g_gesture.touchX;
		y = g_gesture.touchY;

		if (!g_first_tap_detected) {
			/* First tap detected - record position and time */
			g_first_tap_detected = true;
			g_tap_and_release_detected = false;
			g_first_tap_time = current_time;
			g_first_tap_point_x = x;
			g_first_tap_point_y = y;
		} else if (g_tap_and_release_detected) {
			/* Second tap detected - validate timing and distance */
			time_since_first_tap = current_time - g_first_tap_time;
			/* Use squared distance to avoid expensive sqrt operation */
			distance = (g_first_tap_point_x - x) * (g_first_tap_point_x - x) +
				   (g_first_tap_point_y - y) * (g_first_tap_point_y - y);

			if ((time_since_first_tap <= DOUBLE_TAP_THRESHOLD) &&
			    (distance <= MAX_TAP_DISTANCE * MAX_TAP_DISTANCE)) {
				/* Valid double tap at same location */
				g_tap_and_release_detected = false;
				g_first_tap_detected = false;
				return true;
			} else if (time_since_first_tap > DOUBLE_TAP_THRESHOLD) {
				/* Timeout: reset state machine */
				g_tap_and_release_detected = false;
				g_first_tap_detected = false;
			}
		}
	} else {
		/* Touch released - handle timeout reset */
		if (g_first_tap_detected &&
		    ((current_time - g_first_tap_time) > DOUBLE_TAP_THRESHOLD)) {
			/* Timeout: reset state machine */
			g_first_tap_detected = false;
			g_tap_and_release_detected = false;
		}
		/* Mark release after first tap */
		if (g_first_tap_detected)
			g_tap_and_release_detected = true;
	}

	return false;
}

/**
 * @brief Measure travel distance and direction from initial touch point
 *
 * Calculates the distance traveled from the initial touch point and tracks
 * directional movement for gesture analysis. Implements automatic reset logic
 * for excessive Y-axis movement without active swipe detection.
 *
 * @note Updates global gesture structure with distance calculations including
 *       separate X/Y distances and combined absolute distance. Uses signed
 *       distance for directional information.
 */
static void measure_traveled(void)
{
	static uint8_t g_is_first = 1;
	int32_t distance_abs;

	if (!g_gesture.isTouch) {
		/* Touch released: reset all tracking state */
		g_gesture.touchXStart = 0;
		g_gesture.touchYStart = 0;
		g_gesture.tagDragStart = 0;
		g_is_first = 1;
		return;
	}

	if (g_is_first) {
		/* First touch: record initial position and tag */
		g_is_first = 0;
		g_gesture.tagDragStart = g_gesture.tagPressed;
		g_gesture.touchXStart = g_gesture.touchX;
		g_gesture.touchYStart = g_gesture.touchY;
	}

	/* Calculate distance components from initial touch point */
	g_gesture.distanceX = g_gesture.touchX - g_gesture.touchXStart;
	g_gesture.distanceY = g_gesture.touchY - g_gesture.touchYStart;

	/* Calculate absolute distance using Euclidean distance */
	distance_abs = sqrt((g_gesture.distanceX * g_gesture.distanceX) +
			    (g_gesture.distanceY * g_gesture.distanceY));
	g_gesture.distance = distance_abs;

	/* Set negative distance for leftward or upward movement */
	if ((g_gesture.touchX < g_gesture.touchXStart) ||
	    (g_gesture.touchY < g_gesture.touchYStart))
		g_gesture.distance = -distance_abs;

	/* Reset tracking if large Y movement without active swipe */
	if (!g_is_first && (abs(g_gesture.distanceY) > RESET_Y_THRESHOLD) &&
	    !g_gesture.isSwipe)
		g_is_first = 1;
}
/**
 * @brief Detect newly pressed tag events
 *
 * Monitors tag state changes to identify when a tag transitions from
 * unpressed to pressed state. Provides edge detection for tag press events.
 *
 * @return Tag ID that was just pressed, 0 if no new press detected
 *
 * @note Uses static variable to track previous tag state. Returns tag
 *       only once per press event, not continuously while held.
 */
static uint32_t get_tag_pressed_new(void)
{
	static uint32_t g_tag_pressed = 0;
	uint32_t result;

	/* Check if tag state has changed */
	if (g_tag_pressed == g_gesture.tagPressed)
		return 0;

	/* Tag state changed - record new state and return it */
	result = g_gesture.tagPressed;
	g_tag_pressed = g_gesture.tagPressed;
	return result;
}

/**
 * @brief Get tag from completed first tap gesture
 *
 * Tracks the tag that was initially touched when a gesture begins and
 * returns it only when the touch is released, indicating a completed tap.
 * Useful for identifying which UI element received a complete tap gesture.
 *
 * @return Tag ID that received a complete first tap, 0 if no completed tap
 *
 * @note Returns tag only on touch release after a press on tagged element.
 *       Resets internal state after returning a result.
 */
static uint32_t get_tag_first_tap(void)
{
	static uint32_t g_tag_first_tap = 0;
	uint32_t result;

	if (g_gesture.isTouch && g_gesture.tagPressed && !g_tag_first_tap) {
		/* Touch started on a tagged element - record it */
		g_tag_first_tap = g_gesture.tagPressed;
	} else if (!g_gesture.isTouch && g_tag_first_tap) {
		/* Touch released - return the completed first tap tag */
		result = g_tag_first_tap;
		g_tag_first_tap = 0;
		return result;
	}

	return 0;
}

/**
 * @brief Update gesture state from EVE hardware registers
 *
 * Primary gesture processing function that reads touch and tag information
 * from EVE hardware registers, processes all gesture detection algorithms,
 * and updates the global gesture state structure. Should be called once
 * per frame to maintain accurate gesture tracking.
 *
 * Processing includes:
 * - Touch coordinate validation and boundary clamping
 * - Touch confidence tracking for noise filtering
 * - Velocity calculation for both axes
 * - Swipe detection and state management
 * - Double-tap detection (location and tag-based)
 * - Long tap detection with movement tolerance
 * - Distance traveled measurements
 * - Tag state transition tracking
 *
 * @param phost Pointer to EVE hardware abstraction layer context
 * @return Pointer to updated gesture state structure
 *
 * @note This function updates global gesture state and should be called
 *       from the main application loop for real-time gesture processing.
 */
Gesture_Touch_t *utils_gesture_renew(EVE_HalContext *phost)
{
	uint32_t reg_track;
	uint32_t reg_touch_screen_xy;
	uint32_t raw_x, raw_y;

	/* Initialize swipe flag pointers for axis tracking */
	g_gestureX.swipeFlag = &g_gesture.isSwipeX;
	g_gestureY.swipeFlag = &g_gesture.isSwipeY;

	/* Read hardware registers for touch and tag information */
	reg_track = EVE_Hal_rd32(PHOST, REG_TRACKER);
	reg_touch_screen_xy = EVE_Hal_rd32(PHOST, REG_TOUCH_SCREEN_XY);
	g_gesture.tagPressed = EVE_Hal_rd32(PHOST, REG_TOUCH_TAG);

	/* Extract raw touch coordinates from packed register */
	raw_x = reg_touch_screen_xy >> 16;
	raw_y = reg_touch_screen_xy & 0x0000FFFF;

	/* Determine touch state with comprehensive boundary validation */
	g_gesture.isTouch = (reg_touch_screen_xy != EVE_XY_RESET);
	g_gesture.isTouch = g_gesture.isTouch && (raw_x <= phost->Width);
	g_gesture.isTouch = g_gesture.isTouch && (raw_y <= phost->Height);

	/* Update touch confidence counter for noise filtering */
	if (g_gesture.isTouch)
		g_gesture.isTouch3x++;
	else
		g_gesture.isTouch3x--;
	
	/* Clamp confidence counter to prevent overflow */
	g_gesture.isTouch3x = max(-TOUCH_CONFIDENCE_MAX, 
				  min(TOUCH_CONFIDENCE_MAX, g_gesture.isTouch3x));

	/* Clamp touch coordinates to screen boundaries */
	g_gesture.touchX = min(phost->Width, max(0, raw_x));
	g_gesture.touchY = min(phost->Height, max(0, raw_y));

	/* Process tracker data for rotary and linear controls */
	g_gesture.tagTrackTouched = reg_track;
	g_gesture.trackValLine = reg_track >> 16;
	g_gesture.trackValCircle = (reg_track >> 16) * FULL_CIRCLE_DEGREES / TRACKER_SCALE;

	/* Update tag-based gesture detection states */
	g_gesture.tagReleased = get_tag_released();
	g_gesture.tagPressed1 = get_tag_pressed_new();
	g_gesture.tagFirstTap = get_tag_first_tap();

	/* Calculate velocity for both axes */
	g_gesture.velocityX = get_velocity_x();
	g_gesture.velocityY = get_velocity_y();

	/* Update overall swipe state */
	g_gesture.isSwipe = g_gesture.isSwipeX || g_gesture.isSwipeY;

	/* Configure and detect double tap gestures */
	g_gesture.doubleTapMilisecond = DOUBLE_TAP_DEFAULT_MS;
	g_gesture.isDoubleTapXY = is_double_tap_xy();
	g_gesture.isDoubleTapTag = is_double_tap_tag();

	/* Configure and detect long tap gestures */
	g_gesture.longTapMilisecond = LONG_TAP_DEFAULT_MS;
	g_gesture.isLongTap = is_long_tap();

	/* Measure travel distance from initial touch point */
	measure_traveled();

	return &g_gesture;
}

/**
 * @brief Get current gesture state structure
 *
 * Provides read-only access to the current gesture state structure.
 * This function returns a pointer to the latest gesture information
 * that was updated by the most recent call to utils_gesture_renew().
 *
 * @return Pointer to current gesture state structure
 *
 * @note The returned pointer provides access to the global gesture state.
 *       Ensure utils_gesture_renew() has been called at least once before
 *       using this function to get valid gesture data.
 */
Gesture_Touch_t *utils_gesture_get(void)
{
	return &g_gesture;
}
