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

#ifndef __RTP_BDB_13_
#define __RTP_BDB_13_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TEST_RESET_COUNT          8
#define TEST_NEXT_STEP_TIMEOUT_BI ZB_MILLISECONDS_TO_BEACON_INTERVAL(5000)

#define TH_SLEEP_TIME             (15000000) /* sleep 15 seconds */

#define TEST_RESET_TIMEOUT_1_MS   5000
#define TEST_RESET_TIMEOUT_2_MS   3000
#define TEST_RESET_TIMEOUT_3_MS   2500
#define TEST_RESET_TIMEOUT_4_MS   1000
#define TEST_RESET_TIMEOUT_5_MS   800
#define TEST_RESET_TIMEOUT_6_MS   500
#define TEST_RESET_TIMEOUT_7_MS   100
#define TEST_RESET_TIMEOUT_8_MS   0

#endif /* __RTP_BDB_13_ */
