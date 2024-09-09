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
/*  PURPOSE: TP/154/MAC/CHANNEL-ACCESS-03 DUT functionality
*/
#ifndef TP_154_MAC_CHANNEL_ACCESS_03_DUT_HAL_H
#define TP_154_MAC_CHANNEL_ACCESS_03_DUT_HAL_H 1

#include "tp_154_mac_channel_access_03_common.h"

typedef enum test_action_e
{
  DATA_INDICATION_EV_ID,
  LBT_START_EV_ID
} test_action_t;

void test_hal_indicate_action_linux(test_action_t event);
void test_hal_indicate_action_ti1352(test_action_t event);

#ifdef ZB_CONFIG_LINUX_NSNG
#define TEST_HAL_INDICATE_ACTION(event) 	    test_hal_indicate_action_linux(event)
#else
#define TEST_HAL_INDICATE_ACTION(event)         test_hal_indicate_action_ti1352(event)
#endif


#endif	/* TP_154_MAC_CHANNEL_ACCESS_03_DUT_HAL_H */
