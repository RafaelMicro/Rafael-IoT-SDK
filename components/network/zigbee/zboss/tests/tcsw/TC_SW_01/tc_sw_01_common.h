/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
#ifndef __TP_AN_NOH_4_1_COMMON_
#define __TP_AN_NOH_4_1_COMMON_

/* #define TEST_DISABLE_AUTH_TOKEN_REQ */
/* #define TEST_DISABLE_AUTH_TOKEN_RSP */

#define TEST_CHANNEL 21

#define TP_AN_NOH_4_1_BUFFER_TEST_REQ_DELAY (5 * ZB_TIME_ONE_SECOND)
#define TEST_DUT_ZED_POLL_INTERVAL_MS        500

#endif /* __TP_AN_NOH_4_1_COMMON_ */
