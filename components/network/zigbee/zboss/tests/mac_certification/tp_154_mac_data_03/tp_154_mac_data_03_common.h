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
/*  PURPOSE: TP/154/MAC/DATA-03 test constants
*/
#ifndef TP_154_MAC_DATA_03_COMMON_H
#define TP_154_MAC_DATA_03_COMMON_H 1

#define TEST_TH_FFD0_PAN_ID             0x1AAA
#define TEST_DUT_FFD1_PAN_ID            0x1BBB
#define TEST_TH_FFD0_SHORT_ADDRESS      0x1122
#define TEST_DUT_FFD1_SHORT_ADDRESS     0x3344

#define TEST_DUT_FFD1_MAC_ADDRESS       {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_TH_FFD0_MAC_ADDRESS        {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_ASSOCIATION_PERMIT         1
#define TEST_TH_FFD0_RX_ON_WHEN_IDLE    1
#define TEST_DUT_FFD1_RX_ON_WHEN_IDLE   1
#define TEST_ASSOCIATION_CAP_INFO       0x80           /* 80 - "allocate address" */
#define TEST_MSDU_LENGTH                5
#define TEST_MAX_MSDU_LENGTH            0x10
#define TEST_TH_FFD0_MSDU_HANDLE        0xd

/** Test step enumeration. */
enum test_step_e
{
  TEST_STEP_INITIAL,
  TH2DUT_SHORT2SHORT_UNICAST_NOACK,
  TH2DUT_SHORT2SHORT_UNICAST_ACK,
  TH2DUT_SHORT2EXT_UNICAST_ACK,
  TH2DUT_EXT2SHORT_UNICAST_ACK,
  TH2DUT_EXT2EXT_UNICAST_ACK,
  TH2DUT_EXT2BROADCAST,
  TH2DUT_SHORT2BROADCAST,
  TH2DUT_SHORT2BROADCAST_BCAST_PANID,
  DUT2TH_SHORT2SHORT_UNICAST_NOACK,
  DUT2TH_SHORT2SHORT_UNICAST_ACK,
  DUT2TH_SHORT2EXT_UNICAST_ACK,
  DUT2TH_EXT2SHORT_UNICAST_ACK,
  DUT2TH_EXT2EXT_UNICAST_ACK,
  DUT2TH_EXT2BROADCAST,
  DUT2TH_SHORT2BROADCAST,
  DUT2TH_SHORT2BROADCAST_BCAST_PANID,
  TEST_STEP_FINISHED
};

typedef struct zb_dut2th_result_s
{
  zb_bool_t a;
	zb_bool_t b;
	zb_bool_t c;
	zb_bool_t d;
	zb_bool_t e;
	zb_bool_t f;
	zb_bool_t g;
	zb_bool_t g2;
} zb_dut2th_result_t;

typedef struct zb_th2dut_result_s
{
	zb_bool_t h;
	zb_bool_t i;
	zb_bool_t j;
	zb_bool_t k;
	zb_bool_t l;
	zb_bool_t m;
	zb_bool_t n;
	zb_bool_t n2;
} zb_th2dut_result_t;

typedef struct zb_mac_test_s
{
	zb_dut2th_result_t    dut2th;  
  zb_th2dut_result_t    th2dut;          
} zb_mac_test3_t;

extern zb_mac_test3_t test_result3;

#endif /* TP_154_MAC_DATA_03_COMMON_H */
