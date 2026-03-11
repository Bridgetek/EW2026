/**
 * @file utils_box.h
 * @brief Header file for box manipulation and drawing utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_BOX_H
#define UTILS_BOX_H

#include <stdint.h>
#include "eve.h"
#include "utils_color.h"

typedef struct
{
    int32_t x, y, w, h, x_end, y_end, x_mid, y_mid;
} app_box;

/* Box manipulation functions */
void set_app_box(app_box *box, int32_t x_val, int32_t y_val, int32_t w_val, int32_t h_val);
void update_app_box(app_box *box);
void move_box_to(app_box *box, int32_t x_val, int32_t y_val);

/* Box initialization functions */
app_box init_app_box(int32_t x_val, int32_t y_val, int32_t w_val, int32_t h_val);
app_box init_app_box_child_top_left(int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_child_top_right(int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_child_center(int32_t y_val, int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_child_middle(int32_t x_val, int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_child_center_middle(int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_below(int32_t w_val, int32_t h_val, app_box parent);
app_box init_app_box_next_to(int32_t w_val, int32_t h_val, app_box box);
app_box init_app_box_equal_next_to(app_box box);
app_box init_app_box_below_center(int32_t w_val, int32_t h_val, app_box parent);

/* Drawing functions - rectangles */
uint32_t utils_draw_rects(EVE_HalContext *phost, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void draw_rect_border_inside(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h,
                             uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag);
void draw_rect_border_outside(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h,
                              uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag);

/* Drawing functions - boxes */
void draw_box(EVE_HalContext *phost, app_box box);
void draw_box_at(EVE_HalContext *phost, app_box box, int32_t x, int32_t y);
void draw_box_text_center_middle(EVE_HalContext *phost, app_box box, uint8_t *text, int32_t font, uint16_t opt, uint32_t txtcolor);
void draw_box_border_inside(EVE_HalContext *phost, app_box box, uint32_t color, int32_t border, uint32_t bcolor);
void draw_box_border_outside(EVE_HalContext *phost, app_box box, uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag);

/* Drawing functions - circles */
void draw_circle(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r);
void draw_circle_with_id(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
                         int32_t id, int32_t font, uint16_t opt, uint32_t txtcolor);

#endif // UTILS_BOX_H
