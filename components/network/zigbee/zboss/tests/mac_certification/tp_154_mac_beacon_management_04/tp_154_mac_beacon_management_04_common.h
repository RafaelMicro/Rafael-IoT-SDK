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
/*  PURPOSE: TP/154/MAC/BEACON-MANAGEMENT-04 test constants
*/

#ifndef TP_154_MAC_BEACON_MANAGEMENT_04_COMMON_H
#define TP_154_MAC_BEACON_MANAGEMENT_04_COMMON_H 1

#define TEST_PAN_ID                     0x1AAA
#define TEST_ASSOCIATION_PERMIT         1

#define TEST_TIMEOUT                    4000

#define TEST_DUT1_FFD1_IEEE_ADDR        {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT2_FFD0_IEEE_ADDR        {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT1_FFD1_SHORT_ADDRESS    0x1122
#define TEST_DUT2_FFD0_SHORT_ADDRESS    0x3344
#define TEST_ASSOCIATION_CAP_INFO       0x8A
#define TEST_SCAN_TYPE                  ENHANCED_ACTIVE_SCAN
#define TEST_SCAN_DURATION              5
#define TEST_RX_ON_WHEN_IDLE            1
#define TEST_BEACON_PAYLOAD_LENGTH      15
#define TEST_DUT1_BEACON_PAYLOAD        (zb_uint8_t[]){0x00, 0x22, 0x84, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0xFF, 0xFF, 0xFF, 0x00}
#define TEST_DUT2_BEACON_PAYLOAD        (zb_uint8_t[]){0x00, 0x22, 0x84, 0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC, 0xFF, 0xFF, 0xFF, 0x00}

/** Test step enumeration. */
enum test_step_e
{
  TEST_STEP_INITIAL,
  DUT1_DUT2_JP_NOJOIN,
  DUT1_DUT2_JP_ALLJOIN,
  DUT1_DUT2_JP_IEEELISTJOIN_INLIST,
  DUT1_DUT2_JP_IEEELISTJOIN_NOTINLIST,
  TEST_STEP_FINISHED
};

#endif /* TP_154_MAC_BEACON_MANAGEMENT_04_COMMON_H */
