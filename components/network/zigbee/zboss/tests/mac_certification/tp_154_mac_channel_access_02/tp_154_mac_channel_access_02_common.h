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
/*  PURPOSE: TP/154/MAC/CHANNEL-ACCESS-02 test constants
*/
#ifndef TP_154_MAC_CHANNEL_ACCESS_02_COMMON_H
#define TP_154_MAC_CHANNEL_ACCESS_02_COMMON_H 1

#define TEST_PAN_ID                 0x1AAA

#define TEST_DUT_FFD0_MAC_ADDRESS   {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD0_SHORT_ADDRESS 0x0000
#define TEST_DST_SHORT_ADDRESS      0x1234
#define TEST_RX_ON_WHEN_IDLE        1
#define TEST_ASSOCIATION_CAP_INFO   0x80           /* 80 - "allocate address" */
#define TEST_MSDU_LENGTH            10
#define TEST_PACKETS_COUNT	    30

#endif /* TP_154_MAC_CHANNEL_ACCESS_02_COMMON_H */
