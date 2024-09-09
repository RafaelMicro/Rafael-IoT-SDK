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
#ifndef __TP__ZDO__12__H__
#define __TP__ZDO__12__H__

#define ZB_EXIT( _p )

#define TEST_ED1_EP               0x01
#define TEST_ED2_EP               0xF0
#define TEST_BUFFER_LEN           0x10

#define TEST_ENABLED

//#define TEST_CHANNEL (1l << 24)
#define TEST_PAN_ID  0x1AAA

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED2 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TP_ZDO_BV_12_STEP_1_DELAY_ZC           (100 * ZB_TIME_ONE_SECOND)
#define TP_ZDO_BV_12_STEP_1_TIME_ZC            (50 * ZB_TIME_ONE_SECOND)
#define TP_ZDO_BV_12_STEP_4_TIME_ZC            (5 * ZB_TIME_ONE_SECOND)

#define TP_ZDO_BV_12_STEP_2_DELAY_ZED1         (105 * ZB_TIME_ONE_SECOND)
#define TP_ZDO_BV_12_STEP_2_TIME_ZED1          (155 * ZB_TIME_ONE_SECOND)
#define TP_ZDO_BV_12_STEP_5_TIME_ZED1          (5 * ZB_TIME_ONE_SECOND)

#endif
