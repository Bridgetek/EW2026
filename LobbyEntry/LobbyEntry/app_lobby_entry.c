/**
 * @file app_lobby_entry.c
 * @brief Main entry point for the lobby entry application
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "dcmi.h"
#include "eve.h"
#include "utils_eve.h"
#include "utils_box.h"
#include "app_assets.h"
#include "app_lobby_entry.h"

char g_pressed_symbols[MAX_PRESSED_BUTTONS] = "";
int g_num_pages = sizeof(menu_pages) / sizeof(menu_pages[0]);

app_box g_lobby_geo = {.x = 10, .y = 200, .w = 1200, .h = 900};
app_box g_cam_geo = {.x = 0, .y = 950, .w = 1200, .h = 900};

uint32_t g_video_ram_addr = 0;
uint32_t g_video_size = 0;
static volatile bool b_main_touch = true;

uint8_t g_camera_buffer[CAMERA_BUFFER_SIZE] = {0};
__attribute__((section(".dma_buffer"), aligned(32))) uint8_t s_camera_dcmi_buf[DMA_CHUNK] = {0};

PAGE(main, "Main");
PAGE(qr, "QR Code Scanner");
PAGE(directory, "Directory");
PAGE(dialer, "Dialer");
PAGE(help, "Help");
PAGE(call, "Call");

static utils_paging *all_pages[] = {
    &page_main,
    &page_qr,
    &page_directory,
    &page_dialer,
    &page_help,
    &page_call,
};

#define page_count sizeof(all_pages) / sizeof(all_pages[0])
static void preload_pages(EVE_HalContext *phost)
{
    for (int i = 0; i < page_count; i++)
    {
        utils_paging *p = all_pages[i];
        if (!p)
        {
            continue;
        }

        if (p->load)
        {
            p->load();
        }
    }
}

static void load_images_from_sdcard(EVE_HalContext *phost)
{
    eve_printf_debug("Loading %u images from SD card...\n", IMG_COUNT);
    for (int i = 0; i < IMG_COUNT; i++)
    {
        uint32_t image_load_status = 0;

        image_load_status = utils_load_image_from_sd_card(phost, &demo_images[i]);
        if (image_load_status || !demo_images[i].is_loaded) // error with the SDcard reading
        {
            char err_msg[100] = {0};
            snprintf(err_msg, 100, "ERR: loading %s\n", demo_images[i].sd_path);
            EVE_CoCmd_fillWidth(PHOST, phost->Width - 40);
            EVE_CoCmd_text(phost, 20, phost->Height - 500, 28, OPT_FILL, err_msg);
            printf("%s", err_msg);

            utils_display_end(phost);
            while (1)
            {
                EVE_sleep(1000);
            }
        }
    }
}

// load video from sd card to ddr to prevent SD error
static void load_video_from_sdcard(EVE_HalContext *phost, const uint8_t *sd_path)
{
    eve_printf_debug("Loading video from SD card: %s\n", sd_path);

    uint32_t ret = EVE_CoCmd_fsSource(phost, (const char *)sd_path);
    if (ret != 0)
    {
        eve_printf_debug("ERROR loading video from SD card!, code %d\n", ret);
        while (1)
        {
            EVE_sleep(1000);
        }
    }
    g_video_size = EVE_CoCmd_fsSize(phost, sd_path);
    g_video_ram_addr = utils_ddr_alloc_alignment(g_video_size, 32);
    eve_printf_debug("Video size: %u bytes, allocated at 0x%08X\n", g_video_size, g_video_ram_addr);
    if (g_video_size > 0)
    {
        ret = EVE_CoCmd_fsRead(phost, g_video_ram_addr, (const char *)sd_path);
        if (ret != 0)
        {
            eve_printf_debug("ERROR reading video from SD card!, code %d\n", ret);
            while (1)
            {
                EVE_sleep(1000);
            }
        }
    }
}

void lobby_entry_init(EVE_HalContext *pHost)
{
    utils_display_start(pHost);
    page_main.is_active = 1;
    g_is_menu_active = 1;
    page_dialer.is_active = 0;
    page_qr.is_active = 0;
    page_help.is_active = 0;
    page_directory.is_active = 0;

    /* Initialize EVE's DDR memory */
    const uint32_t max_ramg = 2 * 1024 * 1024 * 1024; /* Maximum 2 GB */
    utils_ddr_init(pHost->DDR_RamSize > 0 ? pHost->DDR_RamSize : max_ramg);

    // preload all pages
    preload_pages(pHost);

    // preload all images from sd card
    load_images_from_sdcard(pHost);
    load_video_from_sdcard(pHost, "assets_sdcard_eve/Lobby-entry-ew2026/avi/Building-Lobby.avi");

    utils_display_end(pHost);
    eve_printf_debug("Lobby Entry Initialization Complete\n");
}

static void process_event(void)
{
    if (b_main_touch)
    {
        Gesture_Touch_t *ges = utils_gesture_renew(PHOST);
    }
}

static void drawing(void)
{
    if (g_is_menu_active)
    {

        // display list begin/end handled inside to use playvideo
        utils_display_start(PHOST);
        utils_page_run_one(&page_main);
        uint16_t rp;
        uint16_t wp;
        static int32_t marquee_x = 0;
        bool b_stop = false;

        // playVideo will block coprocessor. So we need to monitor the touch event here.
        while ((rp = EVE_Cmd_rp(PHOST)) != (wp = EVE_Cmd_wp(PHOST)))
        {
            LED_Blink(1, 1);
            b_main_touch = false;
            marquee_x -= MARQUEE_TEXT_SPEED;
            if (marquee_x < -MARQUEE_TEXT_OFFSET_X * MARQUEE_TEXT_STEP)
            {
                marquee_x = 0; // reset to the restart position
            }
            EVE_Hal_wr32(PHOST, REG_MACRO_0, VERTEX_TRANSLATE_X((marquee_x / MARQUEE_TEXT_STEP * 16)));
            Gesture_Touch_t *ges = utils_gesture_renew(PHOST);
            if (ges->tagReleased && ges->tagReleased != 255 && ges->tagReleased != ICON_BACKWARD->tagval)
            {
                b_stop = true;
                eve_printf_debug("tagReleased: %d\n", ges->tagReleased);
                EVE_Hal_wr32(PHOST, REG_PLAY_CONTROL, -1);
                utils_event_menu_selected();
                break;
            }
        }

        if (b_stop)
        {
            // Return to main page
            b_main_touch = true;
            utils_display_end(PHOST);
            EVE_Hal_wr32(PHOST, REG_PLAY_CONTROL, 1);
        }

        return;
    }

    /* Loop through all pages */
    utils_display_start(PHOST);
    utils_page_run(all_pages, page_count);
    utils_display_end(PHOST);
}

uint8_t check_LVDS_connection(void);

void lobby_entry(EVE_HalContext *pHost)
{
    EVE_Hal_wr32(PHOST, REG_CTOUCH_EXTENDED, 0);
    lobby_entry_init(pHost);
    check_LVDS_connection();

    while (1)
    {
        drawing();
        process_event();
    }
}

/**
 * @brief Deselect all pages
 *
 * Deselect all pages and deinit them
 */
void utils_deselect_all_pages()
{
    for (int i = 0; i < g_num_pages; i++)
    {
        utils_paging *p = menu_pages[i].page;
        p->is_active = false;
        p->is_initialized = false;
        p->deinit();
    }
}

/**
 * @brief Check if a menu item is selected
 *
 * Iterate through all menu items. If any menu item is selected, set it to active and
 * return PAGE_CONTINUE. If no menu item is selected, return 0.
 *
 * @return 0 if no menu item is selected, PAGE_CONTINUE if a menu item is selected
 */
int utils_event_menu_selected(void)
{
    Gesture_Touch_t *ges = utils_gesture_get();
    for (int i = 0; i < g_num_pages; i++)
    {
        utils_paging *page = menu_pages[i].page;
        Image *icon = menu_pages[i].icon;
        if (ges->tagReleased == icon->tagval)
        {
            utils_deselect_all_pages();
            g_is_menu_active = false;
            page->is_active = 1;
            eve_printf_debug("Icon: %s, activating page %s\n", icon->sd_path, page->name);
            return PAGE_CONTINUE;
        }
    }
    return PAGE_CONTINUE;
}
