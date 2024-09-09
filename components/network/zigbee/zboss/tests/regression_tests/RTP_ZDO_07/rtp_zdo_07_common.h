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
#ifndef __RTP_ZDO_07_COMMON_
#define __RTP_ZDO_07_COMMON_

#define IEEE_ADDR_TH_ZC   {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define DUT_FB_INITIATOR_DELAY     (15 * ZB_TIME_ONE_SECOND)

#define THR1_FB_DURATION           (20)

#define RTP_ZDO_07_STEP_1_DELAY_ZC (5 * ZB_TIME_ONE_SECOND)
#define RTP_ZDO_07_STEP_1_TIME_ZC  (0)

/* DUT is target, TH is initiator */
#define DUT_ENDPOINT               143
#define THR1_ENDPOINT              10

#endif /* __RTP_ZDO_07_COMMON_ */
