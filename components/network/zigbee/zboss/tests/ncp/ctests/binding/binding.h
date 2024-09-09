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
/* PURPOSE: common include for binding test
*/

#ifndef BINDING_H
#define BINDING_H 1

#include "zb_types.h"

#define ZC_TEST_SEND_DATA_DELAY (2 * ZB_TIME_ONE_SECOND)

#define ZC_TEST_APS_BIND_DELAY (5 * ZB_TIME_ONE_SECOND)
#define ZC_TEST_APS_UNBIND_DELAY (14 * ZB_TIME_ONE_SECOND)

#define ZC_TEST_ZDO_BIND_DELAY (16 * ZB_TIME_ONE_SECOND)
#define ZC_TEST_ZDO_UNBIND_DELAY (23 * ZB_TIME_ONE_SECOND)

#define TEST_SEND_DATA_DELAY ZB_MILLISECONDS_TO_BEACON_INTERVAL(1500)

#define DUT_ENDPOINT 10
#define TH_ENDPOINT 10

#define TEST_CLUSTER_ID 0

#endif /* BINDING_H */
