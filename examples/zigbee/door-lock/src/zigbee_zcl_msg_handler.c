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
static void _zcl_doorlock_process(uint16_t cmd, uint16_t datalen, uint8_t *pdata, uint32_t srcAddr, uint32_t srcEndpint, uint32_t seqnum, uint32_t dstAddr, uint32_t disableDefaultRsp)
{
    uint8_t code_len;
    switch (cmd)
    {
    case ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR:
    case ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR:
    case ZB_ZCL_CMD_DOOR_LOCK_TOGGLE:
    {
        if (cmd == ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR)
        {
            log_info("Receive LOCK command");
        }
        else if (cmd == ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR)
        {
            log_info("Receive UNLOCK command");
        }
        else if (cmd == ZB_ZCL_CMD_DOOR_LOCK_TOGGLE)
        {
            log_info("Receive TOGGLE command");
        }
        if (datalen > 0 && get_pincode()[0] > 0)
        {
            code_len = pdata[0];
            if (code_len > 0 && code_len == get_pincode()[0] && !memcmp(pdata, get_pincode(), code_len))
            {
                log_info("PIN code match");
                set_lock_state((cmd == ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR) ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED :
                               (cmd == ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR) ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNLOCKED :
                               get_lock_state() == ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNLOCKED : ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED);
                log_info("Lockstate: %s", (get_lock_state() == ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED) ? "LOCKED" : "UNLOCKED");
                if(get_lock_state() == ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED) {
                    set_led_onoff(LED_RED, 0);
                }
                else {
                    set_led_onoff(LED_RED, 1);
                }
            }
            else
            {
                log_info("PIN code not match");
            }
        }
        else if (get_pincode()[0] == 0)
        {
            set_lock_state((cmd == ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR) ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED :
                           (cmd == ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR) ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNLOCKED :
                           get_lock_state() == ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED ? ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNLOCKED : ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED);
            log_info("Lockstate: %s", (get_lock_state() == ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED) ? "LOCKED" : "UNLOCKED");
        }
    }
    break;
    case ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE:
    {
        uint8_t status = 0;
        if (datalen > 4)
        {
            if ((pdata[4] + 5) != datalen)
            {
                log_info("pincode len error");
                status = 1;// generic error
            }
            log_info_hexdump("PIN", &pdata[5], pdata[4]);
            set_pincode(pdata + 4);
            pincode_update();
            zcl_data_req_t *pt_data_req;

            ZIGBEE_ZCL_DATA_REQ(pt_data_req, srcAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, srcEndpint, DOOR_LOCK_EP,
                                ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                                ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE_RESPONSE,
                                TRUE, TRUE,
                                ZCL_FRAME_SERVER_CLIENT_DIR, 0, 1)


            if (pt_data_req)
            {
                pt_data_req->specific_seq_num = 1;
                pt_data_req->seq_num = seqnum;
                pt_data_req->cmdFormat[0] = status;
                zigbee_app_zcl_send_command(pt_data_req);
                vPortFree(pt_data_req);
            }
        }
    }
    break;
    case ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE:
    {
        if (datalen != 2)
        {
            break;
        }
        log_info("Received GET PINCODE command");
        uint8_t len;
        uint16_t uid = pdata[1] << 8 & pdata[0];
        log_info("uid %d", uid);
        uint8_t *buf;
        zcl_data_req_t *pt_data_req;
        if (uid != 0)
        {
            len = 2;
            ZIGBEE_ZCL_DATA_REQ(pt_data_req, srcAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, srcEndpint, DOOR_LOCK_EP,
                                ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                                ZB_ZCL_CMD_DEFAULT_RESP,
                                FALSE, TRUE,
                                ZCL_FRAME_SERVER_CLIENT_DIR, 0, len)
            pt_data_req->specific_seq_num = 1;
            pt_data_req->seq_num = seqnum;
            if (pt_data_req)
            {
                pt_data_req->cmdFormat[0] = ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE;
                pt_data_req->cmdFormat[1] = ZB_ZCL_STATUS_FAIL;

                zigbee_app_zcl_send_command(pt_data_req);
                vPortFree(pt_data_req);
            }
        }
        else
        {
            buf = get_pincode();
            len = buf[0] + 5;
            ZIGBEE_ZCL_DATA_REQ(pt_data_req, srcAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, srcEndpint, DOOR_LOCK_EP,
                                ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                                ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE_RESPONSE,
                                TRUE, TRUE,
                                ZCL_FRAME_SERVER_CLIENT_DIR, 0, len)


            if (pt_data_req)
            {
                pt_data_req->specific_seq_num = 1;
                pt_data_req->seq_num = seqnum;

                pt_data_req->cmdFormat[0] = 0;
                pt_data_req->cmdFormat[1] = 0;
                pt_data_req->cmdFormat[2] = 0;
                pt_data_req->cmdFormat[3] = (buf[0] == 0) ? 0xFF : 0x00;
                memcpy(&pt_data_req->cmdFormat[4], buf, buf[0] + 1);
                zigbee_app_zcl_send_command(pt_data_req);
                vPortFree(pt_data_req);
            }
        }
    }
    break;
    case ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE:
    case ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES:
    {
        uint8_t status = 0;
        char *c = (cmd == ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE) ? "CLEAR PINCODE" : "CLEAR ALL PINCODE";
        uint16_t uid = pdata[1] << 8 & pdata[0];
        log_info("Received %s command", c);
        if (uid != 0)
        {
            status = 1;    //fail
        }
        clear_pincode();
        zcl_data_req_t *pt_data_req;

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, srcAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, srcEndpint, DOOR_LOCK_EP,
                            ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                            (cmd == ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE) ? ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE_RESPONSE : ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES_RESPONSE,
                            TRUE, TRUE,
                            ZCL_FRAME_SERVER_CLIENT_DIR, 0, 1)

        if (pt_data_req)
        {
            pt_data_req->specific_seq_num = 1;
            pt_data_req->seq_num = seqnum;
            pt_data_req->cmdFormat[0] = status;
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
    }
    break;
    default:
        break;
    }
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
        case ZB_ZCL_CLUSTER_ID_DOOR_LOCK:
            _zcl_doorlock_process(cmd_info->cmd_id, payload_size, pData, src_addr, src_ep, cmd_info->seq_number, dest_addr, cmd_info->disable_default_response);
            break;
        default:
            break;
      }
    }
    return cmd_processed;
}