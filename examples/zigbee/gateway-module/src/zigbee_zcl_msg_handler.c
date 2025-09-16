/**
 * Copyright (c) 2021 All Rights Reserved.
 */
/** @file zigbee_zcl_msg_handler.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/12/23
 * @license
 * @description
 */

//=============================================================================
//                Include
//=============================================================================
#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"

#include <zigbee_platform.h>
#include "zigbee_api.h"
#include "log.h"
#include "zigbee_cmd_app.h"

#define ZB_TRACE_FILE_ID 294
//=============================================================================
//                Global variables
//=============================================================================
static zcl_read_rsp_cb p_read_rsp_cb = NULL;
static zcl_write_rsp_cb p_write_rsp_cb = NULL;
static zcl_cfg_report_rsp_cb p_cfg_report_rsp_cb = NULL;
static zcl_report_attribute_cb p_report_attribute_cb = NULL;
static zcl_zone_status_change_notification_cb
    p_zone_status_change_notification_cb = NULL;
//=============================================================================
//                Function
//=============================================================================

void zigbee_app_zcl_read_rsp_cb_reg(void* cb) {
    p_read_rsp_cb = (zcl_read_rsp_cb)cb;
}

void zigbee_app_zcl_write_rsp_cb_reg(void* cb) {
    p_write_rsp_cb = (zcl_write_rsp_cb)cb;
}

void zigbee_app_zcl_cfg_report_rsp_cb_reg(void* cb) {
    p_cfg_report_rsp_cb = (zcl_cfg_report_rsp_cb)cb;
}

void zigbee_app_zcl_report_attribute_cb_reg(void* cb) {
    p_report_attribute_cb = (zcl_report_attribute_cb)cb;
}

void zigbee_app_zcl_zone_status_change_notification_reg(void* cb) {
    p_zone_status_change_notification_cb =
        (zcl_zone_status_change_notification_cb)cb;
}

static uint16_t get_attribute_size(uint8_t attr_type, uint8_t *data)
{
    uint16_t ret;
    switch ( attr_type )
    {
    case ZB_ZCL_ATTR_TYPE_8BIT:
    case ZB_ZCL_ATTR_TYPE_U8:
    case ZB_ZCL_ATTR_TYPE_S8:
    case ZB_ZCL_ATTR_TYPE_BOOL:
    case ZB_ZCL_ATTR_TYPE_8BITMAP:
    case ZB_ZCL_ATTR_TYPE_8BIT_ENUM:
        ret = 1;
        break;

    case ZB_ZCL_ATTR_TYPE_16BIT:
    case ZB_ZCL_ATTR_TYPE_U16:
    case ZB_ZCL_ATTR_TYPE_S16:
    case ZB_ZCL_ATTR_TYPE_16BITMAP:
    case ZB_ZCL_ATTR_TYPE_16BIT_ENUM:
    case ZB_ZCL_ATTR_TYPE_SEMI:
    case ZB_ZCL_ATTR_TYPE_CLUSTER_ID:
    case ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID:
        ret = 2;
        break;

    case ZB_ZCL_ATTR_TYPE_32BIT:
    case ZB_ZCL_ATTR_TYPE_U32:
    case ZB_ZCL_ATTR_TYPE_S32:
    case ZB_ZCL_ATTR_TYPE_32BITMAP:
    case ZB_ZCL_ATTR_TYPE_UTC_TIME:
    case ZB_ZCL_ATTR_TYPE_TIME_OF_DAY:
    case ZB_ZCL_ATTR_TYPE_DATE:
    case ZB_ZCL_ATTR_TYPE_BACNET_OID:
    case ZB_ZCL_ATTR_TYPE_SINGLE:
        ret = 4;
        break;

    case ZB_ZCL_ATTR_TYPE_S48:
    case ZB_ZCL_ATTR_TYPE_U48:
    case ZB_ZCL_ATTR_TYPE_48BIT:
    case ZB_ZCL_ATTR_TYPE_48BITMAP:
        ret = 6;
        break;

    case ZB_ZCL_ATTR_TYPE_S24:
    case ZB_ZCL_ATTR_TYPE_U24:
    case ZB_ZCL_ATTR_TYPE_24BIT:
    case ZB_ZCL_ATTR_TYPE_24BITMAP:
        ret = 3;
        break;

    case ZB_ZCL_ATTR_TYPE_U40:
    case ZB_ZCL_ATTR_TYPE_S40:
    case ZB_ZCL_ATTR_TYPE_40BIT:
    case ZB_ZCL_ATTR_TYPE_40BITMAP:
        ret = 5;
        break;

    case ZB_ZCL_ATTR_TYPE_U56:
    case ZB_ZCL_ATTR_TYPE_S56:
    case ZB_ZCL_ATTR_TYPE_56BIT:
    case ZB_ZCL_ATTR_TYPE_56BITMAP:
        ret = 7;
        break;

    case ZB_ZCL_ATTR_TYPE_64BIT:
    case ZB_ZCL_ATTR_TYPE_64BITMAP:
    case ZB_ZCL_ATTR_TYPE_U64:
    case ZB_ZCL_ATTR_TYPE_S64:
    case ZB_ZCL_ATTR_TYPE_DOUBLE:
    case ZB_ZCL_ATTR_TYPE_IEEE_ADDR:
        ret = 8;
        break;
    case ZB_ZCL_ATTR_TYPE_128_BIT_KEY:
        ret = 16;
        break;
    case ZB_ZCL_ATTR_TYPE_OCTET_STRING:
    case ZB_ZCL_ATTR_TYPE_CHAR_STRING:
        ret = data[0] + 1;
        break;
    case ZB_ZCL_ATTR_TYPE_ARRAY:
    case ZB_ZCL_ATTR_TYPE_CUSTOM_32ARRAY:
    case ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING:
        ret = data[0] | (data[1] << 8) + 2;
        break;

    default:
        ret = 0;
        break;
    }
    return ret;
}

void _zcl_read_attr_report_process(uint8_t cmd, uint16_t clusterID, uint16_t srcAddr, uint16_t datalen, uint8_t *pdata)
{
    if (cmd == ZB_ZCL_CMD_READ_ATTRIB_RESP)
    {
        log_info("Read attribute from 0x%04x cluster 0x%04x", srcAddr, clusterID);
    }
    else if (cmd == ZB_ZCL_CMD_REPORT_ATTRIB)
    {
        log_info("Report from 0x%04x cluster 0x%04x", srcAddr, clusterID);
    }
    uint16_t attr_id, attr_len;
    uint8_t attr_type;
    int i = 0, j;

    while (i < datalen)
    {
        attr_id = pdata[i] | (pdata[i + 1] << 8);
        if (cmd == 0x01)
        {
            i++;
        }
        attr_type = pdata[i + 2];

        attr_len = get_attribute_size(attr_type, pdata + i + 3);
        char tempStr[attr_len];
        log_info("attribute id: 0x%04x, ", attr_id);
        log_info("type: 0x%x, ", attr_type);
        for (j = 0; j < attr_len; j++)
        {
            if (attr_type == ZB_ZCL_ATTR_TYPE_OCTET_STRING ||
                    attr_type == ZB_ZCL_ATTR_TYPE_CHAR_STRING)
            {
                if (j > 0)
                {
                    sprintf(tempStr+j,"%c", pdata[i + 3 + j]);
                }
            }
            else if (attr_type == ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING)
            {
                if (j > 1)
                {
                    sprintf(tempStr+j,"%c", pdata[i + 3 + j]);
                }
            }
            else
            {
                sprintf(tempStr+j,"%x", pdata[i + 2 + attr_len - j]);
            }
        }
        i += (3 + attr_len);
        log_info("value: %s", tempStr);
    }
}
uint8_t zigbee_zcl_msg_handler(zb_uint8_t param) {
    zb_bufid_t zcl_cmd_buf = param;
    zb_uint8_t cmd_processed = 0;
    zb_zcl_parsed_hdr_t* cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf,
                                                     zb_zcl_parsed_hdr_t);
    uint16_t payload_size = 0;
    uint16_t src_addr = 0, dest_addr = 0;
    uint8_t src_ep = 0;
    uint8_t* pData;

    //ZB_ZCL_COPY_PARSED_HEADER(param, &cmd_info);

    zb_uint16_t dst_addr =
        ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr;
    zb_uint8_t dst_ep = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint;

    if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_OTA_UPGRADE) {
        return ZB_FALSE;
    }

    if (cmd_info->addr_data.common_data.source.addr_type == 0) {
        src_addr = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr;
    }

    src_ep = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint;

    pData = zb_buf_begin(param);
    payload_size = zb_buf_len(param);

    // log_info("Recv ZCL message 0x%04X -> 0x%04X", src_addr, dest_addr);
    // log_info("Cluster %04x cmd %d seq %d", cmd_info->cluster_id,
    //          cmd_info->cmd_id, cmd_info->seq_number);
    // log_info_hexdump("ZCL", (uint8_t*)pData, payload_size);

    if (cmd_info->is_common_command) {
        if (cmd_info->cmd_id == ZB_ZCL_CMD_READ_ATTRIB_RESP) // Read response
        {
            _zcl_read_attr_report_process(cmd_info->cmd_id, cmd_info->cluster_id, src_addr, payload_size, pData);
            if (p_read_rsp_cb) {
                p_read_rsp_cb(cmd_info->cluster_id, src_addr, src_ep, pData,
                              payload_size);
            }
        } else if (cmd_info->cmd_id == ZB_ZCL_CMD_WRITE_ATTRIB_RESP) // Write response
        {
            if (p_write_rsp_cb) {
                p_write_rsp_cb(cmd_info->cluster_id, src_addr, src_ep, pData,
                               payload_size);
            }
        } else if (cmd_info->cmd_id == ZB_ZCL_CMD_CONFIG_REPORT_RESP) // Config report response
        {
            if (p_cfg_report_rsp_cb) {
                p_cfg_report_rsp_cb(cmd_info->cluster_id, src_addr, src_ep,
                                    pData, payload_size);
            }
        } else if (cmd_info->cmd_id == ZB_ZCL_CMD_REPORT_ATTRIB) // Report
        {
            // zigbee_app_zcl_report_attribute_cb_reg(_zcl_report_attribute_cb);
            // p_report_attribute_cb(cmd_info->cluster_id, src_addr, src_ep, pData, payload_size);
            _zcl_read_attr_report_process(cmd_info->cmd_id, cmd_info->cluster_id, src_addr, payload_size, pData);
            uint8_t report_data[100];
            memcpy(report_data, &(cmd_info->cluster_id), sizeof(zb_uint16_t));
            memcpy(report_data + sizeof(zb_uint16_t), pData, payload_size);
            payload_size += sizeof(zb_uint16_t);
            zigbee_gw_cmd_send(0x00028800, src_addr, 0, src_ep, report_data,
                               payload_size);
        } else if (cmd_info->cmd_id == ZB_ZCL_CMD_DEFAULT_RESP) // defaut response
        {
            uint8_t report_data[4];
            memcpy(report_data, &(cmd_info->cluster_id), sizeof(zb_uint16_t));
            memcpy(report_data + sizeof(zb_uint16_t), pData, payload_size);
            payload_size += sizeof(zb_uint16_t);
            zigbee_gw_cmd_send(0x00018800, src_addr, 0, src_ep, report_data,
                               payload_size);
        }
    } else {
        if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_IDENTIFY) {
            zigbee_gw_cmd_send(0x00048001, src_addr, 0, src_ep, pData,
                               payload_size);
        } else if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_GROUPS) {
            zigbee_gw_cmd_send((0x00058000 | cmd_info->cmd_id), src_addr, 0,
                               src_ep, pData, payload_size);
        } else if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_SCENES) {
            zigbee_gw_cmd_send((0x00068000 | cmd_info->cmd_id), src_addr, 0,
                               src_ep, pData, payload_size);
        } else if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ALARMS) {
            zigbee_gw_cmd_send((0x000A8000 | cmd_info->cmd_id), src_addr, 0,
                               src_ep, pData, payload_size);
        } else if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_IAS_ZONE) {
            if (!(cmd_info->is_common_command)) {
                if (cmd_info->cmd_id
                    == ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID) {
                    ZB_ZCL_IAS_ZONE_SEND_ZONE_ENROLL_RES(
                        zcl_cmd_buf, dst_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                        dst_ep, ZIGBEE_DEFAULT_ENDPOINT, ZB_AF_HA_PROFILE_ID,
                        ZB_FALSE, NULL,
                        ZB_ZCL_IAS_ZONE_ENROLL_RESPONCE_CODE_SUCCESS,
                        7 /* ZONE_ID */);

                    cmd_processed = 1;
                } else if (
                    cmd_info->cmd_id
                    == ZB_ZCL_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID) // zone status change notification
                {
                    // zigbee_app_zcl_zone_status_change_notification_reg(_zcl_zone_status_change_notificatin_cb);
                    // p_zone_status_change_notification_cb(cmd_info->cluster_id, src_addr, src_ep, pData, payload_size);
                    zigbee_gw_cmd_send(0x00230000, src_addr, 0, src_ep, pData,
                                       payload_size);
                }
            }
        } else if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_DOOR_LOCK) {
            zigbee_gw_cmd_send(0x00248000 | cmd_info->cmd_id, src_addr, 0,
                               src_ep, pData, payload_size);
        } else if (cmd_info->cluster_id >= 0xFC00) {

            uint8_t custom_payload_data[payload_size + 6];
            custom_payload_data[0] = cmd_info->cluster_id & 0xFF;
            custom_payload_data[1] = (cmd_info->cluster_id >> 8) & 0xFF;
            custom_payload_data[2] = cmd_info->manuf_specific & 0xFF;
            custom_payload_data[3] = (cmd_info->manuf_specific >> 8) & 0xFF;
            custom_payload_data[4] = cmd_info->cmd_id;
            custom_payload_data[5] = payload_size;
            if (payload_size > 0) {
                memcpy(&custom_payload_data[6], pData, payload_size);
            }
            zigbee_gw_cmd_send(0xFC008000, src_addr, 0, src_ep,
                               custom_payload_data, payload_size + 6);
        }
    }

    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_ZBOSS_CB_IN);
    return cmd_processed;
}