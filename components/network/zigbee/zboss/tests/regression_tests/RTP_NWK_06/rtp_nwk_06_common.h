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
#ifndef __RTP_NWK_06_COMMON_
#define __RTP_NWK_06_COMMON_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZR2 {0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZR3 {0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZR4 {0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_TH_ZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define RTP_NWK_06_STEP_1_DELAY_ZED (15 * ZB_TIME_ONE_SECOND)
#define RTP_NWK_06_STEP_1_TIME_ZED  (1 * ZB_TIME_ONE_SECOND)

#define RTP_NWK_06_CONCENTRATOR_MODE_RADIUS 5
#define RTP_NWK_06_CONCENTRATOR_MODE_DISC_TIME 30

#define RTP_NWK_06_CONCENTRATOR_MODE_STOP_DELAY (3 * ZB_TIME_ONE_SECOND)

#endif /* __RTP_NWK_06_COMMON_ */
