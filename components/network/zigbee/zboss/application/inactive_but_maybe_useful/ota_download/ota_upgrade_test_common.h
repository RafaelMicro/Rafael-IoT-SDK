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

#ifndef ZB_HA_OTA_UPGRADE_TEST_COMMON_H
#define ZB_HA_OTA_UPGRADE_TEST_COMMON_H 1

/* Be compatible with Smart Plug Device test */
#define OTA_UPGRADE_TEST_MANUFACTURER       0xDEAD

#define OTA_UPGRADE_TEST_IMAGE_TYPE         18

/* Device shall use UpgradeTime or RequestTime as an offset time from now. */
/* See 6.8.4 CurrentTime and UpgradeTime/RequestTime Parameters */
#define OTA_UPGRADE_TEST_CURRENT_TIME       0

/* upgrade 10 seconds after current time */
#define OTA_UPGRADE_TEST_UPGRADE_TIME       10

#define OTA_UPGRADE_TEST_DATA_SIZE          32


#endif /* ZB_HA_OTA_UPGRADE_TEST_COMMON_H */
