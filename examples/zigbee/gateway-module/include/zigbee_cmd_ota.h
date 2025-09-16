/**
 * @file zigbee_cmd_nwk.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_CMD_OTA_H
#define __ZIGBEE_CMD_OTA_H

#include "stdint.h"

typedef enum {
    GW_CMD_OTA_START = 0xF0000000,
    GW_CMD_OTA_UPLOAD_START_REQUEST = GW_CMD_OTA_START,
    GW_CMD_OTA_BLOCK_REQUEST = 0xF0000001,
    GW_CMD_OTA_FILE_INSERT_REQUEST = 0xF0000003,
    GW_CMD_OTA_FILE_REMOVE_REQUEST = 0xF0000004,
    GW_CMD_OTA_CANDIDATE_SET = 0xF0000005,
    GW_CMD_OTA_CANDIDATE_REMOVE = 0xF0000006,
    GW_CMD_OTA_CANDIDATE_GET = 0xF0000007,
    GW_CMD_OTA_FILE_INFO_REQUEST = 0xF000000A,
    GW_CMD_OTA_END,

    GW_CMD_OTA_UPLOAD_END_RESPONSE = 0xF0008002,
    GW_CMD_OTA_UPGRADE_STATUS_RESPONSE = 0xF0008008,
    GW_CMD_OTA_QUERY_IMAGE_INFO = 0xF0008009,
} ota_cmd_t;

void _gw_ota_cmd_handle(uint32_t cmd_id, uint8_t *pBuf);
void zigbee_gw_ota_cb(uint8_t param);

#endif // __ZIGBEE_CMD_OTA_H