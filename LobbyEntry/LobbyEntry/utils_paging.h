/**
 * @file utils_paging.h
 * @brief Page management utility for the application
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_PAGING_H_
#define UTILS_PAGING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_FINISH		1
#define PAGE_CONTINUE		0
 
#define PAGE(x, display_name)                      \
    void page_##x##_load(void);                    \
    void page_##x##_init(void);                    \
    void page_##x##_deinit(void);                  \
    int page_##x##_draw(void);                     \
    utils_paging page_##x = {                      \
        .name = "page_" #x,                        \
        .ui_name = display_name,                   \
        .load = page_##x##_load,                   \
        .init = page_##x##_init,                   \
        .deinit = page_##x##_deinit,               \
        .draw = page_##x##_draw,                   \
        .is_active = 0,                            \
        .is_initialized = 0,                       \
        .next_page = 0                             \
    }

typedef struct utils_paging_t {
	char name[100];
	char ui_name[100];
	int (*load)(void);
	void (*init)(void);
	void (*deinit)(void);
	int (*draw)(void);
	bool is_active;
	bool is_initialized;
	struct utils_paging_t *next_page;
} utils_paging;

void utils_page_init(utils_paging *p);
void utils_page_load(utils_paging **pages, int count);
void utils_page_run(utils_paging **pages, int count);
void utils_page_run_one(utils_paging *p);

#endif /* UTILS_PAGING_H_ */