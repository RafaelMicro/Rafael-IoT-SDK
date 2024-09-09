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
#ifndef __RTP_ZCL_14_COMMON_
#define __RTP_ZCL_14_COMMON_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TH_FB_INITIATOR_DELAY     (20 * ZB_TIME_ONE_SECOND)
#define DUT_FB_DURATION           (20)

#define RTP_ZCL_14_STEP_1_DELAY_ZC (1 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_14_STEP_1_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_14_STEP_2_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)

/* DUT is target, TH is initiator */
#define TH_ENDPOINT                    143
#define DUT_ENDPOINT                   10

#define TEST_GROUP_ID                  0xaaaa
#define TEST_SCENE_ID                  0xbb

#define MANUFACTURER_SPECIFIC_CODE 0x117c
#define MANUFACTURER_SPECIFIC_CMD_ID 0x07
#define MANUFACTURER_SPECIFIC_CMD_PAYLOAD_LENGTH 4

#endif /* __RTP_ZCL_14_COMMON_ */
