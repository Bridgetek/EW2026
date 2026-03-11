/**
 * @file app_page_directory.c
 * @brief Directory page implementation with scrollable office list and details dialog
 *
 * This module implements a directory page featuring a kinetic scrolling office list
 * and interactive details dialog. Provides smooth UI navigation with touch gesture
 * support, office selection, and information display functionality. Follows Linux
 * kernel coding style with comprehensive inline comments for maintainability.
 *
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

/* Standard C library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* Application includes */
#include "app_lobby_entry.h"
#include "utils_box.h"
#include "utils_color.h"
#include "utils_vertex_adapter.h"

/* Easing animation constants for smooth transitions */
#define k_linear 0
#define k_fast_in_slow_out 3
#define k_default_ui_easing 5
#define k_fast_out_slow_in 8
#define k_very_fast_easing 12

/* Maximum office entries in directory */
#define MAX_OFFICE_COUNT 30

/**
 * @struct OfficeInfo
 * @brief Structure containing office information for directory display
 */
typedef struct
{
	const char *where;		 /**< Office location identifier */
	const char *name;		 /**< Office company name */
	const char *description; /**< Detailed office description */
} OfficeInfo;

/**
 * @struct building_directory_t
 * @brief Container for all office directory entries
 */
typedef struct
{
	OfficeInfo office[MAX_OFFICE_COUNT]; /**< Array of office information */
	uint32_t count;						 /**< Number of offices in directory */
} building_directory_t;

/**
 * @struct content_div
 * @brief Content area division layout structure
 */
typedef struct
{
	app_box raw;		/**< Raw content area boundaries */
	int32_t margin_top; /**< Top margin spacing */
	app_box display;	/**< Display area boundaries */
} content_div;

/**
 * @struct theme_t
 * @brief Color theme configuration for UI elements
 */
typedef struct
{
	int32_t color_office_selected_bg;	/**< Selected office background color */
	int32_t color_office_selected_text; /**< Selected office text color */
	int32_t color_office_bg;			/**< Normal office background color */
	int32_t color_office_text;			/**< Normal office text color */
	int32_t color_office_info_bg;		/**< Office info dialog background */
	int32_t color_office_info_text;		/**< Office info dialog text color */
	int32_t color_office_title_text;	/**< Office title text color */
} theme_t;

/* Theme configurations */
static const theme_t theme_vintage = {
	.color_office_selected_bg = 0xC5D89D,
	.color_office_selected_text = 0x0D4715,
	.color_office_bg = 0xF6F0D7,
	.color_office_text = 0X89986D,
	.color_office_info_bg = 0xEBE1D1,
	.color_office_info_text = 0x0D4715,
	.color_office_title_text = 0x00FF00,
};

static const theme_t theme_light = {
	.color_office_selected_bg = 0xAEDEFC,
	.color_office_selected_text = 0xEDFFF0,
	.color_office_bg = 0xF9F8F6,
	.color_office_text = 0XC9B59C,
	.color_office_info_bg = 0xD9CFC7,
	.color_office_info_text = 0x8CA9FF,
	.color_office_title_text = 0x00FF00,
};

static const theme_t theme_normal = {
	.color_office_selected_bg = 0x3B9797,
	.color_office_selected_text = 0xEBD5AB,
	.color_office_bg = 0xC3C3C3,
	.color_office_text = 0xffffff,
	.color_office_info_bg = 0x7F7F7F,
	.color_office_info_text = 0xffffff,
	.color_office_title_text = 0x00FF00,
};

static theme_t theme = {0};

/* Layout elements */
static app_box body = {0};
static app_box header = {0};
static content_div content = {.margin_top = 3};
static app_box footer = {0};
static app_box office_info = {0};

/* Directory data */
static building_directory_t directory = {
	.office = {
		{"0101", "Aurora Solutions", "Providing end-to-end IT services including custom software development, system integration, and long-term technical support for enterprises."},
		{"0102", "BluePeak Consulting", "Specializing in business strategy, organizational transformation, and management consulting for growing companies and corporations."},
		{"0103", "NextGen Tech", "Focused on next-generation technologies, delivering innovative digital solutions and modern software architectures."},
		{"0104", "SilverLine Systems", "Designing and deploying enterprise-grade systems with high reliability, security, and performance optimization."},
		{"0105", "Vertex Innovation", "Research and development center dedicated to new products, emerging technologies, and rapid prototyping."},
		{"0201", "NovaCore Office", "Managing core platforms and infrastructure services that support mission-critical business operations."},
		{"0202", "Pioneer Digital", "Helping organizations drive digital transformation through technology consulting and implementation."},
		{"0203", "GreenWave Consulting", "Advising companies on sustainability strategies, ESG compliance, and environmentally responsible business practices."},
		{"0204", "Atlas Software", "Developing custom software solutions, cloud-native applications, and scalable enterprise platforms."},
		{"0205", "SkyBridge Solutions", "Providing IT outsourcing services, system maintenance, and cross-platform technical support."},
		{"0301", "Quantum Labs", "Advanced research laboratory exploring cutting-edge technologies and experimental computing models."},
		{"0302", "IronClad Security", "Delivering comprehensive cybersecurity services including risk assessment, penetration testing, and security operations."},
		{"0303", "CloudNest Systems", "Specialists in cloud infrastructure, DevOps automation, and highly available distributed systems."},
		{"0304", "BrightPath Consulting", "Offering strategic consulting services to improve operational efficiency and long-term business growth."},
		{"0305", "Zenith Technologies", "Building high-performance systems and technology solutions for data-intensive applications."},
		{"0401", "CoreLogic Office", "Providing data processing, analytics, and intelligent reporting solutions for enterprises."},
		{"0402", "Infinity Solutions", "Delivering end-to-end enterprise solutions covering consulting, development, deployment, and support."},
		{"0403", "Momentum Group", "Supporting businesses with growth strategies, market expansion, and organizational advisory services."},
		{"0404", "FusionWorks", "Integrating software, hardware, and automation systems into unified and efficient solutions."},
		{"0405", "Digital Harbor", "Developing digital products, online platforms, and customer-facing web and mobile applications."},
		{"0501", "PrimeTech Office", "Providing professional technology consulting and reliable software delivery services."},
		{"0502", "EagleEye Analytics", "Delivering advanced data analytics, dashboards, and business intelligence solutions."},
		{"0503", "SmartEdge Systems", "Developing IoT platforms and edge computing solutions for real-time data processing."},
		{"0504", "UrbanSoft", "Building enterprise software and modern mobile applications for urban and commercial use cases."},
		{"0505", "AlphaWave Tech", "Specializing in artificial intelligence, machine learning, and data-driven technology solutions."},
		{"0601", "NorthStar Consulting", "Providing corporate strategy, business planning, and executive advisory services."},
		{"0602", "HexaCode Studio", "Designing and developing high-quality software with a focus on clean architecture and usability."},
		{"0603", "BlueOrbit Systems", "Delivering network solutions, infrastructure design, and ongoing technical services."},
		{"0604", "RapidScale Tech", "Building scalable cloud platforms and SaaS solutions for fast-growing businesses."},
		{"0605", "VisionCraft Office", "Providing UI/UX design, digital branding, and user experience optimization services."},
	},
	.count = 30,
};

/* Font handles */
static uint32_t font_32 = 2;
static uint32_t font_33 = 3;
static uint32_t font_34 = 4;

/* Page state flags */
static bool is_page_directory_init = 0;
static bool is_page_directory_deinit = 0;

/* Touch interaction tags */
static uint32_t tag_start_office;
static uint32_t tag_end_office;
static int32_t tag_office_description = 0;
static int32_t tag_close_button = 0;

/* Scrolling physics */
static int32_t velocity_y;
static int32_t friction_y = 1;
static int32_t office_offset_y;
static int32_t bounce_back_y;
static int32_t bounce_back_y_saved;

/* Selection state */
static int32_t selected_office_index = -1;
static int32_t selected_office_count;
static int32_t hover_office_index = -1;

/**
 * @brief Initialize directory page layout and configuration
 *
 * Performs one-time initialization of directory page elements including:
 * - Theme selection and color scheme setup
 * - Layout area calculation and positioning
 * - Touch tag allocation for interactive elements
 * - Font handle assignment for text rendering
 */
static void page_directory_init_one(void)
{
	uint32_t margin_top, margin_lr, margin_bottom;
	uint32_t content_w, content_h;
	int i;

	if (is_page_directory_init == 1)
		return;

	is_page_directory_init = 1;

	/* Apply normal theme as default */
	theme = theme_normal;

	/* Calculate layout dimensions */
	margin_top = 480;
	margin_lr = 50;
	margin_bottom = 150;
	content_w = PHOST->Width - margin_lr * 2;
	content_h = PHOST->Height - margin_top - margin_bottom;

	/* Allocate touch tags for office entries */
	tag_start_office = utils_tag_next();
	for (i = 0; i < directory.count; i++)
		utils_tag_next();
	tag_end_office = tag_start_office + directory.count - 1;

	/* Allocate tags for dialog elements */
	tag_office_description = utils_tag_next();
	tag_close_button = utils_tag_next();

	/* Initialize layout areas */
	body = init_app_box(margin_lr, margin_top, content_w, content_h);
	header = init_app_box_child_top_left(content_w, body.h / 10, body);
	content.display = init_app_box_below(content_w, body.h * 9 / 10, header);
	footer = init_app_box_below(content_w, body.h * 0 / 10, content.display);
	content.raw = init_app_box_child_top_left(content_w, content.display.h / 10 - content.margin_top, content.display);
	office_info = init_app_box_child_center_middle(content.display.w * 6 / 10, content.display.h * 8 / 10, content.display);
}

/**
 * @brief Cleanup directory page resources and reset state
 *
 * Performs one-time deinitialization including font resource cleanup
 * and state flag reset. Called when leaving the directory page.
 */
static void page_directory_deinit_one(void)
{
	if (is_page_directory_deinit == 1)
		return;

	is_page_directory_deinit = 1;

	/* Load ROM fonts for text rendering */
	EVE_CoCmd_romFont(PHOST, font_32, 32);
	EVE_CoCmd_romFont(PHOST, font_33, 33);
	EVE_CoCmd_romFont(PHOST, font_34, 34);
}

/**
 * @brief Smooth value interpolation with configurable easing
 * @param a Current value
 * @param b Target value
 * @param k Easing factor (higher = slower)
 * @param max_step Maximum step size per frame
 * @return Next interpolated value
 *
 * Provides smooth animated transitions between values using configurable
 * easing curves. Used for scroll animations and UI transitions.
 */
static int32_t approach_i32_limit(int32_t a, int32_t b, int32_t k, int32_t max_step)
{
	int32_t delta = b - a;
	int32_t step, next;

	if (delta == 0)
		return b;

	step = delta / k;

	if (step == 0)
		step = (delta > 0) ? 1 : -1;

	if (step > max_step)
		step = max_step;
	if (step < -max_step)
		step = -max_step;

	next = a + step;

	if ((delta > 0 && next > b) || (delta < 0 && next < b))
		return b;

	return next;
}

/**
 * @brief Handle kinetic scrolling physics and boundary constraints
 * @return Always returns 0
 *
 * Implements smooth kinetic scrolling with:
 * - Velocity-based movement with distance divider for smooth motion
 * - Bounce-back physics at scroll boundaries
 * - Boundary clamping to prevent excessive scrolling
 * - Touch interaction detection to stop bouncing
 */
static int event_scrolling(void)
{
	const int32_t distance_devider = 30;
	Gesture_Touch_t *ges = utils_gesture_get();
	int32_t margin_y, limit_y_up, limit_y_down;

	/* Skip scrolling if office is selected or dialog is active */
	if (selected_office_index != -1)
		return 0;

	if (ges->tagPressed == tag_office_description || ges->tagPressed == tag_close_button)
		return 0;

	/* Calculate scrolling boundaries */
	margin_y = content.raw.h;
	limit_y_up = content.display.y_end - margin_y - (content.raw.h + content.margin_top) * directory.count;
	limit_y_down = content.display.y + margin_y;

	/* Handle upward boundary bounce-back */
	if (bounce_back_y == 0 && velocity_y >= 0 && office_offset_y > limit_y_down)
	{
		velocity_y = 0;
		if (bounce_back_y == 0 && office_offset_y != bounce_back_y)
		{
			bounce_back_y = limit_y_down - margin_y * 2;
		}
	}
	/* Handle downward boundary bounce-back */
	else if (bounce_back_y == 0 && velocity_y <= 0 && office_offset_y < limit_y_up)
	{
		velocity_y = 0;
		if (bounce_back_y == 0 && office_offset_y != bounce_back_y)
		{
			bounce_back_y = limit_y_up;
		}
	}
	/* Normal scrolling within boundaries */
	else
	{
		if (office_offset_y < limit_y_up - content.display.h / 2)
		{
			office_offset_y = limit_y_up - content.display.h / 2;
			velocity_y = 0;
			utils_scrolling_stop();
		}
		else if (office_offset_y > limit_y_down + content.display.h / 2)
		{
			office_offset_y = limit_y_down + content.display.h / 2;
			velocity_y = 0;
			utils_scrolling_stop();
		}
		else
		{
			office_offset_y += velocity_y / distance_devider;
		}
	}

	/* Update velocity from gesture */
	velocity_y = ges->velocityY;

	/* Cancel bounce-back on touch */
	if (ges->isTouch)
		bounce_back_y = 0;

	/* Apply bounce-back animation */
	if (bounce_back_y != 0)
	{
		office_offset_y = approach_i32_limit(office_offset_y,
											 bounce_back_y,
											 k_very_fast_easing, 30);
		if (office_offset_y == bounce_back_y)
			bounce_back_y = 0;
	}
	else
	{
		/* Enforce hard boundaries */
		if (office_offset_y < limit_y_up - content.display.h / 2)
		{
			office_offset_y = limit_y_up - content.display.h / 2;
			velocity_y = 0;
			utils_scrolling_stop();
		}
		else if (office_offset_y > limit_y_down + content.display.h / 2)
		{
			office_offset_y = limit_y_down + content.display.h / 2;
			velocity_y = 0;
			utils_scrolling_stop();
		}
		else
		{
			office_offset_y += velocity_y / distance_devider;
		}
	}

	return 0;
}

/**
 * @brief Handle office selection and interaction events
 * @return Always returns 0
 *
 * Manages office entry selection logic including:
 * - Double-tap detection for office selection
 * - Hover state management for visual feedback
 * - Dialog dismissal on outside touches
 * - Swipe gesture filtering to prevent accidental selection
 */
static int event_offices(void)
{
	Gesture_Touch_t *ges = utils_gesture_get();
	int32_t index;

	/* Skip if scrolling is active */
	if (velocity_y > 0)
		return 0;

	/* Handle office entry selection */
	if (ges->tagReleased >= tag_start_office &&
		ges->tagReleased <= tag_end_office)
	{
		/* Ensure it's a tap, not a swipe */
		if (!ges->isSwipe && abs(ges->distance) < 10)
		{
			index = ges->tagReleased - tag_start_office;

			if (hover_office_index != index)
			{
				/* First tap - set hover state */
				selected_office_count = 0;
				selected_office_index = -1;
				hover_office_index = index;
			}
			else
			{
				/* Subsequent tap on same office */
				selected_office_count++;
			}

			if (selected_office_count > 0)
			{
				/* Double-tap detected - show details */
				selected_office_index = index;
			}
			return 0;
		}
	}

	/* Handle dialog dismissal */
	if (selected_office_index != -1)
	{
		if (ges->tagReleased &&
			ges->tagReleased != tag_office_description &&
			ges->tagReleased != tag_close_button)
		{
			/* Touch outside dialog - close it */
			selected_office_index = -1;
			utils_scrolling_stop();
		}
		else if (ges->tagReleased == tag_close_button)
		{
			/* Close button pressed */
			selected_office_index = -1;
			utils_scrolling_stop();
		}
	}

	return 0;
}

/**
 * @brief Main event handler for directory page interactions
 * @return PAGE_FINISH to exit page, PAGE_CONTINUE to remain
 *
 * Central event dispatcher that coordinates:
 * - Back button navigation to return to main menu
 * - Scrolling physics and boundary management
 * - Office selection and detail dialog handling
 */
static int event(void)
{
	Gesture_Touch_t *ges = utils_gesture_get();

	/* Handle back button navigation */
	if (ges->tagReleased == ICON_BACKWARD->tagval)
	{
		g_is_menu_active = 1;
		return PAGE_FINISH;
	}

	/* Process scrolling and selection events */
	event_scrolling();
	event_offices();

	return PAGE_CONTINUE;
}

/**
 * @brief Render office information modal dialog
 * @param office Pointer to office information structure
 * @return Always returns 0
 *
 * Displays a modal dialog overlay containing detailed office information:
 * - Semi-transparent backdrop for modal effect
 * - Centered dialog box with office name and description
 * - Close button for dismissing the dialog
 * - Text wrapping and formatting for readability
 */
static int drawing_office_info_dialog(OfficeInfo *office)
{
	app_box close_button;

	/* Semi-transparent overlay backdrop */
	SET_COLOR_AQUA(PHOST);
	EVE_Cmd_wr32(PHOST, COLOR_A(50));
	EVE_Cmd_wr32(PHOST, TAG(tag_close_button));
	utils_draw_rects(PHOST, header.x, header.y,
			  header.w, header.h + content.display.h);

	/* Dialog box with border */
	EVE_Cmd_wr32(PHOST, COLOR_A(255));
	EVE_Cmd_wr32(PHOST, TAG(tag_office_description));
	draw_box_border_outside(PHOST, office_info, theme.color_office_info_bg,
							2, 0xFFFFFF, tag_office_description);

	/* Office name title */
	set_color_rgb(PHOST, theme.color_office_title_text);
	EVE_CoCmd_fillWidth(PHOST, office_info.w * 9 / 10);
	EVE_Cmd_wr32(PHOST, TAG(tag_office_description));
	EVE_CoCmd_text(PHOST, office_info.x + office_info.w / 2,
				   office_info.y + 200, font_33, OPT_FILL | OPT_CENTER, office->name);

	/* Office description text */
	set_color_rgb(PHOST, theme.color_office_info_text);
	EVE_CoCmd_fillWidth(PHOST, office_info.w * 9 / 10);
	EVE_Cmd_wr32(PHOST, TAG(tag_office_description));
	EVE_CoCmd_text(PHOST, office_info.x + office_info.w / 2,
				   office_info.y + office_info.h / 2 + 20,
				   font_32, OPT_FILL | OPT_CENTER, office->description);

	/* Close button */
	close_button = init_app_box_child_top_right(150, 50, office_info);
	move_box_to(&close_button, close_button.x - 10, close_button.y + 10);

	draw_box_border_outside(PHOST, close_button, 0xCCCCCC, 1, 0x000000, tag_close_button);
	set_color_rgb(PHOST, 0x000000);
	EVE_Cmd_wr32(PHOST, TAG(tag_close_button));
	EVE_CoCmd_text(PHOST, close_button.x_mid, close_button.y_mid, font_32, OPT_CENTER, "Close");

	return 0;
}

/**
 * @brief Render individual office list entry
 * @param office Pointer to office information structure
 * @param y_offset Vertical scroll offset
 * @param nth Office index in list
 * @param tag Touch tag for interaction
 * @return 0 on success, -1 if office is outside visible area
 *
 * Renders a single office entry in the scrollable list:
 * - Calculates position based on scroll offset and index
 * - Applies visual highlighting for hover/selection states
 * - Displays office name and location information
 * - Returns early if entry is outside visible scroll area
 */
static int drawing_office_line(OfficeInfo *office, uint32_t y_offset, uint32_t nth, uint32_t tag)
{
	int office_box_x, office_box_y;

	office_box_x = content.display.x;
	office_box_y = y_offset + (content.raw.h + content.margin_top) * (nth + 1);

	/* Skip rendering if outside visible area */
	if (office_box_y > content.display.y_end)
		return -1;
	else if (office_box_y + content.raw.h < content.display.y)
		return -1;

	/* Apply theme colors based on selection state */
	if (nth == hover_office_index || nth == selected_office_index)
	{
		set_color_rgb(PHOST, theme.color_office_selected_bg);
	}
	else
	{
		set_color_rgb(PHOST, theme.color_office_bg);
	}
	EVE_Cmd_wr32(PHOST, TAG(tag));
	draw_box_at(PHOST, content.raw, office_box_x, office_box_y);

	/* Render office text with appropriate colors */
	if (nth == hover_office_index || nth == selected_office_index)
	{
		set_color_rgb(PHOST, theme.color_office_selected_text);
	}
	else
	{
		set_color_rgb(PHOST, theme.color_office_text);
	}
	EVE_Cmd_wr32(PHOST, TAG(tag));
	EVE_CoCmd_text(PHOST, office_box_x + 10, office_box_y + content.raw.h / 2,
				   font_32, OPT_CENTERY, office->name);

	EVE_CoCmd_text(PHOST, office_box_x + content.raw.w - 10,
				   office_box_y + content.raw.h / 2,
				   font_32, OPT_RIGHTX, office->where);

	return 0;
}

/**
 * @brief Render all office entries in scrollable list
 * @param y_offset Current scroll position offset
 * @return Always returns 0
 *
 * Iterates through all office entries and renders visible ones:
 * - Sets up clipping region for smooth scrolling
 * - Renders each office entry with proper positioning
 * - Restores clipping region after rendering
 */
static int drawing_all_office_box(uint32_t y_offset)
{
	uint32_t i;

	/* Set clipping region for content area */
	EVE_CoDl_scissorXY(PHOST, content.display.x, content.display.y);
	EVE_CoDl_scissorSize(PHOST, content.display.w, content.display.h);

	/* Render all office entries */
	for (i = 0; i < directory.count; i++)
	{
		drawing_office_line(&directory.office[i], y_offset, i,
							tag_start_office + i);
	}

	/* Reset clipping region */
	EVE_CoDl_scissorXY(PHOST, 0, 0);
	EVE_CoDl_scissorSize(PHOST, 2048, 2048);
	return 0;
}

/**
 * @brief Render main content area including header and office list
 * @return Always returns 0
 *
 * Coordinates rendering of the complete content area:
 * - Header with "Building directory" title
 * - Scrollable office list with current offset
 * - Office information dialog (if active)
 */
static int drawing_content(void)
{
	/* Render header with title */
	set_color_rgb(PHOST, 0x5682B1);
	draw_box_text_center_middle(PHOST, header, "Building directory",
								font_32, OPT_CENTER, 0xFFFFFF);

	/* Render content background */
	SET_COLOR_GRAY(PHOST);
	draw_box(PHOST, content.display);

	/* Render scrollable office list */
	drawing_all_office_box(office_offset_y);

	/* Render office info dialog if active */
	if (selected_office_index > -1 &&
		selected_office_index < directory.count)
		drawing_office_info_dialog(&directory.office[selected_office_index]);

	SET_COLOR_WHITE(PHOST);
	return 0;
}

/**
 * @brief Main drawing function for directory page
 * @return Always returns 0
 *
 * Coordinates all visual elements rendering:
 * - Background image
 * - Content area and office list
 * - Navigation back button
 */
static int drawing(void)
{
	/* Render background image */
	utils_draw_image_xy_tag(PHOST, ICON_BACKGROUND, 0, 0, 10);

	/* Render main content */
	drawing_content();

	/* Render back button for navigation */
	utils_draw_back_button();

	return 0;
}

/**
 * @brief Initialize directory page for first use
 *
 * Public initialization function that sets up the directory page
 * and resets scroll position to top of list.
 */
void page_directory_init(void)
{
	page_directory_init_one();
	office_offset_y = 0;
	selected_office_index = -1;
	selected_office_count = 0;
	hover_office_index = -1;
}

/**
 * @brief Cleanup directory page resources
 *
 * Public cleanup function that performs resource deallocation
 * when exiting the directory page.
 */
void page_directory_deinit(void)
{
	page_directory_deinit_one();
}

/**
 * @brief Main draw and event handling function
 * @return PAGE_CONTINUE to stay on page, PAGE_FINISH to exit
 *
 * Primary function called each frame to handle events and rendering.
 * Processes user input, updates animations, and renders the page.
 */
int page_directory_draw(void)
{
	int ret = event();
	drawing();
	return ret;
}

/**
 * @brief Load directory page resources
 * @return Always returns 0
 *
 * Placeholder function for future resource loading implementation.
 */
int page_directory_load(void)
{
	return 0;
}
