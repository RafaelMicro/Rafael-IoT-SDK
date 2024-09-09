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
/* PURPOSE:
*/

#ifndef ZB_DL_DEVICE_OTA_H
#define ZB_DL_DEVICE_OTA_H 1

/* OTA Manufacturer code */
#define DL_DEVICE_MANUFACTURER_CODE  0x1234   /* DSR SmartPlug manufacture code*/
/* ZBOSS SDK for Smart plug application - from zb_ver_sdk_type.h */
#define ZBOSS_SDK_SMART_PLUG_MAJOR 3

/* Basic cluster attributes data */
#define DL_INIT_BASIC_STACK_VERSION     ZBOSS_MAJOR

#define DL_INIT_BASIC_APP_VERSION       1

/* OTA Upgrade client cluster attributes data */
#define DL_INIT_OTA_FILE_VERSION	\
  ( ((zb_uint32_t)ZBOSS_MAJOR) | ((zb_uint32_t)ZBOSS_MINOR << 8) | ((zb_uint32_t)ZBOSS_SDK_SMART_PLUG_MAJOR << 16) | ((zb_uint32_t)DL_INIT_BASIC_APP_VERSION << 24) )
#define DL_INIT_OTA_HW_VERSION			DL_INIT_BASIC_HW_VERSION
#define DL_INIT_OTA_MANUFACTURER		DL_DEVICE_MANUFACTURER_CODE

#define DL_INIT_OTA_IMAGE_TYPE			0x0012

#define DL_INIT_OTA_MIN_BLOCK_REQUE             10
#define DL_INIT_OTA_IMAGE_STAMP			ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE
#define DL_OTA_IMAGE_BLOCK_DATA_SIZE_MAX        32
#define DL_OTA_UPGRADE_SERVER                   { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }
#define DL_OTA_UPGRADE_QUERY_TIMER_COUNTER      (12*60)

#define DL_OTA_UPGARDE_HASH_LENGTH              16
#define DL_OTA_DEVICE_RESET_TIMEOUT             30*ZB_TIME_ONE_SECOND

/* OTA Upgrade client cluster attributes data */
typedef struct dl_device_ota_attr_s
{
  zb_ieee_addr_t upgrade_server;
  zb_uint32_t file_offset;
  zb_uint32_t file_version;
  zb_uint16_t stack_version;
  zb_uint32_t downloaded_file_ver;
  zb_uint16_t downloaded_stack_ver;
  zb_uint8_t image_status;
  zb_uint16_t manufacturer;
  zb_uint16_t image_type;
  zb_uint16_t min_block_reque;
  zb_uint16_t image_stamp;
  zb_uint16_t server_addr;
  zb_uint8_t server_ep;
} dl_device_ota_attr_t;

typedef struct dl_ota_upgrade_ctx_s
{
  zb_uint32_t total_image_size;
  zb_uint32_t addr_to_erase;
  zb_uint32_t address;          /* Supposed to be constant value, init
                                 * on OTA Upgrade start  */
  void       *flash_dev;
  zb_uint32_t fw_version;
  zb_uint8_t param;     // buffer, contain process command (if sheduling process)
  zb_bool_t is_started_manually;
#ifndef ZB_USE_OSIF_OTA_ROUTINES
  zb_uint8_t fw_image_portion[DL_OTA_IMAGE_BLOCK_DATA_SIZE_MAX * 2];
  zb_uint32_t fw_image_portion_size;
  zb_uint32_t file_length;        /*!< OTA file length got from next_image_resp  */

  zb_uint32_t hash_addr;
  zb_uint8_t hash[DL_OTA_UPGARDE_HASH_LENGTH];
  zb_bool_t hash16_calc_ongoing;
#endif
} dl_ota_upgrade_ctx_t;

/*** OTA API ***/
void dl_process_ota_upgrade_cb(zb_uint8_t param);
void dl_ota_start_upgrade(zb_uint8_t param);

#endif /* ZB_DL_DEVICE_OTA_H */
