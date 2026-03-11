/**
 * @file utils_scrolling.h
 * @brief Header file for scrolling logic and gesture handling 
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTIL_SCROLLING_H
#define UTIL_SCROLLING_H

#include <stdint.h>

typedef enum { ROLLING_OFF = 0, ROLLING_ON, ROLLING_BOUNCE_BACK, ROLLING_ADVANCE_TOWARD, ROLLING_FINETURN_ON } rolling_e;
typedef enum { SCROLLING_DIRECTION_UP = 1, SCROLLING_DIRECTION_DOWN = -1 } ScrollingDirection;

typedef struct {
	int8_t **list_str;
	uint32_t *list_numerics;
	uint32_t item_count_total;
	uint32_t item_h;
	uint32_t draw_h;
	uint32_t h;
	int32_t tag_val;
	int32_t position;
	int32_t distance_scroll;
	int32_t distance_drag;
	int32_t draw_position;
	int32_t draw_angle;
	int32_t draw_angle100;
	int32_t min_veloc;
	int32_t max_veloc;
	int32_t deceleration;
	int32_t acceleration;
	rolling_e rolling;
	int32_t target_fineturn;
	int32_t velocity;
	int32_t velocitySign;
} scrolling_t;

void utils_scrolling_stop(void);
int32_t utils_scrolling_y_tag(scrolling_t *sc, ScrollingDirection direction);
int32_t utils_scrolling_x_tag(scrolling_t *sc, ScrollingDirection direction);
scrolling_t *utils_scrolling_init(uint32_t h, uint32_t itemh, uint32_t num_item_total, uint32_t draw_h, int32_t tag_val);

#endif
