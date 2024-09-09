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
#ifndef __RTP_ZDO_05_COMMON_
#define __RTP_ZDO_05_COMMON_

#define IEEE_ADDR_TH {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define RTP_ZDO_05_STEP_1_DELAY_ZC (1 * ZB_TIME_ONE_SECOND)
#define RTP_ZDO_05_STEP_1_TIME_ZC  (3 * ZB_TIME_ONE_SECOND)
#define RTP_ZDO_05_STEP_2_TIME_ZC  (12 * ZB_TIME_ONE_SECOND)

#define DUT_INVALID_CLUSTER_ID 0x1234
#define DUT_NOT_DECLARED_CLUSTER_ID ZB_ZCL_CLUSTER_ID_COLOR_CONTROL

/* DUT is initiator, TH is target */
#define DUT_ENDPOINT               143
#define TH_ENDPOINT              10

#endif /* __RTP_ZDO_05_COMMON_ */
