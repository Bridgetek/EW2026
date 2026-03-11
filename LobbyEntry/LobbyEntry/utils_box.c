/**
 * @file utils_box.c
 * @brief Box manipulation and drawing utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "eve.h"
#include "utils_box.h" 
#include "utils_vertex_adapter.h"

/* ========================================================================== */
/* Box Manipulation Functions                                                */
/* ========================================================================== */

/**
 * @brief Set box properties and calculate derived values
 * @param box Pointer to box structure to modify
 * @param x_val X coordinate of top-left corner
 * @param y_val Y coordinate of top-left corner  
 * @param w_val Width of the box
 * @param h_val Height of the box
 * 
 * Sets the basic box properties and automatically calculates end points
 * and midpoint coordinates for convenience.
 */
void set_app_box(app_box *box, int32_t x_val, int32_t y_val, int32_t w_val, int32_t h_val)
{
    /* Set primary box properties */
    box->x = x_val;
    box->y = y_val;
    box->w = w_val;
    box->h = h_val;
    
    /* Calculate derived coordinates for convenience */
    box->x_end = x_val + w_val;
    box->y_end = y_val + h_val;
    box->x_mid = x_val + (w_val / 2);
    box->y_mid = y_val + (h_val / 2);
}

/**
 * @brief Recalculate derived box properties
 * @param box Pointer to box structure to update
 * 
 * Updates end points and midpoint coordinates based on current x, y, w, h values.
 * Useful after modifying box dimensions directly.
 */
void update_app_box(app_box *box)
{
    set_app_box(box, box->x, box->y, box->w, box->h);
}

/**
 * @brief Move box to new position
 * @param box Pointer to box structure to move
 * @param x_val New X coordinate
 * @param y_val New Y coordinate
 * 
 * Moves the box to a new position while preserving dimensions.
 */
void move_box_to(app_box *box, int32_t x_val, int32_t y_val)
{
    set_app_box(box, x_val, y_val, box->w, box->h);
}

/* ========================================================================== */
/* Box Initialization Functions                                              */
/* ========================================================================== */

/**
 * @brief Initialize a new box with specified dimensions
 * @param x_val X coordinate of top-left corner
 * @param y_val Y coordinate of top-left corner
 * @param w_val Width of the box
 * @param h_val Height of the box
 * @return Initialized app_box structure
 * 
 * Creates and returns a new box structure with all properties calculated.
 */
app_box init_app_box(int32_t x_val, int32_t y_val, int32_t w_val, int32_t h_val)
{
    app_box box;
    
    set_app_box(&box, x_val, y_val, w_val, h_val);
    return box;
}

/**
 * @brief Initialize child box at top-left corner of parent
 * @param w_val Width of the child box
 * @param h_val Height of the child box
 * @param parent Parent box for positioning reference
 * @return Child box positioned at parent's top-left
 */
app_box init_app_box_child_top_left(int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x, parent.y, w_val, h_val);
}

/**
 * @brief Initialize child box at top-right corner of parent
 * @param w_val Width of the child box
 * @param h_val Height of the child box
 * @param parent Parent box for positioning reference
 * @return Child box positioned at parent's top-right
 */
app_box init_app_box_child_top_right(int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x + parent.w - w_val, parent.y, w_val, h_val);
}

/**
 * @brief Initialize child box centered horizontally within parent
 * @param y_val Y coordinate for the child box
 * @param w_val Width of the child box
 * @param h_val Height of the child box
 * @param parent Parent box for horizontal centering reference
 * @return Child box centered horizontally within parent
 */
app_box init_app_box_child_center(int32_t y_val, int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x_mid - (w_val / 2), y_val, w_val, h_val);
}

/**
 * @brief Initialize child box centered vertically within parent
 * @param x_val X coordinate for the child box
 * @param w_val Width of the child box
 * @param h_val Height of the child box
 * @param parent Parent box for vertical centering reference
 * @return Child box centered vertically within parent
 */
app_box init_app_box_child_middle(int32_t x_val, int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(x_val, parent.y_mid - (h_val / 2), w_val, h_val);
}

/**
 * @brief Initialize child box centered both horizontally and vertically within parent
 * @param w_val Width of the child box
 * @param h_val Height of the child box
 * @param parent Parent box for centering reference
 * @return Child box centered in both axes within parent
 */
app_box init_app_box_child_center_middle(int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x_mid - (w_val / 2), parent.y_mid - (h_val / 2), w_val, h_val);
}

/**
 * @brief Initialize box positioned below another box
 * @param w_val Width of the new box
 * @param h_val Height of the new box
 * @param parent Reference box to position below
 * @return New box positioned directly below the reference box
 */
app_box init_app_box_below(int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x, parent.y + parent.h, w_val, h_val);
}

/**
 * @brief Initialize box positioned to the right of another box
 * @param w_val Width of the new box
 * @param h_val Height of the new box
 * @param box Reference box to position next to
 * @return New box positioned to the right of the reference box
 */
app_box init_app_box_next_to(int32_t w_val, int32_t h_val, app_box box)
{
    return init_app_box(box.x + box.w, box.y, w_val, h_val);
}

/**
 * @brief Initialize box with same dimensions positioned to the right
 * @param box Reference box to duplicate and position next to
 * @return New box with identical dimensions positioned to the right
 */
app_box init_app_box_equal_next_to(app_box box)
{
    return init_app_box(box.x + box.w, box.y, box.w, box.h);
}

/**
 * @brief Initialize box positioned below and horizontally centered
 * @param w_val Width of the new box
 * @param h_val Height of the new box
 * @param parent Reference box for positioning
 * @return New box positioned below and centered horizontally
 */
app_box init_app_box_below_center(int32_t w_val, int32_t h_val, app_box parent)
{
    return init_app_box(parent.x_mid - (w_val / 2), parent.y + parent.h, w_val, h_val);
}

/**
 * @brief Draw semi-transparent rectangle overlay
 *
 * Renders a semi-transparent dark rectangle, typically used for background
 * overlays or visual emphasis effects.
 *
 * @param phost Pointer to EVE HAL context
 * @param x Left edge of rectangle
 * @param y Top edge of rectangle
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @return Always returns 0
 */
uint32_t utils_draw_rects(EVE_HalContext *phost, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	/* Configure rectangle rendering properties */
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, LINE_WIDTH(16));
	
	/* Draw rectangle using two corner vertices */
	g_utils_adapter_vertex2f(phost, x, y);
	g_utils_adapter_vertex2f(phost, x + w, y + h);
	
	return 0;
}

/**
 * @brief Draw rectangle with inner border
 * @param phost EVE GPU context
 * @param x X coordinate of rectangle
 * @param y Y coordinate of rectangle
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @param color Interior fill color (RGB format)
 * @param border Border thickness in pixels
 * @param bcolor Border color (RGB format)
 * @param tag Touch tag for interaction
 * 
 * Draws a rectangle with border drawn inward from the specified dimensions.
 */
void draw_rect_border_inside(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h,
                             uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag)
{
    /* Draw border background */
    set_color_rgb(phost, bcolor);
    EVE_Cmd_wr32(phost, TAG(tag));
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, x, y);
    g_utils_adapter_vertex2f(phost, x + w, y + h);
    EVE_Cmd_wr32(phost, END());

    /* Draw inner rectangle */
    set_color_rgb(phost, color);
    EVE_Cmd_wr32(phost, TAG(tag));
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, x + border, y + border);
    g_utils_adapter_vertex2f(phost, x + w - border * 2, y + h - border * 2);
    EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw rectangle with outer border
 * @param phost EVE GPU context
 * @param x X coordinate of inner rectangle
 * @param y Y coordinate of inner rectangle
 * @param w Width of inner rectangle
 * @param h Height of inner rectangle
 * @param color Interior fill color (RGB format)
 * @param border Border thickness in pixels
 * @param bcolor Border color (RGB format)
 * @param tag Touch tag for interaction
 * 
 * Draws a rectangle with border extending outward from the specified dimensions.
 */
void draw_rect_border_outside(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h,
                              uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag)
{
    /* Draw outer border */
    set_color_rgb(phost, bcolor);
    EVE_Cmd_wr32(phost, TAG(tag));
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, x - border, y - border);
    g_utils_adapter_vertex2f(phost, x + w + border * 2, y + h + border * 2);
    EVE_Cmd_wr32(phost, END());

    /* Draw inner rectangle */
    set_color_rgb(phost, color);
    EVE_Cmd_wr32(phost, TAG(tag));
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, x, y);
    g_utils_adapter_vertex2f(phost, x + w, y + h);
    EVE_Cmd_wr32(phost, END());
}

/* ========================================================================== */
/* Drawing Functions - Boxes                                                 */
/* ========================================================================== */

/**
 * @brief Draw a box using its stored coordinates
 * @param phost EVE GPU context
 * @param box Box structure containing position and dimensions
 * 
 * Convenience function to draw a rectangle using box coordinates.
 */
void draw_box(EVE_HalContext *phost, app_box box)
{
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, box.x, box.y);
    g_utils_adapter_vertex2f(phost, box.x_end, box.y_end);
    EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw a box at specified position
 * @param phost EVE GPU context
 * @param box Box structure (dimensions used, position ignored)
 * @param x X coordinate to draw at
 * @param y Y coordinate to draw at
 * 
 * Draws a rectangle with the box's dimensions at a different position.
 */
void draw_box_at(EVE_HalContext *phost, app_box box, int32_t x, int32_t y)
{
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, x, y);
    g_utils_adapter_vertex2f(phost, box.w + x, box.h + y);
    EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw box with centered text
 * @param phost EVE GPU context
 * @param box Box to draw and center text within
 * @param text Text string to display
 * @param font Font handle for text rendering
 * @param opt Text rendering options
 * @param txtcolor Text color (RGB format)
 * 
 * Draws a filled box and renders text centered both horizontally and vertically.
 */
void draw_box_text_center_middle(EVE_HalContext *phost, app_box box, uint8_t *text, int32_t font, uint16_t opt, uint32_t txtcolor)
{
    draw_box(phost, box);
    set_color_rgb(phost, txtcolor);
    EVE_CoCmd_text(phost, box.x_mid, box.y_mid, font, opt, text);
}

/**
 * @brief Draw box with inner border
 * @param phost EVE GPU context
 * @param box Box structure defining outer dimensions
 * @param color Interior fill color (RGB format)
 * @param border Border thickness in pixels
 * @param bcolor Border color (RGB format)
 * 
 * Draws a box with border drawn inward from the box dimensions.
 */
void draw_box_border_inside(EVE_HalContext *phost, app_box box, uint32_t color, int32_t border, uint32_t bcolor)
{
    /* Draw border background */
    set_color_rgb(phost, bcolor);
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, box.x, box.y);
    g_utils_adapter_vertex2f(phost, box.x_end, box.y_end);
    EVE_Cmd_wr32(phost, END());

    /* Draw inner rectangle */
    set_color_rgb(phost, color);
    EVE_Cmd_wr32(phost, BEGIN(RECTS));
    g_utils_adapter_vertex2f(phost, box.x + border, box.y + border);
    g_utils_adapter_vertex2f(phost, box.x_end - border, box.y_end - border);
    EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw box with outer border
 * @param phost EVE GPU context
 * @param box Box structure defining inner dimensions
 * @param color Interior fill color (RGB format)
 * @param border Border thickness in pixels
 * @param bcolor Border color (RGB format)
 * @param tag Touch tag for interaction
 * 
 * Wrapper function for draw_rect_border_outside using box coordinates.
 */
void draw_box_border_outside(EVE_HalContext *phost, app_box box, uint32_t color, int32_t border, uint32_t bcolor, uint32_t tag)
{
    draw_rect_border_outside(phost, box.x, box.y, box.w, box.h, color, border, bcolor, tag);
}

/* ========================================================================== */
/* Drawing Functions - Circles                                               */
/* ========================================================================== */

/**
 * @brief Draw a filled circle
 * @param phost EVE GPU context
 * @param x X coordinate of circle center
 * @param y Y coordinate of circle center
 * @param r Circle radius in pixels
 * 
 * Renders a filled circle using EVE GPU point primitive with radius scaling.
 */
void draw_circle(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r)
{
    EVE_Cmd_wr32(phost, POINT_SIZE(r * 16));  /* EVE uses 1/16 pixel precision */
    EVE_Cmd_wr32(phost, BEGIN(POINTS));
    g_utils_adapter_vertex2f(phost, x, y);
    EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw circle with centered numeric ID
 * @param phost EVE GPU context
 * @param x X coordinate of circle center
 * @param y Y coordinate of circle center
 * @param r Circle radius in pixels
 * @param id Numeric ID to display in center
 * @param font Font handle for text rendering
 * @param opt Text rendering options
 * @param txtcolor Text color (RGB format)
 * 
 * Draws a filled circle and renders a numeric ID centered within it.
 */
void draw_circle_with_id(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
                         int32_t id, int32_t font, uint16_t opt, uint32_t txtcolor)
{
    draw_circle(phost, x, y, r);
    set_color_rgb(phost, txtcolor);
    EVE_CoCmd_text(phost, x, y, font, opt | OPT_CENTER | OPT_FORMAT, "%d", id);
}
