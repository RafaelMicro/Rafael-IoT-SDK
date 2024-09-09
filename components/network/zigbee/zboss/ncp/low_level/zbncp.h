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
/*  PURPOSE: ZBOSS NCP header for include by library user.
*/
#ifndef ZBNCP_INCLUDE_GUARD_ZBNCP_H
#define ZBNCP_INCLUDE_GUARD_ZBNCP_H 1

#include "zbncp_tr.h"
#include "zbncp_ll_proto.h"
#include "zbncp_fragmentation.h"

/**
 * @brief Creates global low level protocol object.
 *
 * @param tr_ops - pointer to a structure containig a table of user-provided
 *                 transport implementation functions
 *
 * @return pointer to the global low-level protocol object
 */
zbncp_ll_proto_t *zbncp_ll_create(const zbncp_transport_ops_t *tr_ops);

#endif /* ZBNCP_INCLUDE_GUARD_ZBNCP_H */
