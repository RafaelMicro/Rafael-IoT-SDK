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

#ifndef __RTP_INT_03_
#define __RTP_INT_03_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/* Delay before sending sleep interrupting signal from TH */
#define TEST_WAKE_UP_SIGNAL_DELAY (30 * ZB_TIME_ONE_SECOND)

/* Allowed min and max time before sleep interrupting to pass the test (seconds) */
#define TEST_MIN_SLEEP_TIME 5
#define TEST_MAX_SLEEP_TIME 30

/* Time threshold of the sleep which will be interrupted (ms) */
#define TEST_SLEEP_THRESHOLD 500000

#define TEST_PIN1_DUT  10
#define TEST_PORT1_DUT 1

#define TEST_PIN1_TH  10
#define TEST_PORT1_TH 1

#endif /* __RTP_INT_03_ */
