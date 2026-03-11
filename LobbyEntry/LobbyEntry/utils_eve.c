/**
 * @file utils_eve.c
 * @brief EVE display controller utility functions
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <stdint.h>

#include "utils_eve.h"
#include "utils_vertex_adapter.h"

/* Constants for scaling operations */
#define SCALING_FACTOR_BASE 65536L
#define PERCENTAGE_BASE 100

/**
 * @brief Get bits per pixel for EVE bitmap formats
 * @param format EVE bitmap format constant
 * @return Bits per pixel for the specified format
 */
static inline int32_t EVE_UTILS_FORMAT_BPP(uint32_t format)
{
	switch (format)
	{
	case ARGB1555:
	case ARGB4:
	case RGB565:
		return 16;
	case ARGB2:
	case L8:
	case RGB332:
	case PALETTED565:
	case PALETTED4444:
	case PALETTED8:
	case BARGRAPH:
		return 8;
	case L4:
		return 4;
	case L2:
		return 2;
	case L1:
		return 1;
	default:
		/* YCBCR and other formats default to 16 bpp */
		return 16;
	}
}

/* Default calibration text color values */
#define CAL_TEXT_COLOR_R 0xFF
#define CAL_TEXT_COLOR_G 0xFF
#define CAL_TEXT_COLOR_B 0xFF

/* Default calibration background color values */
#define CAL_BG_COLOR_R 64
#define CAL_BG_COLOR_G 64
#define CAL_BG_COLOR_B 64

/* Default display colors */
static const uint8_t g_default_bg_color[3] = {0, 0, 0};
static const uint8_t g_default_text_color[3] = {255, 255, 255};

static uint32_t calibration_matrix[6] = {0};

/**
 * @brief Command wait callback for EVE operations
 * @param pHost Pointer to EVE host context
 * @return Always returns true to continue operation
 */
static bool cb_CmdWait(struct EVE_HalContext *pHost)
{
	(void)(pHost);
	return true;
}

/**
 * @brief Calibrate the touch screen controller
 * @param pHost Pointer to EVE host context
 * @return true if calibration was successful, false otherwise
 *
 * This function sets up the touch controller in compatibility mode
 * and performs screen calibration with user interaction.
 */
static bool calibration_show(EVE_HalContext *phost)
{
	uint32_t result;
	uint32_t transMatrix[6] = {0};
	EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);

	eve_printf_debug("App_CoPro_Widget_Calibrate: Start\n");

	EVE_CoCmd_dlStart(phost);
	EVE_CoDl_clearColorRgb(phost, 64, 64, 64);
	EVE_CoDl_clear(phost, 1, 1, 1);
	EVE_CoDl_colorRgb(phost, 0xff, 0xff, 0xff);

	EVE_CoCmd_text(phost, (uint16_t)(phost->Width / 2), (uint16_t)(phost->Height / 2), 31, OPT_CENTER, "Please Tap on the dot");
	result = EVE_CoCmd_calibrate(phost);
	EVE_Cmd_waitFlush(phost);

	eve_printf_debug("App_CoPro_Widget_Calibrate: End\n");

	// Print the configured values
	EVE_Hal_rdMem(phost, (uint8_t *)transMatrix, REG_TOUCH_TRANSFORM_A, 4 * 6); // read all the 6 coefficients
	eve_printf_debug("Touch screen transform values are A 0x%lx,B 0x%lx,C 0x%lx,D 0x%lx,E 0x%lx, F 0x%lx\n",
					 transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

	return result != 0;
}

void calibration_restore(EVE_HalContext *phost)
{
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_A, calibration_matrix[0]);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_B, calibration_matrix[1]);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_C, calibration_matrix[2]);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_D, calibration_matrix[3]);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_E, calibration_matrix[4]);
	EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_F, calibration_matrix[5]);
}
void calibration_save(EVE_HalContext *phost)
{
	calibration_matrix[0] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_A);
	calibration_matrix[1] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_B);
	calibration_matrix[2] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_C);
	calibration_matrix[3] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_D);
	calibration_matrix[4] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_E);
	calibration_matrix[5] = EVE_Hal_rd32(phost, REG_TOUCH_TRANSFORM_F);
}
bool utils_eve_calibrate_set(EVE_HalContext *phost, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f)
{
	printf("utils_calibrateInit with data:0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", a, b, c, d, e, f);

	if ((a | b | c | d | e | f) == 0)
	{
		printf("utils_calibrateInit zero\r\n");
		calibration_show(phost);
		calibration_save(phost);
	}
	else
	{
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_A, a);
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_B, b);
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_C, c);
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_D, d);
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_E, e);
		EVE_Hal_wr32(phost, REG_TOUCH_TRANSFORM_F, f);
		calibration_save(phost);
	}
	return 0;
}

bool utils_eve_calibrate(EVE_HalContext *phost)
{
	return utils_eve_calibrate_set(phost, 0, 0, 0, 0, 0, 0);
}

/**
 * @brief Initialize EVE display controller
 * @param pHost Pointer to EVE host context
 *
 * This function initializes the EVE hardware abstraction layer,
 * configures device parameters, and performs bootup configuration.
 */
void utils_eve_init(EVE_HalContext *pHost)
{
	size_t deviceIdx;
	EVE_HalParameters params;

#if defined(BT8XXEMU_PLATFORM)
	/* Print emulator version information */
	printf(BT8XXEMU_version());
#endif

	/* Initialize HAL and configure device */
	EVE_Hal_initialize();
	deviceIdx = -1;
	EVE_Hal_defaultsEx(&params, deviceIdx);
	params.CbCmdWait = cb_CmdWait;

	/* Initialize host and open device */
	utils_host_init(pHost);
	EVE_Hal_open(pHost, &params);
	EVE_Util_bootupConfig(pHost);
}

/**
 * @brief Release EVE display controller resources
 * @param pHost Pointer to EVE host context
 *
 * This function properly closes the EVE device and releases
 * all associated hardware abstraction layer resources.
 */
void utils_eve_release(EVE_HalContext *pHost)
{
	EVE_Hal_close(pHost);
	EVE_Hal_release();
}

/**
 * @brief Apply scaling transformation to display matrix
 * @param percent Scaling percentage (e.g., 100 for normal size, 50 for half size)
 *
 * This function sets up a scaling transformation matrix that affects
 * all subsequent drawing operations until the matrix is reset.
 */
void utils_eve_scale_and_translate_center(int32_t percent, int32_t im_w, int32_t im_h, uint32_t format)
{
	uint32_t layout_w = percent * im_w / 100;
	uint32_t layout_h = percent * im_h / 100;
	int32_t translate_x = (im_w - layout_w) / 2;
	int32_t translate_y = (im_h - layout_h) / 2;

	EVE_CoDl_bitmapLayout(PHOST, format, layout_w, layout_h);

	EVE_CoCmd_loadIdentity(PHOST);
	EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * translate_x, SCALING_FACTOR_BASE * translate_y);
	EVE_CoCmd_scale(PHOST, 2 * SCALING_FACTOR_BASE, 2 * SCALING_FACTOR_BASE);
	EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * -translate_x, SCALING_FACTOR_BASE * -translate_y);
	EVE_CoCmd_setMatrix(PHOST);
}

void utils_eve_scale_and_translate_to(int32_t im_w, int32_t im_h, int32_t target_w, int32_t target_h, int32_t translate_x, int32_t translate_y, uint32_t format)
{
	uint32_t layout_w = max(target_w, im_w) * 2;
	uint32_t layout_h = max(target_h, im_h) * 2;

	EVE_CoDl_bitmapSize(PHOST, NEAREST, BORDER, BORDER, layout_w, layout_h);

	/* Calculate scale factor to fit image into target area */
	int32_t scale_x = (target_w * SCALING_FACTOR_BASE) / im_w;
	int32_t scale_y = (target_h * SCALING_FACTOR_BASE) / im_h;
	int32_t scale_factor = (scale_x < scale_y) ? scale_x : scale_y;

	EVE_CoCmd_loadIdentity(PHOST);
	EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * translate_x, SCALING_FACTOR_BASE * translate_y);
	EVE_CoCmd_scale(PHOST, scale_factor, scale_factor);
	EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * -translate_x, SCALING_FACTOR_BASE * -translate_y);
	EVE_CoCmd_setMatrix(PHOST);
}

void utils_eve_scale_to(int32_t im_w, int32_t im_h, int32_t target_w, int32_t target_h, uint32_t format, int32_t *translated_x, int32_t *translated_y)
{
	uint32_t layout_w = max(target_w, im_w) * 2;
	uint32_t layout_h = max(target_h, im_h) * 2;

	/* Calculate scale factor to fit image into target area */
	int32_t scale_x = (target_w * SCALING_FACTOR_BASE) / im_w;
	int32_t scale_y = (target_h * SCALING_FACTOR_BASE) / im_h;
	int32_t scale_factor = (scale_x < scale_y) ? scale_x : scale_y;

	uint32_t im_w_new = (im_w * scale_factor) / SCALING_FACTOR_BASE;
	uint32_t im_h_new = (im_h * scale_factor) / SCALING_FACTOR_BASE;
	int32_t translate_x = (target_w - im_w_new) / 2;
	int32_t translate_y = (target_h - im_h_new) / 2;

	if(translated_x){
		*translated_x = translated_x;
	}
	if(translated_y){
		*translated_y = translated_y;
	}

	EVE_CoDl_bitmapSize(PHOST, NEAREST, BORDER, BORDER, im_w * scale_factor, im_h * scale_factor);

	EVE_CoCmd_loadIdentity(PHOST);
	if (im_w > im_w_new || im_h > im_h_new)
	{
		EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * translate_x, SCALING_FACTOR_BASE * translate_y);
	}
	EVE_CoCmd_scale(PHOST, scale_factor, scale_factor);
	if (im_w > im_w_new || im_h > im_h_new)
	{
		EVE_CoCmd_translate(PHOST, SCALING_FACTOR_BASE * -translate_x, SCALING_FACTOR_BASE * -translate_y);
	}
	EVE_CoCmd_setMatrix(PHOST);
}

void utils_eve_scale(int percent)
{
	int32_t scale_factor;

	if (percent <= 0)
		return;

	/* Load identity matrix and apply scaling */
	EVE_CoCmd_loadIdentity(PHOST);
	scale_factor = percent * SCALING_FACTOR_BASE / PERCENTAGE_BASE;
	EVE_CoCmd_scale(PHOST, scale_factor, scale_factor);
	EVE_CoCmd_setMatrix(PHOST);
}

/**
 * @brief Start display list with custom colors
 * @param pHost Pointer to EVE host context
 * @param bgColor Pointer to 3-byte array containing RGB background color
 * @param textColor Pointer to 3-byte array containing RGB text color
 *
 * This function initializes a new display list with specified background
 * and text colors, and sets up the vertex format for drawing operations.
 */
void utils_display_start_color(EVE_HalContext *pHost, uint8_t *bgColor, uint8_t *textColor)
{
	EVE_CoCmd_dlStart(pHost);
	EVE_CoDl_clearColorRgb(pHost, bgColor[0], bgColor[1], bgColor[2]);
	EVE_CoDl_clear(pHost, 1, 1, 1);
	EVE_CoDl_colorRgb(pHost, textColor[0], textColor[1], textColor[2]);
	utils_vertex_format(pHost, 3);
}

/**
 * @brief Start display list with default colors
 * @param pHost Pointer to EVE host context
 *
 * This function initializes a new display list with default black background
 * and white text colors.
 */
void utils_display_start(EVE_HalContext *pHost)
{
	static uint8_t bgColor[] = {0, 0, 0};
	static uint8_t textColor[] = {255, 255, 255};
	utils_display_start_color(pHost, bgColor, textColor);
}

bool take_screenshot = false;

/**
 * @brief Finalize and display the current display list
 * @param pHost Pointer to EVE host context
 *
 * This function completes the current display list, swaps buffers,
 * and waits for all graphics operations to complete.
 */
void utils_display_end(EVE_HalContext *pHost)
{
	EVE_CoDl_display(pHost);
	EVE_CoCmd_swap(pHost);
	EVE_CoCmd_graphicsFinish(pHost);
	EVE_Cmd_waitFlush(pHost);
}

/**
 * @brief Display a modal dialog box with message text
 * @param phost EVE HAL context pointer
 * @param msg Message text to display in dialog
 *
 * Creates a semi-transparent overlay with centered dialog box containing
 * the specified message. Automatically scales based on screen resolution.
 */
void show_dialog_info(EVE_HalContext *phost, const uint8_t *msg)
{
	/* Macro definitions */
#define INPUT_YES 1
#define INPUT_NO 0
	uint8_t ret = INPUT_NO;
	const uint32_t tag_y = 1;
	const uint32_t tag_n = 2;
	const uint16_t border = 2;
	const uint16_t base_w = 800;
	const uint16_t base_h = 600;
	uint16_t w = 400;
	uint16_t h = 200;
	uint16_t x = (base_w - w) / 2;
	uint16_t y = (base_h - h) / 2;
	uint8_t htop = 50;
	uint8_t hbottom = 50;
	uint8_t font = 20;
	uint16_t btn_h = 40;
	uint16_t btn_w = 100;
	uint32_t ratio = base_w * 100 / phost->Width;

	/* Scale dialog based on screen resolution */
	if (ratio != 100)
	{
		w = w * 100 / ratio;
		h = h * 100 / ratio;
		btn_w = btn_w * 100 / ratio;
		btn_h = btn_h * 100 / ratio;
		x = (phost->Width - w) / 2;
		y = (phost->Height - h) / 2;
		htop = htop * 100 / ratio;
		hbottom = hbottom * 100 / ratio;

		ratio += (100 - ratio) / 2;
		font = font * 100 / ratio;
		if (font > 31)
			font = 31;
		if (font < 16)
			font = 16;
		if (font == 17 || font == 19)
			font = 18;
	}

	Display_Start(phost);

	/* Semi-transparent backdrop */
	EVE_Cmd_wr32(phost, COLOR_RGB(0x3F, 0x3F, 0x3F));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	EVE_Cmd_wr32(phost, VERTEX2F(0, 0));
	g_utils_adapter_vertex2f(phost, phost->Width, phost->Height);

	/* Dialog box border */
	EVE_Cmd_wr32(phost, COLOR_RGB(0xE1, 0xE1, 0xE1));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	g_utils_adapter_vertex2f(phost, x, y);
	g_utils_adapter_vertex2f(phost, x + w, y + h);

	/* Dialog box background */
	EVE_Cmd_wr32(phost, COLOR_RGB(0xFF, 0xFF, 0xFF));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	g_utils_adapter_vertex2f(phost, x + border, y + border);
	g_utils_adapter_vertex2f(phost, x + w - border, y + h - border);

	/* Header background */
	EVE_Cmd_wr32(phost, COLOR_RGB(0xF3, 0xF3, 0xF3));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	g_utils_adapter_vertex2f(phost, x + border, y + border);
	g_utils_adapter_vertex2f(phost, x + w - border, y + htop);

	/* Footer background */
	EVE_Cmd_wr32(phost, COLOR_RGB(0xF3, 0xF3, 0xF3));
	EVE_Cmd_wr32(phost, BEGIN(RECTS));
	g_utils_adapter_vertex2f(phost, x + border, y + h - hbottom + border);
	g_utils_adapter_vertex2f(phost, x + w - border, y + h - border);

	/* Message text */
	font = font * 8 / 10;
	if (font < 16)
		font = 16;
	if (font == 17 || font == 19)
		font = 18;
	EVE_Cmd_wr32(phost, COLOR_RGB(0x78, 0x78, 0x78));
	EVE_CoCmd_text(phost, x + border + 30, y + h / 2 - 20, font, 0, msg);

	Display_End(phost);
}

/**
 * @brief Draw a circle with centered text overlay
 * @param phost EVE HAL context pointer
 * @param x Circle center X coordinate
 * @param y Circle center Y coordinate
 * @param r Circle radius
 * @param txt Text string to display
 * @param font Font handle for text rendering
 * @param opt Text rendering options
 * @param txtcolor Text color in RGB format
 */
void draw_circle_with_text(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
						   const char *txt, int32_t font, uint16_t opt, uint32_t txtcolor)
{
	draw_circle(phost, x, y, r);
	set_color_rgb(phost, txtcolor);
	EVE_CoCmd_text(phost, x, y, font, opt | OPT_CENTER, txt);
}

/**
 * @brief Draw a circle with centered text overlay and touch tag
 * @param phost EVE HAL context pointer
 * @param x Circle center X coordinate
 * @param y Circle center Y coordinate
 * @param r Circle radius
 * @param txt Text string to display
 * @param font Font handle for text rendering
 * @param opt Text rendering options
 * @param txtcolor Text color in RGB format
 * @param tagval Touch tag value for interaction
 */
void draw_circle_with_text_tag(EVE_HalContext *phost, int32_t x, int32_t y, int32_t r,
							   const char *txt, int32_t font, uint16_t opt, uint32_t txtcolor, uint8_t tagval)
{
	if (tagval)
		EVE_Cmd_wr32(phost, TAG(tagval));
	draw_circle(phost, x, y, r);
	set_color_rgb(phost, txtcolor);
	if (tagval)
		EVE_Cmd_wr32(phost, TAG(tagval));
	EVE_CoCmd_text(phost, x, y, font, opt | OPT_CENTER, txt);
}

/**
 * @brief Draw a line from starting point with width and height offsets
 * @param phost EVE HAL context pointer
 * @param x Starting X coordinate
 * @param y Starting Y coordinate
 * @param w Width offset from starting point
 * @param h Height offset from starting point
 */
void draw_line(EVE_HalContext *phost, int32_t x, int32_t y, int32_t w, int32_t h)
{
	EVE_Cmd_wr32(phost, BEGIN(LINES));
	g_utils_adapter_vertex2f(phost, x, y);
	g_utils_adapter_vertex2f(phost, x + w, y + h);
	EVE_Cmd_wr32(phost, END());
}

/**
 * @brief Draw a line between two absolute coordinate points
 * @param phost EVE HAL context pointer
 * @param x First point X coordinate
 * @param y First point Y coordinate
 * @param x2 Second point X coordinate
 * @param y2 Second point Y coordinate
 */
void draw_line_xy(EVE_HalContext *phost, int32_t x, int32_t y, int32_t x2, int32_t y2)
{
	EVE_Cmd_wr32(phost, BEGIN(LINES));
	g_utils_adapter_vertex2f(phost, x, y);
	g_utils_adapter_vertex2f(phost, x2, y2);
	EVE_Cmd_wr32(phost, END());
}

bool EVE_Util_loadMediaRam(EVE_HalContext *phost, const uint8_t *srcBuf, uint32_t size, uint32_t *transfered)
{
	uint32_t blockSize = min(16 * 1024, ((phost->MediaFifoSize >> 3) << 2) - 4);
	uint32_t remaining = size;
	uint32_t totalTransferred = 0;

	if (phost->CmdFault)
		return false;

	while (totalTransferred < size)
	{
		uint32_t chunk = (remaining > blockSize) ? blockSize : remaining;
		uint32_t alignedLen = (chunk + 3) & ~3U; // align to 4

		uint32_t transferredPart = 0;
		bool wrRes = EVE_MediaFifo_wrMem(phost, srcBuf, alignedLen, &transferredPart);

		totalTransferred += transferredPart;
		srcBuf += transferredPart;
		if (remaining >= transferredPart)
		{
			remaining -= transferredPart;
		}
		else
		{
			remaining = 0;
		}

		if (transferredPart < alignedLen)
		{
			// FIFO full, stop and return so caller can retry
			break;
		}

		if (!wrRes)
			break;
	}

	if (transfered)
		*transfered += totalTransferred;

	// flush depending on transfer mode
	// return transfered ? EVE_Cmd_waitFlush(phost) : EVE_MediaFifo_waitFlush(phost, false);
	return true;
}
