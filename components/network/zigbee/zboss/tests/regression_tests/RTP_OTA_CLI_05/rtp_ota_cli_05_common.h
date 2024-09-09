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

#ifndef __RTP_OTA_CLI_05_COMMON_
#define __RTP_OTA_CLI_05_COMMON_

#define IEEE_ADDR_TH_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TEST_ENDPOINT_DUT                   10
#define TEST_ENDPOINT_TH                    5

#define TEST_DUT_WAIT_TIME                  (3 * ZB_TIME_ONE_SECOND)

#define OTA_UPGRADE_TEST_CLIENT_HW_VERSION  0x80

#define OTA_UPGRADE_TEST_ZC_STEPS_COUNT 5

/* Min and max hardware sizes for each test step */
#define OTA_UPGRADE_TEST_ZC_HW_SIZES {                                                   \
  /* Step 1: client HW version < min hw version */                                       \
  { OTA_UPGRADE_TEST_CLIENT_HW_VERSION + 5, OTA_UPGRADE_TEST_CLIENT_HW_VERSION + 10 },   \
                                                                                         \
  /* Step 2: client HW version = min hw version */                                       \
  { OTA_UPGRADE_TEST_CLIENT_HW_VERSION, OTA_UPGRADE_TEST_CLIENT_HW_VERSION + 10 },       \
                                                                                         \
  /* Step 3: client HW version > min hw version and < max_hw_version */                  \
  { OTA_UPGRADE_TEST_CLIENT_HW_VERSION - 10, OTA_UPGRADE_TEST_CLIENT_HW_VERSION + 10 },  \
                                                                                         \
  /* Step 4: client HW version = max_hw_version */                                       \
  { OTA_UPGRADE_TEST_CLIENT_HW_VERSION - 10, OTA_UPGRADE_TEST_CLIENT_HW_VERSION },       \
                                                                                         \
  /* Step 5: client HW version > max_hw_version */                                       \
  { OTA_UPGRADE_TEST_CLIENT_HW_VERSION - 20, OTA_UPGRADE_TEST_CLIENT_HW_VERSION - 10 },  \
};

#define OTA_UPGRADE_TEST_FILE_VERSION       0x01010101
#define OTA_UPGRADE_TEST_FILE_VERSION_NEW   0x01020101

#define OTA_UPGRADE_TEST_MANUFACTURER       123

#define OTA_UPGRADE_TEST_IMAGE_TYPE         321

#define OTA_UPGRADE_TEST_CURRENT_TIME       0x12345678
#define OTA_UPGRADE_TEST_UPGRADE_TIME       0x12345678

#define OTA_UPGRADE_TEST_DATA_SIZE          32

#define RTP_OTA_CLI_05_STEP_1_DELAY_ZC (12 * ZB_TIME_ONE_SECOND)
#define RTP_OTA_CLI_05_INSERT_TIME_ZC  (7  * ZB_TIME_ONE_SECOND)
#define RTP_OTA_CLI_05_REMOVE_TIME_ZC  (1  * ZB_TIME_ONE_SECOND)

#endif /* __RTP_OTA_CLI_05_COMMON_ */
