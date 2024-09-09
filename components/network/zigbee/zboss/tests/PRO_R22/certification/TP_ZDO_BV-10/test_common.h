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
/* PURPOSE:
*/
#ifndef __TP__ZDO__10__H__
#define __TP__ZDO__10__H__

#define ZB_EXIT( _p )

#define TEST_ED1_EP               0x01
#define TEST_ED2_EP               0xF0
#define TEST_BUFFER_LEN           0x10

#define TEST_ENABLED

//#define TEST_CHANNEL (1l << 24)
#define TEST_PAN_ID  0x1AAA

#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZED2 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TIME_ZC_CONNECTION            (30 * ZB_TIME_ONE_SECOND)
#define TIME_ZED1_COMPLEX_DESCR_REQ   (5 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TIME_ZED1_USER_DESCR_REQ1     (10 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TIME_ZED1_DISCOVERY_REG_REQ   (15 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TIME_ZED1_USER_DESCR_SET      (20 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TIME_ZED1_USER_DESCR_REQ2     (25 * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)

enum test_step_e
{
  STEP_ZED1_COMPLEX_DESCR_REQ,
  STEP_ZED1_USER_DESCR_REQ1,
  STEP_ZED1_DISCOVERY_REG_REQ,
  STEP_ZED1_USER_DESCR_SET,
  STEP_ZED1_USER_DESCR_REQ2
};

enum test_zdo_clid_e
{
  TEST_ZDO_COMPLEX_DESCR_REQ_CLID = 0x0010,
  TEST_ZDO_USER_DESCR_REQ_CLID = 0x0011,
  TEST_ZDO_USER_DESCR_SET_CLID = 0x0014
};

#endif
