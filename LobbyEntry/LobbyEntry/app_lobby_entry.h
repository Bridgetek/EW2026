/**
 * @file app_lobby_entry.h
 * @brief Header file for the lobby entry application, containing definitions and function prototypes
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#ifndef APP_LOBBY_ENTRY_H
#define APP_LOBBY_ENTRY_H

#include "utils_eve.h"
#include "app_assets.h"
#include "utils_box.h"
#include "utils_vertex_adapter.h"

#define MAX_PRESSED_BUTTONS 18 // how many digits max for call
#define DMA_CHUNK (800*600 / 2) 
#define CAMERA_BUFFER_SIZE (800*600 / 4) 
#define MARQUEE_TEXT_OFFSET_X 2400
#define MARQUEE_TEXT_STEP 100
#define MARQUEE_TEXT_SPEED 10

// paging
typedef struct
{
    utils_paging *page;
    Image *icon;
} menu_item_t;

extern app_box g_lobby_geo;
extern app_box g_cam_geo;
extern uint8_t g_camera_buffer[CAMERA_BUFFER_SIZE];
extern uint8_t s_camera_dcmi_buf[DMA_CHUNK];
extern char  g_pressed_symbols[];
extern bool g_is_menu_active;

// individual pages
extern utils_paging page_main;
extern utils_paging page_qr;
extern utils_paging page_directory;
extern utils_paging page_dialer;
extern utils_paging page_help;
extern utils_paging page_call;
extern int g_num_pages;
static menu_item_t menu_pages[] = {
    {&page_qr, ICON_QR_CODE_SCANNER},
    {&page_directory, ICON_DIRECTORY},
    {&page_dialer, ICON_DIALER},
    {&page_help, ICON_HELP},
    {&page_call, ICON_GREEN_CALL},
};

void utils_deselect_all_pages();
int utils_event_menu_selected();
void lobby_entry_init(EVE_HalContext *pHost);
void lobby_entry(EVE_HalContext *pHost);

#endif // APP_LOBBY_ENTRY_H
