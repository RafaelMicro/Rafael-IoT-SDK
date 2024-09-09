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
#ifndef __RTP_ZCL_23_COMMON_
#define __RTP_ZCL_23_COMMON_


#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define RTP_ZCL_23_STEP_1_DELAY_ZC                     (8 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_23_STEP_TIME_ZC                        (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_23_DELAY_AFTER_NWK_UPDATE              (10 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_23_DELAY_TO_REAL_CHANNEL_CHANGE        (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZCL_23_DELAY_TO_REAL_PANID_CHANGE          (3 * ZB_TIME_ONE_SECOND)

#define RTP_ZCL_23_TEST_SCAN_DURATION ZB_ZDO_NEW_ACTIVE_CHANNEL
#define RTP_ZCL_23_TEST_SCAN_COUNT 0x01

#endif /* __RTP_ZCL_23_COMMON_ */
