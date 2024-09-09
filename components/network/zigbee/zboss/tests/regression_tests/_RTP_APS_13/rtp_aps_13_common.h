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
#ifndef __RTP_APS_13_COMMON_
#define __RTP_APS_13_COMMON_


#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define DUT_FB_INITIATOR_DELAY       (10 * ZB_TIME_ONE_SECOND)
#define TH_FB_DURATION               (10)

#define RTP_APS_13_STEP_1_DELAY_ZC    (1 * ZB_TIME_ONE_SECOND)
#define RTP_APS_13_STEP_1_TIME_ZC     (3 * ZB_TIME_ONE_SECOND)
#define RTP_APS_13_STEP_2_TIME_ZC     (3 * ZB_TIME_ONE_SECOND)
#define RTP_APS_13_STEP_3_TIME_ZC     (3 * ZB_TIME_ONE_SECOND)

#define DUT_ENDPOINT_CLI_1            10

#define DUT_ENDPOINT_SRV              12

#define TH_ENDPOINT_CLI               143

#define DUT_MAX_DATA_LENGTH           100
#define DUT_PAYLOAD_LENGTH_STEP_1     64
#define DUT_PAYLOAD_LENGTH_STEP_2     65

#define DUT_ENDPOINT_SRV_ON_OFF_REPORTING_MIN_INTERVAL 0x0001
#define DUT_ENDPOINT_SRV_ON_OFF_REPORTING_MAX_INTERVAL 0x0000

#define DUT_ENDPOINT_SRV_CURRENT_LEVEL_REPORTING_DELTA 1

#define DUT_ENDPOINT_SRV_CURRENT_LEVEL_INITIAL_VALUE 0

#define TEST_GROUP_ID 0xaaaa

#endif /* __RTP_APS_13_COMMON_ */
