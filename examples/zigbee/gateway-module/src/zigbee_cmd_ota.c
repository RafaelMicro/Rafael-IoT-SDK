/**
 * @file zigbee_cmd_nwk.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"
#include "zboss_api_nwk.h"

#include <zigbee_platform.h>
#include "log.h"
#include "zigbee_api.h"
#include "zigbee_cmd_nwk.h"
#include "zigbee_cmd_app.h"
#include "zigbee_cmd_ota.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define ZB_TRACE_FILE_ID 294
//=============================================================================
//                Private ENUM
//=============================================================================

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
//=============================================================================
//                Private Global Variables
//=============================================================================
static ota_img_info_t gt_img_info;
static uint8_t *gp_ota_imgae_cache = NULL;
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
};
zb_ret_t next_data_ind_cb(zb_uint8_t index, zb_zcl_parsed_hdr_t *zcl_hdr, zb_uint32_t offset,
    zb_uint8_t size, zb_uint8_t **data)
{
    *data = ((ota_file.pdata) + offset);
    return RET_OK;
}
//=============================================================================
//                Function
//=============================================================================
static uint32_t crc32checksum(uint32_t flash_addr, uint32_t data_len)
{
    uint16_t k;
    uint32_t i;
    uint8_t *buf = ((uint8_t *)flash_addr);
    uint32_t chkSum = ~0, len = data_len;

    for (i = 0; i < len; i ++ )
    {
        chkSum ^= *buf++;
        for (k = 0; k < 8; k ++)
        {
            chkSum = chkSum & 1 ? (chkSum >> 1) ^ 0xedb88320 : chkSum >> 1;
        }
    }
    return ~chkSum;
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

    zigbee_gw_cmd_send((GW_CMD_OTA_FILE_REMOVE_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&ret, 4);
}

static void _check_ota_file_valid(ota_file_info_t *file_info)
{
    uint8_t tmp[4];
    uint32_t crc32=0, crcread=0;

    //read the ota image file size
    tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+0x34);
    tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+0x35);
    tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+0x36);
    tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+0x37);
    memcpy(&file_info->image_size, tmp, 4);
    while (flash_check_busy()) {} ;

    if(file_info->image_size<0x73FFC)  //464*1024-4=0x73FFC
    {  
        crc32 = crc32checksum(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS, file_info->image_size);
        gt_img_info.image_size = file_info->image_size;
        log_info("image_size: 0x%08X", file_info->image_size);
        log_info("crc32: 0x%08X", crc32);

        tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+file_info->image_size+0);
        tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+file_info->image_size+1);
        tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+file_info->image_size+2);
        tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+file_info->image_size+3);
        memcpy(&crcread, tmp, 4);
        while (flash_check_busy()) {};
        log_info("crc32 in flash: 0x%08X", crcread);

        if(crc32 == crcread) 
        {
            file_info->image_found = 1;
            file_info->server_ready = ota_image_ready;
            //read the ota image manufacturer_code
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+10);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+11);
            memcpy(&file_info->manufacturer_code, tmp, 2);
            gt_img_info.manufacturer_code = file_info->manufacturer_code;
            log_info("manufacturer_code: 0x%04X", file_info->manufacturer_code);

            //read the ota image type
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+12);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+13);
            memcpy(&file_info->image_type, tmp, 2);
            gt_img_info.image_type = file_info->image_type;
            log_info("image_type: 0x%04X", file_info->image_type);

            //read the ota image file_version
            tmp[0] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+14);
            tmp[1] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+15);
            tmp[2] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+16);
            tmp[3] = flash_read_byte(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS+17);
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
                device_cb_param->status = RET_OK;
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
void _gw_ota_cmd_handle(uint32_t cmd_id, uint8_t *pBuf)
{

    int i;
    uint32_t status = 0;
    static uint8_t *p_tmp_buf;
    static uint32_t recv_cnt = 0;
    static uint32_t flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS, tmp_len = 0;
    uint32_t crc32, poffset;
    ota_img_info_t *upg_data;

    // erase
    if (cmd_id == GW_CMD_OTA_UPLOAD_START_REQUEST)
    {
        recv_cnt = 0;
        tmp_len = 0;
        flash_addr = FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS;
        for (i = 0; i < 0x74; i++)
        {
            // Page erase (4096 bytes)
            while (flash_check_busy());
            flash_erase(FLASH_ERASE_SECTOR, FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + (0x1000 * i));
        }
        zigbee_gw_cmd_send((GW_CMD_OTA_UPLOAD_START_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&status, 4);
    }
    else if (cmd_id == GW_CMD_OTA_BLOCK_REQUEST)
    {
        upg_data = (ota_img_info_t *)pBuf;
        if (upg_data->cur_pkt == 0)
        {
            memcpy((uint8_t *)&gt_img_info, pBuf, sizeof(gt_img_info));

            log_info("File Type: 0x%X", gt_img_info.image_type);
            log_info("Manufacturer Code: 0x%X", gt_img_info.manufacturer_code);
            log_info("File Version: 0x%X", gt_img_info.file_version);
            log_info("File Size: 0x%X", gt_img_info.image_size);
        }

        if (!gp_ota_imgae_cache)
        {
            gp_ota_imgae_cache = pvPortMalloc(0x1000);
        }

        if (gp_ota_imgae_cache)
        {
            if (upg_data->pkt_len + recv_cnt > 0x1000)
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], upg_data->pkt, 0x1000 - recv_cnt);
                tmp_len = upg_data->pkt_len - (0x1000 - recv_cnt);
                
                p_tmp_buf = pvPortMalloc(tmp_len);
                memcpy(p_tmp_buf, &upg_data->pkt[0x1000 - recv_cnt], tmp_len);

                // page program (256 bytes)
                for (i = 0; i < 0x10; i++)
                {
                    while (flash_check_busy());
                    flash_write_page((uint32_t) & ((uint8_t *)gp_ota_imgae_cache)[i * 0x100], flash_addr);
                    flash_addr += 0x100;
                }
                recv_cnt = 0;
                memcpy(&gp_ota_imgae_cache[recv_cnt], p_tmp_buf, tmp_len);
                recv_cnt += tmp_len;

                if (p_tmp_buf)
                {
                    vPortFree(p_tmp_buf);
                }
            }
            else
            {
                memcpy(&gp_ota_imgae_cache[recv_cnt], upg_data->pkt, upg_data->pkt_len);
                recv_cnt += upg_data->pkt_len;
            }
            if (upg_data->cur_pkt == (gt_img_info.total_pkt - 1))
            {
                for (i = 0; i < 0x10; i++)
                {
                    while (flash_check_busy());
                    flash_write_page((uint32_t) & ((uint8_t *)gp_ota_imgae_cache)[i * 0x100], flash_addr);
                    flash_addr += 0x100;
                }
                flush_cache();
                crc32 = crc32checksum(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS, gt_img_info.image_size);

                //if (((0x0007C000 + gt_img_info.image_size) % 0x1000) > 0)
                {
                    poffset = (FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + gt_img_info.image_size) - ((FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + gt_img_info.image_size) % 0x1000);
                }

                for (int i = 0; i < 0x10; i++)
                {
                    while (flash_check_busy());
                    flash_read_page((uint32_t)&gp_ota_imgae_cache[i * 0x100], poffset + (i * 0x100));
                }

                memcpy(&gp_ota_imgae_cache[((FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS + gt_img_info.image_size) % 0x1000)], &crc32, 4);

                while (flash_check_busy());
                flash_erase(FLASH_ERASE_SECTOR, poffset);

                for (int i = 0; i < 0x10; i++)
                {
                    while (flash_check_busy());
                    flash_write_page((uint32_t)&gp_ota_imgae_cache[i * 0x100], poffset + (i * 0x100));
                }
                if (gp_ota_imgae_cache)
                {
                    vPortFree(gp_ota_imgae_cache);
                    gp_ota_imgae_cache = NULL;
                }
            }
            if(upg_data->cur_pkt == (gt_img_info.total_pkt - 1)) {
                zigbee_gw_cmd_send(GW_CMD_OTA_UPLOAD_END_RESPONSE, 0, 0, 0, NULL, 0);
            } else {
                status = upg_data->cur_pkt;
                zigbee_gw_cmd_send((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&status, 4);
            }
        }
        else
        {
            status = 0xFFFFFFFF;
            zigbee_gw_cmd_send((GW_CMD_OTA_BLOCK_REQUEST | 0x8000), 0, 0, 0, (uint8_t *)&status, 4);
        }
    }
    else if (cmd_id == GW_CMD_OTA_FILE_INSERT_REQUEST)
    {
        ota_file_info_t file_info;
        memset(&file_info, 0, sizeof(ota_file_info_t));
        _check_ota_file_valid(&file_info);
        if(file_info.image_found) {
            zb_ret_t ret = 0x00000000;
            log_info("insert ota file");
            _ota_file_insert(FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB_UNCOMPRESS, gt_img_info.image_size + 4, gt_img_info.file_version, gt_img_info.image_type, gt_img_info.manufacturer_code);
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
        _ota_candidate_set(*(uint16_t*)pBuf);
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
