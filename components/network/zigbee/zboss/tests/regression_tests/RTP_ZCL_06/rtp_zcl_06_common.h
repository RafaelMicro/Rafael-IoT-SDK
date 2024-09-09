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
#ifndef __RTP_ZCL_06_COMMON_
#define __RTP_ZCL_06_COMMON_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TH_FB_INITIATOR_DELAY     (10 * ZB_TIME_ONE_SECOND)

#define DUT_FB_DURATION            (20)

#define RTP_ZCL_06_STEP_1_DELAY_ZC (1 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_06_STEP_1_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_06_STEP_2_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_06_STEP_3_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_06_STEP_4_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)

/* DUT is target, TH is initiator */
#define DUT_ENDPOINT               143
#define TH_ENDPOINT              10

#endif /* __RTP_ZCL_06_COMMON_ */
