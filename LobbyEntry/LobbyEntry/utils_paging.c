/**
 * @file utils_paging.c
 * @brief Paging module for handling page transitions and logic
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include "eve.h"
#include "utils_paging.h"

void utils_page_init(utils_paging *p) {}

/**
 * @brief Run a single page's logic
 * @param p Page to run
 *
 * If the page is active, run its draw function. If the draw
 * function returns PAGE_FINISH, mark the page as inactive and
 * uninitialized, and call its deinit function. If the page has a
 * next page, mark it as active.
 */
void utils_page_run_one(utils_paging *p)
{
	if (!p->is_active)
		return;

	if (!p->is_initialized) {
		p->is_initialized = true;
		p->init();
	}

	if (PAGE_FINISH == p->draw()) {
		p->is_active = false;
		p->is_initialized = false;
		p->deinit();

		if (p->next_page)
			p->next_page->is_active = true;
	}
}

/**
 * @brief Load all pages' resources
 * @param pages Array of pages to load
 * @param count Number of pages in the array
 *
 * Iterate through all pages in the array and call their load
 * function if it is not NULL. This function is used to preload all
 * pages' resources before the application's main loop.
 */
void utils_page_load(utils_paging **pages, int count)
{
	for (int i = 0; i < count; i++) {
		if (pages[i] && pages[i]->load)
			pages[i]->load();
	}
}

/**
 * @brief Iterate through all pages in the array and run them
 * @param pages Array of pages to run
 * @param count Number of pages in the array
 *
 * Iterate through all pages in the array and call utils_page_run_one
 * on each page. This function is used to drive the application's main
 * loop and keep all pages responsive.
 */
void utils_page_run(utils_paging **pages, int count)
{
	for (int i = 0; i < count; i++) {
		if (pages[i])
			utils_page_run_one(pages[i]);
	}
}

