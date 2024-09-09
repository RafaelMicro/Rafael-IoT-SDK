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
/*  PURPOSE: TP/154/MAC/BEACON-MANAGEMENT-02 test constants
*/

#ifndef TP_154_MAC_BEACON_MANAGEMENT_02_COMMON_H
#define TP_154_MAC_BEACON_MANAGEMENT_02_COMMON_H 1

#define ZB_TEST_ADDR                    0x1122
#define TEST_PAN_ID                     0x1AAA
#define TEST_ASSOCIATION_PERMIT         0

#define TEST_DUT_FFD0_IEEE_ADDR         {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_TH_FFD1_IEEE_ADDR          {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD0_SHORT_ADDRESS     0x1122
#define TEST_SCAN_TYPE                  ACTIVE_SCAN
#define TEST_SCAN_DURATION              5
#define TEST_RX_ON_WHEN_IDLE            1

#endif /* TP_154_MAC_BEACON_MANAGEMENT_02_COMMON_H */
