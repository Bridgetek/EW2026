/**
 * @file app_page_dialer.c
 * @brief Dialer page implementation with keypad input and call trigger functionality
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_lobby_entry.h"

const Image *IMG_NUMBERS[] = {
    ICON_DIGIT_0,
    ICON_DIGIT_1,
    ICON_DIGIT_2,
    ICON_DIGIT_3,
    ICON_DIGIT_4,
    ICON_DIGIT_5,
    ICON_DIGIT_6,
    ICON_DIGIT_7,
    ICON_DIGIT_8,
    ICON_DIGIT_9};
const uint32_t icon_number_count = sizeof(IMG_NUMBERS) / sizeof(IMG_NUMBERS[0]);

/* Static state variables */
static bool g_is_page_dialer_init = false;
static bool g_is_page_dialer_deinit = false;
static uint32_t g_frame_count = 0;
static uint32_t g_last_time = 0;
static uint32_t g_fps_int = 0;  /* Integer part of FPS */
static uint32_t g_fps_frac = 0; /* Fractional part (1 decimal) */
static uint32_t g_sumbol_len = 0;

/**
 * @brief Initialize dialer page once
 */
static void page_dialer_init_one()
{
    if (g_is_page_dialer_init)
        return;
    g_is_page_dialer_init = true;
}

/**
 * @brief Deinitialize dialer page once
 */
static void page_dialer_deinit_one()
{
    if (g_is_page_dialer_deinit)
        return;

    g_is_page_dialer_deinit = true;
}

/**
 * @brief Handle touch events for dialer page
 * @return PAGE_FINISH if back/call pressed, PAGE_CONTINUE otherwise
 */
static int event()
{
    Gesture_Touch_t *ges = utils_gesture_get();
    if (ges->tagReleased == 0)
    {
        /* No tag released, ignore */
        return PAGE_CONTINUE;
    }

    /* Handle back button */
    if (ges->tagReleased == ICON_BACKWARD->tagval)
    {
        g_is_menu_active = 1;
        return PAGE_FINISH;
    }

    /* Handle backspace button */
    if (ges->tagReleased == ICON_BACKSPACE->tagval)
    {
        if (g_sumbol_len > 0){
            g_pressed_symbols[g_sumbol_len - 1] = '\0';
            g_sumbol_len--;
        }
        return PAGE_CONTINUE;
    }

    /* Handle number buttons */
    for (uint32_t i = 0; i < icon_number_count; i++)
    {
        if (ges->tagReleased == IMG_NUMBERS[i]->tagval && g_sumbol_len < MAX_PRESSED_BUTTONS-1)
        {
            g_pressed_symbols[g_sumbol_len] = '0' + i;
            g_pressed_symbols[g_sumbol_len+1] = 0;
            g_sumbol_len++;
            break;
        }
    }

    /* Handle call button */
    if (ges->tagReleased == ICON_GREEN_CALL->tagval && g_sumbol_len > 0)
    {
        utils_event_menu_selected();
        return PAGE_FINISH;
    }

    return PAGE_CONTINUE;
}

/**
 * @brief Draw dialer page elements
 * @return 0 on success
 */
static int drawing()
{
    const uint32_t margin = 10;
    const uint32_t btn_w = ICON_DIGIT_0->w + margin;
    const uint32_t btn_h = ICON_DIGIT_0->h + margin;
    const uint32_t base_x = (PHOST->Width - btn_w * 3) / 2;
    const uint32_t base_y = 100 + ICON_DIGIT_0->h * 2;
    const uint32_t bottom_y = base_y + 3 * btn_h;

    /* Calculate and display FPS */
    g_frame_count++;
    uint32_t current_time = EVE_millis();
    uint32_t elapsed = current_time - g_last_time;
    if (elapsed >= 1000)
    {
        /* FPS * 10 to keep one decimal place */
        uint32_t fps_x10 = (g_frame_count * 10000) / elapsed;
        g_fps_int = fps_x10 / 10;
        g_fps_frac = fps_x10 % 10;
        g_frame_count = 0;
        g_last_time = current_time;
    }

    /* Draw input text */
    int text_x = base_x + btn_w * 3 - margin;
    EVE_CoCmd_text(PHOST, text_x, 380, 34, OPT_RIGHTX, g_pressed_symbols);

    /* Draw backspace button */
    utils_draw_image_xy(PHOST, ICON_BACKSPACE, text_x + 20, 370);

    /* Draw number pad 1-9 */
    for (uint32_t i = 0; i < 9; i++)
    {
        uint32_t col = i % 3, row = i / 3;
        utils_draw_image_xy(PHOST, IMG_NUMBERS[1 + i],
                            base_x + col * btn_w, base_y + row * btn_h);
    }

    /* Draw 0 and call buttons */
    utils_draw_image_xy(PHOST, IMG_NUMBERS[0], base_x + btn_w, bottom_y);
    utils_draw_image_xy(PHOST, ICON_GREEN_CALL, base_x + btn_w * 2, bottom_y);

    utils_draw_back_button();
    return 0;
}

/**
 * @brief Initialize dialer page
 */
void page_dialer_init()
{
    page_dialer_init_one();
    g_pressed_symbols[0] = 0;
    g_sumbol_len = 0;
}

/**
 * @brief Deinitialize dialer page
 */
void page_dialer_deinit()
{
    page_dialer_deinit_one();
}

/**
 * @brief Draw dialer page and handle events
 * @return Page status (PAGE_FINISH or PAGE_CONTINUE)
 */
int page_dialer_draw()
{
    int ret = event();
    drawing();
    return ret;
}

/**
 * @brief Load dialer page resources
 * @return 0 on success
 */
int page_dialer_load()
{
    return 0;
}
