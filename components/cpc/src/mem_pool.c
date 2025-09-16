/**
 * @file mem_pool.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mem_pool.h"
#include "cpc_api.h"

#include <stddef.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#define MEM_POOL_OUT_OF_MEMORY 0xFFFFFFFF
#define MEM_POOL_REQUIRED_PADDING(obj_size)                                    \
    (((sizeof(size_t) - ((obj_size) % sizeof(size_t))) % sizeof(size_t)))

/***************************************************************************/ /**
 * Creates a memory pool
 ******************************************************************************/
void mem_pool_create(mem_pool_handle_t* mem_pool, uint32_t block_size,
                     uint32_t block_count, void* buffer, uint32_t buffer_size) {
    configASSERT(mem_pool != NULL);
    configASSERT(buffer != NULL);
    configASSERT(block_count != 0);
    configASSERT(block_size != 0);
    configASSERT(buffer_size
                 >= block_count
                        * (block_size + MEM_POOL_REQUIRED_PADDING(block_size)));

    mem_pool->block_size = block_size + MEM_POOL_REQUIRED_PADDING(block_size);
    mem_pool->block_count = block_count;
    mem_pool->data = buffer;
    mem_pool->free_block_addr = mem_pool->data;

    uint32_t block_addr = (uint32_t)mem_pool->data;

    // Populate the list of free blocks (except last block)
    for (uint32_t i = 0; i < (block_count - 1); i++) {
        *(uint32_t*)block_addr = (uint32_t)(block_addr + mem_pool->block_size);
        block_addr += mem_pool->block_size;
    }

    // Last element will indicate OOM
    *(uint32_t*)block_addr = MEM_POOL_OUT_OF_MEMORY;
}

/***************************************************************************/ /**
 * Allocates an object from a memory pool
 ******************************************************************************/
void* mem_pool_alloc(mem_pool_handle_t* mem_pool) {
    configASSERT(mem_pool != NULL);
    MCU_ENTER_CRITICAL();
    if ((uint32_t)mem_pool->free_block_addr == MEM_POOL_OUT_OF_MEMORY) {
        MCU_EXIT_CRITICAL();
        return NULL;
    }
    // Get the next free block
    void* block_addr = mem_pool->free_block_addr;
    // Update the next free block using the address saved in that block
    mem_pool->free_block_addr = (void*)*(uint32_t*)block_addr;
    MCU_EXIT_CRITICAL();
    return block_addr;
}

/***************************************************************************/ /**
 * Frees an object previously allocated to a memory pool.
 ******************************************************************************/
void mem_pool_free(mem_pool_handle_t* mem_pool, void* block) {
    configASSERT(mem_pool != NULL);
    // Validate that the provided address is in the buffer range
    configASSERT((block >= mem_pool->data)
                 && ((uint32_t)block
                     <= ((uint32_t)mem_pool->data
                         + (mem_pool->block_size * mem_pool->block_count))));
    MCU_ATOMIC_SECTION({
        // Save the current free block addr in this block
        *(uint32_t*)block = (uint32_t)mem_pool->free_block_addr;
        mem_pool->free_block_addr = block;
    });
}
