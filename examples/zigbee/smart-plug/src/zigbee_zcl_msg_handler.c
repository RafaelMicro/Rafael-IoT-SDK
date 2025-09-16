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
#include "device_api.h"
#include "log.h"

#define ZB_TRACE_FILE_ID 294
//=============================================================================
//                Global variables
//=============================================================================
//=============================================================================
//                Function
//=============================================================================

static void _zcl_common_command_process(uint16_t cmd, uint16_t datalen, uint8_t *pdata, uint32_t clusterID)
{
    do
    {
        if (cmd == ZB_ZCL_CMD_WRITE_ATTRIB ||
                cmd == ZB_ZCL_CMD_WRITE_ATTRIB_UNDIV ||
                cmd == ZB_ZCL_CMD_WRITE_ATTRIB_RESP ||
                cmd == ZB_ZCL_CMD_WRITE_ATTRIB_NO_RESP)
        {
            if (clusterID == ZB_ZCL_CLUSTER_ID_IDENTIFY && (pdata[3] | (pdata[4] << 8)) != 0)
            {
                log_info("Identify process start, duration = %d", pdata[3] | (pdata[4] << 8));
                zigbee_start_identify();
            }
        }
    } while (0);
}
static void _zcl_basic_process(uint16_t cmd, uint16_t datalen, uint8_t *pdata)
{
    if (cmd == ZB_ZCL_CMD_BASIC_RESET_ID)
    {
        reset_attr();
    }
}
static void _zcl_indentify_process(uint16_t cmd, uint16_t datalen, uint8_t *pdata, uint32_t dir)
{
    if (cmd == ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID && dir == ZCL_FRAME_CLIENT_SERVER_DIR)
    {
        log_info("Identify process start, duration = %d", pdata[0] | (pdata[1] << 8));
        zigbee_start_identify();
    }
}
static void _zcl_onoff_process(uint16_t cmd, uint16_t datalen, uint8_t *pdata)
{
    do
    {
        uint8_t onoff;
        if (cmd == ZB_ZCL_CMD_ON_OFF_OFF_ID ||
                cmd == ZB_ZCL_CMD_ON_OFF_ON_ID  ||
                cmd == ZB_ZCL_CMD_ON_OFF_TOGGLE_ID )
        {
            if(cmd == ZB_ZCL_CMD_ON_OFF_OFF_ID) {
                onoff = 0;
            }
            else if(cmd == ZB_ZCL_CMD_ON_OFF_ON_ID) {
                onoff = 1;
            }
            else if(cmd == ZB_ZCL_CMD_ON_OFF_TOGGLE_ID) {
                onoff = !get_on_off_status();
            }
            const char *status[3] = {"OFF", "ON", "Toggle"};
            log_info("Received %s command\n", status[cmd]);
            set_plug_status(onoff);
        }
    } while (0);
}
uint8_t zigbee_zcl_msg_handler(zb_uint8_t param) {
    zb_bufid_t zcl_cmd_buf = param;
    zb_uint8_t cmd_processed = 0;

    uint16_t payload_size = 0, src_addr = 0, dest_addr = 0;
    uint8_t src_ep = 0, dst_ep = 0;
    uint8_t* pData;

    zb_zcl_parsed_hdr_t* cmd_info = ZB_BUF_GET_PARAM(zcl_cmd_buf,
                                                     zb_zcl_parsed_hdr_t);
    dest_addr = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_addr;
    dst_ep = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).dst_endpoint;
    src_addr = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).source.u.short_addr;
    src_ep = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint;

    pData = zb_buf_begin(param);
    payload_size = zb_buf_len(param);

    // log_info("Recv ZCL message 0x%04X -> 0x%04X", src_addr, dest_addr);
    // log_info("Cluster %04x cmd %d seq %d", cmd_info->cluster_id,
    //          cmd_info->cmd_id, cmd_info->seq_number);
    // log_info_hexdump("ZCL", (uint8_t*)pData, payload_size);
    
    if (cmd_info->is_common_command) {
        _zcl_common_command_process(cmd_info->cmd_id, payload_size, pData, cmd_info->cluster_id);
    }
    else {
        switch (cmd_info->cluster_id) {
        case ZB_ZCL_CLUSTER_ID_BASIC:
            _zcl_basic_process(cmd_info->cmd_id, payload_size, pData);
            break;
        case ZB_ZCL_CLUSTER_ID_IDENTIFY:
            _zcl_indentify_process(cmd_info->cmd_id, payload_size, pData, cmd_info->cmd_direction);
            break;
        case ZB_ZCL_CLUSTER_ID_ON_OFF:
            _zcl_onoff_process(cmd_info->cmd_id, payload_size, pData);
            break;
        default:
            break;
      }
    }
    return cmd_processed;
}