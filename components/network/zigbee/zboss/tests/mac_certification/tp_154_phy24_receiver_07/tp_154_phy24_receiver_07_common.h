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
/*  PURPOSE: TP/154/PHY24/RECEIVER-07
*/
#ifndef TP_154_PHY24_RECEIVER_07_COMMON_H
#define TP_154_PHY24_RECEIVER_07_COMMON_H 1

#define TEST_DUT_MAC_ADDRESS  {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_SHORT_ADDRESS 0x1122
#define TEST_PAN_ID 0x1AAA

/* Switch channel each 2 minutes */
#define TEST_SWITCH_CHANNEL_TMO (2 * 60 * ZB_TIME_ONE_SECOND)

#define TEST_CHANNEL_MIN 11
#define TEST_CHANNEL_MAX 26

#define TEST_MEASUREMENTS_NUM 10

#endif /* TP_154_PHY24_RECEIVER_07_COMMON_H */
