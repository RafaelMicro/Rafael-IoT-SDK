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
/* PURPOSE: General IAS zone device configuration
*/

#ifndef IZS_CONFIG_H
#define IZS_CONFIG_H 1

#define IZS_DEFAULT_ENROLLMENT_METHOD      ZB_ZCL_WWAH_ENROLLMENT_MODE_AUTO_ENROLL_REQUEST

/* Basic cluster attributes data */
#define IZS_INIT_BASIC_APP_VERSION         1
#define IZS_INIT_BASIC_STACK_VERSION       0
#define IZS_INIT_BASIC_HW_VERSION          0
#define MAP_16BITVERSION_TO_8BITVERSION(a) ((zb_uint8_t) ( ((a & 0x0F00) >> 4) | (a & 0x000F)) )
#define IZS_DEVICE_MANUFACTURER_CODE       0xDEAD   /* DSR custom manufacture code*/

#define IZS_INIT_BASIC_MANUF_NAME	       "DSR"

#define IZS_INIT_BASIC_DEFAULT_MODEL_ID    "DSR_IZS"
#define IZS_INIT_BASIC_DATE_CODE           "2017-02-01"
#define IZS_INIT_BASIC_LOCATION_ID         "Unknown"
#define IZS_INIT_BASIC_PH_ENV			   0

/* OTA data */
#define IZS_INIT_OTA_HW_VERSION            IZS_INIT_BASIC_HW_VERSION
#define IZS_OTA_IMAGE_BLOCK_DATA_SIZE_MAX  32
#define IZS_OTA_UPGRADE_SERVER             { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }
#define IZS_INIT_OTA_FILE_VERSION          ( ((zb_uint32_t)IZS_INIT_BASIC_APP_VERSION<<16) | (IZS_INIT_BASIC_STACK_VERSION) )
#define IZS_INIT_OTA_MANUFACTURER          IZS_DEVICE_MANUFACTURER_CODE
#define IZS_INIT_OTA_IMAGE_TYPE            1
#define IZS_INIT_OTA_MIN_BLOCK_REQUE       10
#define IZS_INIT_OTA_IMAGE_STAMP           ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE
#define IZS_OTA_UPGARDE_HASH_LENGTH        16

/* Used endpoint number */
#define IZS_DEVICE_ENDPOINT                5
/* IAS zone IEEE address */
#define IZS_DEFAULT_EXTENDED_ADDRESS       {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}
/* Default channel */
#define IZS_DEFAULT_APS_CHANNEL_MASK       (1l<<21) /* ZB_TRANSCEIVER_ALL_CHANNELS_MASK */

#endif  /* IZS_CONFIG_H */
