/**
 * @file mem_pool.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DOXYGEN
/***************************************************************************/ /**
 * @addtogroup mempool Memory Pool
 * @brief Memory Pool
 * @{
 ******************************************************************************/

/// @brief Memory Pool Handle
#endif // DOXYGEN
typedef struct mem_pool_handle{
  void* free_block_addr;   ///< Pointer to head of free block list.
  void* data;              ///< Pointer to buffer.
  uint16_t block_size;     ///< Size of the blocks.
  uint16_t block_count;    ///< Total number of blocks in pool.
} mem_pool_handle_t;

#define MEM_POOL_PAD_OBJECT_SIZE(block_size) (block_size + sizeof(uint32_t) - 1)

#define MEM_POOL_BUFFER_SIZE(block_size, block_count) (block_count                                \
                                                           * MEM_POOL_PAD_OBJECT_SIZE(block_size) \
                                                           / sizeof(uint32_t))

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Helper macro to create a memory pool buffer.
 *
 * @param[in] mempool_name  Name of memory pool.
 * @param[in] block_size  Size of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 ******************************************************************************/
#endif // DOXYGEN
#define MEM_POOL_DECLARE_BUFFER(mempool_name, block_size, block_count) \
  static uint32_t mempool_name##_buffer[MEM_POOL_BUFFER_SIZE(block_size, block_count)]

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Helper macro to create a memory pool buffer by providing the block type.
 *
 * @param[in] mempool_name  Name of memory pool.
 * @param[in] block_type  Type of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 ******************************************************************************/
#endif // DOXYGEN
#define MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_name, block_type, block_count) \
  static struct {                                                                    \
    union {                                                                          \
      void *ptr;                                                                     \
      block_type entry;                                                              \
    };                                                                               \
  } mempool_name##_buffer[block_count]

#ifndef DOXYGEN
// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * Creates a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 * @param[in] block_size  Size of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 * @param[in] buffer  Pointer to buffer to use as pool storage.
 * @param[in] buffer_size  Size of the buffer to use as pool storage.
 ******************************************************************************/
#endif // DOXYGEN
void mem_pool_create(mem_pool_handle_t *mem_pool,
                         uint32_t block_size,
                         uint32_t block_count,
                         void* buffer,
                         uint32_t buffer_size);

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Allocates an object from a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 *
 * @return Pointer to allocated block. Null if pool is empty.
 ******************************************************************************/
#endif // DOXYGEN
void* mem_pool_alloc(mem_pool_handle_t *mem_pool);

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Frees an object previously allocated to a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 * @param[in] block  Pointer to block to free.
 ******************************************************************************/
#endif // DOXYGEN
void mem_pool_free(mem_pool_handle_t *mem_pool, void *block);

#ifndef DOXYGEN
/** @} (end addtogroup mempool) */
#endif // DOXYGEN

#ifdef __cplusplus
}
#endif

#endif // MEM_POOL_H
