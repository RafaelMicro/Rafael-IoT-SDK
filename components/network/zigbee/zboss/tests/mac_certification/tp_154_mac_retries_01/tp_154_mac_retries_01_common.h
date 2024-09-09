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
/*  PURPOSE: TP/154/MAC/RETRIES-01 test constants
*/

#ifndef TP_154_MAC_RETRIES_01_COMMON_H
#define TP_154_MAC_RETRIES_01_COMMON_H 1

#define TEST_PAN_ID                       0x1AAA
#define TEST_ASSOCIATION_PERMIT           0

#define TEST_DUT_FFD0_IEEE_ADDR           {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD0_SHORT_ADDRESS       0x1122
#define TEST_FAKE_DEST_SHORT_ADDRESS      0x3344
#define TEST_DUT_FFD0_RX_ON_WHEN_IDLE     ZB_TRUE
#define TEST_DUT_FFD0_MAX_FRAME_RETRIES   1
#define TEST_MSDU_LENGTH                  5

enum test_step_e
{
  TEST_STEP_INITIAL,
  TEST_STEP_DEFAULT_MAC_RETRIES_NUMBER,
  TEST_STEP_NON_DEFAULT_MAC_RETRIES_NUMBER,
};

#endif /* TP_154_MAC_RETRIES_01_COMMON_H */
