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
/* PURPOSE: TP/154/PHY24/RECEIVER-05 common constants
MAC-only build
*/

#define TEST_SCAN_TIMEOUT                   ZB_MILLISECONDS_TO_BEACON_INTERVAL(100)
#define TEST_POWER_LEVEL_CHANGE_TIMEOUT     (10 * ZB_TIME_ONE_SECOND)
#define TEST_CHANNEL_CHANGE_TIMEOUT         (10 * ZB_TIME_ONE_SECOND)
#define TEST_SCAN_DURATION                  1
#define TEST_SCANS_PER_POWER_LEVEL          10
#define TEST_MAX_POWER_LEVEL_OFFSET         40
#define TEST_POWER_LEVEL_STEP               2
