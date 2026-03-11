/**
 * @file utils_scrolling.c
 * @brief Scrolling logic and gesture handling
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils_gesture.h"
#include "utils_scrolling.h"

/* Mathematical utility macros */
#define SIGN(x)         (((x) > 0) - ((x) < 0))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))

/* Scrolling physics constants */
#define MIN_VELOCITY        2
#define DEFAULT_DECEL       5
#define DEFAULT_ACCEL       5
#define DISTANCE_DIVISOR    12
#define MIN_STEP           3
#define ANGLE_MULTIPLIER   20
#define ANGLE_MODULO       36000
#define ANGLE_DIVISOR      100

/* Global gesture interface */
extern Gesture_Touch_t *utils_gesture_get(void);

/**
 * @brief Advance a value smoothly towards target with easing
 * @param current Current value
 * @param target Target value to reach
 * @return New value closer to target
 * 
 * Uses a simple easing algorithm with diminishing step size as the
 * distance decreases for smooth animation.
 */
static int32_t advance_towards(int32_t current, int32_t target)
{
    int32_t distance, step;
    
    if (current == target)
        return current;
    
    distance = abs(target - current);
    step = distance / DISTANCE_DIVISOR + MIN_STEP;
    
    if (current < target)
        return (current + step > target) ? target : current + step;
    
    return (current - step < target) ? target : current - step;
}

/**
 * @brief Core scrolling logic with gesture handling
 * @param sc Scrolling context structure
 * @param new_distance Current frame distance from gesture
 * @param new_velocity Current velocity from gesture
 * @param new_velocity_total Total accumulated velocity
 * @return Updated draw position
 * 
 * Handles touch pressed, velocity updates, and deceleration physics.
 * Updates position, velocity, and calculated drawing parameters.
 */
static int32_t scrolling_tag(scrolling_t *sc, int new_distance, int new_velocity, int new_velocity_total)
{
    Gesture_Touch_t *ges;
    int32_t deceleration, acceleration;
    
    ges = utils_gesture_get();
    deceleration = sc->deceleration ? sc->deceleration : DEFAULT_DECEL;
    acceleration = sc->acceleration ? sc->acceleration : DEFAULT_ACCEL;

    /* Handle initial touch press - reset motion state */
    if (ges->tagPressed1 == sc->tag_val) {
        sc->position += sc->distance_scroll * acceleration / 100 + sc->distance_drag;
        sc->velocity = 0;
        sc->distance_scroll = 0;
        sc->rolling = ROLLING_ON;
    }

    /* Process scrolling state machine */
    if (sc->rolling == ROLLING_ON) {
        if (ges->tagPressed == sc->tag_val) {
            /* Active dragging - track distance, stop velocity */
            sc->distance_drag = new_distance;
            sc->velocity = 0;
            sc->distance_scroll = 0;
        } else if (new_velocity && ges->tagVelocity == sc->tag_val) {
            /* Start kinetic scrolling with initial velocity */
            sc->velocity = new_velocity;
            sc->distance_scroll = new_velocity_total;
            sc->velocitySign = SIGN(sc->velocity) ?: 1;
        } else if (sc->velocity != 0) {
            /* Apply deceleration physics */
            if (deceleration == 100) {
                sc->velocity = 0;
            } else if (deceleration > 0) {
                sc->velocity = (sc->velocity * (100 - deceleration)) / 100;
                if (abs(sc->velocity) < MIN_VELOCITY)
                    sc->velocity = 0;
            }
            sc->distance_scroll += sc->velocity;
        } else if (sc->velocity == 0) {
            /* Finalize position when motion stops */
            sc->position += sc->distance_drag + sc->distance_scroll * acceleration / 100;
            sc->distance_drag = sc->distance_scroll = 0;
        }
        
        /* Calculate final drawing parameters */
        sc->draw_position = sc->position + sc->distance_scroll * acceleration / 100 + sc->distance_drag;
        sc->draw_angle100 = (-sc->draw_position * ANGLE_MULTIPLIER) % ANGLE_MODULO;
        sc->draw_angle = sc->draw_angle100 / ANGLE_DIVISOR;
    }
    
    return sc->draw_position;
}

/**
 * @brief Handle Y-axis scrolling with direction control
 * @param sc Scrolling context structure
 * @param direction Scroll direction (up/down)
 * @return Updated Y position for drawing
 * 
 * Processes vertical gestures and applies direction multiplier
 * for consistent scroll behavior.
 */
int32_t utils_scrolling_y_tag(scrolling_t *sc, ScrollingDirection direction)
{
    Gesture_Touch_t *ges;
    
    ges = utils_gesture_get();
    
    return scrolling_tag(sc, 
        direction < 0 ? -ges->distanceY : ges->distanceY,
        direction < 0 ? -ges->velocityY : ges->velocityY,
        direction < 0 ? -ges->velocityY_total : ges->velocityY_total);
}

/**
 * @brief Handle X-axis scrolling with direction control
 * @param sc Scrolling context structure
 * @param direction Scroll direction (left/right)
 * @return Updated X position for drawing
 * 
 * Processes horizontal gestures and applies direction multiplier
 * for consistent scroll behavior.
 */
int32_t utils_scrolling_x_tag(scrolling_t *sc, ScrollingDirection direction)
{
    Gesture_Touch_t *ges;
    
    ges = utils_gesture_get();
    
    return scrolling_tag(sc,
        direction < 0 ? -ges->distanceX : ges->distanceX,
        direction < 0 ? -ges->velocityX : ges->velocityX,
        direction < 0 ? -ges->velocityX_total : ges->velocityX_total);
}

/**
 * @brief Initialize scrolling context with parameters
 * @param h Total content height
 * @param itemh Individual item height
 * @param num_item_total Total number of items
 * @param draw_h Visible drawing area height
 * @param tag_val Touch tag value for this scrollable area
 * @return Allocated scrolling context or NULL on failure
 * 
 * Allocates and initializes a new scrolling context with zero
 * initial position and velocity.
 */
scrolling_t *utils_scrolling_init(uint32_t h, uint32_t itemh, uint32_t num_item_total, uint32_t draw_h, int32_t tag_val)
{
    scrolling_t *sc;
    
    sc = malloc(sizeof(scrolling_t));
    if (!sc)
        return NULL;
    
    /* Initialize all fields to zero */
    memset(sc, 0, sizeof(scrolling_t));
    
    /* Set configuration parameters */
    sc->h = h;
    sc->item_count_total = num_item_total;
    sc->draw_h = draw_h;
    sc->item_h = itemh;
    sc->tag_val = tag_val;
    
    return sc;
}

/**
 * @brief Stop all scrolling motion immediately
 * 
 * Resets velocity and swipe state in the gesture system to halt
 * any ongoing kinetic scrolling motion.
 */
void utils_scrolling_stop(void)
{
    Gesture_Touch_t *ges;
    
    ges = utils_gesture_get();
    
    /* Clear all velocity and swipe state */
    ges->velocityX = ges->velocityX_total = 0;
    ges->velocityY = ges->velocityY_total = 0;
    ges->isSwipeX = ges->isSwipeY = 0;
}
