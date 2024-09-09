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
/*  PURPOSE: NCP LL packet FIFO implementation.
*/
#ifndef ZBNCP_INCLUDE_GUARD_LL_FIFO_H
#define ZBNCP_INCLUDE_GUARD_LL_FIFO_H 1

#include "zbncp_fifo.h"
#include "zbncp_ll_pktbuf.h"

/** @brief LL packet buffer FIFO for packet transmission */
typedef struct zbncp_ll_fifo_s
{
  zbncp_fifo_t hdr;           /**< FIFO book-keeping structure */
  zbncp_ll_pktbuf_t *pool;    /**< Packet buffer pool */
}
zbncp_ll_fifo_t;

/** @brief Initialize the packet buffer FIFO */
static inline void zbncp_ll_fifo_init(zbncp_ll_fifo_t *fifo, zbncp_ll_pktbuf_t *pool, zbncp_size_t size)
{
  zbncp_fifo_init(&fifo->hdr, (zbncp_fifo_size_t) size);
  fifo->pool = pool;
}

/** @brief Obtain a pointer to the head packet buffer in the FIFO */
static inline zbncp_ll_pktbuf_t *zbncp_ll_fifo_head(const zbncp_ll_fifo_t *fifo)
{
  if (!zbncp_fifo_is_empty(&fifo->hdr))
  {
    zbncp_fifo_size_t head = zbncp_fifo_head(&fifo->hdr);
    return &fifo->pool[head];
  }
  return ZBNCP_NULL;
}

/** @brief Enqueue a packet buffer in the FIFO and return a pointer to the buffer */
static inline zbncp_ll_pktbuf_t *zbncp_ll_fifo_enqueue(zbncp_ll_fifo_t *fifo)
{
  if (!zbncp_fifo_is_full(&fifo->hdr))
  {
    zbncp_fifo_size_t tail = zbncp_fifo_enqueue(&fifo->hdr);
    return &fifo->pool[tail];
  }
  return ZBNCP_NULL;
}

/** @brief Dequeue a packet buffer from the FIFO */
static inline void zbncp_ll_fifo_dequeue(zbncp_ll_fifo_t *fifo)
{
  if (!zbncp_fifo_is_empty(&fifo->hdr))
  {
    zbncp_fifo_dequeue(&fifo->hdr);
  }
}

#endif /* ZBNCP_INCLUDE_GUARD_LL_FIFO_H */
