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
#ifndef _TP_NWK_BV_01_TEST_COMMON
#define _TP_NWK_BV_01_TEST_COMMON

//#define TEST_CHANNEL (1l << 24)

#define TEST_PAN_ID_DUT 0x1AAA
#define TEST_PAN_ID_G   0x1AAB

#define TP_NWK_BV_01_STEP_4_DELAY_DUTZED1  (60 * ZB_TIME_ONE_SECOND)
#define TP_NWK_BV_01_STEP_4_TIME_DUTZED1   (5 * ZB_TIME_ONE_SECOND)

#endif  /* _TP_NWK_BV_01_TEST_COMMON */
