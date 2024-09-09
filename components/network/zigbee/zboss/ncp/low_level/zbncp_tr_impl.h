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
/*  PURPOSE: NCP low level protocol transport wrappers declaration.
*/
#ifndef ZBNCP_INCLUDE_GUARD_TR_IMPL_H
#define ZBNCP_INCLUDE_GUARD_TR_IMPL_H 1

#include "zbncp_tr.h"

/** @brief Transport proxy used by low-level protocol. */
struct zbncp_transport_s
{
  zbncp_transport_ops_t ops;  /**< Table of user-provided transport operations */
};

/**
 * @brief Construct transport proxy object.
 *
 * @param tr - pointer to the transport proxy object
 * @param ops - pointer to the table with user-provided real operations
 *
 * @return nothing
 */
void zbncp_transport_construct(zbncp_transport_t *tr, const zbncp_transport_ops_t *ops);

/**
* @brief Initialize the transport proxy object used by LL protocol.
 *
 * @param tr - pointer to the transport proxy object
 * @param cb - pointer to the transport callbacks provided by LL protocol
 *
 * @return nothing
*/
void zbncp_transport_init(zbncp_transport_t *tr, const zbncp_transport_cb_t *cb);

/**
 * @brief Send the packet of data via the transport proxy object used by LL protocol.
 *
 * @param tr - pointer to the transport proxy object
 * @param mem - constnt memory reference to the buffer with the data to send
 *
 * @return nothing
 */
void zbncp_transport_send(zbncp_transport_t *tr, zbncp_cmemref_t mem);

/**
 * @brief Receive the packet of data from the transport proxy object used by LL protocol.
 *
 * @param tr - pointer to the transport proxy object
 * @param mem - memory reference to the buffer for the data to receive
 *
 * @return nothing
 */
void zbncp_transport_recv(zbncp_transport_t *tr, zbncp_memref_t mem);

#endif /* ZBNCP_INCLUDE_GUARD_TR_IMPL_H */
