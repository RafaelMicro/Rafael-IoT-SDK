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
/* PURPOSE: HA OTA Upgrade test common definition
*/

#ifndef __RTP_OTA_CLI_03_COMMON_
#define __RTP_OTA_CLI_03_COMMON_

#define TEST_ENDPOINT_DUT                   10
#define TEST_ENDPOINT_TH                    5

#define TEST_DUT_WAIT_TIME                  (3 * ZB_TIME_ONE_SECOND)

#define OTA_UPGRADE_TEST_FILE_VERSION       0x01010101
#define OTA_UPGRADE_TEST_FILE_VERSION_NEW   0x01020101

#define OTA_UPGRADE_TEST_MANUFACTURER       123

#define OTA_UPGRADE_TEST_IMAGE_TYPE         321

#define OTA_UPGRADE_TEST_IMAGE_SIZE         72

#define OTA_UPGRADE_TEST_CURRENT_TIME       0x12345678

#define OTA_UPGRADE_TEST_UPGRADE_TIME       0x12345978

#define OTA_UPGRADE_TEST_DATA_SIZE          32

#define RTP_OTA_CLI_03_STEP_1_DELAY_ZC (7 * ZB_TIME_ONE_SECOND)
#define RTP_OTA_CLI_03_STEP_1_TIME_ZC  (10  * ZB_TIME_ONE_SECOND)

#endif /* __RTP_OTA_CLI_03_COMMON_ */
