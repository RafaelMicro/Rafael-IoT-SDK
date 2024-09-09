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

#ifndef __RTP_NVRAM_02_
#define __RTP_NVRAM_02_

#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TEST_APP_NVRAM_PAYLOAD_SIZE 543
#define TEST_APP_NVRAM_REWRITES_COUNT 15

#define RTP_NVRAM_02_STEP_1_DELAY_ZR (15 * ZB_TIME_ONE_SECOND)
#define RTP_NVRAM_02_STEP_1_TIME_ZR  (3 * ZB_TIME_ONE_SECOND)

#define RTP_NVRAM_02_STEP_1_DELAY_ZC (10 * ZB_TIME_ONE_SECOND)
#define RTP_NVRAM_02_STEP_1_TIME_ZC  (1 * ZB_TIME_ONE_SECOND)

#endif /* __RTP_NVRAM_02_ */
