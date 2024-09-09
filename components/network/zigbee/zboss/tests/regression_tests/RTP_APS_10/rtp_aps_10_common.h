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
#ifndef __RTP_APS_10_COMMON_
#define __RTP_APS_10_COMMON_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_TH_ZR2 {0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define DUT_FB_INITIATOR_DELAY       (15 * ZB_TIME_ONE_SECOND)
#define TH_FB_DURATION               (30)

#define RTP_APS_10_STEP_1_DELAY_ZC    (1 * ZB_TIME_ONE_SECOND)
#define RTP_APS_10_STEP_1_TIME_ZC     (10 * ZB_TIME_ONE_SECOND)
#define RTP_APS_10_STEP_2_TIME_ZC     (10 * ZB_TIME_ONE_SECOND)

/* DUT is initiator, TH is target */
#define DUT_ENDPOINT                  143
#define TH_ENDPOINT                   10

#define DUT_MAX_DATA_LENGTH           115
#define DUT_PAYLOAD_LENGTH_STEP_1     90
#define DUT_PAYLOAD_LENGTH_STEP_2     65
#define DUT_PAYLOAD_LENGTH_STEP_3     65
#define DUT_PAYLOAD_LENGTH_STEP_4     90

#define TEST_CUSTOM_CLUSTER_TEST_PAYLOAD_CMD_ID 0xaa

#endif /* __RTP_APS_10_COMMON_ */
