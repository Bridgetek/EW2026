/**
 * @file utils_ddr.c
 * @brief Dynamic memory allocation for EVE using DDR memory
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "utils_ddr.h"

/* In the RAM_G address space, BT82X allocates the top 2.5MB for DDR */
static const uint32_t ddr_top_ram = 2621440;
static uint32_t ddr_offset = 2* 1024*1024; // reserve 2MB for playVideo()
static uint32_t ddr_size = 128 * 1024 * 1024; /* 128 MB default */

/**
 * utils_ddr_init() - Initialize DDR memory management
 * @total_size: Total size of available DDR memory in bytes
 *
 * Sets up the DDR memory allocator with the specified total size.
 * Minimum size must be at least EVE_MEMORY_ALIGNMENT bytes.
 */
void utils_ddr_init(uint32_t total_size)
{
	if (total_size < EVE_MEMORY_ALIGNMENT) {
		printf("DDR size is too small\n");
		return;
	}
	ddr_size = total_size;
}

/**
 * utils_ddr_alloc_alignment() - Allocate aligned DDR memory
 * @size: Size of memory to allocate in bytes
 * @alignment: Required alignment in bytes
 *
 * Allocates DDR memory with specified alignment requirement.
 *
 * Returns: Offset to allocated memory, or 0 on failure
 */
uint32_t utils_ddr_alloc_alignment(uint32_t size, uint32_t alignment)
{
	ddr_offset = (ddr_offset + alignment - 1) & ~(alignment - 1);
	return utils_ddr_alloc(size);
}

/**
 * utils_ddr_alloc() - Allocate DDR memory  
 * @size: Size of memory to allocate in bytes
 *
 * Allocates DDR memory aligned to EVE_MEMORY_ALIGNMENT.
 * If size is 0, returns current offset without allocating.
 *
 * Returns: Offset to allocated memory, or 0 on failure
 */
uint32_t utils_ddr_alloc(uint32_t size)
{
	uint32_t start_addr = 0;

	if (size == 0)
		return ddr_offset;
	
	/* Align size to EVE_MEMORY_ALIGNMENT bytes */
	size = (size + EVE_MEMORY_ALIGNMENT - 1) & ~(EVE_MEMORY_ALIGNMENT - 1);
	
	start_addr = ddr_offset;
	if (ddr_offset + size > (ddr_size - ddr_top_ram)) {
		printf("Not enough DDR memory\n");
		return 0;
	}
	
	ddr_offset += size;
	return start_addr;
}
