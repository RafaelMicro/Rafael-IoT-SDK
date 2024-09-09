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
/*  PURPOSE: TP/154/MAC/ACK-FRAME-DELIVERY-01 test constants
*/

#ifndef TP_154_MAC_ACK_FRAME_DELIVERY_01_COMMON_H
#define TP_154_MAC_ACK_FRAME_DELIVERY_01_COMMON_H 1

#define TEST_PAN_ID                     0x1AAA
#define TEST_ASSOCIATION_PERMIT         0

#define TEST_DUT_FFD0_IEEE_ADDR         {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_TH_FFD1_IEEE_ADDR          {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD0_SHORT_ADDRESS     0x1122
#define TEST_TH_FFD1_SHORT_ADDRESS      0x3344
#define TEST_DUT_FFD0_RX_ON_WHEN_IDLE   ZB_TRUE
#define TEST_SCAN_TYPE                  ACTIVE_SCAN
#define TEST_SCAN_DURATION              5
#define TEST_MSDU_LENGTH                5
#define TEST_ASSOCIATION_CAP_INFO       0x82
#define TEST_DATA_FRAME_PERIOD          ZB_TIME_ONE_SECOND

enum test_step_th_e
{
  TEST_STEP_TH_INITIAL,
  TEST_STEP_TH_DISABLE_AUTO_ACK1,
  TEST_STEP_TH_RX_OFF_WHEN_IDLE1,
  TEST_STEP_TH_ENABLE_AUTO_ACK,
  TEST_STEP_TH_RX_ON_WHEN_IDLE,
  TEST_STEP_TH_DISABLE_AUTO_ACK2,
  TEST_STEP_TH_RX_OFF_WHEN_IDLE2,
  TEST_STEP_TH_FINISHED,
};

enum test_step_dut_e
{
  TEST_STEP_DUT_INITIAL,
  TEST_STEP_DUT_SEND_DATA_FRAME1,
  TEST_STEP_DUT_SEND_DATA_FRAME2,
  TEST_STEP_DUT_SEND_DATA_FRAME3,
  TEST_STEP_DUT_FINISHED,
};

#endif /* TP_154_MAC_ACK_FRAME_DELIVERY_01_COMMON_H */
