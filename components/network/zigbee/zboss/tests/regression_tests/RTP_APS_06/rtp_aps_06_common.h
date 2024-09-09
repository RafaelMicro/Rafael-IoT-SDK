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
#ifndef __RTP_APS_06_COMMON_
#define __RTP_APS_06_COMMON_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_TH_ZR2 {0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_TH_ZR3 {0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_TH_ZR4 {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TH_FB_INITIATOR_DELAY     (10 * ZB_TIME_ONE_SECOND)

#define DUT_FB_DURATION            (20)

#define RTP_APS_06_STEP_1_DELAY_ZED (10 * ZB_TIME_ONE_SECOND)
#define RTP_APS_06_STEP_1_TIME_ZED  (15 * ZB_TIME_ONE_SECOND)
#define RTP_APS_06_STEP_2_TIME_ZED  (30 * ZB_TIME_ONE_SECOND)
#define RTP_APS_06_STEP_3_TIME_ZED  (80 * ZB_TIME_ONE_SECOND)

/* DUT is target, TH is initiator */
#define DUT_ENDPOINT               143
#define TH_ENDPOINT              10

#define LIGHT_CONTROL_DIMM_STEP 15
#define LIGHT_CONTROL_DIMM_TRANSACTION_TIME 2

#define TEST_COMMAND_INTERVAL_1 (3 * ZB_TIME_ONE_SECOND)
#define TEST_COMMAND_INTERVAL_2 (1 * ZB_TIME_ONE_SECOND)

#define TEST_COMMANDS_TO_CHANGE_INTERVAL 1

#endif /* __RTP_APS_06_COMMON_ */
