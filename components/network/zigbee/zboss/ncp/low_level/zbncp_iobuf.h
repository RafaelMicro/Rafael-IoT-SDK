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
/*  PURPOSE: Genereic I/O buffer implementation.
*/
#ifndef ZBNCP_INCLUDE_GUARD_IOBUF_H
#define ZBNCP_INCLUDE_GUARD_IOBUF_H 1

#include "zbncp_types.h"
#include "zbncp_mem.h"

/** @brief Generic Input/Output Buffer */
typedef struct zbncp_io_buffer_s
{
  zbncp_memref_t mem;   /**< Memory reference to the buffer data */
  zbncp_size_t rpos;    /**< Current read position in the buffer data */
  zbncp_size_t wpos;    /**< Current write position in the buffer data */
}
zbncp_io_buffer_t;

/** @brief Predicate to check whether the buffer has no data right now. */
static inline zbncp_bool_t zbncp_io_buffer_is_empty(const zbncp_io_buffer_t *iob)
{
  return (iob->rpos == iob->wpos);
}

/** @brief Reset the IO buffer . */
static inline void zbncp_io_buffer_reset(zbncp_io_buffer_t *iob)
{
  iob->rpos = 0u;
  iob->wpos = 0u;
}

/** @brief Initialize the IO buffer . */
static inline void zbncp_io_buffer_init(zbncp_io_buffer_t *iob, void *data, zbncp_size_t size)
{
  iob->mem = zbncp_make_memref(data, size);
  zbncp_io_buffer_reset(iob);
}

/** @brief Collect any unused space in the IO buffer . */
static inline void zbncp_io_buffer_compact(zbncp_io_buffer_t *iob)
{
  char *ptr = zbncp_memref_at(iob->mem, 0u);
  zbncp_size_t offset = iob->rpos;
  zbncp_size_t size = iob->wpos - iob->rpos;

  if (offset != 0u) {
    iob->rpos -= offset;
    iob->wpos -= offset;
    zbncp_mem_move(ptr, &ptr[offset], size);
  }
}

/** @brief Read the data from the IO buffer . */
static inline zbncp_size_t zbncp_io_buffer_read(zbncp_io_buffer_t *iob, zbncp_memref_t mem)
{
  char *ptr = zbncp_memref_at(iob->mem, iob->rpos);
  zbncp_size_t size = size_min(mem.size, iob->wpos - iob->rpos);
  zbncp_mem_copy(mem.ptr, ptr, size);
  iob->rpos += size;
  return size;
}

/** @brief Write the data to the IO buffer . */
static inline zbncp_size_t zbncp_io_buffer_write(zbncp_io_buffer_t *iob, zbncp_cmemref_t mem)
{
  char *ptr = zbncp_memref_at(iob->mem, iob->wpos);
  zbncp_size_t size = size_min(mem.size, iob->mem.size - iob->wpos);
  zbncp_mem_copy(ptr, mem.ptr, size);
  iob->wpos += size;
  return size;
}

#endif /* ZBNCP_INCLUDE_GUARD_IOBUF_H */
