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
#ifndef __RTP_APS_03_COMMON_
#define __RTP_APS_03_COMMON_

#define RTP_APS_03_STEP_1_DELAY_ZED    (10 * ZB_TIME_ONE_SECOND)
#define RTP_APS_03_STEP_1_TIME_ZED     (15 * ZB_TIME_ONE_SECOND)

#define TABLE_EMPTY_CHECKS_MAX_RETRIES 15
#define TEST_DUT_BULB_COUNT            2
#define DUT_ENDPOINT                   143
#define TH_ENDPOINT                    10

#endif /* __RTP_APS_03_COMMON_ */
