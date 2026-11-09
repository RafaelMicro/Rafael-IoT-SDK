/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file zigbee_cmd_nwk.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * 
 */
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"
#include "zboss_api_nwk.h"

#include <zigbee_platform.h>
#include "log.h"
#include "hosal_flash.h"
#include "zigbee_api.h"
#include "zigbee_cmd_nwk.h"
#include "zigbee_cmd_app.h"
#include "zigbee_cmd_ota.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define ZB_TRACE_FILE_ID 294

#define OTA_CACHE_SIZE              0x1000
#define OTA_FLASH_PAGE_SIZE         0x100
#define OTA_CRC_LEN                 4
#define OTA_FILE_TOTAL_SIZE_OFFSET  0x34
#define OTA_STATUS_SUCCESS          0x00000000
#define OTA_STATUS_FAIL             0xFFFFFFFF
#define OTA_STATUS_BUSY             0xFFFFFFFE  /* a Zigbee OTA client is downloading */
#define OTA_CLIENT_MAX              8
/* a client that stops requesting blocks without Upgrade End is dropped after this */
#define OTA_CLIENT_IDLE_TIMEOUT_MS  60000
//=============================================================================
//                Private ENUM
//=============================================================================
typedef enum {
    OTA_UPLOAD_IDLE = 0,
    OTA_UPLOAD_STARTED,
    OTA_UPLOAD_DONE,
} ota_upload_state_t;

//=============================================================================
//                Private Struct
//=============================================================================

typedef struct __attribute__((packed))
{
    uint16_t image_type;
    uint16_t manufacturer_code;
    uint32_t file_version;
    uint32_t image_size;
    uint32_t total_pkt;
    uint32_t cur_pkt;
    uint16_t pkt_len;
    uint8_t pkt[];
}
ota_img_info_t;
typedef struct __attribute__((packed)) ota_file_info{
    uint8_t  image_found;
    uint8_t  server_ready;
    uint16_t manufacturer_code;
    uint16_t  image_type;
    uint32_t image_size;
    uint32_t image_version;
} ota_file_info_t;

typedef ZB_PACKED_PRE struct ota_upgrade_test_file_s
{
    zb_zcl_ota_upgrade_file_header_t head;
    uint8_t *pdata;

} ZB_PACKED_STRUCT ota_upgrade_test_file_t;

typedef struct {
    ota_upload_state_t state;
    uint8_t has_expected_crc;
    uint32_t expected_crc;  /* CRC32 of the whole file, from Upload Start */
    uint32_t running_crc;   /* CRC32 of the bytes accepted so far */
    uint32_t next_pkt;
    uint32_t recv_bytes;
    uint32_t cache_len;
    uint32_t flash_addr;
    uint32_t result;        /* status reported in Upload End Response */
} ota_upload_ctx_t;

typedef struct {
    uint8_t used;
    uint16_t short_addr;
    TickType_t last_tick;
} ota_client_t;
//=============================================================================
//                Private Global Variables
//=============================================================================
static ota_img_info_t gt_img_info;
static uint8_t *gp_ota_imgae_cache = NULL;
static ota_upload_ctx_t gt_upload;
/* written from the ZBOSS callback, read from the gateway command handler */
static ota_client_t gt_ota_clients[OTA_CLIENT_MAX];
uint8_t ota_image_ready = 0;
static uint16_t ota_candidate=0x0000;

/* OTA upgrade data */
ota_upgrade_test_file_t ota_file =
{
    {
        ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_ID,         // OTA upgrade file identifier
        ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_VERSION,    // OTA Header version
        0x38,                                           // OTA Header length
        0x10,                                           // OTA Header Field control
        123,                  // Manufacturer code
        321,                    // Image type
        0x01010101,              // File version
        ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO,       // Zigbee Stack version
        /* OTA Header string */
        {
            0x54, 0x68, 0x65, 0x20, 0x6c, 0x61, 0x74, 0x65,   0x73, 0x74, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x67,
            0x72, 0x65, 0x61, 0x74, 0x65, 0x73, 0x74, 0x20,   0x75, 0x70, 0x67, 0x72, 0x61, 0x64, 0x65, 0x2e,
        },
        56,                    // Total Image size (including header)
    },
    NULL,
};
zb_ret_t next_data_ind_cb(zb_uint8_t index, zb_zcl_parsed_hdr_t *zcl_hdr, zb_uint32_t offset,
    zb_uint8_t size, zb_uint8_t **data)
{
    /* size is chosen by the OTA client, up to 255 */
    static uint8_t temp_buf[UINT8_MAX];
    for(uint8_t i = 0; i < size; i++) {
        temp_buf[i] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS + offset + i);
        while (flash_check_busy()) {}
    }
    *data = temp_buf;
    return RET_OK;
}
//=============================================================================
//                Function
//=============================================================================
uint32_t crc32checksum(uint32_t flash_addr, uint32_t data_len) {
    uint32_t i;
    uint16_t j, k;
    uint32_t ChkSum = ~0;
    uint8_t Read;

    for (i = 0; i < data_len; i++) {
        //get 8 bits at one time
        Read = flash_read_byte((flash_addr + i));
        while (flash_check_busy()) {}
        //get the CRC of 8 bits
        ChkSum ^= Read;
        for (k = 0; k < 8; k++) {
            ChkSum = (ChkSum & 1) ? (ChkSum >> 1) ^ 0xedb88320
                                    : ChkSum >> 1;
        }
    }
    ChkSum = ~ChkSum;
    return ChkSum;
}

static void prv_ota_client_touch(uint16_t short_addr)
{
    ota_client_t *p_slot = &gt_ota_clients[0];
    TickType_t now = xTaskGetTickCount();

    taskENTER_CRITICAL();
    for (int i = 0; i < OTA_CLIENT_MAX; i++) {
        if (gt_ota_clients[i].used && gt_ota_clients[i].short_addr == short_addr) {
            p_slot = &gt_ota_clients[i];
            break;
        }
        /* otherwise take a free slot, or the least recently active one */
        if (!gt_ota_clients[i].used) {
            if (p_slot->used) {
                p_slot = &gt_ota_clients[i];
            }
        } else if (p_slot->used
                   && (now - gt_ota_clients[i].last_tick) > (now - p_slot->last_tick)) {
            p_slot = &gt_ota_clients[i];
        }
    }
    p_slot->used = 1;
    p_slot->short_addr = short_addr;
    p_slot->last_tick = now;
    taskEXIT_CRITICAL();
}

static void prv_ota_client_remove(uint16_t short_addr)
{
    taskENTER_CRITICAL();
    for (int i = 0; i < OTA_CLIENT_MAX; i++) {
        if (gt_ota_clients[i].used && gt_ota_clients[i].short_addr == short_addr) {
            gt_ota_clients[i].used = 0;
        }
    }
    taskEXIT_CRITICAL();
}

static void prv_ota_client_clear(void)
{
    taskENTER_CRITICAL();
    memset(gt_ota_clients, 0, sizeof(gt_ota_clients));
    taskEXIT_CRITICAL();
}

/* true while any Zigbee client is downloading the inserted image */
static uint8_t prv_ota_client_busy(void)
{
    uint8_t busy = 0;
    TickType_t now = xTaskGetTickCount();

    taskENTER_CRITICAL();
    for (int i = 0; i < OTA_CLIENT_MAX; i++) {
        if (!gt_ota_clients[i].used) {
            continue;
        }
        if ((now - gt_ota_clients[i].last_tick) > pdMS_TO_TICKS(OTA_CLIENT_IDLE_TIMEOUT_MS)) {
            gt_ota_clients[i].used = 0;
        } else {
            busy = 1;
        }
    }
    taskEXIT_CRITICAL();
    return busy;
}

void insert_ota_file(zb_uint8_t param)
{
    zb_ret_t ret;
    ZB_ZCL_OTA_UPGRADE_INSERT_FILE(param, 1, 0, (zb_uint8_t *)(&ota_file), 0, ZB_TRUE, ret);
    ota_image_ready = 1;
}
void remove_ota_file(zb_uint8_t param)
{
    zb_ret_t ret;
    uint8_t ota_ep = get_endpoint_by_cluster(ZB_ZCL_CLUSTER_ID_OTA_UPGRADE, ZB_ZCL_CLUSTER_SERVER_ROLE);
    ZB_ZCL_OTA_UPGRADE_REMOVE_FILE(param, ota_ep, 0, ret);
    ota_image_ready = 0;
    /* the file is gone, so are the downloads using it */
    prv_ota_client_clear();

    zigbee_gw_cmd_send((GW_CMD_OTA_FILE_REMOVE_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&ret, 4);
}

static void _check_ota_file_valid(ota_file_info_t *file_info)
{
    uint8_t tmp[4];
    uint32_t crc32=0, crcread=0;

    //read the ota image file size
    tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+0x34);
    tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+0x35);
    tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+0x36);
    tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+0x37);
    memcpy(&file_info->image_size, tmp, 4);
    while (flash_check_busy()) {} ;

    // image + appended CRC32 must fit in the FOTA bank
    if(file_info->image_size <= SIZE_OF_FOTA_BANK - OTA_CRC_LEN)
    {  
        crc32 = crc32checksum(FOTA_UPDATE_BUFFER_FW_ADDRESS, file_info->image_size);
        gt_img_info.image_size = file_info->image_size;
        log_info("image_size: 0x%08X", file_info->image_size);
        log_info("crc32: 0x%08X", crc32);

        tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+file_info->image_size+0);
        tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+file_info->image_size+1);
        tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+file_info->image_size+2);
        tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+file_info->image_size+3);
        memcpy(&crcread, tmp, 4);
        while (flash_check_busy()) {};
        log_info("crc32 in flash: 0x%08X", crcread);

        if(crc32 == crcread) 
        {
            file_info->image_found = 1;
            file_info->server_ready = ota_image_ready;
            //read the ota image manufacturer_code
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+10);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+11);
            memcpy(&file_info->manufacturer_code, tmp, 2);
            gt_img_info.manufacturer_code = file_info->manufacturer_code;
            log_info("manufacturer_code: 0x%04X", file_info->manufacturer_code);

            //read the ota image type
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+12);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+13);
            memcpy(&file_info->image_type, tmp, 2);
            gt_img_info.image_type = file_info->image_type;
            log_info("image_type: 0x%04X", file_info->image_type);

            //read the ota image file_version
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+14);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+15);
            tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+16);
            tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS+17);
            memcpy(&file_info->image_version, tmp, 4);
            gt_img_info.file_version = file_info->image_version;
            log_info("file_version: 0x%08X", file_info->image_version);

            while (flash_check_busy()) {};
        }
    }
    else
    {
        log_info("No OTA image existed!");
    }
    
}
void _ota_file_insert(uint32_t file_addr, uint32_t file_size, uint32_t file_version, uint16_t image_type, uint16_t manufacturer_code)
{
    zb_bufid_t buf;
    do{
        ota_file.head.manufacturer_code = manufacturer_code;
        ota_file.head.image_type = image_type;
        ota_file.head.file_version = file_version;
        ota_file.head.total_image_size = file_size;
        ota_file.pdata = (uint8_t *)file_addr;

        buf = zb_buf_get_out();
        ZB_SCHEDULE_APP_CALLBACK(insert_ota_file, buf);
    } while (0);
}
static void _ota_file_remove(void)
{
    zb_bufid_t buf;
    do
    {
        buf = zb_buf_get_out();
        ZB_SCHEDULE_APP_CALLBACK(remove_ota_file, buf);
    } while (0);
}

static void _ota_candidate_set(uint16_t  candidate)
{
    uint8_t status = 0;
    ota_candidate = candidate;
    log_info("Set OTA Candidate 0x%04x", candidate);
    zigbee_gw_cmd_send((GW_CMD_OTA_CANDIDATE_SET | 0x8000), 0x0000, 0, 0,
                        &status, 1);
}

static void _ota_candidate_remove(void)
{
    uint8_t status = 0;
    ota_candidate = 0;
    log_info("Remove OTA Candidate");
    zigbee_gw_cmd_send((GW_CMD_OTA_CANDIDATE_REMOVE | 0x8000), 0x0000, 0, 0,
                        &status, 1);
}

static void _ota_candidate_get(void)
{
    log_info("Get OTA Candidate 0x%04x", ota_candidate);
    zigbee_gw_cmd_send((GW_CMD_OTA_CANDIDATE_GET | 0x8000), 0x0000, 0, 0,
                        (void*)&ota_candidate, 2);
}
static void _ota_file_info_get(void)
{
    ota_file_info_t file_info;

    memset(&file_info, 0, sizeof(ota_file_info_t));
    _check_ota_file_valid(&file_info);

    if(file_info.image_found != 1) {
    memset(&file_info, 0, sizeof(ota_file_info_t));
    }

    zigbee_gw_cmd_send((GW_CMD_OTA_FILE_INFO_REQUEST | 0x8000), 0x0000, 0, 0,
                            (uint8_t *)&file_info, sizeof(ota_file_info_t));
}

static inline uint8_t progress_percent(uint32_t offset,
    uint32_t total_length)
{
    if (total_length == 0) {
    return 0xFF;
    }

    uint32_t percent = offset * 100u / total_length;
    if (percent > 100u) {
    percent = 100u;
    }

    static uint8_t last_percent = 0xFFu;

    if (percent != last_percent) {
    last_percent = (uint8_t)percent;
    return last_percent;
    }

    return 0xFFu;
}

void zigbee_gw_ota_cb(uint8_t param) {
    uint8_t data[10];
    uint8_t progress = 0;
    static uint8_t count=0;
    zb_zcl_device_callback_param_t *device_cb_param = ZB_BUF_GET_PARAM(param, zb_zcl_device_callback_param_t);
    switch (device_cb_param->device_cb_id)
    {
        case ZB_ZCL_OTA_UPGRADE_SRV_QUERY_IMAGE_CB_ID:
        {
            zb_zcl_ota_upgrade_srv_query_img_param_t *value = &device_cb_param->cb_param.ota_upgrade_srv_query_img_param;
            if((value->zcl_addr->u.short_addr == ota_candidate || ota_candidate == 0xFFFF) && ota_image_ready==1)
            {
                device_cb_param->status = RET_OK;
            }
            else
                device_cb_param->status = RET_DEVICE_NOT_FOUND;
            
            memcpy(data, &value->zcl_addr->u.short_addr, 2);
            memcpy(data+2, &value->manufacturer_code, 2);
            memcpy(data+4, &value->image_type, 2);
            memcpy(data+6, &value->version, 4);
            zigbee_gw_cmd_send(GW_CMD_OTA_QUERY_IMAGE_INFO, 0x0000, 0, 0, &data[0], 10);
        }
        break;
        case ZB_ZCL_OTA_UPGRADE_SRV_IMAGE_BLOCK_CB_ID:
        {
            zb_zcl_ota_upgrade_srv_image_block_param_t *value1 = &device_cb_param->cb_param.ota_upgrade_srv_image_block_param;
            prv_ota_client_touch(value1->zcl_addr->u.short_addr);
            /* OTA server process image block upgrade */
            if(count==50)
            {
                progress = progress_percent(value1->file_offset, gt_img_info.image_size);
                if(progress != 0xFF)
                {   
                    //data[0~1]: device short address
                    //data[2]:   status, 0:update progress in percentage
                    //                   1:update finished & successed
                    //                   2:update aborted
                    //data[3]:   progress in percent when "status" is 0 or 1, not valid for status = 2 
                    memcpy(data, &value1->zcl_addr->u.short_addr, 2);
                    data[2] = 0;
                    data[3] = progress;
                    
                    zigbee_gw_cmd_send(GW_CMD_OTA_UPGRADE_STATUS_RESPONSE, 0x0000, 0, 0, &data[0], 4);
                }
                count=0;
            }
            else
                count++; 
        }
        break;        
        case ZB_ZCL_OTA_UPGRADE_SRV_UPGRADE_ABORTED_CB_ID:
        {
            zb_zcl_ota_upgrade_srv_upgrade_aborted_param_t *value2 = &device_cb_param->cb_param.ota_upgrade_srv_upgrade_aborted_param;
            prv_ota_client_remove(value2->zcl_addr->u.short_addr);
            /* OTA server process upgrade aborted */
            //data[0~1]: device short address
            //data[2]:   status, 0:update progress in percentage
            //                   1:update finished & successed
            //                   2:server update aborted
            //                   3:client update aborted
            //data[3]:   progress in percent when "status" is 0 or 1, not valid for status is 2 or 3  
            memcpy(data, &value2->zcl_addr->u.short_addr, 2);
            data[2] = 2; //aborted
            data[3] = 0xFF;
            
            zigbee_gw_cmd_send(GW_CMD_OTA_UPGRADE_STATUS_RESPONSE, 0x0000, 0, 0, &data[0], 4);
        }
        break;
        case ZB_ZCL_OTA_UPGRADE_SRV_UPGRADE_END_CB_ID:
        {
            /* OTA server process upgrade end */
            zb_zcl_ota_upgrade_srv_upgrade_end_param_t *value3 = &device_cb_param->cb_param.ota_upgrade_srv_upgrade_end_param;
            /* the client is done with the image, whatever the status */
            prv_ota_client_remove(value3->zcl_addr->u.short_addr);
            memcpy(data, &value3->zcl_addr->u.short_addr, 2);

            if(value3->status == 0x00)
            {
                data[2] = 1; //success
                data[3] = 0x64; //100% finished
                
                zigbee_gw_cmd_send(GW_CMD_OTA_UPGRADE_STATUS_RESPONSE, 0x0000, 0, 0, &data[0], 4);
            }
            if(value3->status == 0x95)
            {
                data[2] = 3; //client aborted
                data[3] = 0xFF; 
                
                zigbee_gw_cmd_send(GW_CMD_OTA_UPGRADE_STATUS_RESPONSE, 0x0000, 0, 0, &data[0], 4);
            }
        }
        break;
    }
}
static uint32_t prv_crc32_update(uint32_t crc, const uint8_t *p_data, uint32_t len)
{
    /* same polynomial as crc32checksum(); crc starts at ~0 and is inverted at the end */
    while (len--) {
        crc ^= *p_data++;
        for (int k = 0; k < 8; k++) {
            crc = (crc & 1) ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
        }
    }
    return crc;
}

static uint32_t prv_flash_read_u32(uint32_t flash_addr)
{
    uint8_t tmp[4];
    uint32_t value;

    for (int i = 0; i < 4; i++) {
        tmp[i] = flash_read_byte(flash_addr + i);
        while (flash_check_busy()) {}
    }
    memcpy(&value, tmp, 4);
    return value;
}

static void prv_ota_rsp(uint32_t cmd_id, uint32_t status)
{
    zigbee_gw_cmd_send(cmd_id, 0, 0, 0, (uint8_t *)&status, 4);
}

static void prv_ota_cache_free(void)
{
    if (gp_ota_imgae_cache) {
        vPortFree(gp_ota_imgae_cache);
        gp_ota_imgae_cache = NULL;
    }
}

static int prv_ota_cache_flush(void)
{
    uint32_t pages = (gt_upload.cache_len + OTA_FLASH_PAGE_SIZE - 1) / OTA_FLASH_PAGE_SIZE;

    if (gt_upload.flash_addr + pages * OTA_FLASH_PAGE_SIZE
        > FOTA_UPDATE_BUFFER_FW_ADDRESS + SIZE_OF_FOTA_BANK) {
        log_error("OTA flash write out of FOTA bank");
        return -1;
    }

    /* unused tail of the last page stays erased */
    memset(&gp_ota_imgae_cache[gt_upload.cache_len], 0xFF,
           pages * OTA_FLASH_PAGE_SIZE - gt_upload.cache_len);

    // page program (256 bytes)
    for (uint32_t i = 0; i < pages; i++) {
        while (flash_check_busy());
        flash_write_page((uint32_t)&gp_ota_imgae_cache[i * OTA_FLASH_PAGE_SIZE], gt_upload.flash_addr);
        gt_upload.flash_addr += OTA_FLASH_PAGE_SIZE;
    }
    while (flash_check_busy());
    gt_upload.cache_len = 0;
    return 0;
}

static int prv_ota_cache_put(const uint8_t *p_data, uint32_t len)
{
    while (len) {
        uint32_t n = OTA_CACHE_SIZE - gt_upload.cache_len;

        if (n > len) {
            n = len;
        }
        memcpy(&gp_ota_imgae_cache[gt_upload.cache_len], p_data, n);
        gt_upload.cache_len += n;
        p_data += n;
        len -= n;

        if (gt_upload.cache_len == OTA_CACHE_SIZE && prv_ota_cache_flush() != 0) {
            return -1;
        }
    }
    return 0;
}

static uint32_t prv_ota_upload_finish(void)
{
    uint32_t crc32 = ~gt_upload.running_crc;

    log_info("OTA upload crc32: 0x%08X", crc32);

    if (gt_upload.recv_bytes != gt_img_info.image_size) {
        log_error("OTA size mismatch, recv %u, expect %u", gt_upload.recv_bytes, gt_img_info.image_size);
        return OTA_STATUS_FAIL;
    }

    if (gt_upload.has_expected_crc && crc32 != gt_upload.expected_crc) {
        log_error("OTA crc32 mismatch, expect 0x%08X", gt_upload.expected_crc);
        return OTA_STATUS_FAIL;
    }

    /* the CRC32 appended after the image marks it valid for _check_ota_file_valid() */
    if (prv_ota_cache_put((uint8_t *)&crc32, OTA_CRC_LEN) != 0
        || prv_ota_cache_flush() != 0) {
        return OTA_STATUS_FAIL;
    }
    flush_cache();

    if (prv_flash_read_u32(FOTA_UPDATE_BUFFER_FW_ADDRESS) != ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FILE_ID
        || prv_flash_read_u32(FOTA_UPDATE_BUFFER_FW_ADDRESS + OTA_FILE_TOTAL_SIZE_OFFSET) != gt_img_info.image_size) {
        log_error("OTA file header invalid");
        return OTA_STATUS_FAIL;
    }

    /* read back what was programmed */
    if (crc32checksum(FOTA_UPDATE_BUFFER_FW_ADDRESS, gt_img_info.image_size) != crc32
        || prv_flash_read_u32(FOTA_UPDATE_BUFFER_FW_ADDRESS + gt_img_info.image_size) != crc32) {
        log_error("OTA flash verify failed");
        return OTA_STATUS_FAIL;
    }

    return OTA_STATUS_SUCCESS;
}

static void prv_ota_upload_start(uint8_t *pBuf, uint16_t len)
{
    /* erasing the bank would corrupt the image a client is downloading */
    if (prv_ota_client_busy()) {
        log_warn("OTA in progress, reject upload start");
        prv_ota_rsp((GW_CMD_OTA_UPLOAD_START_REQUEST | 0x8000), OTA_STATUS_BUSY);
        return;
    }

    prv_ota_cache_free();
    memset(&gt_upload, 0, sizeof(gt_upload));
    gt_upload.flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS;
    gt_upload.running_crc = ~0;

    /* optional parameter: CRC32 of the whole OTA file */
    if (len >= 4) {
        memcpy(&gt_upload.expected_crc, pBuf, 4);
        gt_upload.has_expected_crc = 1;
        log_info("OTA expected crc32: 0x%08X", gt_upload.expected_crc);
    }

    /* the inserted image is about to be erased: stop accepting new clients
     * until a new image is inserted */
    ota_image_ready = 0;

    for (int i = 0; i < SIZE_OF_FOTA_BANK / 0x1000; i++)
    {
        // Page erase (4096 bytes)
        while (flash_check_busy());
        taskENTER_CRITICAL();
        flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BUFFER_FW_ADDRESS + (0x1000 * i));
        taskEXIT_CRITICAL();
    }
    while (flash_check_busy());

    gt_upload.state = OTA_UPLOAD_STARTED;
    prv_ota_rsp((GW_CMD_OTA_UPLOAD_START_REQUEST | 0x8000), OTA_STATUS_SUCCESS);
}

static void prv_ota_upload_block(uint8_t *pBuf, uint16_t len)
{
    ota_img_info_t hdr;
    const uint8_t *p_data = pBuf + sizeof(ota_img_info_t);

    if (len < sizeof(ota_img_info_t)) {
        goto nack;
    }
    memcpy(&hdr, pBuf, sizeof(ota_img_info_t));

    if (hdr.pkt_len != len - sizeof(ota_img_info_t)) {
        log_error("OTA block length mismatch");
        goto nack;
    }

    /* host did not get the Upload End Response, report the result again */
    if (gt_upload.state == OTA_UPLOAD_DONE && hdr.cur_pkt + 1 == gt_img_info.total_pkt) {
        prv_ota_rsp(GW_CMD_OTA_UPLOAD_END_RESPONSE, gt_upload.result);
        return;
    }

    if (gt_upload.state != OTA_UPLOAD_STARTED) {
        log_error("OTA block without upload start");
        goto nack;
    }

    if (prv_ota_client_busy()) {
        log_warn("OTA in progress, reject block %u", hdr.cur_pkt);
        prv_ota_rsp((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), OTA_STATUS_BUSY);
        return;
    }

    /* host did not get the block ack and resent it: ack again, do not write */
    if (hdr.cur_pkt + 1 == gt_upload.next_pkt) {
        prv_ota_rsp((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), hdr.cur_pkt);
        return;
    }

    if (hdr.cur_pkt != gt_upload.next_pkt) {
        log_error("OTA block %u out of sequence, expect %u", hdr.cur_pkt, gt_upload.next_pkt);
        goto nack;
    }

    if (hdr.cur_pkt == 0) {
        if (hdr.total_pkt == 0 || hdr.image_size == 0
            || hdr.image_size > SIZE_OF_FOTA_BANK - OTA_CRC_LEN) {
            log_error("OTA image info invalid, size 0x%X", hdr.image_size);
            goto nack;
        }
        memcpy((uint8_t *)&gt_img_info, &hdr, sizeof(gt_img_info));

        log_info("File Type: 0x%X", gt_img_info.image_type);
        log_info("Manufacturer Code: 0x%X", gt_img_info.manufacturer_code);
        log_info("File Version: 0x%X", gt_img_info.file_version);
        log_info("File Size: 0x%X", gt_img_info.image_size);
    } else if (hdr.image_size != gt_img_info.image_size || hdr.total_pkt != gt_img_info.total_pkt) {
        log_error("OTA block %u image info changed", hdr.cur_pkt);
        goto nack;
    }

    if (gt_upload.recv_bytes + hdr.pkt_len > gt_img_info.image_size) {
        log_error("OTA block %u exceeds image size", hdr.cur_pkt);
        goto nack;
    }

    if (!gp_ota_imgae_cache) {
        gp_ota_imgae_cache = pvPortMalloc(OTA_CACHE_SIZE);
        if (!gp_ota_imgae_cache) {
            goto nack;
        }
    }

    if (prv_ota_cache_put(p_data, hdr.pkt_len) != 0) {
        goto abort;
    }
    gt_upload.running_crc = prv_crc32_update(gt_upload.running_crc, p_data, hdr.pkt_len);
    gt_upload.recv_bytes += hdr.pkt_len;
    gt_upload.next_pkt++;

    if (hdr.cur_pkt + 1 == gt_img_info.total_pkt) {
        gt_upload.result = prv_ota_upload_finish();
        gt_upload.state = OTA_UPLOAD_DONE;
        prv_ota_cache_free();
        prv_ota_rsp(GW_CMD_OTA_UPLOAD_END_RESPONSE, gt_upload.result);
    } else {
        prv_ota_rsp((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), hdr.cur_pkt);
    }
    return;

abort:
    /* flash state is unknown, host has to restart from Upload Start */
    gt_upload.state = OTA_UPLOAD_IDLE;
    prv_ota_cache_free();
nack:
    prv_ota_rsp((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), OTA_STATUS_FAIL);
}

void _gw_ota_cmd_handle(uint32_t cmd_id, uint8_t *pBuf, uint16_t len)
{
    if (cmd_id == GW_CMD_OTA_UPLOAD_START_REQUEST)
    {
        prv_ota_upload_start(pBuf, len);
    }
    else if (cmd_id == GW_CMD_OTA_BLOCK_REQUEST)
    {
        prv_ota_upload_block(pBuf, len);
    }
    else if (cmd_id == GW_CMD_OTA_FILE_INSERT_REQUEST)
    {
        ota_file_info_t file_info;

        if (prv_ota_client_busy() || gt_upload.state == OTA_UPLOAD_STARTED) {
            zb_ret_t ret = OTA_STATUS_BUSY;
            log_warn("OTA in progress, reject file insert");
            zigbee_gw_cmd_send((GW_CMD_OTA_FILE_INSERT_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&ret, 4);
            return;
        }

        memset(&file_info, 0, sizeof(ota_file_info_t));
        _check_ota_file_valid(&file_info);
        if(file_info.image_found) {
            zb_ret_t ret = 0x00000000;
            log_info("insert ota file");
            _ota_file_insert(FOTA_UPDATE_BUFFER_FW_ADDRESS, gt_img_info.image_size + 4, gt_img_info.file_version, gt_img_info.image_type, gt_img_info.manufacturer_code);
            zigbee_gw_cmd_send((GW_CMD_OTA_FILE_INSERT_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&ret, 4);
        }
        else
        {
            zb_ret_t ret = 0xFFFFFFFF;
            zigbee_gw_cmd_send((GW_CMD_OTA_FILE_INSERT_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&ret, 4);
        }
    }
    else if (cmd_id == GW_CMD_OTA_FILE_REMOVE_REQUEST)
    {
        _ota_file_remove();
    }
    else if (cmd_id == GW_CMD_OTA_CANDIDATE_SET)
    {
        uint16_t candidate;

        if (len < sizeof(candidate)) {
            return;
        }
        memcpy(&candidate, pBuf, sizeof(candidate));
        _ota_candidate_set(candidate);
    }
    else if (cmd_id == GW_CMD_OTA_CANDIDATE_REMOVE)
    {
        _ota_candidate_remove();
    }
    else if (cmd_id == GW_CMD_OTA_CANDIDATE_GET)
    {
        _ota_candidate_get();
    }
    else if (cmd_id == GW_CMD_OTA_FILE_INFO_REQUEST)
    {
        _ota_file_info_get();
    }
}
