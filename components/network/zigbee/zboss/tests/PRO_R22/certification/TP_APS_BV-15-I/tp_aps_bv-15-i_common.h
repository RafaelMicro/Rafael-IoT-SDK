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
#ifndef __APS__BV__15__
#define __APS__BV__15__

#define ZB_EXIT( _p )

#ifdef ZB_EMBER_TESTS
#define USE_EMBER_ZR1
//#define USE_EMBER_ZR2
#endif

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#ifndef USE_EMBER_ZR1
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#else
#define IEEE_ADDR_R1 = {0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00};
#endif
#ifndef USE_EMBER_ZR2
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
#else
#define IEEE_ADDR_R2 {0xf0, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00};
#endif

//#define TEST_CHANNEL (1l << 24)

#define ZR1_SHORT_ADDR            0x0001
#define ZR2_SHORT_ADDR            0x0002

#define GROUP_ADDR                0x0001
#define GROUP_EP                  0xF0

#define TIME_ZC_CONNECTION        (80 * ZB_TIME_ONE_SECOND)

#define TP_APS_BV_15_I_STEP_1_DELAY_ZC     (10  * ZB_TIME_ONE_SECOND + TIME_ZC_CONNECTION)
#define TP_APS_BV_15_I_STEP_1_TIME_ZC      (25 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_15_I_STEP_2_TIME_ZC      (5 * ZB_TIME_ONE_SECOND)

/* These are initial conditions */
#define TIME_ZR1_ADD_TO_GROUP     (1 * ZB_TIME_ONE_SECOND)
#define TIME_ZR2_ADD_TO_GROUP     (1 * ZB_TIME_ONE_SECOND)

#endif
