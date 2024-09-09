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
/*  PURPOSE: NCP transport creation routines declaration
*/
#ifndef NCP_TR_H
#define NCP_TR_H 1

#include "low_level/zbncp.h"

/****************************** NCP TRANSPORT API ******************************/

/**
 *  @addtogroup NCP_TRANSPORT_API
 * @{
 */

#if !(defined DOXYGEN && defined NCP_MODE_HOST)
/**
 * @brief Create user transport object for NCP device.
 *
 * @return pointer to the table with transport operations
 * to be passed to the low-level protocol.
 */
const zbncp_transport_ops_t *ncp_dev_transport_create(void);
#endif

#if defined NCP_MODE_HOST || defined ZB_NCP_HOST_RPI || (defined SNCP_MODE && defined ZB_NSNG)
/**
 * @brief Create user transport object for NCP host.
 *
 * @return pointer to the table with transport operations
 * to be passed to the low-level protocol.
 */
const zbncp_transport_ops_t *ncp_host_transport_create(void);
#endif /* NCP_MODE_HOST || ZB_NCP_HOST_RPI || (SNCP_MODE && ZB_NSNG) */

/**
   @}
*/

#endif /* NCP_TR_H */
