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
/*  PURPOSE: Genereic FIFO indexing implementation.
*/
#ifndef ZBNCP_INCLUDE_GUARD_FIFO_H
#define ZBNCP_INCLUDE_GUARD_FIFO_H 1

#include "zbncp_defs.h"
#include "zbncp_types.h"
#include "zbncp_debug.h"

/** @brief Integer type used as FIFO index. */
#if ZBNCP_USE_U8_FIFO_INDEX
typedef zbncp_uint8_t zbncp_fifo_size_t;
#else /* ZBNCP_USE_U8_FIFO_INDEX */
typedef zbncp_size_t zbncp_fifo_size_t;
#endif /* ZBNCP_USE_U8_FIFO_INDEX */

/**
 * @brief Constant describing the maximum possible count of the FIFO entries.
 *
 * This implementation does not wrap the head and the tail indices
 * on enqueue/dequeue operations but instead it continuously
 * increments them and do wrapping only on head/tail accessing.
 * This enables us to always know the current count of stored
 * elements by subtracting the head index from the tail index.
 * This operation works correctly even in the case of integer overflow
 * (i.e. when the actual value of the tail index is larger than the
 * value of the head index) but only if the difference is less than
 * the half of the maximum value that can be stores in the index type.
 *
 * NB! To get the half of the maximum value and avoid an overflow we
 * need to caclulate it using the formula 2 ^ (SIZE-OF-TYPE-IN-BITS - 1)
 * instead of (2 ^ SIZE-OF-TYPE-IN-BITS) / 2.
 */
#define FIFO_MAX_SIZE_ALLOWED  (1ul << ((sizeof(zbncp_fifo_size_t) * 8u) - 1u))

/** @brief Book-keeping structure implementing FIFO indexing. */
typedef struct zbncp_fifo_s
{
  zbncp_fifo_size_t size;   /**< Total count of FIFO entries */
  zbncp_fifo_size_t tail;   /**< Tail index to enqueue items to */
  zbncp_fifo_size_t head;   /**< Head index to dequeue items from */
}
zbncp_fifo_t;

/** @brief Initialize FIFO structure. */
static inline void zbncp_fifo_init(zbncp_fifo_t *fifo, zbncp_fifo_size_t size)
{
  ZBNCP_DBG_ASSERT(size <= FIFO_MAX_SIZE_ALLOWED);

  fifo->size = size;
  fifo->head = 0u;
  fifo->tail = 0u;
}

/** @brief Obtain current FIFO item count. */
static inline zbncp_fifo_size_t zbncp_fifo_item_count(const zbncp_fifo_t *fifo)
{
  return (zbncp_fifo_size_t)(fifo->tail - fifo->head);
}

/** @brief Predicate to check whether the FIFO is empty now. */
static inline zbncp_bool_t zbncp_fifo_is_empty(const zbncp_fifo_t *fifo)
{
  return (zbncp_fifo_item_count(fifo) == 0u);
}

/** @brief Predicate to check whether the FIFO is full now. */
static inline zbncp_bool_t zbncp_fifo_is_full(const zbncp_fifo_t *fifo)
{
  return (zbncp_fifo_item_count(fifo) == fifo->size);
}

/** @brief Obtain the head index of the FIFO. */
static inline zbncp_fifo_size_t zbncp_fifo_head(const zbncp_fifo_t *fifo)
{
  return (fifo->head % fifo->size);
}

/** @brief Obtain the tail index of the FIFO. */
static inline zbncp_fifo_size_t zbncp_fifo_tail(const zbncp_fifo_t *fifo)
{
  return (fifo->tail % fifo->size);
}

/** @brief Enqueue the new item to the tail of the FIFO and return its index. */
static inline zbncp_fifo_size_t zbncp_fifo_enqueue(zbncp_fifo_t *fifo)
{
  zbncp_fifo_size_t tail = zbncp_fifo_tail(fifo);
  ++fifo->tail;
  return tail;
}

/** @brief Dequeue an item from the head of the FIFO. */
static inline void zbncp_fifo_dequeue(zbncp_fifo_t *fifo)
{
  ++fifo->head;
}

#endif /* ZBNCP_INCLUDE_GUARD_FIFO_H */
