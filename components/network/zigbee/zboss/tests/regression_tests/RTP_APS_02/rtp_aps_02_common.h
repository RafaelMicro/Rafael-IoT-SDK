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
/* PURPOSE: common definitions for test
*/
#ifndef __RTP_APS_02_COMMON_
#define __RTP_APS_02_COMMON_

#define TEST_DUT_BULB_COUNT         4
#define TEST_DUT_ON_OFF_CMD_COUNT   2

#define RTP_APS_02_STEP_1_DELAY_ZED (10 * ZB_TIME_ONE_SECOND)
#define RTP_APS_02_STEP_1_TIME_ZED  (25 * ZB_TIME_ONE_SECOND)
#define RTP_APS_02_STEP_2_TIME_ZED  (0)

/* DUT is target, TH is initiator */
#define DUT_ENDPOINT                143
#define TH_ENDPOINT                 10

#endif /* __RTP_APS_02_COMMON_ */
