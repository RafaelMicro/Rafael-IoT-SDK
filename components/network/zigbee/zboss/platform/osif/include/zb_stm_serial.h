/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/* PURPOSE: Searial trace for Cortex M3 & M4
*/
#ifndef ZB_STM_SERIAL_H
#define ZB_STM_SERIAL_H 1

/*! \addtogroup ZB_OSIF_STM */
/*! @{ */


/* For 8051 there buffers and structs are defined differently */
#ifndef ZB8051
#ifdef ZB_HAVE_SERIAL

#include "zb_ringbuffer.h"
#include "Zboss_api_core.h"

ZB_RING_BUFFER_DECLARE(serial_iobuf, zb_uint8_t, 254);

typedef struct zb_serial_ctx_s
{
  zb_uint8_t inited;
  volatile zb_uint8_t tx_in_progress; /* if set, we are waiting for tx complete int */
  serial_iobuf_t tx_buf;
  zb_byte_array_t *p_tx_buf;
  zb_ushort_t tx_buf_cap;
  zb_callback_t byte_received_cb;
}
zb_serial_ctx_t;

typedef struct zb_io_ctx_s
{
  zb_serial_ctx_t serial_ctx;
}
zb_io_ctx_t;

#endif  /* ZB_HAVE_SERIAL */
#endif  /* 8051 */
/*! @} */


#endif /* ZB_STM_SERIAL_H */
