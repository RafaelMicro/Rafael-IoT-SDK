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
/* PURPOSE: HA OTA Upgrade test common definition
*/

#ifndef __RTP_OTA_CLI_07_COMMON_
#define __RTP_OTA_CLI_07_COMMON_


#define OTA_UPGRADE_TEST_FILE_VERSION       0x01010101
#define OTA_UPGRADE_TEST_FILE_VERSION_NEW   0x01020101

#define OTA_UPGRADE_TEST_MANUFACTURER       123

#define OTA_UPGRADE_TEST_IMAGE_TYPE         321

#define OTA_UPGRADE_TEST_IMAGE_SIZE         104

/* The difference between upgrade time and current time */
#define OTA_UPGRADE_TEST_UPGRADE_TIME_DELTA 10

/* The server time offset relative to client time */
#define OTA_UPGRADE_SERVER_TIME_DELTA 20


#define OTA_UPGRADE_TEST_DATA_SIZE          32

#define RTP_OTA_CLI_07_STEP_1_DELAY_ZC (10 * ZB_TIME_ONE_SECOND)
#define RTP_OTA_CLI_07_STEP_1_TIME_ZC  (10  * ZB_TIME_ONE_SECOND)

#endif /* __RTP_OTA_CLI_07_COMMON_ */
