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
/*  PURPOSE: NCP low level protocol time-related type and constants.
*/
#ifndef ZBNCP_INCLUDE_GUARD_LL_TIME_H
#define ZBNCP_INCLUDE_GUARD_LL_TIME_H 1

#include "zbncp_types.h"

/** @brief NCP low-level protocol time representation type. */
typedef zbncp_size_t zbncp_ll_time_t;

/** @brief Constant defining infinite NCP low-level protocol timeout. */
#define ZBNCP_LL_TIMEOUT_INFINITE ((zbncp_ll_time_t) -1)
/** @brief Constant defining zero NCP low-level protocol timeout. */
#define ZBNCP_LL_TIMEOUT_NOW      ((zbncp_ll_time_t)  0)

#endif /* ZBNCP_INCLUDE_GUARD_LL_TIME_H */
