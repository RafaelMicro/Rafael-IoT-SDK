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
#ifndef _TP_APS_BV_18_I_TEST_COMMON_
#define _TP_APS_BV_18_I_TEST_COMMON_

#define TEST_PAN_ID  0x1AAA

#define TP_APS_BV_18_DELAY_BETWEEN_REQS_IN_PAIR (4 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_18_DELAY_BETWEEN_REQS_PAIRS   (20 * ZB_TIME_ONE_SECOND)

#define TP_APS_BV_18_STEP_1_DELAY_ZC    (15 * ZB_TIME_ONE_SECOND)
#define TP_APS_BV_18_STEP_1_TIME_ZC     TP_APS_BV_18_DELAY_BETWEEN_REQS_IN_PAIR
#define TP_APS_BV_18_STEP_2_TIME_ZC     TP_APS_BV_18_DELAY_BETWEEN_REQS_PAIRS
#define TP_APS_BV_18_STEP_3_TIME_ZC     TP_APS_BV_18_DELAY_BETWEEN_REQS_IN_PAIR
#define TP_APS_BV_18_STEP_4_TIME_ZC     TP_APS_BV_18_DELAY_BETWEEN_REQS_PAIRS


#endif  /* _TP_APS_BV_18_I_TEST_COMMON_ */
