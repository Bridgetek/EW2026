/**
 * @file utils_image.c
 * @brief Image loading and rendering utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "app_lobby_entry.h"
#include "utils_vertex_adapter.h"

/**
 * @brief Configure bitmap settings and prepare image for rendering
 *
 * This function sets up the EVE graphics context for rendering a specific image,
 * including bitmap handle assignment, palette configuration for indexed color formats,
 * and optional scaling parameters.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to image structure containing bitmap properties
 * @param palette Memory address of palette data (0 if no external palette)
 * @return 0 on success
 */
static uint32_t utils_set_image(EVE_HalContext *phost, Image *image, uint32_t palette)
{
	uint32_t frameW, frameH;

	/* Configure palette source for indexed color formats */
	if (palette) {
		EVE_Cmd_wr32(phost, PALETTE_SOURCE(palette));
	} else if (image->bitmap_format == PALETTEDARGB8) {
		EVE_Cmd_wr32(phost, PALETTE_SOURCE(image->ptr));
		EVE_Cmd_wr32(phost, PALETTE_SOURCEH2(image->ptr));
	}

	/* Set bitmap properties */
	EVE_CoCmd_setBitmap(phost, image->ptr, image->bitmap_format, image->w, image->h);

	/* Apply scaling if specified and within valid range */
	if (image->scale != 100 && image->scale > 0 && image->scale < 1000) {
		frameW = image->w * image->scale / 100;
		frameH = image->h * image->scale / 100;
		EVE_Cmd_wr32(phost, BITMAP_SIZE(NEAREST, BORDER, BORDER, 
					  min(511, frameW), min(511, frameH)));
		EVE_Cmd_wr32(phost, BITMAP_SIZE_H(frameW >> 9, frameH >> 9));
	}
	
	return 0;
}

/**
 * @brief Load raw image data from SD card into EVE memory
 *
 * Loads uncompressed image data directly from SD card storage into EVE graphics memory.
 * This function handles memory allocation, file reading, and bitmap handle assignment
 * for raw image formats.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to image structure to populate
 * @return 0 on success, 1 if file not found/empty, error code otherwise
 */
static uint32_t utils_load_raw_from_sd_card(EVE_HalContext *phost, Image *image)
{
	uint32_t image_size, load_image_offset, result;

	/* Get file size from SD card */
	image_size = EVE_CoCmd_fsSize(phost, (char *)image->sd_path);
	EVE_Cmd_waitFlush(phost);
	if (image_size == (uint32_t)-1 || image_size == 0)
		return 1;

	/* Allocate aligned memory for image data */
	load_image_offset = utils_ddr_alloc_alignment(image_size, 32);
	
	/* Read image data from SD card to EVE memory */
	result = EVE_CoCmd_fsRead(phost, load_image_offset, (char *)image->sd_path);
	EVE_Cmd_waitFlush(phost);
	if (result)
		return result;

	/* Update image properties */
	image->ptr = load_image_offset;
	image->is_loaded = 1;
	
	/* Allocate bitmap handle if not specified */
	if (image->bitmap_handler == BMHL_AUTO)
		image->bitmap_handler = utils_bitmap_handler_get();
		
	/* Configure bitmap if valid handle obtained */
	if (IS_BITMAP_HANDLER_VALID(image->bitmap_handler)){
		/* Assign bitmap handle for this image */
		EVE_Cmd_wr32(phost, BITMAP_HANDLE((uint16_t)image->bitmap_handler));
		utils_set_image(phost, image, 0);
	}
		
	/* Allocate tag value */
	image->tagval = utils_tag_alloc(image->tagval);

	return 0;
}

/**
 * @brief Load compressed image from SD card with automatic format detection
 *
 * Loads and decompresses various image formats (JPEG, PNG, etc.) from SD card storage.
 * This function automatically detects image properties, allocates appropriate memory,
 * and configures the bitmap for rendering.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to image structure to populate
 * @return 0 on success, 1 if file empty, 3 if source setup failed
 */
uint32_t utils_load_image_from_sd_card(EVE_HalContext *phost, Image *image)
{
	uint32_t result, image_size, load_image_offset;
	uint32_t w, h, img_addr, palette, fmt;

	/* Handle raw format images separately */
	if (image->fmt == IMG_FMT_RAW)
		return utils_load_raw_from_sd_card(phost, image);

	/* Set up file source for compressed image loading */
	result = EVE_CoCmd_fsSource(phost, (char *)image->sd_path);
	EVE_Cmd_waitFlush(phost);
	if (result)
		return 3;

	/* Get file size */
	image_size = EVE_CoCmd_fsSize(phost, (char *)image->sd_path);
	EVE_Cmd_waitFlush(phost);
	if (image_size == (uint32_t)-1 || image_size == 0)
		return 1;

	/* Allocate memory and load compressed image */
	load_image_offset = utils_ddr_alloc(0);
	EVE_CoCmd_loadImage(phost, load_image_offset, 
			    image->opt | OPT_FS | OPT_NODL | OPT_DITHER);
	EVE_Cmd_waitFlush(phost);

	/* Retrieve image properties after decompression */
	w = h = img_addr = palette = fmt = 0;
	EVE_CoCmd_getProps(phost, &img_addr, &w, &h);
	if (w) image->w = w;
	if (h) image->h = h;
	image->ptr = img_addr ? img_addr : load_image_offset;
	
	/* Allocate additional memory for image data */
	utils_ddr_alloc(image->w * image->h * 2);
	EVE_Cmd_waitFlush(phost);

	/* Get detailed image format information */
	EVE_CoCmd_getImage(phost, &load_image_offset, &fmt, &w, &h, &palette);
	image->bitmap_format = fmt;
	EVE_Cmd_waitFlush(phost);

	/* Allocate bitmap handle and tag */
	if (image->bitmap_handler == BMHL_AUTO)
		image->bitmap_handler = utils_bitmap_handler_get();
	image->tagval = utils_tag_alloc(image->tagval);
	image->is_loaded = 1;

	/* Configure bitmap if valid handle obtained */
	if (IS_BITMAP_HANDLER_VALID(image->bitmap_handler)){
		/* Assign bitmap handle for this image */
		EVE_Cmd_wr32(phost, BITMAP_HANDLE((uint16_t)image->bitmap_handler));
		utils_set_image(phost, image, palette);
	}
		
	return 0;
}

/**
 * @brief Draw image at its default position
 *
 * Renders the specified image at its configured default position (image->x, image->y).
 * This is a convenience wrapper around utils_draw_image_xy().
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to loaded image structure
 * @return 0 on success, 1 if image not loaded
 */
uint32_t utils_draw_image(EVE_HalContext *phost, Image *image)
{
	return utils_draw_image_xy(phost, image, image->x, image->y);
}

/**
 * @brief Draw image centered within specified rectangle
 *
 * Centers the image within the given rectangular area. The image position is
 * calculated to place the image center at the center of the specified rectangle.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to loaded image structure
 * @param x Left edge of target rectangle
 * @param y Top edge of target rectangle
 * @param w Width of target rectangle
 * @param h Height of target rectangle
 * @return 0 on success, 1 if image not loaded
 */
uint32_t utils_draw_image_center(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	return utils_draw_image_xy(phost, image, x + w/2 - image->w/2, y + h/2 - image->h/2);
}

/**
 * @brief Draw image at specified position with optional touch tag
 *
 * Renders the image at the given coordinates and optionally assigns a touch tag
 * for user interaction detection. This is the core image rendering function.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to loaded image structure
 * @param x X coordinate for image placement
 * @param y Y coordinate for image placement
 * @param tag Touch tag value (0 for no tag)
 * @return 0 on success, 1 if image not loaded
 */
uint32_t utils_draw_image_xy_tag(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y, uint32_t tag)
{
	/* Verify image is loaded */
	if (!image->is_loaded)
		return 1;
	
	/* Set touch tag if specified */
	if (tag)
		EVE_Cmd_wr32(phost, TAG(tag));
	
	/* Begin bitmap rendering */
	EVE_Cmd_wr32(phost, BEGIN(BITMAPS));
	
	/* Configure bitmap handle or set image properties */
	if (IS_BITMAP_HANDLER_VALID(image->bitmap_handler)) {
		SET_BITMAP_HANDLE(phost, (uint16_t)image->bitmap_handler);
	} else {
		/* Save context for scaled images */
		if (image->scale > 100)
			EVE_Cmd_wr32(phost, SAVE_CONTEXT());
		
		EVE_Cmd_wr32(phost, BITMAP_HANDLE(0));
		utils_set_image(phost, image, 0);
		
		/* Restore context for scaled images */
		if (image->scale > 100)
			EVE_Cmd_wr32(phost, RESTORE_CONTEXT());
	}
	
	/* Render image at specified position */
	g_utils_adapter_vertex2f(phost, x, y);
	
	return 0;
}

/**
 * @brief Draw image at specified position using its default tag
 *
 * Renders the image at the given coordinates using the image's configured
 * tag value for touch interaction.
 *
 * @param phost Pointer to EVE HAL context
 * @param image Pointer to loaded image structure
 * @param x X coordinate for image placement
 * @param y Y coordinate for image placement
 * @return 0 on success, 1 if image not loaded
 */
uint32_t utils_draw_image_xy(EVE_HalContext *phost, Image *image, uint32_t x, uint32_t y)
{
	return utils_draw_image_xy_tag(phost, image, x, y, image->tagval);
}

/**
 * @brief Draw back navigation button at fixed position
 *
 * Renders the back button icon at a predefined location (560, 1800) using
 * the global PHOST context and ICON_BACKWARD image resource.
 */
void utils_draw_back_button(void)
{
	/* Draw back button icon at fixed position */
	utils_draw_image_xy(PHOST, ICON_BACKWARD, 560, 1800);
}