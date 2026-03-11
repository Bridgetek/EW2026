/**
 * @file app_page_help.c
 * @brief Help page implementation for lobby entry application
 *
 * This module implements the help page that displays usage instructions
 * for the lobby entry system features including QR code scanner,
 * directory viewer, and dialer functionality.
 *
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_lobby_entry.h"

/* External variables */
extern bool g_is_menu_active;

/* Static variables for page state tracking */
static bool g_is_page_help_init = false;
static bool g_is_page_help_deinit = false;

/**
 * @brief Initialize help page resources (one-time only)
 *
 * Performs one-time initialization of help page resources.
 * Uses static flag to ensure initialization occurs only once.
 */
static void page_help_init_one(void)
{
	if (g_is_page_help_init)
		return;

	/* Perform one-time help page initialization */
	g_is_page_help_init = true;
}
    
/**
 * @brief Deinitialize help page resources (one-time only)
 *
 * Performs one-time cleanup of help page resources.
 * Uses static flag to ensure cleanup occurs only once.
 */
static void page_help_deinit_one(void)
{
	if (g_is_page_help_deinit)
		return;

	/* Perform one-time help page cleanup */
	g_is_page_help_deinit = true;
}

/**
 * @brief Handle touch events for help page
 *
 * Processes touch events and gesture input for the help page.
 * Handles back button navigation to return to main menu.
 *
 * @return PAGE_FINISH if page should exit, PAGE_CONTINUE otherwise
 */
static int handle_events(void)
{
	Gesture_Touch_t *gesture;

	gesture = utils_gesture_get();

	/* Check for back button press */
	if (gesture->tagReleased == ICON_BACKWARD->tagval) {
		/* Return to main menu */
		g_is_menu_active = true;
		return PAGE_FINISH;
	}

	return PAGE_CONTINUE;
}

/**
 * @brief Draw help page content
 *
 * Renders the help page UI elements including feature icons
 * and descriptive text for each lobby entry system feature.
 *
 * @return 0 on success
 */
static int draw_page_content(void)
{
	/* Draw main background (no tag for help page) */
	utils_draw_image_xy_tag(PHOST, ICON_BACKGROUND, 0, 0, 0);

	/* Configure text fill width for descriptions */
	EVE_CoCmd_fillWidth(PHOST, 800);

	/* QR Code Scanner section */
	utils_draw_image_xy_tag(PHOST, ICON_QR_CODE_SCANNER, 100, 600, 0);
	EVE_CoCmd_text(PHOST, 380, 680, 32, OPT_FILL,
		       "QR Code Scanner to access the building");

	/* Directory Viewer section */
	utils_draw_image_xy_tag(PHOST, ICON_DIRECTORY, 100, 1000, 0);
	EVE_CoCmd_text(PHOST, 380, 1080, 32, OPT_FILL,
		       "Directory Viewer to find tenant information");

	/* Dialer section */
	utils_draw_image_xy_tag(PHOST, ICON_DIALER, 100, 1400, 0);
	EVE_CoCmd_text(PHOST, 380, 1500, 32, OPT_FILL,
		       "Dialer to contact the tenant");

	/* Draw navigation back button */
	utils_draw_back_button();

	return 0;
}

/**
 * @brief Initialize help page
 *
 * Public interface function to initialize the help page.
 * Calls internal one-time initialization routine.
 */
void page_help_init(void)
{
	page_help_init_one();
}

/**
 * @brief Deinitialize help page
 *
 * Public interface function to clean up help page resources.
 * Calls internal one-time deinitialization routine.
 */
void page_help_deinit(void)
{
	page_help_deinit_one();
}

/**
 * @brief Main help page drawing and event handling
 *
 * Processes events and renders the help page content.
 * This function is called continuously while the help page is active.
 *
 * @return PAGE_FINISH if page should exit, PAGE_CONTINUE otherwise
 */
int page_help_draw(void)
{
	int result;

	/* Handle touch events and user input */
	result = handle_events();

	/* Draw page content */
	draw_page_content();

	return result;
}

/**
 * @brief Load help page resources
 *
 * Loads any necessary resources for the help page.
 * Currently no resources need to be loaded.
 *
 * @return 0 on success
 */
int page_help_load(void)
{
	/* No resources to load for help page */
	return 0;
}
