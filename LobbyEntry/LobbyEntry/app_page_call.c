/**
 * @file app_page_call.c
 * @brief Video Call page implementation with modern UI
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32h743xx.h>
#include "app_lobby_entry.h"
#include "app_hal_events.h"
#include "app_camera_control.h"
#include "camera.h"
#include "dcmi.h"
#include "i2c.h"
#include "jpeg.h"
#include "utils_jpeg_parser.h"
#include "utils_vertex_adapter.h"
#include "utils_color.h"
#include "utils_box.h"

#define ENABLE_CAMERA_DISPLAY 1
#define ALLOW_VIDEO_UPSCALE 0

#define SCREEN_W 1200
#define SCREEN_H 1920
#define HEADER_BG_ALPHA 180

#define HEADER_FONT 30
#define DURATION_FONT 31
#define VIDEO_LABEL_FONT 28
#define CONTROL_LABEL_FONT 27
#define TOAST_FONT 28

#define ZOOM_MIN 10
#if ALLOW_VIDEO_UPSCALE
#define ZOOM_MAX 400
#else
#define ZOOM_MAX 100
#endif
#define ZOOM_STEP 10

#define REPEAT_DELAY 1200
#define REPEAT_RATE 80

#define TOAST_DURATION_MS 2000

#define TAG_BTN_SWAP 10
#define TAG_BTN_CAMERA 11
#define TAG_BTN_HANGUP 12
#define TAG_ZOOM_IN 30
#define TAG_ZOOM_OUT 31
#define TAG_ZOOM_RESET 32
#define TAG_ZOOM_FULLSCREEN 33
#define TAG_LOCAL_VIDEO 40
#define TAG_REMOTE_VIDEO 41

#define COLOR_CONTAINER_BG 0x0a0a12
#define COLOR_CONTAINER_BORDER 0x2a2a3e
#define COLOR_BG_DARK 0x1a1a2e
#define COLOR_BG_MID 0x16213e
#define COLOR_BG_LIGHT 0x0f3460
#define COLOR_GREEN 0x4ade80
#define COLOR_RED 0xef4444
#define COLOR_PURPLE1 0x667eea
#define COLOR_PURPLE2 0x764ba2
#define COLOR_WHITE 0xffffff
#define COLOR_GRAY_LIGHT 0x404050
#define COLOR_GRAY_DARK 0x202030

#define LVDS_RX_W 800
#define LVDS_RX_H 600
#define CAMERA_LOCAL_W 800
#define CAMERA_LOCAL_H 600
#define CAMERA_LOCAL_DIMENSION (CAMERA_LOCAL_W * CAMERA_LOCAL_H)

#define ANIM_SPEED 8
#define SWAP_ANIM_TICK_MS 16
#define SWAP_ANIM_MAX_CATCHUP_STEPS 8

#define CAMERA_MEM_SIZE (100 * 1024) // DMA_CHUNK
#define JPEG_MEM_SIZE (100 * 1024)

typedef enum
{
    CAMERA_DATA_STATE_NO_SIGNAL,
    CAMERA_DATA_STATE_BUFFER_READY,
    CAMERA_DATA_STATE_JPEG_PARSING,
    CAMERA_DATA_STATE_JPEG_READY,
    CAMERA_DATA_STATE_FIFO_UPLOADING,
    CAMERA_DATA_STATE_FIFO_READY,
    CAMERA_DATA_STATE_IDLE
} camera_state_t;

static app_box box_lcd;      // area of: full screen dimension
static app_box box_body;     // area of: where the application put on
static app_box box_header;   // area of: contain top header such as calling, fps, time call duration
static app_box box_videos;   // area of: contains local video and remote video
static app_box box_controls; // area of: bottom control buttons

static app_box s_box_video_remote_fullcreen;
static app_box s_box_video_local_fullcreen;
static app_box s_box_remote_video_default;
static app_box s_box_local_video_default;

extern bool g_is_menu_active;
extern char g_pressed_symbols[MAX_PRESSED_BUTTONS];
static uint32_t s_camera_lvds_handler = 0;
static uint32_t s_camera_local_handler = 0;
static uint32_t s_camera_local_ram_addr = 0;

uint32_t eve_fifo_size = CAMERA_LOCAL_DIMENSION * 3;
uint32_t eve_fifo_ram_addr = 0;

jpeg_info_t jpeg_parse_info = {0};
jpeg_check_result_t jpeg_check_result = JPEG_CHECK_PARSE_ERROR;
static volatile camera_state_t s_camera_local_data_state = CAMERA_DATA_STATE_NO_SIGNAL;
static uint8_t s_LVDS_connected = 1;
static uint32_t s_fps_frame_count = 0;
static uint32_t s_fps_last_tick = 0;
static uint32_t s_fps_display = 0;
static uint32_t s_call_start_tick = 0;
static bool s_call_active = true;
static bool s_video_swapped = false;
static bool s_camera_on = true;
static bool s_swap_animating = false;
app_box s_box_local_anim = {0};
app_box s_box_remote_anim = {0};
static int32_t s_local_video_x = 0;
static int32_t s_local_video_y = 0;
static bool s_local_video_dragging = false;
static int32_t s_drag_diff_x = 0;
static int32_t s_drag_diff_y = 0;
static int32_t s_zoom_level = 100;
static bool s_fullscreen = false;
static char s_toast_message[64] = {0};
static uint32_t s_toast_start_tick = 0;
static bool s_toast_visible = false;
static char s_caller_display[64] = {0};
static char s_caller_initials[3] = {0};
static uint8_t s_pressed_tag = 0;
static uint8_t s_repeat_tag = 0;
static uint32_t s_repeat_start_tick = 0;
static uint32_t s_repeat_last_tick = 0;
bool volatile local_has_video = false;
static uint8_t *s_camera_mem_pool = &s_camera_dcmi_buf[0];
static uint8_t *jpeg_mem_pool = &g_camera_buffer[0];
uint8_t g_using_line_event = 1;

JpegValidationStats stats;
JpegValidationResult result;
bool verbose = false;

/**
 * @brief Process JPEG buffer, extract frame, and update state.
 * @return true if frame found and processed, false otherwise.
 */
static bool jpeg_processing(void)
{
    static uint32_t jpgcount = 0;
    if (!s_camera_on)
    {
        return false;
    }
    if (s_camera_local_data_state != CAMERA_DATA_STATE_BUFFER_READY)
    {
        return false; // drop frame
    }
    s_camera_local_data_state = CAMERA_DATA_STATE_JPEG_PARSING;

    memset(&jpeg_parse_info, 0, sizeof(jpeg_parse_info));
    jpeg_check_result = eve_validate_jpeg(jpeg_mem_pool, JPEG_MEM_SIZE, &jpeg_parse_info, 0);

    if (jpeg_check_result != JPEG_CHECK_OK)
    {
        printf("JPEG validated not ok, skip a frame\n");
        goto load_data_done;
    }
    jpgcount++;

    if (jpeg_parse_info.data_size < JPEG_MEM_SIZE)
    {
        EVE_CoCmd_mediaFifo(PHOST, eve_fifo_ram_addr, eve_fifo_size);
        uint32_t transfered = 0;
        uint32_t data_size_aligneed = (jpeg_parse_info.data_size + 3) & ~3; // align to 4 bytes for DMA
        bool ret = EVE_Util_loadMediaRam(PHOST, &jpeg_mem_pool[jpeg_parse_info.data_start], jpeg_parse_info.data_size, &transfered);
        if (!ret || transfered != data_size_aligneed)
        {
            printf("Failed to load JPEG to Media FIFO data_size=%d, transfered=%d\n", jpeg_parse_info.data_size, transfered);
            EVE_MediaFifo_close(PHOST);
            EVE_MediaFifo_set(PHOST, eve_fifo_ram_addr, eve_fifo_size);
            goto load_data_done;
        }

        EVE_CoCmd_loadImage(PHOST, s_camera_local_ram_addr, OPT_NODL | OPT_MEDIAFIFO | OPT_YCBCR);
        EVE_Cmd_waitFlush(PHOST);

        if (!local_has_video)
        {
            local_has_video = true;
            EVE_Cmd_wr32(PHOST, BITMAP_HANDLE(s_camera_local_handler));
            EVE_CoCmd_setBitmap(PHOST, s_camera_local_ram_addr, YCBCR, FRAME_WIDTH, FRAME_HEIGHT - 8);
        }
        s_fps_frame_count++;
    }

load_data_done:
    Camera_Start_Next();
    s_camera_local_data_state = CAMERA_DATA_STATE_IDLE;

    return true;
}

/**
 * @brief DCMI line event callback for camera DMA.
 * @param hdcmi Pointer to DCMI handle.
 */
static void my_HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (hdcmi->Instance != DCMI)
        return;
    if (s_camera_local_data_state == CAMERA_DATA_STATE_NO_SIGNAL || s_camera_local_data_state == CAMERA_DATA_STATE_IDLE)
    {
        SCB_CleanInvalidateDCache_by_Addr(s_camera_mem_pool, CAMERA_MEM_SIZE / 4);
        memcpy(jpeg_mem_pool, s_camera_mem_pool, CAMERA_MEM_SIZE);
        s_camera_local_data_state = CAMERA_DATA_STATE_BUFFER_READY;
    }
    else
    {
        s_camera_local_data_state = CAMERA_DATA_STATE_IDLE;
    }
}

/**
 * @brief HAL DCMI frame event callback - print debug message when frame event occurs
 * @param hdcmi Pointer to DCMI handle structure
 */
static void my_HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
}

/**
 * @brief Format seconds as HH:MM:SS string.
 * @param seconds Input seconds.
 * @param buf Output buffer.
 * @param len Buffer length.
 */
static void format_duration(uint32_t seconds, char *buf, size_t len)
{
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;
    snprintf(buf, len, "%02lu:%02lu:%02lu", h, m, s);
}
/**
 * @brief Show a toast notification with a message.
 * @param message Message to display.
 */
static void enable_toast(const char *message)
{
    strncpy(s_toast_message, message, sizeof(s_toast_message) - 1);
    s_toast_message[sizeof(s_toast_message) - 1] = '\0';
    s_toast_start_tick = HAL_GetTick();
    s_toast_visible = true;
}
/**
 * @brief Smoothly animate a value towards a target.
 * @param current Current value.
 * @param target Target value.
 * @param speed Animation speed (higher = slower).
 * @return New value after animation step.
 */
static int32_t animate_value(int32_t current, int32_t target, int32_t speed)
{
    if (current == target)
        return current;
    int32_t diff = target - current;
    int32_t step = diff / speed;
    if (step == 0)
    {
        step = (diff > 0) ? 1 : -1;
    }
    return current + step;
}

static void animate_box(app_box *box, app_box target)
{
    box->x = animate_value(box->x, target.x, ANIM_SPEED);
    box->y = animate_value(box->y, target.y, ANIM_SPEED);
    box->w = animate_value(box->w, target.w, ANIM_SPEED);
    box->h = animate_value(box->h, target.h, ANIM_SPEED);
}

/**
 * @brief Update video swap animation state.
 * @return true if animation is still in progress, false if done.
 */
static bool update_swap_animation(void)
{
    app_box local_pip = init_app_box(s_local_video_x, s_local_video_y,
                                     s_box_local_video_default.w, s_box_local_video_default.h);
    app_box local_target = s_video_swapped ? s_box_remote_video_default : local_pip;
    app_box remote_target = s_video_swapped ? local_pip : s_box_remote_video_default;

    animate_box(&s_box_local_anim, local_target);
    animate_box(&s_box_remote_anim, remote_target);

    bool local_done = (s_box_local_anim.x == local_target.x && s_box_local_anim.y == local_target.y &&
                       s_box_local_anim.w == local_target.w && s_box_local_anim.h == local_target.h);
    bool remote_done = (s_box_remote_anim.x == remote_target.x && s_box_remote_anim.y == remote_target.y &&
                        s_box_remote_anim.w == remote_target.w && s_box_remote_anim.h == remote_target.h);
    bool was_animating = s_swap_animating;
    s_swap_animating = !(local_done && remote_done);

    /* Resume camera when animation finishes */
    if (was_animating && !s_swap_animating && s_camera_on)
    {
        Camera_Resume();
    }
    return s_swap_animating;
}

/**
 * @brief Update video swap animation state using EVE_millis() time base.
 * @return true if animation is still in progress, false if done.
 */
static bool update_swap_animation_ms(void)
{
    static uint32_t s_last_anim_ms = 0;
    uint32_t now = EVE_millis();

    if (!s_swap_animating)
    {
        s_last_anim_ms = 0;
        return false;
    }

    if (s_last_anim_ms == 0)
    {
        s_last_anim_ms = now;
        return update_swap_animation();
    }

    uint32_t elapsed = now - s_last_anim_ms;
    uint32_t steps = elapsed / SWAP_ANIM_TICK_MS;

    if (steps == 0)
    {
        return true;
    }

    if (steps > SWAP_ANIM_MAX_CATCHUP_STEPS)
    {
        steps = SWAP_ANIM_MAX_CATCHUP_STEPS;
    }

    s_last_anim_ms += steps * SWAP_ANIM_TICK_MS;
    while (steps-- > 0 && s_swap_animating)
    {
        update_swap_animation();
    }

    return s_swap_animating;
}

uint8_t check_LVDS_connection(void)
{
    uint32_t reg_val = EVE_Hal_rd32(PHOST, REG_LVDSRX_STAT) & (CHn_DLL_LOCKED << 24);
    if (reg_val != (CHn_DLL_LOCKED << 24))
    {
        if (s_LVDS_connected == 2)
        {
            reg_val = EVE_Hal_rd32(PHOST, REG_LVDSRX_CTRL);
            reg_val &= ~((CHn_PWDN_B_ON << 8) | CHn_PWDN_B_ON);
            EVE_Hal_wr32(PHOST, REG_LVDSRX_CTRL, reg_val);
            s_LVDS_connected = 0;
        }
        else if (s_LVDS_connected == 0)
        {
            reg_val = EVE_Hal_rd32(PHOST, REG_LVDSRX_CTRL);
            reg_val |= ((CHn_PWDN_B_ON << 8) | CHn_PWDN_B_ON);
            EVE_Hal_wr32(PHOST, REG_LVDSRX_CTRL, reg_val);
            s_LVDS_connected = 1;
        }
    }
    else
    {
        if (s_LVDS_connected == 1)
        {
            s_LVDS_connected = 2;
        }
    }
    return s_LVDS_connected;
}

/**
 * @brief Handle drag events for picture-in-picture (PIP) video.
 * @param ges Pointer to gesture/touch event structure.
 */
static void handle_pip_video_drag(Gesture_Touch_t *ges)
{
    uint8_t pip_tag = s_video_swapped ? TAG_REMOTE_VIDEO : TAG_LOCAL_VIDEO;
    if (ges->tagPressed == pip_tag && !s_local_video_dragging)
    {
        s_local_video_dragging = true;
        s_drag_diff_x = s_local_video_x - ges->touchX;
        s_drag_diff_y = s_local_video_y - ges->touchY;
    }
    else if (s_local_video_dragging && ges->isTouch)
    {
        s_local_video_x = ges->touchX + s_drag_diff_x;
        s_local_video_y = ges->touchY + s_drag_diff_y;
        int32_t margin = 20;
        if (s_local_video_x < margin)
            s_local_video_x = margin;
        if (s_local_video_x > SCREEN_W - s_box_local_video_default.w / 2 - margin)
            s_local_video_x = SCREEN_W - s_box_local_video_default.w / 2 - margin;
        if (s_local_video_y < margin)
            s_local_video_y = margin;
        if (s_local_video_y > SCREEN_H - s_box_local_video_default.h / 2 - margin)
            s_local_video_y = SCREEN_H - s_box_local_video_default.h / 2 - margin;
    }
    if (!ges->isTouch)
    {
        s_local_video_dragging = false;
    }
}
/**
 * @brief Handle control button events (swap, camera on/off).
 * @param ges Pointer to gesture/touch event structure.
 */
static void handle_control_events(Gesture_Touch_t *ges)
{
    switch (ges->tagReleased)
    {
    case TAG_BTN_SWAP:
        s_video_swapped = !s_video_swapped;
        s_swap_animating = true;
        s_zoom_level = 100;
        Camera_Pause(); /* Pause camera during animation */
        enable_toast(s_video_swapped ? "Local Video Enlarged" : "Remote Video Enlarged");
        break;
    case TAG_BTN_CAMERA:
        s_camera_on = !s_camera_on;
        if (s_camera_on)
        {
            Camera_Resume();
        }
        else
        {
            Camera_Pause();
        }
        enable_toast(s_camera_on ? "Camera On" : "Camera Off");
        break;
    }
}
/**
 * @brief Execute zoom action (in/out/reset/fullscreen).
 * @param tag Zoom button tag.
 * @param enable_toast_msg Show toast message if true.
 */
static void do_zoom_action(uint8_t tag, bool enable_toast_msg)
{
    int32_t delta = 0;
    if (tag == TAG_ZOOM_IN)
        delta = ZOOM_STEP;
    else if (tag == TAG_ZOOM_OUT)
        delta = -ZOOM_STEP;

    if (delta == 0)
        return;

    int32_t next = s_zoom_level + delta;
    if (next > ZOOM_MAX)
        next = ZOOM_MAX;
    if (next < ZOOM_MIN)
        next = ZOOM_MIN;

    if (next != s_zoom_level)
    {
        s_zoom_level = next;
        if (enable_toast_msg)
            enable_toast(delta > 0 ? "Zoom In" : "Zoom Out");
    }
}

static bool is_zoom_tag(uint8_t tag)
{
    return tag == TAG_ZOOM_IN || tag == TAG_ZOOM_OUT;
}
/**
 * @brief Handle zoom button events and repeat logic.
 * @param ges Pointer to gesture/touch event structure.
 */
static void handle_zoom_events(Gesture_Touch_t *ges)
{
    uint32_t current_tick = HAL_GetTick();
    if (is_zoom_tag(ges->tagPressed))
    {
        s_repeat_tag = ges->tagPressed;
        s_repeat_start_tick = current_tick;
        s_repeat_last_tick = current_tick;
        do_zoom_action(s_repeat_tag, true);
    }
    if (ges->isTouch && is_zoom_tag(ges->tagPressed) && s_repeat_tag == ges->tagPressed)
    {
        uint32_t elapsed_from_start = current_tick - s_repeat_start_tick;
        uint32_t elapsed_from_last = current_tick - s_repeat_last_tick;
        if (elapsed_from_start >= REPEAT_DELAY && elapsed_from_last >= REPEAT_RATE)
        {
            do_zoom_action(s_repeat_tag, false);
            s_repeat_last_tick = current_tick;
        }
    }
    if (!ges->isTouch || !is_zoom_tag(ges->tagPressed))
    {
        s_repeat_tag = 0;
    }
    switch (ges->tagReleased)
    {
    case TAG_ZOOM_RESET:
        s_zoom_level = 100;
        s_local_video_x = s_box_local_video_default.x;
        s_local_video_y = s_box_local_video_default.y;
        enable_toast("Zoom Reset");
        break;
    case TAG_ZOOM_FULLSCREEN:
        s_fullscreen = !s_fullscreen;
        enable_toast(s_fullscreen ? "Fullscreen" : "Exit Fullscreen");
        break;
    }
}
/**
 * @brief Main event handler for call page (touch, animation, state).
 * @return PAGE_FINISH or PAGE_CONTINUE.
 */
static int event(void)
{
    jpeg_processing();
    update_swap_animation();
    Gesture_Touch_t *ges = utils_gesture_get();
    s_pressed_tag = ges->tagPressed;
    if (ges->tagReleased == TAG_BTN_HANGUP)
    {
        s_call_active = false;
        g_is_menu_active = true;
        return PAGE_FINISH;
    }
    handle_pip_video_drag(ges);
    handle_control_events(ges);
    handle_zoom_events(ges);
    if (s_fullscreen && (ges->tagReleased == TAG_REMOTE_VIDEO || ges->tagReleased == TAG_LOCAL_VIDEO))
    {
        s_fullscreen = false;
    }
    return PAGE_CONTINUE;
}

/**
 * @brief Draw a rounded rectangle using EVE display list.
 * @param x X position.
 * @param y Y position.
 * @param w Width.
 * @param h Height.
 * @param r Corner radius.
 */
static void draw_rounded_rect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r)
{
    EVE_CoDl_pointSize(PHOST, r * 16);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, x + r, y + r);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, x + w - r, y + r);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, x + r, y + h - r);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, x + w - r, y + h - r);

    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, x + r, y);
    g_utils_adapter_vertex2f(PHOST, x + w - r, y + h);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, x, y + r);
    g_utils_adapter_vertex2f(PHOST, x + w, y + h - r);
}

/**
 * @brief Draw a rectangular frame highlight around a button.
 * @param x Left position.
 * @param y Top position.
 * @param w Frame outer width.
 * @param h Frame outer height.
 * @param thickness Border thickness.
 */
static void draw_pressed_frame(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thickness)
{
    int32_t inner_x = x + thickness;
    int32_t inner_y = y + thickness;
    int32_t inner_w = w - thickness * 2;
    int32_t inner_h = h - thickness * 2;
    if (inner_w < 2)
        inner_w = 2;
    if (inner_h < 2)
        inner_h = 2;

    /* Outer frame */
    set_color_rgb(PHOST, COLOR_WHITE);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, x, y);
    g_utils_adapter_vertex2f(PHOST, x + w, y + h);
    EVE_CoDl_end(PHOST);

    /* Punch inner area with control bar background color to create border */
    set_color_rgb(PHOST, 0x000000);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, inner_x, inner_y);
    g_utils_adapter_vertex2f(PHOST, inner_x + inner_w, inner_y + inner_h);
    EVE_CoDl_end(PHOST);
}

/**
 * @brief Draw icon button using asset image with optional disabled/pressed alpha.
 * @param icon Icon image asset.
 * @param center_x X center of button slot.
 * @param center_y Y center of button slot.
 * @param tag Touch tag.
 * @param enabled True to draw normal state, false to dim icon.
 */
static void draw_image_button(Image *icon, int32_t center_x, int32_t center_y, uint8_t tag, bool enabled)
{
    if (s_pressed_tag == tag)
    {
        EVE_Cmd_wr32(PHOST, COLOR_RGB(0, 0, 255));
    }
    else
    {
        EVE_Cmd_wr32(PHOST, COLOR_RGB(255, 255, 255));
    }

    utils_draw_image_xy_tag(PHOST, icon,
                            center_x - icon->w / 2,
                            center_y - icon->h / 2,
                            tag);
}

/**
 * @brief Draw camera container background and border for a video area.
 * @param x X position.
 * @param y Y position.
 * @param w Width.
 * @param h Height.
 */
static void draw_camera_container_at(app_box box)
{
    int32_t x = box.x;
    int32_t y = box.y;
    int32_t w = box.w;
    int32_t h = box.h;
    int32_t min_dim = (w < h) ? w : h;
    int32_t padding = (min_dim * 3) / 100;
    int32_t radius = (min_dim * 6) / 100;
    if (padding < 2)
        padding = 2;
    if (radius < 6)
        radius = 6;

    set_color_rgb(PHOST, COLOR_CONTAINER_BORDER);
    draw_rounded_rect(x, y, w - padding * 2, h - padding * 2, radius);
}
/**
 * @brief Draw camera containers for both videos (larger first).
 */
static void draw_camera_container(void)
{
    if (s_fullscreen)
        return;
    int32_t remote_area = s_box_remote_anim.w * s_box_remote_anim.h;
    int32_t local_area = s_box_local_anim.w * s_box_local_anim.h;
    if (remote_area >= local_area)
    {
        draw_camera_container_at(s_box_remote_anim);
        draw_camera_container_at(s_box_local_anim);
    }
    else
    {
        draw_camera_container_at(s_box_local_anim);
        draw_camera_container_at(s_box_remote_anim);
    }
}

/**
 * @brief Draw video area (remote/local) with optional label and touch tag.
 * @param box Target area box.
 * @param im_w Image width.
 * @param im_h Image height.
 * @param bitmap_hdlr Bitmap handle.
 * @param format Pixel format.
 * @param has_video True if video data is available.
 * @param tag Touch tag.
 * @param label Label text.
 */
static void draw_video(app_box box,
                       int32_t im_w, int32_t im_h, uint32_t bitmap_hdlr, uint32_t format,
                       bool has_video, uint8_t tag, const char *label)
{
    int32_t min_dim = (box.w < box.h) ? box.w : box.h;
    int32_t radius = (min_dim * 4) / 100;
    int32_t shadow = (min_dim * 2) / 100;
    int32_t border = (min_dim * 2) / 100;
    int32_t label_pad = (min_dim * 4) / 100;
    bool is_pip_mode = (box.w < im_w);
    if (radius < 6)
        radius = 6;
    if (shadow < 2)
        shadow = 2;
    if (border < 2)
        border = 2;
    if (label_pad < 6)
        label_pad = 6;
    if (is_pip_mode)
    {
        EVE_CoDl_colorA(PHOST, 80);
        set_color_rgb(PHOST, 0x000000);
        draw_rounded_rect(box.x + shadow, box.y + shadow, box.w, box.h, radius);
        EVE_CoDl_colorA(PHOST, 255);
        uint8_t pip_tag = s_video_swapped ? TAG_REMOTE_VIDEO : TAG_LOCAL_VIDEO;
        bool is_dragging = (tag == pip_tag && s_local_video_dragging);
        set_color_rgb(PHOST, is_dragging ? COLOR_GREEN : 0x404050);
        draw_rounded_rect(box.x - border, box.y - border,
                          box.w + border * 2, box.h + border * 2,
                          radius + border);
    }
    EVE_CoDl_tag(PHOST, tag);
    set_color_rgb(PHOST, 0x1a1a1a);
    draw_rounded_rect(box.x, box.y, box.w, box.h, radius);
#if ENABLE_CAMERA_DISPLAY
    if (has_video)
    {
        set_color_rgb(PHOST, COLOR_WHITE);
        int32_t display_w, display_h;
        int32_t offset_x, offset_y;
        int32_t translate_x = 0;
        int32_t translate_y = 0;
        bool need_scale = false;

        if (is_pip_mode)
        {
            /* PIP mode: always scale to fit container while maintaining aspect ratio */
            int32_t scale_w = (box.w * 100) / im_w;
            int32_t scale_h = (box.h * 100) / im_h;
            int32_t scale = (scale_w < scale_h) ? scale_w : scale_h; /* Use smaller scale to fit */
            display_w = (im_w * scale) / 100;
            display_h = (im_h * scale) / 100;
            offset_x = (box.w - display_w) / 2;
            offset_y = (box.h - display_h) / 2;
            need_scale = true;
        }
        else
        {
            /* Main video: apply zoom level */
            int32_t max_display_w = box.w;
            int32_t max_display_h = box.h;
#if !ALLOW_VIDEO_UPSCALE
            if (max_display_w > im_w)
                max_display_w = im_w;
            if (max_display_h > im_h)
                max_display_h = im_h;
#endif
            display_w = (max_display_w * s_zoom_level) / 100;
            display_h = (max_display_h * s_zoom_level) / 100;
            offset_x = (box.w - display_w) / 2;
            offset_y = (box.h - display_h) / 2;
            need_scale = (s_zoom_level != 100);
        }

        EVE_CoDl_saveContext(PHOST);
        EVE_CoDl_scissorXY(PHOST, box.x, box.y);
        EVE_CoDl_scissorSize(PHOST, box.w, box.h + 20);
        EVE_CoDl_tag(PHOST, tag);
        SET_BITMAP_HANDLE(PHOST, bitmap_hdlr);
        EVE_CoDl_begin(PHOST, BITMAPS);
        if (need_scale)
        {
            utils_eve_scale_to(im_w, im_h - 8, display_w, display_h, format, &translate_x, &translate_y);
        }
        g_utils_adapter_vertex2f(PHOST, box.x + offset_x, box.y + offset_y);
        EVE_CoDl_scissorXY(PHOST, 0, 0);
        EVE_CoDl_scissorSize(PHOST, 2048, 2048);
        EVE_CoDl_restoreContext(PHOST);
    }
    else
    {
        EVE_CoDl_tag(PHOST, tag);
        set_color_rgb(PHOST, 0x555555);
        EVE_CoCmd_text(PHOST, box.x + box.w / 2, box.y + box.h / 2, is_pip_mode ? 30 : 31, OPT_CENTER, "No Video");
    }
#else
    EVE_CoDl_tag(PHOST, tag);
    set_color_rgb(PHOST, 0x555555);
    EVE_CoCmd_text(PHOST, box.x + box.w / 2, box.y + box.h / 2, is_pip_mode ? 30 : 31, OPT_CENTER, "No Video");
#endif
    if (!s_fullscreen)
    {
        int32_t label_w = is_pip_mode ? (box.w * 35) / 100 : (box.w * 20) / 100;
        int32_t label_h = is_pip_mode ? (box.h * 18) / 100 : (box.h * 10) / 100;
        int32_t label_x = box.x + label_pad;
        int32_t label_y = box.y + box.h - label_h - label_pad;
        if (label_w < 50)
            label_w = 50;
        if (label_h < 24)
            label_h = 24;
        EVE_CoDl_colorA(PHOST, 180);
        set_color_rgb(PHOST, 0x000000);
        draw_rounded_rect(label_x, label_y, label_w, label_h, is_pip_mode ? 8 : 10);
        EVE_CoDl_colorA(PHOST, 255);
        set_color_rgb(PHOST, COLOR_WHITE);
        EVE_CoCmd_text(PHOST, label_x + label_w / 2, label_y + label_h / 2,
                       is_pip_mode ? 26 : VIDEO_LABEL_FONT, OPT_CENTER, label);
    }
    EVE_CoDl_tag(PHOST, 0);
}
/**
 * @brief Draw control bar with all buttons.
 */
static void draw_control_bar(void)
{
    int32_t bar_x = box_controls.x;
    int32_t bar_y = box_controls.y;
    int32_t bar_w = box_controls.w;
    int32_t bar_h = box_controls.h;
    int32_t btn_size = (bar_h * 60) / 100;
    int32_t gap = (bar_w * 3) / 100;
    int32_t pad_x = (bar_w * 6) / 100;
    int32_t row_y = bar_y + bar_h / 2;
    if (btn_size < 32)
        btn_size = 32;
    if (gap < 12)
        gap = 12;
    if (pad_x < 16)
        pad_x = 16;

    EVE_CoDl_colorA(PHOST, 200);
    set_color_rgb(PHOST, 0x000000);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, bar_x, bar_y);
    g_utils_adapter_vertex2f(PHOST, bar_x + bar_w, bar_y + bar_h);
    EVE_CoDl_colorA(PHOST, 255);

    int32_t total_w = 2 * btn_size + ICON_RED_HANGUP->w + 2 * gap;
    int32_t btn_x = bar_x + (bar_w - total_w) / 2;
    draw_image_button(ICON_ARROW_CLOCKWISE_EN, btn_x + btn_size / 2, row_y, TAG_ZOOM_RESET, true);
    btn_x += btn_size + gap;
    draw_image_button(ICON_RED_HANGUP, btn_x + ICON_RED_HANGUP->w / 2, row_y + 5, TAG_BTN_HANGUP, true);
    btn_x += ICON_RED_HANGUP->w + gap;
    draw_image_button(s_camera_on ? ICON_CAMERA_VIDEO_FILL_EN : ICON_CAMERA_VIDEO_OFF_FILL_EN,
                      btn_x + btn_size / 2, row_y, TAG_BTN_CAMERA, true);

    int32_t left_x = bar_x + pad_x + btn_size / 2;
    draw_image_button(ICON_ZOOM_IN_EN, left_x, row_y, TAG_ZOOM_IN, s_zoom_level < ZOOM_MAX);
    left_x += btn_size + gap;
    draw_image_button(ICON_ZOOM_OUT_EN, left_x, row_y, TAG_ZOOM_OUT, s_zoom_level > ZOOM_MIN);

    int32_t right_x = bar_x + bar_w - pad_x - btn_size / 2;
    draw_image_button(ICON_ARROWS_ANGLE_EXPAND_EN, right_x, row_y, TAG_ZOOM_FULLSCREEN, true);

    right_x -= btn_size + gap;
    draw_image_button(ICON_ARROW_LEFT_RIGHT_EN, right_x, row_y, TAG_BTN_SWAP, true);
}
/**
 * @brief Draw zoom level indicator bar.
 */
static void draw_zoom_indicator(void)
{
    if (s_zoom_level == 100 || s_fullscreen)
        return;

    /* Compute zoom indicator dimensions from s_box_remote_video_default */
    int32_t zoom_ind_w = (s_box_remote_video_default.w * 14) / 100;
    int32_t zoom_ind_h = (s_box_remote_video_default.h * 3) / 100;
    int32_t zoom_ind_x = s_box_remote_video_default.x + (s_box_remote_video_default.w * 2) / 100;
    int32_t zoom_ind_y = s_box_remote_video_default.y + (s_box_remote_video_default.h * 2) / 100;
    int32_t zoom_bar_w = (s_box_remote_video_default.w * 7) / 100;
    int32_t text_offset = (zoom_ind_w * 12) / 100;
    int32_t bar_offset = (zoom_ind_w * 48) / 100;
    int32_t bar_h = 6;

    if (zoom_ind_h < 24)
        zoom_ind_h = 24;
    if (text_offset < 12)
        text_offset = 12;
    if (bar_offset < 50)
        bar_offset = 50;

    /* Draw background */
    EVE_CoDl_colorA(PHOST, 180);
    set_color_rgb(PHOST, 0x000000);
    draw_rounded_rect(zoom_ind_x, zoom_ind_y, zoom_ind_w, zoom_ind_h, 20);
    EVE_CoDl_colorA(PHOST, 255);

    /* Draw zoom percentage text */
    char zoom_str[16];
    snprintf(zoom_str, sizeof(zoom_str), "%ld%%", s_zoom_level);
    set_color_rgb(PHOST, COLOR_WHITE);
    EVE_CoCmd_text(PHOST, zoom_ind_x + text_offset, zoom_ind_y + zoom_ind_h / 2, 27, OPT_CENTERY, zoom_str);

    /* Draw zoom bar background */
    int32_t bar_x = zoom_ind_x + bar_offset;
    int32_t bar_y = zoom_ind_y + zoom_ind_h / 2 - bar_h / 2;
    set_color_rgb(PHOST, 0x404050);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, bar_x, bar_y);
    g_utils_adapter_vertex2f(PHOST, bar_x + zoom_bar_w, bar_y + bar_h);
    EVE_CoDl_end(PHOST);

    /* Draw zoom bar fill */
    int32_t fill_w = ((s_zoom_level - ZOOM_MIN) * zoom_bar_w) / (ZOOM_MAX - ZOOM_MIN);
    set_color_rgb(PHOST, COLOR_GREEN);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, bar_x, bar_y);
    g_utils_adapter_vertex2f(PHOST, bar_x + fill_w, bar_y + bar_h);
    EVE_CoDl_end(PHOST);
}
/**
 * @brief Draw toast notification if visible.
 */
static void draw_toast(void)
{
    if (!s_toast_visible)
        return;

    uint32_t elapsed = HAL_GetTick() - s_toast_start_tick;
    if (elapsed > TOAST_DURATION_MS)
    {
        s_toast_visible = false;
        return;
    }

    /* Compute toast dimensions from box_lcd */
    int32_t toast_h = (box_lcd.h * 3) / 100;
    int32_t toast_y = box_lcd.y + (box_lcd.h * 16) / 100;
    int32_t toast_w = strlen(s_toast_message) * 12 + 60;
    int32_t toast_x = box_lcd.x + (box_lcd.w - toast_w) / 2;
    int32_t toast_center_x = box_lcd.x + box_lcd.w / 2;
    int32_t toast_radius = (toast_h * 30) / 100;

    if (toast_h < 24)
        toast_h = 24;
    if (toast_radius < 10)
        toast_radius = 10;

    /* Calculate fade-out alpha */
    uint8_t alpha = 230;
    if (elapsed > TOAST_DURATION_MS - 300)
    {
        alpha = (uint8_t)(230 * (TOAST_DURATION_MS - elapsed) / 300);
    }

    /* Draw toast background */
    EVE_CoDl_colorA(PHOST, alpha);
    set_color_rgb(PHOST, 0x000000);
    draw_rounded_rect(toast_x, toast_y, toast_w, toast_h, toast_radius);

    /* Draw toast message */
    set_color_rgb(PHOST, COLOR_WHITE);
    EVE_CoCmd_text(PHOST, toast_center_x, toast_y + toast_h / 2, TOAST_FONT, OPT_CENTER, s_toast_message);
    EVE_CoDl_colorA(PHOST, 255);
}

/**
 * @brief Draw header section with caller info and call duration using box_header layout.
 */
static void draw_headers(void)
{
    if (s_fullscreen)
        return;

    /* Compute header dimensions from box_header */
    int32_t header_bg_alpha = 180;
    int32_t avatar_size = (box_header.w * 6) / 100;
    int32_t avatar_x = box_header.x + (box_header.w * 9) / 100;
    int32_t avatar_y = box_header.y + (box_header.h * 30) / 100;
    int32_t caller_name_x = box_header.x + (box_header.w * 18) / 100;
    int32_t caller_name_y = box_header.y + (box_header.h * 30) / 100;
    int32_t caller_status_y = box_header.y + (box_header.h * 50) / 100;
    int32_t call_duration_x = box_header.x + box_header.w / 2;
    int32_t call_duration_y = box_header.y + (box_header.h * 80) / 100;
    int32_t status_indicator_size = 5;
    int32_t status_text_offset = 15;

    char duration_str[16];
    uint32_t elapsed = (HAL_GetTick() - s_call_start_tick) / 1000;
    format_duration(elapsed, duration_str, sizeof(duration_str));

    /* Draw header background */
    EVE_CoDl_colorA(PHOST, header_bg_alpha);
    set_color_rgb(PHOST, 0x000000);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, box_header.x, box_header.y);
    g_utils_adapter_vertex2f(PHOST, box_header.x + box_header.w, box_header.y + box_header.h);
    EVE_CoDl_end(PHOST);
    EVE_CoDl_colorA(PHOST, 255);

    /* Draw avatar circle */
    set_color_rgb(PHOST, COLOR_PURPLE1);
    EVE_CoDl_pointSize(PHOST, (avatar_size / 2) * 16);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, avatar_x, avatar_y + avatar_size / 2);
    EVE_CoDl_end(PHOST);

    /* Draw avatar initials */
    set_color_rgb(PHOST, COLOR_WHITE);
    EVE_CoCmd_text(PHOST, avatar_x, avatar_y + avatar_size / 2, 30, OPT_CENTER, s_caller_initials);

    /* Draw caller name */
    EVE_CoCmd_text(PHOST, caller_name_x, caller_name_y, HEADER_FONT, 0, s_caller_display);

    /* Draw status indicator and text */
    set_color_rgb(PHOST, COLOR_GREEN);
    EVE_CoDl_pointSize(PHOST, status_indicator_size * 16);
    EVE_CoDl_begin(PHOST, POINTS);
    g_utils_adapter_vertex2f(PHOST, caller_name_x, caller_status_y);
    EVE_CoDl_end(PHOST);
    EVE_CoCmd_text(PHOST, caller_name_x + status_text_offset, caller_status_y, 27, OPT_CENTERY, "Connected");

    /* Draw call duration */
    set_color_rgb(PHOST, COLOR_WHITE);
    EVE_CoCmd_text(PHOST, call_duration_x, call_duration_y, DURATION_FONT, OPT_CENTER, duration_str);

    /* Draw performance metrics if enabled */
    {
        uint32_t current_tick = HAL_GetTick();
        if (current_tick - s_fps_last_tick >= 1000)
        {
            s_fps_display = s_fps_frame_count;
            s_fps_frame_count = 0;
            s_fps_last_tick = current_tick;
        }
        char fps_str[32];
        snprintf(fps_str, sizeof(fps_str), "%lu FPS", s_fps_display);
        set_color_rgb(PHOST, COLOR_WHITE);
        const uint32_t margin = 20;
        int32_t fps_x = box_header.x + box_header.w - margin;
        EVE_CoCmd_text(PHOST, fps_x, caller_name_y, 30, OPT_RIGHTX, fps_str);
    }
}

/**
 * @brief Draw controls section (buttons and zoom controls) using box_controls layout.
 */
static void draw_controls(void)
{
    if (s_fullscreen)
        return;
    draw_control_bar();
    draw_zoom_indicator();
}

/**
 * @brief Draw videos section (remote and local video streams) using box_videos and box_body layout.
 */
static void draw_videos(void)
{
    draw_camera_container();

    int32_t remote_area = s_box_remote_anim.w * s_box_remote_anim.h;
    int32_t local_area = s_box_local_anim.w * s_box_local_anim.h;
    bool is_pip = (s_box_remote_anim.w < (s_box_remote_video_default.w + s_box_local_video_default.w) / 2);
    const char *remote_label = g_pressed_symbols;
    bool remote_has_video = (s_LVDS_connected == 2);

    if (s_fullscreen)
    {
        draw_video(s_box_video_remote_fullcreen, LVDS_RX_W, LVDS_RX_H, s_camera_lvds_handler, RGB565, remote_has_video, TAG_REMOTE_VIDEO, g_pressed_symbols);
        draw_video(s_box_video_local_fullcreen, LVDS_RX_W, LVDS_RX_H, s_camera_local_handler, YCBCR, local_has_video, TAG_LOCAL_VIDEO, "You");
    }
    else if (remote_area >= local_area)
    {
        draw_video(s_box_remote_anim, LVDS_RX_W, LVDS_RX_H, s_camera_lvds_handler, RGB565, remote_has_video, TAG_REMOTE_VIDEO, remote_label);
        draw_video(s_box_local_anim, FRAME_WIDTH, FRAME_HEIGHT - 8, s_camera_local_handler, YCBCR, local_has_video && s_camera_on, TAG_LOCAL_VIDEO, "You");
    }
    else
    {
        draw_video(s_box_local_anim, FRAME_WIDTH, FRAME_HEIGHT - 8, s_camera_local_handler, YCBCR, local_has_video && s_camera_on, TAG_LOCAL_VIDEO, "You");
        draw_video(s_box_remote_anim, LVDS_RX_W, LVDS_RX_H, s_camera_lvds_handler, RGB565, remote_has_video, TAG_REMOTE_VIDEO, remote_label);
    }
}

static void draw_body(void)
{
    /* Draw background */
    set_color_rgb(PHOST, COLOR_BG_DARK);
    EVE_CoDl_begin(PHOST, RECTS);
    g_utils_adapter_vertex2f(PHOST, 0, 0);
    g_utils_adapter_vertex2f(PHOST, SCREEN_W, SCREEN_H);
    EVE_CoDl_end(PHOST);
}
/**
 * @brief Main drawing function for call page UI.
 * @return Always 0.
 */
static int drawing(void)
{
    draw_body();
    draw_videos();
    draw_headers();
    draw_controls();
    draw_toast();

    return 0;
}

/**
 * @brief Initialize layout boxes used across the call page UI.
 *
 * This sets up the canonical boxes requested by the UI: box_lcd, box_body,
 * box_header, box_videos and box_controls.
 */
static void init_layout(void)
{
    /* Full screen */
    box_lcd = init_app_box(0, 0, SCREEN_W, SCREEN_H);

    /* Header at the top */
    int32_t header_h = (box_lcd.h * 10) / 100;
    box_header = init_app_box(box_lcd.x, box_lcd.y, box_lcd.w, header_h);

    /* Control bar at the bottom */
    int32_t control_h = (box_lcd.h * 10) / 100;
    int32_t control_y = box_lcd.y + box_lcd.h - control_h;
    box_controls = init_app_box(box_lcd.x, control_y, box_lcd.w, control_h);

    /* Body area between header and controls */
    int32_t body_y = box_header.y + box_header.h;
    int32_t body_h = box_controls.y - body_y;
    box_body = init_app_box(box_lcd.x, body_y, box_lcd.w, body_h);

    /* Default remote/local video boxes (derived from body) */
    int32_t camera_box_w = box_body.w * 95 / 100;
    int32_t camera_box_h = box_body.h * 95 / 100;
    app_box camera_box = init_app_box_child_center_middle(camera_box_w, camera_box_h, box_body);

    int32_t camera_local_h = camera_box.h * 18 / 100;
    int32_t camera_remote_h = camera_box.h * 8 / 10;
    int32_t camera_local_margin_bottom = camera_box.h * 2 / 100;

    int32_t camera_local_w = camera_local_h * 3 / 4;
    s_box_local_video_default = init_app_box_child_top_right(camera_local_w, camera_local_h, camera_box);
    int32_t camera_remote_x = camera_box.x;
    int32_t camera_remote_y = s_box_local_video_default.y_end + camera_local_margin_bottom;
    int32_t camera_remote_w = camera_box.w;
    s_box_remote_video_default = init_app_box(camera_remote_x, camera_remote_y, camera_remote_w, camera_remote_h);

    /* Videos region sits inside the body with content margin */
    box_videos = s_box_remote_video_default;

    /* Fullscreen video boxes */
    s_box_video_remote_fullcreen = init_app_box(box_lcd.x, box_lcd.y, box_lcd.w, box_lcd.h / 2);
    s_box_video_local_fullcreen = init_app_box(box_lcd.x, box_lcd.y + box_lcd.h / 2, box_lcd.w, box_lcd.h / 2);
}

/**
 * @brief Main draw entry point for call page.
 * @return PAGE_FINISH or PAGE_CONTINUE.
 */
int page_call_draw(void)
{
    int ret = event();
    drawing();
    return ret;
}
/**
 * @brief Initialize call page state, camera, and UI.
 */
void page_call_init(void)
{
    g_using_line_event = 1;
    memset(jpeg_mem_pool, 0, JPEG_MEM_SIZE);
    memset(&jpeg_parse_info, 0, sizeof(jpeg_parse_info));
    memset(&jpeg_check_result, 0, sizeof(jpeg_check_result));

    s_call_start_tick = HAL_GetTick();
    s_call_active = true;
    snprintf(s_caller_display, sizeof(s_caller_display), "Calling %s...", g_pressed_symbols);
    s_caller_initials[0] = g_pressed_symbols[0] ? g_pressed_symbols[0] : '?';
    s_caller_initials[1] = g_pressed_symbols[1] ? g_pressed_symbols[1] : '\0';
    s_caller_initials[2] = '\0';
    s_video_swapped = false;
    s_camera_on = true;
    s_zoom_level = 100;
    s_fullscreen = false;
    s_swap_animating = false;
    local_has_video = false;

    /* Initialize layout boxes for the page */
    init_layout();

    s_local_video_x = s_box_local_video_default.x;
    s_local_video_y = s_box_local_video_default.y;
    s_box_remote_anim = s_box_remote_video_default;
    s_box_local_anim = s_box_local_video_default;

    if (check_LVDS_connection() == 2)
    {
        SET_BITMAP_HANDLE(PHOST, s_camera_lvds_handler);
        EVE_CoCmd_setBitmap(PHOST, SWAPCHAIN_2, RGB565, LVDS_RX_W, LVDS_RX_H);
    }
    set_HAL_DCMI_FrameEventCallback(my_HAL_DCMI_FrameEventCallback);
    set_HAL_DCMI_LineEventCallback(my_HAL_DCMI_LineEventCallback);
    Camera_On(FRAMESIZE_SVGA, PIXFORMAT_JPEG, DCMI_MODE_CONTINUOUS, s_camera_mem_pool, CAMERA_MEM_SIZE);

    EVE_MediaFifo_close(PHOST);
    if (!EVE_MediaFifo_set(PHOST, eve_fifo_ram_addr, eve_fifo_size))
    {
        printf("MediaFifo set error!\n");
    }

    printf("Call page initialized\n");
}
/**
 * @brief Deinitialize call page and camera.
 */
void page_call_deinit(void)
{
    Camera_Off();
    s_LVDS_connected = 1;
    EVE_MediaFifo_close(PHOST);
}
/**
 * @brief Load call page resources and allocate buffers.
 * @return 0 on success.
 */
int page_call_load(void)
{
    s_camera_lvds_handler = utils_bitmap_handler_get();
    s_camera_local_handler = utils_bitmap_handler_get();
    s_camera_local_ram_addr = utils_ddr_alloc(CAMERA_LOCAL_DIMENSION * 2);
    eve_fifo_ram_addr = utils_ddr_alloc(eve_fifo_size);

    return 0;
}
