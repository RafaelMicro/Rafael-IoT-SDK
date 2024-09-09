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
/*  PURPOSE: TP/154/MAC/SCANNING-06 test constants
*/
#ifndef TP_154_MAC_SCANNING_06_COMMON_H
#define TP_154_MAC_SCANNING_06_COMMON_H 1


#define TEST_SCAN_TYPE             ACTIVE_SCAN
#define TEST_SCAN_DURATION         5
#define TEST_DUT_FFD0_MAC_ADDRESS   {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_ASSOCIATION_PERMIT    0
#define TEST_BEACON_PAYLOAD_LENGTH 0
#define TEST_BEACON_PAYLOAD        "\0"
#define TEST_RX_ON_WHEN_IDLE       1
#define TEST_DUT_SCAN_TIMEOUT      ZB_TIME_ONE_SECOND

#endif /* TP_154_MAC_SCANNING_03_COMMON_H */
