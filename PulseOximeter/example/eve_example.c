/**
 @file eve_example.c
 */
 /*
  * ============================================================================
  * History
  * =======
  *
  *
  *
  *
  *
  *
  * (C) Copyright,  Bridgetek Pte. Ltd.
  * ============================================================================
  *
  * This source code ("the Software") is provided by Bridgetek Pte Ltd
  * ("Bridgetek") subject to the licence terms set out
  * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <EVE.h>
#include <MCU.h>  // For DEBIG_PRINTF

#include "eve_example.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

// declare global variables for ir graphing function
uint16_t chart_result_buffer[272];
uint16_t chart_value = 0;
uint16_t chart_output_index;
uint16_t chart_index_x;
uint16_t chart_input_index = 0;
uint32_t waveform_value = 0;

// declare global variables for sensor
uint8_t status_bio;
bioData_t body;

// declare global variables for LED readings from sesnsor
#define led_sample_range 272
uint32_t led_max = 0;
uint32_t led_min = 0;
uint32_t led_range = 0;
uint32_t led_vals[led_sample_range]; 
uint16_t led_vals_index = 0;

// declare global variables or screen readouts
int8_t spo2 = 0;
int16_t spo2_arc = 0;
int16_t heart_rate = 0;
int8_t confidence = 0;
int16_t progress = 0;

// declare global varibles for timing functionliaty
uint32_t current_ms = 0;
uint32_t elapsed_ms = 0;
uint32_t heart_rate_interval = 0;

// declare global varibles for touch detection
uint8_t TagVal = 0;
uint8_t LastTagVal = 0;
uint8_t Pen_Down_Tag = 0;
uint8_t Pen_Up_Tag = 0;
// for CMD_TRACK
uint32_t TrackValue = 0;

// declare global variable for backlight level (0-100)
int8_t backlight_value = 100;

// declare booleans for button press states
bool settings_menu_press = false;
bool screen_rotation_press = false;
bool mute_volume_press = false;

// declare boolen for settings menu render
bool settings = false;
// declare boolen for screen rotation and mute
bool screen_rotation = false;
bool mute_volume = false;

// declare global varibles for static screen content
uint32_t static_screen_size = 0;
uint32_t static_screen_location = 0;

// ######################################################################################################################################################################################################
// #######################################################                     Custom widgets for EVE screen updates                  ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Helper funciton to add a gradietn bitmap into the alpha composited shape in the arc_point_gauge. 
 @param arc_centerx x value for centre of widget
 @param arc_centery y value for centre of widget
 @param arc_radius variable to ste the radius off the arc in px
 @param arc_thickness variable to set the thickness of the arc in px
 @param colour1 input colour for the left side of our gradient
 @param colour2 input colour for the right side of our gradient
 */
void addGradient(uint16_t arc_centerx, uint16_t arc_centery, uint16_t arc_radius, uint16_t arc_thickness, uint32_t color1, uint32_t color2)
{

    // begin bitmaps
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // selecty handle
    EVE_BITMAP_HANDLE(BITMAP_GRAD);
    // configure size, repeat the bitmap along the x axis
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_REPEAT, EVE_WRAP_BORDER, (arc_radius +1), 0);
    // set colour 1
    EVE_COLOR_RGB(((uint8_t)(color1 >> 16)), ((uint8_t)(color1 >> 8)), ((uint8_t)(color1)));
    // place bitmmap at the top left of the arc
    EVE_VERTEX2F((arc_centerx - arc_radius - 1) * 16, (arc_centery - arc_radius) *16);
    // set colour 2
    EVE_COLOR_RGB(((uint8_t)(color2 >> 16)), ((uint8_t)(color2 >> 8)), ((uint8_t)(color2)));
    // place bitmap at the top centre of the arc
    EVE_VERTEX2F((arc_centerx) * 16, (arc_centery - arc_radius) *16);

}

/**
 @brief Function to render custom arc gauge widget on the screen.
 @param arc_centerx x value for centre of widget
 @param arc_centery y value for centre of widget
 @param arc_radius variable to ste the radius off the arc in px
 @param arc_thickness variable to set the thickness of the arc in px
 @param arc_active_color input colour for the filled section of the arc 
 @param arc_inactive_color input colour for the unfilled section of the arc 
 @param arc_value_user value input for arc widget reading (0 - 255)
 */
void arc_point_gauge(uint16_t arc_centerx, uint16_t arc_centery, uint16_t arc_radius, uint16_t arc_thickness, uint32_t arc_active_color, uint32_t arc_inactive_color, uint8_t arc_value_user)
{
    
    // local variables to define arc start/end degrees and total range
    uint16_t arc_min_limit = 52;
    uint16_t arc_max_limit = 308;
    uint16_t arc_range = (arc_max_limit - arc_min_limit);

    // ensure user input value is within range 
    arc_value_user = min(arc_value_user, 255);
    // and set arc value to min + nomralised user input value
    uint16_t arc_value = arc_min_limit + (((arc_value_user) * arc_range) / 255);
    

    // local variables for arc drawing positioning 
    // if this is 0 the point shadow will be the same size as the arc_thickness (increment this value to grow the shadow around the indicator point)
    uint8_t shadow_size = 5;
    uint8_t alpha_value = 50;
 
    int16_t arc_activex = 0;
    int16_t arc_activey = 0;
 
    int16_t arc_start_x = 0;
    int16_t arc_start_y = 0;

    int16_t arc_fill_x = 0;
    int16_t arc_fill_y = 0;
 
    int16_t arc_end_x = 0;
    int16_t arc_end_y = 0;

    int16_t point_start_x = 0;
    int16_t point_start_y = 0;

    int16_t point_end_x = 0;
    int16_t point_end_y = 0;

    // Ensure the value is within limits
    if (arc_value > arc_max_limit)
       arc_value = arc_max_limit;
    if (arc_value < arc_min_limit)
       arc_value = arc_min_limit;

    // ensure that the arc thickness is even number so we can simplify the maths later for positining the end points and value indicator points
    if((arc_thickness % 2) != 0)
        arc_thickness ++;

    //--------------------------------------------------------------------------------------------------------
    // Process the angle data which will be used to make a uniform motion of the arc
    //--------------------------------------------------------------------------------------------------------
 
    // Calculate the coordinates of the starting point, the gauge arc and the point at the tip of the arc
 
    // for the arc gauge value itself
    //we want to draw this in the middle of the arc so we take the width and divide it by 2, then negate this from the raidus
    //multiply by 16 so we can feed this number directly int the VERTEX2F command with our desired pixel precision
    arc_activex = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_value));
    arc_activey = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_value));

    arc_fill_x = CIRC_X((arc_radius * 16 + 8), DEG2FURMAN(arc_value));
    arc_fill_y = CIRC_Y((arc_radius * 16 + 8), DEG2FURMAN(arc_value));
    
    //multiply by 32 so the edge strips we draw with these values extend past the edge of the circle
    // for the starting angle 
    arc_start_x = CIRC_X((arc_radius * 32), DEG2FURMAN(arc_min_limit));
    arc_start_y = CIRC_Y((arc_radius * 32), DEG2FURMAN(arc_min_limit));
 
    // for the finishing angle 
    arc_end_x = CIRC_X((arc_radius * 32), DEG2FURMAN(arc_max_limit));
    arc_end_y = CIRC_Y((arc_radius * 32), DEG2FURMAN(arc_max_limit));

    //for the rounded ends
    point_start_x = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_min_limit));
    point_start_y = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_min_limit));
    point_end_x = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_max_limit));
    point_end_y = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_max_limit));

    //--------------------------------------------------------------------------------------------------------
    // Write to the alpha buffer and disable writing colours to the screen to make an invisible arc
    //--------------------------------------------------------------------------------------------------------

    //save current graphics context
    EVE_SAVE_CONTEXT();

    EVE_SCISSOR_SIZE((arc_radius * 2) + 1, (arc_radius * 2) + 1);
    EVE_SCISSOR_XY((arc_centerx - arc_radius), (arc_centery - arc_radius));

    //set desried pixel precision format
    EVE_VERTEX_FORMAT(4);
 
    //firstly we want ot paint the fill and outline shapes into the alpah buffer, and use a stencil
    //disable all colours bar the alpha channel
    EVE_COLOR_MASK(0, 0, 0, 1);
    EVE_CLEAR(1, 1, 0);
    EVE_COLOR_A(255);

    //here we want to start incrementing the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);

    //we want to add the fill into the alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_COLOR_A(255);
    //--------------------------------------------------------------------------------------------------------
    // Draw the edge strips ( or circle) which will fill in the arc
    //--------------------------------------------------------------------------------------------------------

    if (arc_value >= 360){

        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(arc_radius * 16);
        EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);

    }else{
        // These are drawn per quadrant as each edge strip will only work well on an angle up to 90 deg
        // 0 - 89 Deg
        if ((arc_value > 0) && (arc_value < 90))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx * 16) - arc_fill_x ), (arc_centery * 16) + arc_fill_y);
        }
        else if (arc_value > 0){
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx - arc_radius) * 16), (arc_centery) * 16);
        }

        // 90 - 179 deg
        if ((arc_value >= 90) && (arc_value < 180))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx * 16) - arc_fill_x ), (arc_centery * 16) + arc_fill_y);
        }
        else if (arc_value > 90)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery - arc_radius) * 16);
        }

        // 180 - 269 deg
        if ((arc_value >= 180) && (arc_value < 270))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((arc_centerx - 1) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx * 16) - arc_fill_x), (arc_centery * 16) + arc_fill_y);
        }
        else if (arc_value > 180)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((arc_centerx - 1) * 16), (arc_centery) * 16);
            EVE_VERTEX2F(((arc_centerx + arc_radius + 1) * 16), (arc_centery) * 16);
        }

        // 270 - 359 deg
        if ((arc_value >= 270) && (arc_value < 360))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
            EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery -1) * 16);
            EVE_VERTEX2F((arc_centerx * 16) - arc_fill_x, (arc_centery * 16) + arc_fill_y);
        }
    } 

    // draw the outer radius here and blend destination alpha to source alpha to maintain the aliased edge of the circle we are going to draw for the outline
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_SRC_ALPHA);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F(((arc_centerx) * 16), (arc_centery) * 16);

    //--------------------------------------------------------------------------------------------------------
    // Draw the outline section for the arc
    //--------------------------------------------------------------------------------------------------------

    // here we want to keep or increment the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
 
    // begin drawing circles for our arc
    //-------------------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // set alpha to a lower value so this is see through
    EVE_COLOR_A(alpha_value);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
 
    // here we want to increment or keep the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_KEEP);
    // reset alpha to full
    EVE_COLOR_A(255);
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_radius - arc_thickness) * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));


    // edge strips to ensure nothing outwith the arc is coloured
    // here we want to increment/increment the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_ZERO, EVE_STENCIL_ZERO);
    //-------------------------------------------------------------------------
    // adjust edge strip draw shape based on input angles
    if(arc_min_limit >= 135){
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
        EVE_VERTEX2F((arc_centerx * 16), ((arc_centery + arc_radius) * 16));
    }
    else
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);

    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx * 16) - arc_start_x), ((arc_centery * 16) + arc_start_y));
    

    // adjust edge strip draw based on input angles
    if(arc_max_limit <= 225){
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
        EVE_VERTEX2F((arc_centerx * 16), ((arc_centery + arc_radius) * 16));
    }
    else
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
    
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx * 16) - arc_end_x), ((arc_centery * 16) + arc_end_y));

    // if we need to add some rectangles to cover the areas that are missed when swapping from a bottom to a right or left edge strip:
    if(arc_min_limit >= 135){
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
        EVE_VERTEX2F(((arc_centerx - arc_radius) * 16), ((arc_centery + arc_radius) * 16));
    }
    if(arc_max_limit <= 225){
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
        EVE_VERTEX2F(((arc_centerx + arc_radius) * 16), ((arc_centery + arc_radius) * 16));
    }

    // add final shapes for rounded ends
    //-------------------------------------------------------------------------
    // replace stenicl, ensure this point always has a value of 2
    EVE_STENCIL_OP(EVE_STENCIL_REPLACE, EVE_STENCIL_REPLACE);
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 2, 255);
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // add to alpha buffer
    EVE_POINT_SIZE((arc_thickness/2) * 16);
    // left side
    EVE_VERTEX2F(((arc_centerx * 16) - point_start_x) , ((arc_centery * 16) + point_start_y));

    // the point at the we do not wish to overlay this onto the arc as this will blend in an additive fashion
    // draw only where the arc isnt but on top of the blanking edge strip
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 0, 255);
    // the point at the end needs to remiain see through
    EVE_COLOR_A(alpha_value);
    EVE_POINT_SIZE((arc_thickness/2) * 16);
    EVE_VERTEX2F(((arc_centerx * 16) - point_end_x) , ((arc_centery * 16) + point_end_y));
    

    // these circles are used to indicate current value;
    //-------------------------------------------------------------------------
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 1, 255);
    EVE_COLOR_A(255);
    // keep stencil buffer as is from this point
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_KEEP);
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(((arc_thickness/2) + shadow_size) * 16); //guage thickness + shadow_size increase
 
    // draw point based on current input value
    EVE_VERTEX2F((arc_centerx * 16) - (arc_activex), (arc_centery * 16) + (arc_activey));
 
    // keep stencil buffer as is from this point
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    //  add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_thickness/3) * 16); //this should be slighlty smaller than the guage thickness
 
    // draw point based on current input value
    EVE_VERTEX2F((arc_centerx * 16) - (arc_activex), (arc_centery * 16) + (arc_activey));

    // keep stencil buffer as is from this point, reset alpha
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    //--------------------------------------------------------------------------------------------------------
    // Draw a circle which will fill the arc
    //--------------------------------------------------------------------------------------------------------
 
    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 1);
    // blend in colour
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);

    // draw only where the stencil value is <=1 (unfilled section of arc)
    EVE_STENCIL_FUNC(EVE_TEST_LEQUAL, 1, 255);

    // colour based on input colour fopr unfilled section
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR_RGB(((uint8_t)(arc_inactive_color >> 16)), ((uint8_t)(arc_inactive_color >> 8)), ((uint8_t)(arc_inactive_color)));
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));

    // draw only where the stencil value is >=3 (section of arc that has been filled)
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 2, 255);

    // we are using a trick here with an L8 bitmap to blend in colours to the start and end points on this guage
    addGradient(arc_centerx, arc_centery, arc_radius, arc_thickness, 0xDD0000, 0x00FF00);

    // colour based on input colour for filled section
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR_RGB(((uint8_t)(arc_active_color >> 16)), ((uint8_t)(arc_active_color >> 8)), ((uint8_t)(arc_active_color)));
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
 
    // end drawing
    EVE_END();
    //--------------------------------------------------------------------------------------------------------
    // Clean up to avoid affecting other items later in the display list
    //--------------------------------------------------------------------------------------------------------
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_SCISSOR_SIZE(2048,2048);
    EVE_SCISSOR_XY(0,0);

    // restore previous graphics context
    EVE_RESTORE_CONTEXT();
 
}

/**
 @brief Function to render a simple progress bar on screen. 
 @param input_x x value for centre left of widget
 @param input_y y value for centre left of widget
 @param lenght variable to ste the lenght off the progress bar in px
 @param line_width variable to set the line width (thickness) of the progress bar
 @param colour input colour for the progress bar 
 @param value variable to set the fill value of the progress bar (0 - 255)
 */
void graph_line(uint16_t chart_topleft_x, uint16_t chart_topleft_y, uint16_t chart_width, uint16_t chart_height, uint8_t value)
{
    // ensure value doesnt roll over
    value = min(value, 255);
    value = max(value, 0);

    // calculate where the point in the line strip is going to be relative to chart height and value input
    // multiple by 16 so we can add this into the VERTEX2F command directly 
    uint16_t point = ((((value) * chart_height) * 16) / 255);

    // First, we add the new measurement taken above to the results buffer
    chart_result_buffer[chart_input_index] = point;
    // Now, we increment the pointer in a circular fashion. This sets up the pointer for the next reading we take the next time round the loop...
    chart_input_index++;

    // Wrap around at the max x value
    if (chart_input_index >= chart_width)
    {
        chart_input_index = 0;
    }

    // ... and also points to the starting location for the chart drawing
    chart_output_index = 0; //chart_input_index;

 
    EVE_LINE_WIDTH(1 * 16);                                      // Set the width
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);                               // A line strip will be used to plot the chart
    EVE_COLOR_RGB(255, 255, 255);
    EVE_COLOR_A(255);

    chart_index_x = 0;                                           // Chart will be drawn from offset 0 (left hand side) moving rightwards

    while (chart_index_x < chart_width)                                   // for each of the chart points...
    {
        chart_value = chart_result_buffer[chart_output_index];         

        // Draw the current point of the line strip
        // The X coordinate is increased from 0 to 400 each time round this loop
        // The Y coordinate is derived from going to the bottom of the chart area and subtracting the value corresponding to the current value to be displayed (in pixels)

        if (chart_index_x == (chart_input_index)) 
        {
            EVE_COLOR_A(0);
        }
        if (chart_index_x == (((chart_input_index) + 15)))
        {
            EVE_COLOR_A(255);
        }

        EVE_VERTEX2F((chart_topleft_x * 16 + (chart_index_x * 16)), ((chart_topleft_y + chart_height) * 16) - (chart_value));

        // Increment the counter to draw the next point the following time round this loop
        // Because we begin the plot from the location after the last data added, it won't always begin from index 0.
        // Therefore, must handle roll-over.
        chart_output_index++;
        if (chart_output_index >= chart_width)
            chart_output_index = 0;

        chart_index_x++;
    }
    // end line strip
    EVE_END();
    // reset alpha 
    EVE_COLOR_A(255);
}

/**
 @brief Function to render a simple progress bar on screen. 
 @param input_x x value for centre left of widget
 @param input_y y value for centre left of widget
 @param lenght variable to ste the lenght off the progress bar in px
 @param line_width variable to set the line width (thickness) of the progress bar
 @param colour input colour for the progress bar 
 @param value variable to set the fill value of the progress bar (0 - 255)
 */
void progress_bar(uint16_t input_x, uint16_t input_y, uint16_t lenght, uint16_t line_width, uint32_t colour, uint8_t value){

    // ensure value is in range
    value = max(value, 0);
    value = min(value, 255);

    // ensure line width is at least 1
    line_width = max(line_width, 1);

    // normalise value to widget size, also multiple by current pixel precision so we can add this value straight into 
    // the vertex 2F call
    uint16_t progress = (((value * 16) * lenght) / 255);

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw fill
    //----------------------------------------

    // draw shadow   
    EVE_COLOR_A(50);
    // use input colour for lines
    EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour)));
    // begin lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    // set line width
    EVE_LINE_WIDTH(line_width * 16);
    // draw intial shadow bar
    EVE_VERTEX2F((input_x * 16), (input_y * 16));
    EVE_VERTEX2F(((input_x + lenght) * 16), (input_y  * 16));
    // reset alpha
    EVE_COLOR_A(255);
    // draw indication line
    // if the input value/progress is greater than 0
    if (progress > 0){
        EVE_VERTEX2F((input_x * 16), (input_y * 16));
        EVE_VERTEX2F(((input_x * 16) + progress), (input_y  * 16));
    }
    // end drawing
    EVE_END();

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

// ######################################################################################################################################################################################################
// #######################################################               Helper functions for EVE operation (audio/touch)             ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to enable the audio amplifier on the IDM2040-43A moudle
 */
void enable_sound(void){

    // declare local variables
	uint16_t regGpiox;
	uint16_t regGpioxDir;

	// read GPIOX_DIR register
	regGpioxDir = EVE_LIB_MemRead16(EVE_REG_GPIOX_DIR);
	// set bit  2 of  GPIO_DIR register  to output (GPIO2)
	regGpioxDir = regGpioxDir | 0x0004;
	// Enable GPIO2 as an output
	EVE_LIB_MemWrite16(EVE_REG_GPIOX_DIR, regGpioxDir);

	// read REG_GPIOX
	regGpiox = EVE_LIB_MemRead16(EVE_REG_GPIOX);
	// set bit 2 of GPIOX register (GPIO2) high
	regGpiox = regGpiox | 0x0004;
	// Enable the GPIO2 signal to the Audio Driver
	EVE_LIB_MemWrite16(EVE_REG_GPIOX, regGpiox);

	// turn synthesizer volume up
	EVE_LIB_MemWrite8(EVE_REG_VOL_SOUND, 255);
	// set synthesizer to mute
	EVE_LIB_MemWrite8(EVE_REG_SOUND, 0x60);
	//play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);

}

/**
 @brief Function to play the beep sound on the audio synthesizer 
 */
void play_beep(void) {

	// set synthesizer to 1 short pip
	EVE_LIB_MemWrite8(EVE_REG_SOUND, 0x10); 
	// play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);

}

/**
 @brief Function to set screen rotation register via a co-processor list and SETROTATE command
 */
void set_screen_rotation(uint8_t value){

    // begin co-processor list
    EVE_LIB_BeginCoProList();

    EVE_CMD_SYNC();
    // set the screen rotation
    EVE_CMD_SETROTATE(value);

    // send co-processor list to EVE
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
}

/**
 @brief Helper funciton to read current touch inputs and update screen rendering varibles accordingly.
 */
void check_touch_status(void)
{
    // ================ Check for button presses ===========================

    // read REG_TOUCH_TAG and determine what TAG value is returned
    TagVal = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);   

   //-------- Check for pen up and pen down tags -------
	if ((LastTagVal == 0) && (TagVal != 0))	// if there was previously no touch but now there is
		Pen_Down_Tag = TagVal;
	if ((LastTagVal != 0) && (TagVal == 0)) // if there was previously touch but now there isnt
		Pen_Up_Tag = LastTagVal;

	LastTagVal = TagVal;

    //-------- perform logic for settings button-------

    // if the current tag value equals settings tag
	if (TagVal == SETTINGS_TAG)
		settings_menu_press = true; // set settings press to true
    else
	    settings_menu_press = false; // set settings press to false

    // if the pen up tag equals settings tag
	if (Pen_Up_Tag == SETTINGS_TAG){

		//reset variables
		Pen_Down_Tag = 0;
        Pen_Up_Tag = 0;

        // flip settings mode varibale
        settings = !settings;
   	}

    //-------- perform logic for screen rotation button------

    // if the current tag value equals rotation tag
	if (TagVal == SCREEN_ROTATE_TAG)
		screen_rotation_press = true; // set rotation press to true
    else
	    screen_rotation_press = false; // set rotation press to false
    

    // if the pen up tag equals screen rotation tag
	if (Pen_Up_Tag == SCREEN_ROTATE_TAG){
		//reset variables
		Pen_Down_Tag = 0;
        Pen_Up_Tag = 0;

        // flip rotation varibale
        screen_rotation = !screen_rotation;

        // rotate the screen
        if(!screen_rotation){
            set_screen_rotation(3);
        }else{
            set_screen_rotation(2);
        }
   	}


    //-------- perform logic for mute button------

    // if the current tag value equals mute tag
	if (TagVal == MUTE_VOLUME_TAG)
		mute_volume_press = true; // set mute press to true
    else
	    mute_volume_press = false; // set mute press to false
    

    // if the pen up tag equals mute tag
	if (Pen_Up_Tag == MUTE_VOLUME_TAG){
		//reset variables
		Pen_Down_Tag = 0;
        Pen_Up_Tag = 0;

        // flip mute varibale
        mute_volume = !mute_volume;

        if(mute_volume){        
            // turn synthesizer volume down
            EVE_LIB_MemWrite8(EVE_REG_VOL_SOUND, 0);
        }
        else
        {
            // turn synthesizer volume up
            EVE_LIB_MemWrite8(EVE_REG_VOL_SOUND, 255);

        }     
   	}

    //-------- perform logic for lcd backlight slider-------
    // if the tag value is equal to the bbrightness tag i.e. the item using CMD_TRACKER
    if (TagVal == BRIGHTNESS_TAG){

        // reset variables
		Pen_Down_Tag = 0;
		Pen_Up_Tag = 0;    

        // read the tracking register
        TrackValue = EVE_LIB_MemRead32(EVE_REG_TRACKER);

        // normalise 16 bit tracker reading to a 0-100 value
        backlight_value = (((100 * (((TrackValue >> 16)) & 0xffff)) / 0x10000));
   
        // write the backlight strength register based upon the current backlight_value
        // add 27 here to the backlight value so the value range becomes 27-127
        EVE_LIB_MemWrite32(EVE_REG_PWM_DUTY, (backlight_value + 27));
    
    }

    return;
}

// ######################################################################################################################################################################################################
// #######################################################                        EVE screen rendering functions                      ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to render and error screen and message if the sensor encounters an error.
 */
void error_screen(char *message, uint8_t status)
{
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(255, 0, 0);
    EVE_CLEAR(1, 1, 1);

    //rotate the screen
    EVE_CMD_SETROTATE(2);

    EVE_CMD_TEXT(272/2, 0, FONT_SMALL, EVE_OPT_CENTERX, "Pulse Oximeter");
    EVE_CMD_TEXT(272/2, 50, 26, EVE_OPT_CENTERX, "Error message:");
    EVE_CMD_TEXT(272/2, 70, 26, EVE_OPT_CENTERX, message);
    if (status != SFE_BIO_SUCCESS)
        EVE_CMD_TEXT(272/2, 90, 26, EVE_OPT_CENTERX, SparkFun_Bio_Sensor_Hub_error(status));
    
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    while (1)
    {};
}

/**
 @brief Function to add the settigns menu items into the display list.
 */
void settings_menu(void)
{

    // set colour
    EVE_COLOR_RGB(255, 255, 255); // white

    // add text description for brightness slider
    EVE_CMD_TEXT(136, 237, FONT_SMALL, EVE_OPT_CENTERX, "Brightness:");

    // enable tagging
    EVE_TAG_MASK(1);

    // tag brightness item with a CMD_TRACK command
    // make this slightly taller/wider than the slider height
    EVE_CMD_TRACK(70, 250, 132, 30, BRIGHTNESS_TAG);

    // tag screen brightness item with an invisbale rectangle with the same size as the slider above
    EVE_TAG(BRIGHTNESS_TAG);
    // begin rectangles
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // set alpha to 0
    EVE_COLOR_A(0);
    // place rectangle
    EVE_VERTEX2F((70 * 16), (250 * 16)); // 91, 197
    EVE_VERTEX2F(((70 + 132) * 16), ((250 + 30) * 16)); // 185, 227
    // reset alpha
    EVE_COLOR_A(255);

    // add slider for screen brightness
    EVE_CMD_SLIDER(76, 260, 120, 10, EVE_OPT_FLAT, backlight_value, 100);

    // colour if a touch is currently on the button
    if(settings_menu_press)
        EVE_COLOR_RGB(255, 127, 0); // orange
    else
        EVE_COLOR_RGB(255, 255, 255); // white

    // begin bitmaps
    EVE_BEGIN(EVE_BEGIN_BITMAPS);

    // tag screen rotate item
    EVE_TAG(SCREEN_ROTATE_TAG);

    // colour if a touch is currently on the button
    if(screen_rotation_press)
        EVE_COLOR_RGB(255, 127, 0); // orange
    else
        EVE_COLOR_RGB(255, 255, 255); // white

    // select bitmap handle
    EVE_BITMAP_HANDLE(BITMAP_ROTATE);
    // place bitmap
    EVE_VERTEX2F((111 * 16),(167 * 16));

    // tag mute volume item
    EVE_TAG(MUTE_VOLUME_TAG);

    // colour if a touch is currently on the button
    if(mute_volume_press)
        EVE_COLOR_RGB(255, 127, 0); // orange
    else
        EVE_COLOR_RGB(255, 255, 255); // white
    
    // select bitmap handle
    EVE_BITMAP_HANDLE(BITMAP_VOLUME);
    // pick cell based on state
    if(!mute_volume)
        EVE_CELL(0);
    else
        EVE_CELL(1);
    // place bitmap
    EVE_VERTEX2F((111 * 16),(290 * 16));
    
    // end bitmaps
    EVE_END();

    // disable tagging
    EVE_TAG_MASK(0);

}

/**
 @brief Function to generate a display list containing the static screen elements and copy these into RAM_G so they can be appended into screen updates.
 */
void generate_static_screen_components(){

    // create display list for the static screen content
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR_TAG(60); // clear tag to a value we arent using
    EVE_CLEAR(1, 1, 1); // clear c,s,t

    // disable tagging, this prevents items being drawn with tag = 255 when we havent explcitly tagged them 
    EVE_TAG_MASK(0);

    // background gradient section
    //-------------------------------
    // scisscor 1 area
    EVE_SCISSOR_XY(0, 0);
    EVE_SCISSOR_SIZE(272, 480);
    // add gradient
    EVE_CMD_GRADIENT(136, 200, 0x330100, 136, 352, 0x750000);

    // scissor 2 area
    EVE_SCISSOR_XY(0, 380);
    EVE_SCISSOR_SIZE(272, 100);
    // add gradient
    EVE_CMD_GRADIENT(136, 380, 0x750000, 136, 475, 0x0A007F);

    // reset scissor 
    EVE_SCISSOR_XY(0, 0);
    EVE_SCISSOR_SIZE(272, 480);
    //-------------------------------

    // place heart bitmap on screen
    //-------------------------------

    // rectangle to cover/colour the top of the screen
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // set colour
    EVE_COLOR_RGB(1, 32, 95);
    // place verticies
    EVE_VERTEX2F(0,0); // 0, 0
    EVE_VERTEX2F((272 * 16), (3 * 16)); // 272, 3
    // end rectangles
    EVE_END();

    // set colour
    EVE_COLOR_RGB(255, 255, 255); // white
    // begin bitmaps
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use heart logo handle
    EVE_BITMAP_HANDLE(BITMAP_HEART);
    // place on screen
    EVE_VERTEX2F(0, (3 * 16)); // 0, 3
    // end bitmaps
    EVE_END();

    // draw shading rectangle for usage prompt
    // set colour
    EVE_COLOR_RGB(0, 0, 0); // black
    // set alpha
    EVE_COLOR_A(100);
    // set line width
    EVE_LINE_WIDTH(32); // 2
    // begin rectanlges
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // place vertecies
    EVE_VERTEX2F((50 * 16), (97 * 16)); // 0, 100
    EVE_VERTEX2F((223 * 16), (125 * 16)); // 272, 127
    // end rectangles
    EVE_END();

    //-------------------------------
    // add heart rate text
    //-------------------------------
    // set colour
    EVE_COLOR_RGB(255, 255, 255); // white
    // set text alpha
    EVE_COLOR_A(100);
    // add text
    EVE_CMD_TEXT(136, 9, FONT_SMALL, EVE_OPT_CENTER, "Heartrate (BPM)");


    // save context as we are going to do some alpha compositing for the following shapes
    EVE_SAVE_CONTEXT();

    // disable colours, leave alpha enabbled
    EVE_COLOR_MASK(0, 0, 0, 1);
    // clear colours
    EVE_CLEAR(1, 0, 0); 
    // set transparency for following shapes in alpha buffer
    EVE_COLOR_A(100);

    // add shapes to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_SRC_ALPHA);

    // add shading shapes or confidence and settings
    //------------------------------
    // begin edge strip L
    EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
    // place vertecies for confidence
    EVE_VERTEX2F((45 * 16), (339 * 16)); // 45, 339
    EVE_VERTEX2F((81 * 16), (374 * 16)); // 81, 374
    // begin edge strip R
    EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
    // enable tagging
    EVE_TAG_MASK(1);
    // set tag
    EVE_TAG(SETTINGS_TAG);
    // place verticies for settings
    EVE_VERTEX2F((226 * 16), (339 * 16)); // 226, 339 
    EVE_VERTEX2F((190 * 16), (374 * 16)); // 190, 374
    // disable tagging
    EVE_TAG_MASK(0);
    // end rectangles
    EVE_END();


    // add arc guage shadow,
    //-------------------------------
    // draw a shadow point behind the gauge
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // set  full transparency for following shapes in alpha buffer
    // as were going to remove tis from the compositied shape to cut out a circular
    // edge from the edge strips added to the buffer above
    EVE_COLOR_A(255);
    // remove shape from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // set point size + 25 of arc radius
    EVE_POINT_SIZE(135 * 16);
    // place point on screen
    EVE_VERTEX2F((136 * 16), (250 * 16)); // 136, 255

    // add shapes to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // set transparency for following shapes in alpha buffer
    EVE_COLOR_A(100);
    // set point size + 5 of arc radius
    EVE_POINT_SIZE(115 * 16);
    // place point on screen
    EVE_VERTEX2F((136 * 16), (250 * 16)); // 136, 255
    // end points
    EVE_END();
    //-------------------------------

    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 0);
    // use bbblend funciton to add colour to shape in alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
    // set colour
    EVE_COLOR_RGB(0, 0, 0); // bblack
    // draw shape to cover items in alpha buffer (can just paint the whole screen here)
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_VERTEX2F(0, 0); //  0, 0
    EVE_VERTEX2F((272 * 16), (480 * 16)); // 272, 480
    EVE_END();


    // end of alpha compositiong, restore context
    EVE_RESTORE_CONTEXT();

    // add  text/image for confidence and settings
    //------------------------------

    // set colour
    EVE_COLOR_RGB(255, 255, 255); // white
    // set alpha
    EVE_COLOR_A(150);
    // add confidecnce text
    EVE_CMD_TEXT(4, 356, FONT_SMALL, 0, "Conf %");

    // add settings icon onto screen
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // enable tagging
    EVE_TAG_MASK(1);
    // set tag
    EVE_TAG(SETTINGS_TAG);
    // select settings handle
    EVE_BITMAP_HANDLE(BITMAP_SETTINGS);
    // place and tag bitmap
    EVE_VERTEX2F((235 * 16), (340 * 16));
    // end bitmaps
    EVE_END();
    // disable tagging
    EVE_TAG_MASK(0);
    //------------------------------


    // add IR graph background and text
    //-------------------------------
    // add text, but add the space the text occupies to the stencil buffer
    // so we can draw the graph lines later without these touching the text

    // replace stencil buffer
    EVE_STENCIL_OP(EVE_STENCIL_REPLACE, EVE_STENCIL_REPLACE);
    // replace following items with value = 1
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 1, 255);
    // set colour
    EVE_COLOR_RGB(255,255,255);
    // set alpha
    EVE_COLOR_A(100);
    // add text (this text is in the stencil buffer now)
    EVE_CMD_TEXT(131, 431, FONT_MEDIUM, EVE_OPT_CENTER, " IR led");
    // keep the stencil buffer from this point
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    // where the stencil is NOT equal to 1
    EVE_STENCIL_FUNC(EVE_TEST_NOTEQUAL, 1, 255);

    // draw the graph lines bitmap
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use graph lines handle
    EVE_BITMAP_HANDLE(BITMAP_GRAPH);
    // configure the size so the bitmap repeats in the X and Y direction
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_REPEAT, EVE_WRAP_REPEAT, 0, 0);
    // place bitmpa on screeen
    EVE_VERTEX2F(0, (383 * 16)); // 0, 383
    // end bitmaps
    EVE_END();

    // reset stencil function
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 0);

    // draw shading rectangle over graph lines
    // set colour
    EVE_COLOR_RGB(0, 0, 0); // black
    // begin rectanlges
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // place vertecies
    EVE_VERTEX2F(0, (383 * 16)); // 0, 383
    EVE_VERTEX2F((272 * 16), (480 * 16)); // 272, 480
    // end rectangles
    EVE_END();

    // send list to the co-processor but dont display it
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // determine RAM_DL usage for the static parts of the screen
    static_screen_size = EVE_LIB_MemRead32(EVE_REG_CMD_DL);

    // copy static screeen DL into a unoccpuied section of RAM_G
    EVE_LIB_BeginCoProList();
    EVE_CMD_MEMCPY(static_screen_location, EVE_RAM_DL, static_screen_size);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

}

/**
 @brief Function to append static screen elemments into a display list, and then add dynamic elements into screen updates based upon
 current global variable values.
 */
void render_screen_update(void){

    // before we render the screen check if a touch has occured
    check_touch_status();

    // generate display list to render the screen
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();

    // append static section of dispplay list
    EVE_CMD_APPEND(static_screen_location, static_screen_size);

    // set colour
    EVE_COLOR_RGB(255, 255, 255);
    // set alpha
    EVE_COLOR_A(255);

    // add current heartrate number
    EVE_CMD_NUMBER(135, 45, FONT_MEDIUM, EVE_OPT_CENTER, heart_rate);

    // add prompt message and compute 'progress' value
    // determine message based on current readings state and finger status
    // use current live sensor readings for finger position
    if (body.status < 1){
        // add text
        EVE_CMD_TEXT(136, 107, FONT_SMALL, EVE_OPT_CENTER, "Place finger on sensor");
        // compute progress
        if (progress > 0)
            progress = max((progress - 4) , 0);

    }else if ((body.confidence > 90 && body.oxygen > 0) || (body.heartRate > 0 && body.oxygen > 0)){
        // add text
        EVE_CMD_TEXT(136, 107, FONT_SMALL, EVE_OPT_CENTER, "Reading success!");
        // compute progress
        if(progress < 255)
            progress ++;

    }else if (body.status >= 1){
        if (body.status >=2 && (body.heartRate <= 0 || body.oxygen <= 0)){
            // add text 
            EVE_CMD_TEXT(136, 107, FONT_SMALL, EVE_OPT_CENTER, "Adjust finger position");
            // compute progress
            if(progress < 170)
                progress ++;
            if (progress > 170)
                progress --;
        }            
        else{
            // add text
            EVE_CMD_TEXT(136, 107, FONT_SMALL, EVE_OPT_CENTER, "Hold finger steady");
            // compute progress
            if(progress < 85)
                progress ++;
            if (progress > 85)
                progress --;

        }
    }

    // add progress bar
    progress_bar(65, 120, 142, 3, 0x00FF00, progress);

    // do we need to render the settings menu?
    // this is place on scree where the Spo2 readout is, so decide if we want to render the menu
    // or render the Spo2 ARC and readouts
    if (settings) {
        // add settigns menu items onto screen
        settings_menu();
    }else {
        // add custom arc guage widget 
        arc_point_gauge(136, 250, 110, 24, 0xFFFF00, 0xFF00FF, ((spo2_arc * 255)/250)); // varible range is from 0-250

        // place the SPo2 readout on the screen if we have a reading in the valid range   , if not print the finger icon
        if(spo2 >= 90 ){
            EVE_CMD_NUMBER(136, 250, FONT_LARGE, EVE_OPT_CENTER, spo2);
        }else{
            // add finger icon onto screen
            EVE_BEGIN(EVE_BEGIN_BITMAPS);
            // set bitmap handle to use
            EVE_BITMAP_HANDLE(BITMAP_FINGER);
            // place bitmap
            EVE_VERTEX2F((92 * 16), (200 * 16));
            // end bitmaps
            EVE_END();
        }

        // place description text on the screen
        EVE_CMD_TEXT(137, 335 , FONT_MEDIUM, EVE_OPT_CENTER, "SPo2 %");
    }

    // set colour for confidence number
    // determine value based on current confidence reading
    if (confidence < 20)
        EVE_COLOR_RGB(255, 0, 0); // red
    else if (confidence >= 20 && confidence < 80)
        EVE_COLOR_RGB(255, 255, 0); // yellow 
    else if (confidence > 80)
        EVE_COLOR_RGB(0, 255, 0); // green
    
    // add current confidence number
    EVE_CMD_NUMBER(20, 349, FONT_SMALL, EVE_OPT_CENTER, confidence);

    // add custom line graph widget onto screen
    graph_line(0, 384, 272, 94, waveform_value); 
    //-------------------------------

    // if the debug level is > 0 print the current sensor readings on the screen (not the associated globals)
#if (DEBUG_LEVEL > 0)
    EVE_CMD_NUMBER(0,200,27,0, body.status);
    EVE_CMD_NUMBER(0,225,27,0, body.confidence);
    EVE_CMD_NUMBER(0,250,27,0, body.heartRate);
    EVE_CMD_NUMBER(0,275,27,0, body.oxygen);  
#endif

    EVE_DISPLAY();
    EVE_DISPLAY(); // per BRT_TN_005
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

}

// ######################################################################################################################################################################################################
// #######################################################               Main loop to read sensor and call screen updates             ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to send display list updates to EVE within a while(1) 'main' loop and read sensor inputs. This fucntion will generate the static components of the screen. It thenreads data from the attached pulse oximeter sensor and
 selectiovely updates assoicated readout global varibles within a while (1) loop. It will then call screen updates when required to update the dynamic sections of the display based on sensor readings.
 of the screen.
 */
void eve_display(void)
{
    // local varibles 
    uint8_t num_available_samples;

    uint8_t algoRange = 80; // ADC Range (0-100%)
    uint8_t algoStepSize = 20; // Step Size (0-100%)
    uint8_t algoSens = 20; // Sensitivity (0-100%)
    uint8_t algoSamp = 10; // Number of samples to average (0-255)

    // turn on the audio amplifier
    enable_sound();

    // set the screen rotation
    set_screen_rotation(3);

    // generate static screen sections
    generate_static_screen_components();
 
    // start and configure sensor
    DEBUG_PRINTF("Starting the SparkFun Bio Sensor Hub...\n");
    // This example will use I2C0 on the default SDA and SCL pins
    status_bio = SparkFun_Bio_Sensor_Hub_begin();
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Could not start SparkFun Bio Sensor Hub", status_bio);
    DEBUG_PRINTF("Done\n");

    // set ACG algo range
    status_bio = SparkFun_Bio_Sensor_Hub_setAlgoRange(algoRange);
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Could not set algo range", status_bio);

    // set ACG algo step size
    status_bio = SparkFun_Bio_Sensor_Hub_setAlgoStepSize(algoStepSize);
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Could not set algo step size", status_bio);

    // set ACG algo sensitivity
    status_bio = SparkFun_Bio_Sensor_Hub_setAlgoSensitivity(algoSens);
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Could not set algo sensetivity", status_bio);

    // set ACG algo samples
    status_bio = SparkFun_Bio_Sensor_Hub_setAlgoSamples(algoSamp);
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Could not set algo samples", status_bio);

    DEBUG_PRINTF("Configuring Sensor...\n"); 
    status_bio = SparkFun_Bio_Sensor_Hub_configSensorBpm(SPARKFUN_BIO_SENSOR_HUB_MODE_ONE); // Configuring BPM settings for MODE one. 
    if (status_bio != SFE_BIO_SUCCESS)
        error_screen("Error configuring sensor for BPM.", status_bio);
    DEBUG_PRINTF("Done.");

    while(1)
    {
        // reset samples available counter
        num_available_samples = 0;

        // check the number of samples aviable in the FIFO
        status_bio = SparkFun_Bio_Sensor_Hub_numSamplesOutFifo(&num_available_samples);
            if (status_bio != SFE_BIO_SUCCESS)
                error_screen("Error reading from sensor.", status_bio);

        // if we have some readings in the fifo
        if(num_available_samples > 0){
            // for every sample in the FIFO
            for (int i=0; i < num_available_samples; i++){  // only take the three latest samples
                // set time for update calculation
                current_ms = platform_get_time();
                // read sensor values
                status_bio = SparkFun_Bio_Sensor_Hub_readSensorBpm(&body);
                // print debug message
                if(status_bio == SFE_BIO_SUCCESS){
                    DEBUG_PRINTF("%d %d %d LEDs ->  IR: %d  RED: %d  Body-> Conf:%d  HR:%d  Oxy:%d  Status: %d\n", current_ms, num_available_samples, i, body.irLed, body.redLed, body.confidence, body.heartRate, body.oxygen, body.status);
                }else{
                    error_screen("Error reading data from FIFO.", status_bio);
                }

                // if the finger status is > 0
                // we want to start calculating the waveform values
                // -----------------------------------------------------------
                if(body.status > 0){

                    // add current red LED reading into array
                    led_vals[led_vals_index] = body.irLed;
                    
                    // find max in current historical array
                    led_max = led_vals[led_vals_index];
                    for(int i = 0; i < led_sample_range; i++){
                        if(led_vals[i] > led_max){
                            // adjust max
                            led_max = led_vals[i];
                        }
                    }

                    // find min in current historical array
                    led_min = led_vals[led_vals_index];
                    for(int j = 0; j < led_sample_range; j++){
                        if(led_vals[j] < led_min){
                            // set new min
                            led_min = led_vals[j];
                        }
                    }

                     // calculate current range
                    led_range = (led_max - led_min);

                    // if we have a valid range, and a confidence level > 90, calculate the wavefrom level
                    if ((led_range > 0 && body.confidence > 90) || (led_range > 0 && body.status >=1)){
                        //(xi – min(x)) / (max(x) – min(x))  
                        waveform_value = (((body.irLed - led_min) * 255) / led_range); 
                        //DEBUG_PRINTF("%d %d %d %d %d Wav: %d\n", led_vals_index, led_max, led_min, led_range, body.irLed, waveform_value);
                    }else{
                        waveform_value = 0;
                    }

                    // increment index
                    led_vals_index ++;

                    // roll over if we reach the end of the array
                    if(led_vals_index == led_sample_range){
                        led_vals_index = 0;
                    }

                }else{
                    // set waveform value to 0
                    waveform_value = 0;
                }

                // if the confidence is < 90 or the finger status isnt 3 set the screen readout variables to 0
                // else we want ot update these with the current values and sweep the spo2 arc
                if (body.status > 2 && body.confidence > 90){
                    // update spo2
                    spo2 = body.oxygen;

                    // make sure we have a spo2 reading before updating the heart rate
                    if (body.oxygen > 0)
                        heart_rate = body.heartRate;

                    // sweep reading on the arc gauge
                    if(spo2 >= 90){
                        if (spo2_arc < ((spo2-90) * 25))
                            spo2_arc ++;
                        
                        if (spo2_arc > ((spo2-90) * 25))
                            spo2_arc --;
                    }
                }

                // if the finger moves off the sensor reset the confidence varaible
                // else set the current confidence level
                if (body.status <= 1){
                    confidence = 0;
                } else{
                    confidence = body.confidence;
                }

                // play beep to indicate heart beat if we the global for heart rate is greater than 0
                // use global heart rate here
                if (heart_rate > 0){
                    // detrmine heart rate interval in ms
                    heart_rate_interval = (60000/heart_rate); 
                    
                    // play the beep if we have reached the heart rate interval
                    if((current_ms - elapsed_ms) >= heart_rate_interval){
                        // set elapsed ms to current ms
                        elapsed_ms = current_ms;
                        // play beep (if we are not muted)
                        if (!mute_volume)
                            play_beep();
                    }
                }

                // render the screen 
                render_screen_update();
            }             
        }
        else{

            // if the finger is off of the sensor then roll back the global values
            if(body.status < 1 ){
                spo2 = max((spo2 - 5),0);
                spo2_arc =  max((spo2_arc - 10),0);
                heart_rate = max((heart_rate - 5),0);
                confidence = 0;
            }

           //call screen render funciton
           render_screen_update();
        }
    }
}

// ######################################################################################################################################################################################################
// #######################################################                        Application Code begins here                        ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to start the EVE applicaiton, called from main.c. This funciton will call seperate funcitons to initailize EVE,
 calibrate the touch screen, set asset properties, and load assets into EVEs RAM_G. Finally it will call eve_display() to run the
 main display loop and update the screen. 
 */
void eve_example(void)
{
    DEBUG_PRINTF("EVE_Init\n");
    EVE_Init();             // Initialise the display

    DEBUG_PRINTF("eve_calibrate\n");
    eve_calibrate();        // Calibrate the display

    DEBUG_PRINTF("eve_display_load_assets\n");
    static_screen_location = eve_display_load_assets();

    DEBUG_PRINTF("eve_display\n");
    eve_display();
}