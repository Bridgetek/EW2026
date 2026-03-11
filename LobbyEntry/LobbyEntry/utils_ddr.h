/**
 * @file utils_ddr.h
 * @brief Header file for dynamic memory allocation utilities
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#ifndef UTILS_DDR_H
#define UTILS_DDR_H

#define EVE_MEMORY_ALIGNMENT	4

/**
 * struct memory_block - Memory block descriptor
 * @size: Size of the memory block
 * @is_free: Whether the block is free
 * @next: Pointer to next block in list
 */
struct memory_block {
	uint32_t size;
	bool is_free;
	struct memory_block *next;
};

/**
 * utils_ddr_init() - Initialize DDR memory management
 * @total_size: Total size of available DDR memory
 */
void utils_ddr_init(uint32_t total_size);

/**
 * utils_ddr_alloc() - Allocate DDR memory
 * @size: Size of memory to allocate
 *
 * Returns: Offset to allocated memory, or 0 on failure
 */
uint32_t utils_ddr_alloc(uint32_t size);

/**
 * utils_ddr_alloc_alignment() - Allocate aligned DDR memory
 * @size: Size of memory to allocate
 * @alignment: Required alignment
 *
 * Returns: Offset to allocated memory, or 0 on failure
 */
uint32_t utils_ddr_alloc_alignment(uint32_t size, uint32_t alignment);

#endif /* UTILS_DDR_H */