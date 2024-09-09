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
#ifndef __APS__BV__14__
#define __APS__BV__14__

#define ZB_EXIT( _p )

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

//#define TEST_CHANNEL (1l << 24)
#define TEST_PAN_ID  (0x1AAA)

#define USE_NWK_MULTICAST ZB_FALSE

#define GROUP_ADDR      0x0001
#define GROUP_EP        0xF0

#define TIME_ZC_CONNECTION        (25 * ZB_TIME_ONE_SECOND)

#define TP_APS_BV_14_I_STEP_1_DELAY_ZC     (1  * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TP_APS_BV_14_I_STEP_1_TIME_ZC      (45 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_14_I_STEP_2_TIME_ZC      (5 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_14_I_STEP_3_DELAY_ZR2    (30 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_14_I_STEP_3_TIME_ZR2     (5 * ZB_TIME_ONE_SECOND)

#endif
