/**
 * @file app_page_main.c
 * @brief Main page implementation with menu and video overlay
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "dcmi.h"
#include "main.h"

#include "utils_paging.h"
#include "utils_image.h"
#include "utils_gesture.h"
#include "utils_box.h"
#include "app_lobby_entry.h"

bool g_is_menu_active = 0;

uint32_t video_w = 1024;
uint32_t video_h = 600;

extern uint32_t g_video_size;
extern uint32_t g_video_ram_addr;

/**
 * @brief Handle gesture events and update menu states
 *
 * This function processes gesture input to manage menu interactions.
 * It handles the release of gesture tags, toggles menu activity,
 * and updates the hover state. If the close tag is released, all pages
 * are deselected. The function also checks if the main menu icon is
 * activated and toggles the menu's active state accordingly.
 *
 * @return PAGE_CONTINUE always
 */
static int event()
{
    return PAGE_CONTINUE;
}

/**
 * @brief Draw a menu item with frame
 * @param itemimg The menu item image
 * @param x The x position of the menu item
 * @param y The y position of the menu item
 */
static void draw_menu_item(Image *itemimg, int x, int y, int zoom, int tag, const char *page_name)
{
    if (tag == 0)
        tag = itemimg->tagval;

    int w = itemimg->w * zoom / 100;
    int h = itemimg->h * zoom / 100;

    int x_center = x + w / 2;
    int y_center = y + h / 2;
    int x_item = (x_center - itemimg->w * zoom / 100 / 2);
    int y_item = (y_center - itemimg->h * zoom / 100 / 2);

    if (zoom != 100)
    {
        EVE_CoDl_saveContext(PHOST);
        utils_eve_scale(zoom);
        EVE_CoDl_restoreContext(PHOST);
    }

    EVE_Cmd_wr32(PHOST, TAG(tag));

    EVE_Cmd_wr32(PHOST, COLOR_RGB(20, 20, 20));
    EVE_Cmd_wr32(PHOST, COLOR_A(50));
    utils_draw_rects(PHOST, (x - 20), (y - 20), (itemimg->w + 30), (itemimg->h + 30));

    EVE_Cmd_wr32(PHOST, COLOR_RGB(255, 255, 255));
    EVE_Cmd_wr32(PHOST, COLOR_A(255));
    utils_draw_image_xy_tag(PHOST, itemimg, x_item, y_item, tag);
}

void draw_main_menu()
{
    utils_draw_image_xy_tag(PHOST, ICON_BACKGROUND, 0, 0, 0); // background image, no touch tag

    app_box menu_box = {.x = g_lobby_geo.x, .y = g_lobby_geo.y, .w = g_lobby_geo.w, .h = g_lobby_geo.h};
    app_box menu_item_box = {0, 0, ICON_QR_CODE_SCANNER->w, ICON_QR_CODE_SCANNER->h}; // icon sizes are same

    int margin = 100;

    // auto calculation
    int num_frame_per_row = 2;
    int num_frame_col = 2;

    int row_width = num_frame_col * (menu_item_box.w + margin);
    int row_height = num_frame_per_row * (menu_item_box.h + margin);

    int offsetx = (menu_box.w - row_width) / 2;
    int offsety = (menu_box.h - row_height) / 2 + 200;

    for (int i = 0; i < g_num_pages - 1; i++)
    {
        int col = i % num_frame_per_row;
        int row = i / num_frame_per_row;

        int x = offsetx + menu_box.x + col * (menu_item_box.w + margin);
        int y = offsety + menu_box.y + row * (menu_item_box.h + margin);

        draw_menu_item(menu_pages[i].icon, x, y, 100, 0, menu_pages[i].page->ui_name);
    }

    EVE_CoDl_tagMask(PHOST, 0);
}

static void draw_video()
{
    EVE_CoDl_vertexTranslateX(PHOST, (PHOST->Width - video_w) / 2 * 16);
    EVE_CoDl_vertexTranslateY(PHOST, 1200 * 16);

    EVE_CoCmd_mediaFifo(PHOST, g_video_ram_addr, g_video_size + 10 * 1024); // set a mediafifo larger than data
    EVE_CoCmd_regWrite(PHOST, REG_MEDIAFIFO_READ, 0);
    EVE_CoCmd_regWrite(PHOST, REG_MEDIAFIFO_WRITE, g_video_size); // only pass part of video data to mediafifo

    EVE_CoCmd_playVideo(PHOST, OPT_OVERLAY | OPT_YCBCR | OPT_MEDIAFIFO);
}

static void draw_marquee_text()
{

    static int32_t marquee_x = 0;
    const char *marquee_str = "Welcome to the Lobby Entry Demo Application! This application showcases EVE's graphics capabilities.";
    EVE_CoDl_macro(PHOST, 0);
    EVE_CoCmd_text(PHOST, marquee_x, 1850, 31, 0, marquee_str);
    EVE_CoCmd_text(PHOST, marquee_x + MARQUEE_TEXT_OFFSET_X, 1850, 31, 0, marquee_str);
}

static bool drawing()
{
    if (g_is_menu_active == 0)
    {
        return;
    }
    draw_main_menu();
    draw_marquee_text();
    draw_video();

    return 0;
}

static void page_main_init_one()
{
    static bool is_page_main_init = 0;
    if (is_page_main_init == 1)
        return;
    is_page_main_init = 1;
}

int page_main_load()
{
    return 0;
}

int page_main_draw()
{
    int ret = event();
    drawing();
    return ret;
}

void page_main_init()
{
    page_main_init_one();
}

void page_main_deinit() {}
