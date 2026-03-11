/**
 * @file app_page_qr.c
 * @brief QR camera page implementation for lobby entry system
 *
 * This module provides QR code detection and processing functionality including
 * camera capture, image processing, QR code decoding with debouncing logic,
 * and real-time rendering of detection status.
 *
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

/* Standard library includes */
#include <stdint.h>
#include <string.h>

/* Third-party includes */
#include <quirc.h>

/* Application includes */
#include "app_lobby_entry.h"
#include "app_hal_events.h"
#include "app_camera_control.h"
// #include "camera.h"
#include "dcmi.h"
#include "i2c.h"
#include "utils_color.h"
#include "quirc_internal.h"

static app_box box_lcd; // area of: full screen dimension
static app_box box_body; // area of: where the application put on
static app_box box_camera;// area of: contain camera preview
static app_box box_controls; // area of: controls items below the camera
static app_box box_dialog_qr_result; // area of: QR result dialog

#define SCREEN_W  1200
#define SCREEN_H  1920

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240
#define MAX_QR_PAYLOAD 100
#define QR_detection_THRESHOLD 1
#define QR_RESET_THRESHOLD 6

/* Dialog UI constants */
#define TAG_DIALOG_CLOSE 201
#define TAG_SHOW_DIALOG_TOGGLE 202
#define TAG_PAUSE_CAMERA_TOGGLE 203
#define TAG_CONTINUE_BTN 204
#define DIALOG_TITLE_FONT 31
#define DIALOG_TEXT_FONT 30

/* Status text UI constants */
#define ENABLE_QR_TEXT_WRAP 1
#define QR_TEXT_WRAP_NUM_CHARS 35

#define CAMERA_MEM_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT)

typedef enum
{
    CAMERA_DATA_STATE_NO_SIGNAL,
    CAMERA_DATA_STATE_BUFFER_READY,
    CAMERA_DATA_STATE_QR_PARSING,
    CAMERA_DATA_STATE_QR_READY,
    CAMERA_DATA_STATE_FIFO_UPLOADING,
    CAMERA_DATA_STATE_FIFO_READY,
    CAMERA_DATA_STATE_IDLE
} camera_state_t;

/**
 * @brief QR code detection state structure
 */
typedef struct
{
    char last_payload[MAX_QR_PAYLOAD]; /* Last successfully decoded payload */
    int last_payload_len;              /* Length of last payload */
    int success_count;                 /* Consecutive successful detections */
    int fail_count;                    /* Consecutive failed detections */
    bool is_stable;                    /* Stable detection state flag */
    bool has_valid_qr;                 /* Valid QR code detected flag */
} qr_detection_t;

/**
 * @brief QR code detection status structure
 */
typedef struct
{
    bool is_stable;    /* Stable detection status */
    int success_count; /* Success counter */
    int fail_count;    /* Failure counter */
    int threshold;     /* Detection threshold */
} qr_status_t;

static volatile camera_state_t s_camera_local_data_state = CAMERA_DATA_STATE_NO_SIGNAL;
static uint8_t (*s_camera_mem_pool)[IMAGE_WIDTH] = &s_camera_dcmi_buf[0];
static struct quirc qr_ctx;
static struct quirc_flood_fill_vars flood_fill_vars[IMAGE_HEIGHT * 2 / 3];
static quirc_pixel_t pixels;
__attribute__((section(".qr_buffer"), aligned(32))) static uint8_t grayscale_mem_pool [CAMERA_MEM_SIZE];
static uint8_t *qr_mem_pool = &g_camera_buffer[0];
static uint8_t qr_txt[MAX_QR_PAYLOAD+10] = {0}; /* Buffer for constructing QR code messages for display */
static qr_detection_t g_detection_state = {0};
static uint32_t s_qr_ram_addr = 0;
static uint32_t s_qr_handler = 0;
static bool s_found_valid = false;
static struct quirc_point s_qr_corners[4] = {0};
static bool s_has_qr_corners = false;
static bool s_dialog_visible = false;
static bool s_camera_paused = false;
static uint16_t s_show_dialog_enabled = 65535;  /* Toggle state: 0=OFF, 65535=ON */
static uint16_t s_pause_camera_enabled = 65535; /* Toggle state: 0=OFF, 65535=ON */
extern bool g_is_menu_active;
extern uint8_t g_using_line_event;

/**
 * @brief Compare decoded payload with reference buffer
 * @param d1: Decoded QR data structure
 * @param p2: Reference payload buffer
 * @param l2: Length of reference payload
 * @return: true if payloads match, false otherwise
 */
static bool payloads_match(const struct quirc_data *d1, const char *p2, int l2)
{
    return (d1->payload_len == l2) && (memcmp(d1->payload, p2, l2) == 0);
}

/**
 * @brief Reset detection state and set UI message to NOT DETECTED
 */
static void reset_detection(void)
{
    const char *msg = "NOT DETECTED";
    size_t len = strlen(msg);

    if (len >= sizeof(qr_txt))
        len = sizeof(qr_txt) - 1;

    memcpy(qr_txt, msg, len);
    qr_txt[len] = '\0';

    /* Reset detection counters */
    g_detection_state.success_count = 0;
    g_detection_state.fail_count = 0;
    g_detection_state.is_stable = false;
    g_detection_state.has_valid_qr = false;
    g_detection_state.last_payload_len = 0;

    s_has_qr_corners = false;
    s_camera_paused = false;
    s_dialog_visible = false;
    s_found_valid = false;

    Camera_Resume();
}

/**
 * @brief Update detection state using newly decoded QR data
 * @param data: Decoded QR data structure
 * @return: true if data is accepted by detection logic
 */
static bool update_detection(const struct quirc_data *data)
{
    if (g_detection_state.has_valid_qr &&
        payloads_match(data, g_detection_state.last_payload,
                       g_detection_state.last_payload_len))
    {
        /* Same QR code detected consecutively */
        g_detection_state.fail_count = 0;
        g_detection_state.success_count++;
    }
    else
    {
        /* New QR code or first detection */
        memcpy(g_detection_state.last_payload, data->payload, data->payload_len);
        g_detection_state.last_payload_len = data->payload_len;
        g_detection_state.success_count = 1;
        g_detection_state.fail_count = 0;
        g_detection_state.has_valid_qr = true;
    }

    g_detection_state.is_stable = true;
    return true;
}

/**
 * @brief Get current QR detection detection status
 * @param status: Output status structure pointer
 */
static void get_qr_status(qr_status_t *status)
{
    status->is_stable = g_detection_state.is_stable;
    status->success_count = g_detection_state.success_count;
    status->fail_count = g_detection_state.fail_count;
    status->threshold = QR_detection_THRESHOLD;
}

/**
 * @brief Wrap QR code text by inserting newlines
 *
 * Inserts newline characters into qr_txt so that no line
 * exceeds QR_TEXT_WRAP_NUM_CHARS characters. Tries to break at spaces
 * when possible, otherwise breaks at the limit.
 */
static void wrap_qr_text(void)
{
#if ENABLE_QR_TEXT_WRAP
    char temp[MAX_QR_PAYLOAD];
    size_t src_len = strlen((char *)qr_txt);
    size_t src_idx = 0;
    size_t dst_idx = 0;
    size_t line_len = 0;
    size_t last_space_src = 0;
    size_t last_space_dst = 0;
    bool has_space = false;

    if (src_len == 0 || src_len >= MAX_QR_PAYLOAD)
        return;

    while (src_idx < src_len && dst_idx < MAX_QR_PAYLOAD - 1)
    {
        char c = qr_txt[src_idx];

        /* Track last space position for word wrapping */
        if (c == ' ')
        {
            last_space_src = src_idx;
            last_space_dst = dst_idx;
            has_space = true;
        }

        /* Check if we need to wrap */
        if (line_len >= QR_TEXT_WRAP_NUM_CHARS && c != '\n')
        {
            if (has_space && last_space_dst > 0)
            {
                /* Wrap at last space - replace space with newline */
                temp[last_space_dst] = '\n';
                line_len = dst_idx - last_space_dst;
            }
            else
            {
                /* No space found, force break at current position */
                if (dst_idx < MAX_QR_PAYLOAD - 1)
                {
                    temp[dst_idx++] = '\n';
                    line_len = 0;
                }
            }
            has_space = false;
        }

        /* Copy character */
        if (c == '\n')
        {
            line_len = 0;
            has_space = false;
        }
        else
        {
            line_len++;
        }

        temp[dst_idx++] = c;
        src_idx++;
    }

    temp[dst_idx] = '\0';
    memcpy(qr_txt, temp, dst_idx + 1);
#endif
}

/**
 * @brief Copy decoded QR payload into display buffer
 * @param data: Decoded QR data structure
 */
static void print_qr_data(const struct quirc_data *data)
{
    size_t copy_len = data->payload_len;

    if (copy_len >= sizeof(qr_txt))
        copy_len = sizeof(qr_txt) - 1;

    memcpy(qr_txt, data->payload, copy_len);
    qr_txt[copy_len] = '\0';

    /* Wrap text if enabled */
    wrap_qr_text();
}

/**
 * @brief Process and decode QR codes in the current frame
 * @param q: QUIRC context structure
 */
static void process_qr_codes(struct quirc *q)
{
    int count;
    int i;
    struct quirc_code code;
    struct quirc_data data;
    quirc_decode_error_t err;

    count = quirc_count(q);
    s_found_valid = false;

    /* Process each detected QR code */
    for (i = 0; i < count; i++)
    {
        quirc_extract(q, i, &code);
        err = quirc_decode(&code, &data);

        if (err)
        {
            /* Decode error - copy error message */
            const char *error_msg = quirc_strerror(err);
            size_t msg_len = strlen(error_msg);

            if (msg_len >= sizeof(qr_txt))
                msg_len = sizeof(qr_txt) - 1;

            memcpy(qr_txt, error_msg, msg_len);
            qr_txt[msg_len] = '\0';
            continue;
        }

        /* Successfully decoded QR code */
        s_found_valid = true;
        update_detection(&data);
        print_qr_data(&data);

        /* Save QR code corners for drawing frame */
        memcpy(s_qr_corners, code.corners, sizeof(s_qr_corners));
        s_has_qr_corners = true;

        /* Pause camera if pause toggle is ON */
        if (s_pause_camera_enabled)
        {
            s_camera_paused = true;
            Camera_Pause();
        }

        /* Show dialog if dialog toggle is ON */
        if (s_show_dialog_enabled)
        {
            s_dialog_visible = true;
            s_camera_paused = true;
        }
        break; /* Process only first valid QR code */
    }

    if (!s_found_valid)
    {
        reset_detection();
    }
}

/**
 * @brief Process camera image for QR code detection
 * @return: 0 on success, -1 on error
 */
static int process_image_for_qr()
{
    uint8_t *qr_image;

    /* Get QUIRC image buffer and copy L8 data */
    qr_image = quirc_begin(&qr_ctx, NULL, NULL);
    if (!qr_image)
    {
        return -1;
    }
    memcpy(qr_image, grayscale_mem_pool, CAMERA_MEM_SIZE);
    quirc_end(&qr_ctx);

    /* Process QR codes in the image */
    process_qr_codes(&qr_ctx);

    return 0;
}

/**
 * @brief Handle user input events on QR page
 * @return: PAGE_FINISH to exit page, PAGE_CONTINUE to remain
 */
static int event(void)
{
    Gesture_Touch_t *ges = utils_gesture_get();

    /* Handle toggle for show dialog option */
    if (ges->tagReleased == TAG_SHOW_DIALOG_TOGGLE)
    {
        s_show_dialog_enabled = s_show_dialog_enabled ? 0 : 65535;
        return PAGE_CONTINUE;
    }

    /* Handle toggle for pause camera option */
    if (ges->tagReleased == TAG_PAUSE_CAMERA_TOGGLE)
    {
        s_pause_camera_enabled = s_pause_camera_enabled ? 0 : 65535;
        return PAGE_CONTINUE;
    }

    /* Handle continue button to resume camera */
    if (s_camera_paused && ges->tagReleased == TAG_CONTINUE_BTN)
    {
        reset_detection();
        return PAGE_CONTINUE;
    }

    /* Handle dialog close button */
    if (ges->tagPressed == TAG_DIALOG_CLOSE)
    {
        reset_detection();
        return PAGE_CONTINUE;
    }

    /* Check for back button tap */
    if (ges->tagReleased == ICON_BACKWARD->tagval)
    {
        /* Close dialog first if visible */
        if (s_dialog_visible)
        {
            reset_detection();
        }
        g_is_menu_active = true;
        return PAGE_FINISH;
    }

    return PAGE_CONTINUE;
}

/**
 * @brief Draw green frame around detected QR code on camera preview
 */
static void draw_qr_frame_on_preview(void)
{
#define QR_FRAME_LINE_WIDTH 3

    if (!s_has_qr_corners)
        return;

    /* Compute scale factor from box_camera */
    int32_t preview_scale = 3;
    
    /* Transform QR corners from image coordinates to screen coordinates */
    int x0 = box_camera.x + s_qr_corners[0].x * preview_scale;
    int y0 = box_camera.y + s_qr_corners[0].y * preview_scale;
    int x1 = box_camera.x + s_qr_corners[1].x * preview_scale;
    int y1 = box_camera.y + s_qr_corners[1].y * preview_scale;
    int x2 = box_camera.x + s_qr_corners[2].x * preview_scale;
    int y2 = box_camera.y + s_qr_corners[2].y * preview_scale;
    int x3 = box_camera.x + s_qr_corners[3].x * preview_scale;
    int y3 = box_camera.y + s_qr_corners[3].y * preview_scale;

    /* Draw green frame around QR code */
    SET_COLOR_LIME(PHOST);
    EVE_Cmd_wr32(PHOST, LINE_WIDTH(QR_FRAME_LINE_WIDTH * 16));
    EVE_Cmd_wr32(PHOST, BEGIN(LINE_STRIP));
    g_utils_adapter_vertex2f(PHOST, x0, y0);
    g_utils_adapter_vertex2f(PHOST, x1, y1);
    g_utils_adapter_vertex2f(PHOST, x2, y2);
    g_utils_adapter_vertex2f(PHOST, x3, y3);
    g_utils_adapter_vertex2f(PHOST, x0, y0);

    /* Reset color */
    SET_COLOR_WHITE(PHOST);
}

/**
 * @brief Draw QR result dialog with border, title, content and close button
 */
static void draw_qr_result_dialog(void)
{
    int32_t dialog_x = box_dialog_qr_result.x;
    int32_t dialog_y = box_dialog_qr_result.y;
    int32_t dialog_w = box_dialog_qr_result.w;
    int32_t dialog_h = box_dialog_qr_result.h;
    int32_t border_width = (dialog_w * 0.5) / 100;
    int32_t title_y = dialog_y + (dialog_h * 8) / 100;
    int32_t line_y = dialog_y + (dialog_h * 17) / 100;
    int32_t label_y = dialog_y + (dialog_h * 25) / 100;
    int32_t data_y = dialog_y + (dialog_h * 42) / 100;
    int32_t btn_w = (dialog_w * 35) / 100;
    int32_t btn_h = (dialog_h * 15) / 100;
    int32_t btn_x = dialog_x + (dialog_w - btn_w) / 2;
    int32_t btn_y = dialog_y + dialog_h - btn_h - (dialog_h * 7) / 100;
    int32_t line_x1 = dialog_x + (dialog_w * 5) / 100;
    int32_t line_x2 = dialog_x + dialog_w - (dialog_w * 5) / 100;
    int32_t dialog_center_x = dialog_x + dialog_w / 2;
    
    if (border_width < 2)
        border_width = 2;

    /* Draw dialog border (outer rectangle) */
    SET_COLOR_LIME(PHOST);
    EVE_Cmd_wr32(PHOST, BEGIN(RECTS));
    g_utils_adapter_vertex2f(PHOST, dialog_x - border_width, dialog_y - border_width);
    g_utils_adapter_vertex2f(PHOST, dialog_x + dialog_w + border_width, dialog_y + dialog_h + border_width);

    /* Draw dialog background (inner rectangle) */
    EVE_CoDl_colorRgb(PHOST, 0x20, 0x20, 0x30);
    EVE_Cmd_wr32(PHOST, BEGIN(RECTS));
    g_utils_adapter_vertex2f(PHOST, dialog_x, dialog_y);
    g_utils_adapter_vertex2f(PHOST, dialog_x + dialog_w, dialog_y + dialog_h);

    /* Draw title */
    SET_COLOR_LIME(PHOST);
    EVE_CoCmd_text(PHOST, dialog_center_x, title_y, DIALOG_TITLE_FONT,
                   OPT_CENTERX, "QR Code Detected!");

    /* Draw horizontal line under title */
    EVE_Cmd_wr32(PHOST, BEGIN(LINES));
    EVE_Cmd_wr32(PHOST, LINE_WIDTH(2 * 16));
    g_utils_adapter_vertex2f(PHOST, line_x1, line_y);
    g_utils_adapter_vertex2f(PHOST, line_x2, line_y);

    /* Draw QR content label */
    SET_COLOR_WHITE(PHOST);
    EVE_CoCmd_text(PHOST, dialog_center_x, label_y, DIALOG_TEXT_FONT,
                   OPT_CENTERX, "Content: ");

    /* Draw detected QR data */
    SET_COLOR_YELLOW(PHOST);
    size_t qr_txt_len = strlen((char *)qr_txt);
    const uint32_t PIXEL_PER_CHAR = 15;
    uint32_t available_width = dialog_w - (dialog_w * 10) / 100;
    uint32_t text_width = qr_txt_len * PIXEL_PER_CHAR;
    int32_t offset = (int32_t)(available_width - text_width) / 2;
    if (offset < 0)
        offset = 0;
    uint32_t text_x = dialog_x + (dialog_w * 5) / 100 + offset;
    EVE_CoCmd_fillWidth(PHOST, available_width);
    EVE_CoCmd_text(PHOST, text_x, data_y, DIALOG_TEXT_FONT, OPT_FILL, qr_txt);

    /* Draw close button */
    SET_COLOR_WHITE(PHOST);
    EVE_CoDl_tag(PHOST, TAG_DIALOG_CLOSE);
    EVE_CoCmd_button(PHOST, btn_x, btn_y, btn_w, btn_h,
                     DIALOG_TEXT_FONT, 0, "Close");
    EVE_CoDl_tag(PHOST, 0);
}

/**
 * @brief Draw page background
 */
static void draw_body(void)
{
    utils_draw_image_xy_tag(PHOST, ICON_BACKGROUND, 0, 0, 0);
}

/**
 * @brief Draw camera preview and QR detection status
 */
static void draw_videos(void)
{
    /* Compute status text position below camera */
    int32_t status_y = box_camera.y + box_camera.h + (box_body.h * 3) / 100;
    app_box box_status = init_app_box(box_camera.x, status_y, box_camera.w, box_body.h);

    /* Process camera image only if not paused */
    if (!s_camera_paused)
    {
        if (s_camera_local_data_state == CAMERA_DATA_STATE_BUFFER_READY)
        {
            s_camera_local_data_state = CAMERA_DATA_STATE_QR_PARSING;
            if (process_image_for_qr(IMAGE_WIDTH, IMAGE_HEIGHT) == 0)
            {
                s_camera_local_data_state = CAMERA_DATA_STATE_FIFO_UPLOADING;
                EVE_Hal_wrMem(PHOST, s_qr_ram_addr, grayscale_mem_pool, CAMERA_MEM_SIZE);
            }
            Camera_Start_Next();
            s_camera_local_data_state = CAMERA_DATA_STATE_IDLE;
        }
    }

    /* Render camera preview with scaling */
    if (s_camera_local_data_state != CAMERA_DATA_STATE_NO_SIGNAL)
    {
        EVE_Cmd_wr32(PHOST, BEGIN(BITMAPS));
        SET_BITMAP_HANDLE(PHOST, s_qr_handler);
        EVE_CoDl_bitmapSize(PHOST, NEAREST, BORDER, BORDER, box_camera.w, box_camera.h);

        utils_eve_scale(300);
        g_utils_adapter_vertex2f(PHOST, box_camera.x, box_camera.y);
        EVE_CoCmd_loadIdentity(PHOST);
        EVE_CoCmd_setMatrix(PHOST);
    }

    /* Draw green frame around detected QR code */
    if (s_has_qr_corners)
        draw_qr_frame_on_preview();

    SET_COLOR_WHITE(PHOST);
}

/**
 * @brief Draw control buttons and toggles
 */
static void draw_controls(void)
{
    const uint32_t btn_continue_w = 180;
    const uint32_t btn_continue_h = 60;
    uint32_t y = box_controls.y + 30;
    uint32_t x = box_controls.x;
#define LABEL_STATUS_FONT 30
#define LABEL_STATUS_FILLWIDTH 350

    app_box box_btn_continue = init_app_box(x + (box_controls.w - btn_continue_w) / 2, y, btn_continue_w, btn_continue_h);

    /* Draw continue button when camera is paused */
    if (s_camera_paused && !s_show_dialog_enabled)
    {
        SET_COLOR_LIME(PHOST);
        EVE_CoDl_tag(PHOST, TAG_CONTINUE_BTN);
        EVE_CoCmd_button(PHOST, box_btn_continue.x, box_btn_continue.y, btn_continue_w, btn_continue_h, 30, 0, "Continue");
        EVE_CoDl_tag(PHOST, 0);
        SET_COLOR_WHITE(PHOST);
    }

    /* Draw toggles when camera is not paused */
    if (!s_camera_paused)
    {
        SET_COLOR_WHITE(PHOST);
        EVE_CoCmd_text(PHOST, x, y, 30, 0, "Show dialog:");
        EVE_CoDl_tag(PHOST, TAG_SHOW_DIALOG_TOGGLE);
        if (s_show_dialog_enabled)
            SET_COLOR_LIME(PHOST);
        else
            SET_COLOR_CHOCOLATE(PHOST);
        EVE_CoCmd_toggle(PHOST, x + 230, y+10, 100, 30, 0, s_show_dialog_enabled, "OFF\xFFON");
        EVE_CoDl_tag(PHOST, 0);
        SET_COLOR_WHITE(PHOST);

        x = box_controls.x_mid;
        EVE_CoCmd_text(PHOST, x, y, 30, 0, "Pause on QR:");
        if (s_pause_camera_enabled)
            SET_COLOR_LIME(PHOST);
        else
            SET_COLOR_CHOCOLATE(PHOST);
        EVE_CoDl_tag(PHOST, TAG_PAUSE_CAMERA_TOGGLE);
        EVE_CoCmd_toggle(PHOST, x + 250, y+10, 100, 30, 0, s_pause_camera_enabled, "OFF\xFFON");
        EVE_CoDl_tag(PHOST, 0);
    }

    y += 100;
    /* Display status text */
    EVE_CoCmd_text(PHOST, box_controls.x, y, LABEL_STATUS_FONT, 0, "QR code:");

    /* Color code the detection result */
    if (s_found_valid)
        SET_COLOR_LIME(PHOST); /* Bright green for success */
    else
        SET_COLOR_RED(PHOST); /* Red for failure */

    EVE_CoCmd_fillWidth(PHOST, LABEL_STATUS_FILLWIDTH);
    EVE_CoCmd_text(PHOST, box_controls.x + 170, y, LABEL_STATUS_FONT, OPT_FILL, qr_txt);

    /* Reset color */
    SET_COLOR_WHITE(PHOST);

    /* Draw back button */
    SET_COLOR_WHITE(PHOST);
    utils_draw_back_button();
}

/**
 * @brief Render QR page graphics and detection status
 * @return: 0 on success
 */
static int drawing(void)
{
    draw_body();
    draw_videos();
    
    /* Draw result dialog if visible */
    if (s_dialog_visible)
    {
        draw_qr_result_dialog();
    }
    
    draw_controls();

    return 0;
}

static void my_HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
}

/**
 * @brief HAL DCMI frame event callback - copy camera frame to L8 buffer
 * @param hdcmi: DCMI handle structure pointer
 */
static void my_HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    if (s_camera_local_data_state != CAMERA_DATA_STATE_NO_SIGNAL && s_camera_local_data_state != CAMERA_DATA_STATE_IDLE)
    {
        return;
    }

    int32_t x = 0, y = 0;
    SCB_CleanInvalidateDCache_by_Addr(s_camera_mem_pool, CAMERA_MEM_SIZE / 4);
    memcpy(grayscale_mem_pool, s_camera_mem_pool, CAMERA_MEM_SIZE);
    s_camera_local_data_state = CAMERA_DATA_STATE_BUFFER_READY;
}

/**
 * @brief Execute one QR page frame - handle events and render graphics
 * @return: PAGE_* state indicating navigation
 */
int page_qr_draw(void)
{
    int event_result;

    event_result = event();
    drawing();

    return event_result;
}

/**
 * @brief Perform QR page load-time initialization
 * @return: 0 on success
 */
int page_qr_load(void)
{
    return 0;
}

/**
 * @brief Initialize layout boxes used across the QR page UI.
 *
 * This sets up the layout boxes: box_lcd (full screen), box_body (main area),
 * box_camera (camera preview area), and box_controls (control buttons area).
 */
static void init_layout(void)
{
    /* Full screen */
    box_lcd = init_app_box(0, 0, SCREEN_W, SCREEN_H);
    box_body = init_app_box_child_center_middle(box_lcd.w, box_lcd.h, box_lcd);

    /* Camera preview area (centered in body, 60% of body height) */
    int32_t camera_w = box_body.w *  8 / 10;
    int32_t camera_h = camera_w * 3 / 4;
    box_camera = init_app_box_child_center_middle(camera_w, camera_h, box_body);
    box_controls = init_app_box_below(box_camera.w, box_body.h / 3, box_camera);
    box_dialog_qr_result = init_app_box_child_center_middle(box_camera.w, box_camera.h, box_lcd);
}

/**
 * @brief One-time initialization of QR page resources
 */
static void page_qr_init_one(void)
{
    static bool s_is_page_qr_init = false;
    if (s_is_page_qr_init)
        return;
    s_is_page_qr_init = true;

    /* Allocate EVE memory and configure bitmap */
    s_qr_ram_addr = utils_ddr_alloc_alignment(IMAGE_WIDTH * IMAGE_HEIGHT, 32);
    s_qr_handler = utils_bitmap_handler_get();
}

/**
 * @brief Initialize QR page camera and resources
 */
void page_qr_init(void)
{
    page_qr_init_one();
    
    g_using_line_event = 0;
    /* Initialize layout boxes */
    init_layout();

    memset(grayscale_mem_pool, 0, sizeof(grayscale_mem_pool));
    memset(g_camera_buffer, 0, sizeof(g_camera_buffer));
    memset(qr_txt, 0, sizeof(qr_txt));
    memset(&qr_ctx, 0, sizeof(qr_ctx));
    memset(flood_fill_vars, 0, sizeof(flood_fill_vars));
    memset(&pixels, 0, sizeof(pixels));

    qr_ctx.w = IMAGE_WIDTH;
    qr_ctx.h = IMAGE_HEIGHT;
    qr_ctx.image = qr_mem_pool;
    qr_ctx.flood_fill_vars = flood_fill_vars;
    qr_ctx.pixels = &pixels;
    quirc_resize_no_malloc(&qr_ctx, IMAGE_WIDTH, IMAGE_HEIGHT);

    set_HAL_DCMI_FrameEventCallback(my_HAL_DCMI_FrameEventCallback);
    set_HAL_DCMI_LineEventCallback(my_HAL_DCMI_LineEventCallback);
    Camera_On(FRAMESIZE_QVGA, PIXFORMAT_GRAYSCALE, DCMI_MODE_SNAPSHOT, s_camera_mem_pool, CAMERA_MEM_SIZE);
    EVE_Cmd_wr32(PHOST, BITMAP_HANDLE(s_qr_handler));
    EVE_CoCmd_setBitmap(PHOST, s_qr_ram_addr, L8, IMAGE_WIDTH, IMAGE_HEIGHT);

    /* Reset detection state */
    reset_detection();
}

/**
 * @brief Deinitialize QR page resources
 */
static void page_qr_deinit_one(void)
{
    static bool s_is_page_qr_deinit = false;
    if (s_is_page_qr_deinit)
        return;
    s_is_page_qr_deinit = true;
}

/**
 * @brief Deinitialize QR page resources
 */
void page_qr_deinit(void)
{
    reset_detection();
    page_qr_deinit_one();

    /* Stop camera capture */
    Camera_Off();
}
