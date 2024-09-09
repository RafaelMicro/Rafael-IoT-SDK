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
/* PURPOSE: common definitions for this test.
*/

#ifndef __RTP_INT_01_
#define __RTP_INT_01_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TEST_PIN1_DUT  11
#define TEST_PORT1_DUT 1

#define TEST_PIN2_DUT  13
#define TEST_PORT2_DUT 1

#define RTP_INT_01_STEP_1_DELAY_ZR (1 * ZB_TIME_ONE_SECOND)
#define RTP_INT_01_STEP_1_TIME_ZR  (2 * ZB_TIME_ONE_SECOND)

#define TEST_DUT_MAX_ATTEMPTS      2
#define TEST_DUT_NEXT_STEP_DELAY   (ZB_TIME_ONE_SECOND)

#endif /* __RTP_INT_01_ */
