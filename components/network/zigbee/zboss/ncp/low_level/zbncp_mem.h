/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE: Genereic memory-related helpers.
*/
#ifndef ZBNCP_INCLUDE_GUARD_MEM_H
#define ZBNCP_INCLUDE_GUARD_MEM_H 1

#include "zbncp_types.h"
#include "zbncp_utils.h"

#if ZBNCP_USE_STDMEM

#include <string.h>

#ifndef ZBNCP_NULL
/** @bref Defenition of the null pointer. */
#define ZBNCP_NULL  NULL
#endif /* ZBNCP_NULL */

/**
 * @brief Memory block copy.
 *
 * Wrapper around standard library memcpy().
 *
 * @param dest - pointer to the destination memory buffer
 * @param src  - constant pointer to the source memory buffer
 * @param size - count of the bytes to copy
 *
 * @return nothing
 */
static inline void zbncp_mem_copy(void *dest, const void *src, zbncp_size_t size)
{
  (void) memcpy(dest, src, (size_t)size);
}

/**
 * @brief Memory block move.
 *
 * Wrapper around standard library memmove().
 *
 * @param dest - pointer to the destination memory buffer
 * @param src  - constant pointer to the source memory buffer
 * @param size - count of the bytes to move
 *
 * @return nothing
 */
static inline void zbncp_mem_move(void *dest, const void *src, zbncp_size_t size)
{
  (void) memcpy(dest, src, (size_t)size);
}

/**
 * @brief Memory block fill with value.
 *
 * Wrapper around standard library memset().
 *
 * @param mem  - pointer to the destination memory buffer
 * @param size - count of the bytes to fill
 * @param val  - value to fill memory with
 *
 * @return nothing
 */
static inline void zbncp_mem_fill(void *mem, zbncp_size_t size, zbncp_uint8_t val)
{
  (void) memset(mem, (int)(unsigned)val, (size_t)size);
}

#else

#ifndef ZBNCP_NULL
/** @bref Defenition of the null pointer. */
#define ZBNCP_NULL  ((void *) 0)
#endif /* ZBNCP_NULL */

/**
 * @brief Custom implementation of memory block copy.
 *
 * The function has undefined behaviour if the memory regions overlap.
 *
 * @param dest - pointer to the destination memory buffer
 * @param src  - constant pointer to the source memory buffer
 * @param size - count of the bytes to copy
 *
 * @return nothing
 */
void zbncp_mem_copy(void *dest, const void *src, zbncp_size_t size);

/**
 * @brief Custom implementation of memory block move.
 *
 * The function properly handles the overlapping memory regions.
 *
 * @param dest - pointer to the destination memory buffer
 * @param src  - constant pointer to the source memory buffer
 * @param size - count of the bytes to move
 *
 * @return nothing
 */
void zbncp_mem_move(void *dest, const void *src, zbncp_size_t size);

/**
 * @brief Custom implementation of memory block fill with value.
 *
 * @param mem  - pointer to the destination memory buffer
 * @param size - count of the bytes to fill
 * @param val  - value to fill memory with
 *
 * @return nothing
 */
void zbncp_mem_fill(void *mem, zbncp_size_t size, zbncp_uint8_t val);

#endif

/**
 * @brief Memory block fill with zero value.
 *
 * @param mem  - pointer to the destination memory buffer
 * @param size - count of the bytes to fill
 *
 * @return nothing
 */
static inline void zbncp_mem_zero(void *mem, zbncp_size_t size)
{
  zbncp_mem_fill(mem, size, 0u);
}

/**
 * @brief Offset any pointer value by an 'offset' bytes.
 *
 * Helper function for accessing individual bytes in any memory buffer.
 *
 * @param mem  - pointer to the memory buffer
 * @param size - count of bytes to offset the pointer by
 *
 * @return offset pointer
 */
static inline char *zbncp_offset_ptr(void *mem, zbncp_size_t offset)
{
  char *ptr = (char *) mem;
  return &ptr[offset];
}

/**
 * @brief Memory reference
 *
 * This structure enables to handle pointer to a memory buffer and its
 * size together, so it is easy to check buffer's boundaries.
 */
typedef struct zbncp_memref_s
{
  void *ptr;            /**< Pointer to the memory buffer */
  zbncp_size_t size;    /**< Size of the memory buffer */
}
zbncp_memref_t;

/**
 * @brief Helper function to avoid IAR C-STAT checker false-positive warning.
 *
 * IAR C-STAT checker incorrectly detects that mem.ptr and mem.size fields
 * are assigned but not used. So we need some way to tell the compiler that
 * they are actually used (because we return the whole structure). Pass this
 * two fields to a dummy function which denotes its arguments as unused.
 *
 * @param ptr - pointer to the memory buffer
 * @param size - size of the memory buffer
 *
 * @return nothing
 */
static inline void zbncp_mem_unused(const void *ptr, zbncp_size_t size)
{
  ZBNCP_UNUSED(ptr);
  ZBNCP_UNUSED(size);
}

/**
 * @brief Memory reference constructor.
 *
 * @param ptr - pointer to the memory buffer
 * @param size - size of the memory buffer
 *
 * @return constructed memory reference
 */
static inline zbncp_memref_t zbncp_make_memref(void *ptr, zbncp_size_t size)
{
  zbncp_memref_t mem;
  mem.ptr = ptr;
  mem.size = size;
  zbncp_mem_unused(mem.ptr, mem.size);
  return mem;
}

/**
 * @brief NULL memory reference constructor.
 *
 * @return NULL memory reference
 */
static inline zbncp_memref_t zbncp_memref_null(void)
{
  return zbncp_make_memref(ZBNCP_NULL, 0u);
}

/**
 * @brief Predicate to check if memory reference is NULL.
 *
 * @param mem - memory reference to be checked
 *
 * @return true if memory reference is NULL, false otherwise
 */
static inline zbncp_bool_t zbncp_memref_is_null(zbncp_memref_t mem)
{
  return (mem.ptr == ZBNCP_NULL);
}

/**
 * @brief Predicate to check if memory reference is of zero size.
 *
 * @param mem - memory reference to be checked
 *
 * @return true if memory reference is zero-sized, false otherwise
 */
static inline zbncp_bool_t zbncp_memref_is_empty(zbncp_memref_t mem)
{
  return (mem.size == 0u);
}

/**
 * @brief Predicate to check if memory reference is not NULL and has non-zero size.
 *
 * @param mem - memory reference to be checked
 *
 * @return true if memory reference is valid, false otherwise
 */
static inline zbncp_bool_t zbncp_memref_is_valid(zbncp_memref_t mem)
{
  return (!zbncp_memref_is_null(mem) && !zbncp_memref_is_empty(mem));
}

/**
 * @brief Obtain a pointer to a byte inside a memory buffer referenced by the memory reference.
 *
 * Function does boundary checking and return NULL if an offset is out of boundaries.
 *
 * @param mem  - memory reference
 * @param size - count of bytes to offset the pointer by
 *
 * @return pointer to a byte if the offset is in the boundaries of the buffer, NULL otherwise
 */
static inline char *zbncp_memref_at(zbncp_memref_t mem, zbncp_size_t offset)
{
  return ((offset < mem.size) ? zbncp_offset_ptr(mem.ptr, offset) : ZBNCP_NULL);
}

/**
 * @brief Make memory refernce to reference the same buffer but of a smaller size.
 *
 * Function does boundary checking and does not allow to shrink more than the buffer size.
 *
 * @param mem  - pointer to a memory reference object
 * @param size - count of bytes to decrease the size by
 *
 * @return new size of the referenced memory
 */
static inline zbncp_size_t zbncp_memref_shrink(zbncp_memref_t *mem, zbncp_size_t size)
{
  zbncp_size_t delta = size_min(size, mem->size);
  mem->size -= delta;
  return delta;
}

/**
 * @brief Make memory refernce to reference the end part of the buffer of a smaller size.
 *
 * Function does boundary checking and does not allow to advance more than the buffer size.
 *
 * @param mem  - pointer to a memory reference object
 * @param size - count of bytes to offset the pointer by
 *
 * @return nothing
 */
static inline void zbncp_memref_advance(zbncp_memref_t *mem, zbncp_size_t offset)
{
  zbncp_size_t delta = zbncp_memref_shrink(mem, offset);
  mem->ptr = zbncp_offset_ptr(mem->ptr, delta);  
}

/**
 * @brief Constatnt memory reference
 *
 * This structure enables to handle constant pointer to a memory buffer
 * and its size together, so it is easy to check buffer's boundaries.
 */
typedef struct zbncp_cmemref_s
{
  const void *ptr;      /**< Constant pointer to the memory buffer */
  zbncp_size_t size;    /**< Size of the memory buffer */
}
zbncp_cmemref_t;

/**
 * @brief Constant memory reference constructor.
 *
 * @param ptr - constant pointer to the memory buffer
 * @param size - size of the memory buffer
 *
 * @return constructed constant memory reference
 */
static inline zbncp_cmemref_t zbncp_make_cmemref(const void *ptr, zbncp_size_t size)
{
  zbncp_cmemref_t mem;
  mem.ptr = ptr;
  mem.size = size;
  zbncp_mem_unused(mem.ptr, mem.size);
  return mem;
}

/**
 * @brief NULL constant memory reference constructor.
 *
 * @return NULL constant memory reference
 */
static inline zbncp_cmemref_t zbncp_cmemref_null(void)
{
  return zbncp_make_cmemref(ZBNCP_NULL, 0u);
}

/**
 * @brief Predicate to check if constant memory reference is NULL.
 *
 * @param mem - constant memory reference to be checked
 *
 * @return true if constant memory reference is NULL, false otherwise
 */
static inline zbncp_bool_t zbncp_cmemref_is_null(zbncp_cmemref_t mem)
{
  return (mem.ptr == ZBNCP_NULL);
}

/**
 * @brief Predicate to check if constant memory reference is of zero size.
 *
 * @param mem - constant memory reference to be checked
 *
 * @return true if constant memory reference is zero-sized, false otherwise
 */
static inline zbncp_bool_t zbncp_cmemref_is_empty(zbncp_cmemref_t mem)
{
  return (mem.size == 0u);
}

/**
 * @brief Predicate to check if constant memory reference is not NULL
 * and has non-zero size.
 *
 * @param mem - constant memory reference to be checked
 *
 * @return true if constant memory reference is valid, false otherwise
 */
static inline zbncp_bool_t zbncp_cmemref_is_valid(zbncp_cmemref_t mem)
{
  return (!zbncp_cmemref_is_null(mem) && !zbncp_cmemref_is_empty(mem));
}

#endif /* ZBNCP_INCLUDE_GUARD_MEM_H */
