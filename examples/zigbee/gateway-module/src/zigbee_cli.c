
/**
 * @file zigbee_cli.c
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
#include "zb_zdo_globals.h"
#include "zboss_api.h"
#include "zboss_api_nwk.h"

#include "cli.h"
#include "log.h"
#include "util_string.h"
#include "zigbee_api.h"
#include "zigbee_cmd_nwk.h"
#include "zigbee_cmd_app.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define ZB_TRACE_FILE_ID        294

//=============================================================================
//                Private ENUM
//=============================================================================

//=============================================================================
//                Private Struct
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================

//=============================================================================
//                Callback Functions
//=============================================================================

static void bind_req_cb(zb_uint8_t param)
{
    if(param) {
        zb_buf_free(param);
    }
}
static void _permit_join_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd;

    do {
        p_fail_pd = zb_buf_get_status(param);

        zb_buf_free(param);
    } while (0);

    zigbee_gw_cmd_send((ZIGBEE_CMD_PERMIT_JOIN_REQUEST | 0x8000), 0x0000, 0, 0,
                       &p_fail_pd, sizeof(p_fail_pd));
}
static void _act_ep_req_cb(zb_uint8_t param)
{
    zb_uint8_t *zdp_cmd = zb_buf_begin(param);
    zb_zdo_ep_resp_t *resp = (zb_zdo_ep_resp_t *)zdp_cmd;
    zb_uint8_t *ep_list = zdp_cmd + sizeof(zb_zdo_ep_resp_t);
    do
    {
        if (resp->status != ZB_ZDP_STATUS_SUCCESS)
        {
            break;
        }
        log_info("Addr 0x%04X Active Endpoint:", resp->nwk_addr);

        for (uint8_t i = 0; i < resp->ep_count; i++)
        {
            log_info("[%02x]", ep_list[i]);
        }

        zb_buf_free(param);

    } while (0);
}

static void _simple_desc_cb(zb_bufid_t param)
{
    uint8_t total_cluster_count, i;
    zb_uint8_t *zdp_cmd = zb_buf_begin(param);
    zb_zdo_simple_desc_resp_t *resp = (zb_zdo_simple_desc_resp_t *)(zdp_cmd);

    do
    {

        if (resp->hdr.status != ZB_ZDP_STATUS_SUCCESS)
        {
            break;
        }
        total_cluster_count = resp->simple_desc.app_input_cluster_count + resp->simple_desc.app_output_cluster_count;

        log_info("Simple Descriptor of 0x%04X EP %02x:", resp->hdr.nwk_addr, resp->simple_desc.endpoint);
        log_info("Device ID 0x%04X", resp->simple_desc.app_device_id);
        log_info("Server cluster:");
        for (i = 0; i < resp->simple_desc.app_input_cluster_count; i++)
        {
            log_info("0x%04X, ", *(resp->simple_desc.app_cluster_list + i));
        }
        log_info("Client cluster:");
        for (; i < resp->simple_desc.app_output_cluster_count; i++)
        {
            log_info("0x%04X, ", *(resp->simple_desc.app_cluster_list + i));
        }

        zb_buf_free(param);

    } while (0);
}

//=============================================================================
//                ZDO Functions
//=============================================================================
static int _cli_cmd_bind(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    uint8_t src_ep, dst_ep, mode;
    uint16_t src_address, dst_address, cluster;
    zb_bufid_t buf;
    zb_ieee_addr_t ieee_address;
    zb_zdo_bind_req_param_t *bind_param = NULL;
    do {
        if(argc < 7) {
            break;
        }
        if(strcmp(argv[1], "u") == 0) {
            mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
        }
        else if(strcmp(argv[1], "g") == 0) {
            mode = ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;
        }
        else {
            log_error("Invalid binding mode");
            break;
        }
        src_address = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 4)
                : utility_strtol(argv[2], 0);

        src_ep = (*(argv[3] + 1) == 'x')
                ? utility_strtox(argv[3] + 2, 0, 2)
                : utility_strtol(argv[3], 0);

        dst_address = (*(argv[4] + 1) == 'x')
                ? utility_strtox(argv[4] + 2, 0, 4)
                : utility_strtol(argv[4], 0);

        dst_ep = (*(argv[5] + 1) == 'x')
                ? utility_strtox(argv[5] + 2, 0, 2)
                : utility_strtol(argv[5], 0);

        cluster = (*(argv[6] + 1) == 'x')
                ? utility_strtox(argv[6] + 2, 0, 4)
                : utility_strtol(argv[6], 0);

        ZB_THREAD_SAFE(
            buf = zb_buf_get_any();
            if (buf == 0U) {
                break;
            }
            bind_param = ZB_BUF_GET_PARAM(buf, zb_zdo_bind_req_param_t);
            bind_param->req_dst_addr = src_address;
            bind_param->dst_endp = dst_ep;
            bind_param->src_endp = src_ep;
            bind_param->cluster_id = cluster;
            bind_param->dst_addr_mode = mode;
            if (mode == ZB_APS_ADDR_MODE_64_ENDP_PRESENT) {
                if(zb_address_ieee_by_short(dst_address, ieee_address) != RET_OK) {
                    log_error("unknown dst address");
                    if(buf != 0) {
                        zb_buf_free(buf);
                    }
                    break;
                }
                memcpy(bind_param->dst_address.addr_long, ieee_address, sizeof(zb_ieee_addr_t));
            }
            else if (mode == ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT) {
                bind_param->dst_address.addr_short = dst_address;
            }
            if(zb_address_ieee_by_short(src_address, ieee_address) != RET_OK) {
                log_error("unknown src address");
                if(buf != 0) {
                    zb_buf_free(buf);
                }
                break;
            }
            memcpy(bind_param->src_address, ieee_address, sizeof(zb_ieee_addr_t));
            zb_zdo_bind_req(buf, bind_req_cb);
        );

    } while (0);
    return 0;
}

static int _cli_cmd_unbind(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    uint8_t src_ep, dst_ep, mode;
    uint16_t src_address, dst_address, cluster;
    zb_bufid_t buf;
    zb_ieee_addr_t ieee_address;
    zb_zdo_bind_req_param_t *bind_param = NULL;
    do {
        if(argc < 7) {
            break;
        }
        if(strcmp(argv[1], "u") == 0) {
            mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
        }
        else if(strcmp(argv[1], "g") == 0) {
            mode = ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;
        }
        else {
            log_error("Invalid binding mode");
            break;
        }
        src_address = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 4)
                : utility_strtol(argv[2], 0);

        src_ep = (*(argv[3] + 1) == 'x')
                ? utility_strtox(argv[3] + 2, 0, 2)
                : utility_strtol(argv[3], 0);

        dst_address = (*(argv[4] + 1) == 'x')
                ? utility_strtox(argv[4] + 2, 0, 4)
                : utility_strtol(argv[4], 0);

        dst_ep = (*(argv[5] + 1) == 'x')
                ? utility_strtox(argv[5] + 2, 0, 2)
                : utility_strtol(argv[5], 0);

        cluster = (*(argv[6] + 1) == 'x')
                ? utility_strtox(argv[6] + 2, 0, 4)
                : utility_strtol(argv[6], 0);

        ZB_THREAD_SAFE(
            buf = zb_buf_get_any();
            if (buf == 0U) {
                break;
            }
            bind_param = ZB_BUF_GET_PARAM(buf, zb_zdo_bind_req_param_t);
            bind_param->req_dst_addr = src_address;
            bind_param->dst_endp = dst_ep;
            bind_param->src_endp = src_ep;
            bind_param->cluster_id = cluster;
            bind_param->dst_addr_mode = mode;
            if (mode == ZB_APS_ADDR_MODE_64_ENDP_PRESENT) {
                if(zb_address_ieee_by_short(dst_address, ieee_address) != RET_OK) {
                    log_error("unknown dst address");
                    zb_buf_free(buf);
                    break;
                }
                memcpy(bind_param->dst_address.addr_long, ieee_address, sizeof(zb_ieee_addr_t));
            }
            else if (mode == ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT) {
                bind_param->dst_address.addr_short = dst_address;
            }
            if(zb_address_ieee_by_short(src_address, ieee_address) != RET_OK) {
                log_error("unknown src address");
                zb_buf_free(buf);
                break;
            }
            memcpy(bind_param->src_address, ieee_address, sizeof(zb_ieee_addr_t));
            zb_zdo_unbind_req(buf, bind_req_cb);
        );

    } while (0);
    return 0;
}

static int
_cli_cmd_zdo_act_ep_req(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t addr;
    zb_bufid_t buf = 0U;
    zb_zdo_active_ep_req_t *ep_req;

    do
    {
        if (argc < 2)
        {
            break;
        }

        addr = (*(argv[1] + 1) == 'x')
               ? utility_strtox(argv[1] + 2, 0, 4)
               : utility_strtol(argv[1], 0);
        ZB_THREAD_SAFE(
            buf = zb_buf_get_any();
            if (buf == 0U) {
                break;
            }

            ep_req = zb_buf_initial_alloc(buf, sizeof(zb_zdo_active_ep_req_t));
            ep_req->nwk_addr = addr;
            zb_zdo_active_ep_req(buf, _act_ep_req_cb);
        );

    } while (0);
    return 0;
}

static int
_cli_cmd_zdo_simple_desc_req(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t addr;
    uint8_t endpoint;
    zb_bufid_t buf = 0U;
    zb_zdo_simple_desc_req_t *req;
    do
    {
        if (argc < 3)
        {
            break;
        }

        addr = (*(argv[1] + 1) == 'x')
               ? utility_strtox(argv[1] + 2, 0, 4)
               : utility_strtol(argv[1], 0);

        endpoint = (*(argv[2] + 1) == 'x')
                   ? utility_strtox(argv[2] + 2, 0, 2)
                   : utility_strtol(argv[2], 0);

        ZB_THREAD_SAFE(
            buf = zb_buf_get_any();
            if (buf == 0U) {
                break;
            }
            req = zb_buf_initial_alloc(buf, sizeof(zb_zdo_simple_desc_req_t));
            req->nwk_addr = addr;
            req->endpoint = endpoint;

            zb_zdo_simple_desc_req(buf, _simple_desc_cb);
        );
    } while (0);
    return 0;
}
static int _cli_cmd_edscan_req(int argc, char** argv, cb_shell_out_t log_out, void* pExtra) {
    zigbee_app_mac_ed_scan_command();
}
static int _cli_cmd_pj(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    zb_zdo_mgmt_permit_joining_req_param_t* req_param;
    zb_bufid_t buf_zc,buf;
    uint8_t duration;
    do {
        if(argc < 2) {
            duration = 180;
        }
        else {
            duration = (*(argv[1] + 1) == 'x')
                    ? utility_strtox(argv[1] + 2, 0, 2)
                    : utility_strtol(argv[1], 0);
        }

        ZB_THREAD_SAFE(
        buf_zc = zb_buf_get_out();
        if (buf_zc == 0U) {
            break;
        }

        req_param = ZB_BUF_GET_PARAM(buf_zc,
                                     zb_zdo_mgmt_permit_joining_req_param_t);

        req_param->dest_addr = 0x0000;
        req_param->permit_duration = duration;
        req_param->tc_significance = 1;

        zb_zdo_mgmt_permit_joining_req(buf_zc, _permit_join_response);

        buf = zb_buf_get_out();
        if (buf == 0U) {
            break;
        }
        req_param = ZB_BUF_GET_PARAM(buf,
                                     zb_zdo_mgmt_permit_joining_req_param_t);

        req_param->dest_addr = 0xFFFC;
        req_param->permit_duration = duration;
        req_param->tc_significance = 1;

        zb_zdo_mgmt_permit_joining_req(buf, NULL);
        );

    } while (0);
    return 0;
}

static int _cli_cmd_start(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    uint8_t channel, max_child, reset;
    uint16_t panid;
    do {
        if(argc < 5) {
            break;
        }

        reset = (*(argv[1] + 1) == 'x')
                ? utility_strtox(argv[1] + 2, 0, 2)
                : utility_strtol(argv[1], 0);

        channel = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 2)
                : utility_strtol(argv[2], 0);

        panid = (*(argv[3] + 1) == 'x')
                ? utility_strtox(argv[3] + 2, 0, 4)
                : utility_strtol(argv[3], 0);

        max_child = (*(argv[4] + 1) == 'x')
                ? utility_strtox(argv[4] + 2, 0, 2)
                : utility_strtol(argv[4], 0);

        zigbee_app_nwk_start(channel, max_child, panid, reset);

    } while (0);
    return 0;
}

//=============================================================================
//                ZCL Functions
//=============================================================================
static int
_cli_cmd_zcl_attr_read(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t* pt_data_req;
    uint16_t dst_addr, cluster_ID, attr_ID;
    uint8_t dst_ep;

    do
    {
        if (argc < 5)
        {
            break;
        }

        dst_addr = (*(argv[1] + 1) == 'x')
                   ? utility_strtox(argv[1] + 2, 0, 4)
                   : utility_strtol(argv[1], 0);

        dst_ep = (*(argv[2] + 1) == 'x')
                 ? utility_strtox(argv[2] + 2, 0, 2)
                 : utility_strtol(argv[2], 0);

        cluster_ID = (*(argv[3] + 1) == 'x')
                     ? utility_strtox(argv[3] + 2, 0, 4)
                     : utility_strtol(argv[3], 0);

        attr_ID = (*(argv[4] + 1) == 'x')
                  ? utility_strtox(argv[4] + 2, 0, 4)
                  : utility_strtol(argv[4], 0);

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, dst_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                            dst_ep, ZIGBEE_DEFAULT_ENDPOINT,
                            cluster_ID, ZB_ZCL_CMD_READ_ATTRIB, FALSE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, 2)
        log_info("attr_ID 0x%04X", attr_ID);
        if (pt_data_req) {
            memcpy(pt_data_req->cmdFormat, &attr_ID, 2);
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }


    } while (0);
    return 0;
}

static int
_cli_cmd_zcl_attr_write(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t* pt_data_req;
    uint8_t attr_value;
    uint16_t dst_addr, cluster_ID, attr_ID;
    uint8_t dst_ep, attr_Type, attr_len = 0;

    do
    {
        if (argc < 7)
        {
            break;
        }

        dst_addr = (*(argv[1] + 1) == 'x')
                   ? utility_strtox(argv[1] + 2, 0, 4)
                   : utility_strtol(argv[1], 0);

        dst_ep = (*(argv[2] + 1) == 'x')
                 ? utility_strtox(argv[2] + 2, 0, 2)
                 : utility_strtol(argv[2], 0);

        cluster_ID = (*(argv[3] + 1) == 'x')
                     ? utility_strtox(argv[3] + 2, 0, 4)
                     : utility_strtol(argv[3], 0);

        attr_ID = (*(argv[4] + 1) == 'x')
                  ? utility_strtox(argv[4] + 2, 0, 4)
                  : utility_strtol(argv[4], 0);

        attr_Type = (*(argv[5] + 1) == 'x')
                    ? utility_strtox(argv[5] + 2, 0, 2)
                    : utility_strtol(argv[5], 0);

        if (attr_Type == ZB_ZCL_ATTR_TYPE_OCTET_STRING ||
                attr_Type == ZB_ZCL_ATTR_TYPE_CHAR_STRING)
        {
            attr_len = strlen(argv[6]) + 1;
        }
        else if (attr_Type == ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING )
        {
            attr_len = strlen(argv[6]) + 2;
        }
        else
        {
            attr_value = (*(argv[6] + 1) == 'x')
                         ? utility_strtox(argv[6] + 2, 0, 4)
                         : utility_strtol(argv[6], 0);
            attr_len = zb_zcl_get_attribute_size(attr_Type, &attr_value);
        }
        ZIGBEE_ZCL_DATA_REQ(pt_data_req, dst_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                            dst_ep, ZIGBEE_DEFAULT_ENDPOINT,
                            cluster_ID, ZB_ZCL_CMD_WRITE_ATTRIB, FALSE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_len+3)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = attr_ID & 0xFF;
            pt_data_req->cmdFormat[1] = (attr_ID >> 8) & 0xFF;
            pt_data_req->cmdFormat[2] = attr_Type;
            for (uint8_t i = 0; i < attr_len ; i++)
            {
                if (attr_Type == ZB_ZCL_ATTR_TYPE_OCTET_STRING ||
                        attr_Type == ZB_ZCL_ATTR_TYPE_CHAR_STRING)
                {
                    if (i == 0)
                    {
                        pt_data_req->cmdFormat[i+3] = strlen(argv[6]);
                    }
                    else
                    {
                        pt_data_req->cmdFormat[i+3] = *(argv[6] + i - 1);
                    }
                }
                else if (attr_Type == ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING)
                {
                    if (i < 2)
                    {
                        pt_data_req->cmdFormat[3] = strlen(argv[6]) & 0xFF;
                        pt_data_req->cmdFormat[4] = (strlen(argv[6]) >> 8) & 0xFF;
                    }
                    else
                    {
                        pt_data_req->cmdFormat[4] = *(argv[6] + i - 2);
                    }
                }
                else
                {
                    pt_data_req->cmdFormat[i+3] = (attr_value >> 8 * i) & 0xFF;
                }
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
    } while (0);
    return 0;
}
static int
_cli_cmd_zcl_group(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr, group_id, ep, cmd;

    do
    {
        if (argc < 4)
        {
            break;
        }

        if (memcmp("a", argv[1], 1) == 0)
        {
            cmd = ZB_ZCL_CMD_GROUPS_ADD_GROUP;
        }
        else if (memcmp("r", argv[1], 1) == 0)
        {
            cmd = ZB_ZCL_CMD_GROUPS_REMOVE_GROUP;
        }
        else
        {
            break;
        }

        addr = (*(argv[2] + 1) == 'x')
               ? utility_strtox(argv[2] + 2, 0, 4)
               : utility_strtol(argv[2], 0);
        ep = (*(argv[3] + 1) == 'x')
             ? utility_strtox(argv[3] + 2, 0, 2)
             : utility_strtol(argv[3], 0);
        group_id = (*(argv[4] + 1) == 'x')
                   ? utility_strtox(argv[4] + 2, 0, 4)
                   : utility_strtol(argv[4], 0);


        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                            ep, ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_GROUPS, cmd, TRUE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, 3)


        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = group_id & 0xFF;
            pt_data_req->cmdFormat[1] = (group_id >> 8) & 0xFF;
            pt_data_req->cmdFormat[2] = 0x00;     /* string lenght */
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}

static int
_cli_cmd_zcl_identify(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr;
    uint8_t ep;

    do
    {
        if (argc < 3)
        {
            break;
        }

        addr = (*(argv[1] + 1) == 'x')
               ? utility_strtox(argv[1] + 2, 0, 4)
               : utility_strtol(argv[1], 0);

        ep = (*(argv[2] + 1) == 'x')
             ? utility_strtox(argv[2] + 2, 0, 2)
             : utility_strtol(argv[2], 0);



        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                            ep, ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_IDENTIFY, ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID, TRUE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, 2)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = 0x05;
            pt_data_req->cmdFormat[1] = 0x00;
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}

static int _cli_cmd_onoff(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    zcl_data_req_t *pt_data_req;
    uint8_t cmd,ep;
    uint16_t address;
    do {
        if(argc < 4) {
            break;
        }
        address = (*(argv[1] + 1) == 'x')
                ? utility_strtox(argv[1] + 2, 0, 4)
                : utility_strtol(argv[1], 0);

        ep = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 2)
                : utility_strtol(argv[2], 0);

        if(strcmp(argv[3],"on") == 0)
        {
            cmd = ZB_ZCL_CMD_ON_OFF_ON_ID;
        }
        else if(strcmp(argv[3],"off") == 0)
        {
            cmd = ZB_ZCL_CMD_ON_OFF_OFF_ID;
        }
        else if(strcmp(argv[3],"toggle") == 0)
        {
            cmd = ZB_ZCL_CMD_ON_OFF_TOGGLE_ID;
        }
        else {
            log_error("Invalid command");
            break;
        }

    ZIGBEE_ZCL_DATA_REQ(pt_data_req, address, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_ON_OFF,
                        cmd, TRUE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, 0, 0)


    if (pt_data_req)
    {
        zigbee_app_zcl_send_command(pt_data_req);
        vPortFree(pt_data_req);
    }


    } while (0);
    return 0;
}
static int _cli_cmd_level(int argc, char** argv, cb_shell_out_t log_out,
                       void* pExtra) {
    zcl_data_req_t *pt_data_req;
    uint8_t level,ep;
    uint16_t address;
    do {
        if(argc < 4) {
            break;
        }
        address = (*(argv[1] + 1) == 'x')
                ? utility_strtox(argv[1] + 2, 0, 4)
                : utility_strtol(argv[1], 0);

        ep = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 2)
                : utility_strtol(argv[2], 0);

        level = (*(argv[3] + 1) == 'x')
                ? utility_strtox(argv[3] + 2, 0, 2)
                : utility_strtol(argv[3], 0);
                

    ZIGBEE_ZCL_DATA_REQ(pt_data_req, address, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                        ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF, TRUE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, 0, 3)


    if (pt_data_req)
    {
        pt_data_req->cmdFormat[0] = level;
        pt_data_req->cmdFormat[1] = 0xA;
        pt_data_req->cmdFormat[2] = 0;
        zigbee_app_zcl_send_command(pt_data_req);
        vPortFree(pt_data_req);
    }


    } while (0);
    return 0;
}

static int
_cli_cmd_zcl_scene(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr, group_id, scence_id, cmd, ep;

    do
    {

        if (memcmp("s", argv[1], 1) == 0)
        {
            cmd = ZB_ZCL_CMD_SCENES_STORE_SCENE;
        }
        else if (memcmp("rc", argv[1], 2) == 0)
        {
            cmd = ZB_ZCL_CMD_SCENES_RECALL_SCENE;
        }
        else if (memcmp("re", argv[1], 2) == 0)
        {
            cmd = ZB_ZCL_CMD_SCENES_REMOVE_SCENE;
        }
        else if (memcmp("v", argv[1], 1) == 0)
        {
            cmd = ZB_ZCL_CMD_SCENES_VIEW_SCENE;
        }

        addr = (*(argv[2] + 1) == 'x')
               ? utility_strtox(argv[2] + 2, 0, 4)
               : utility_strtol(argv[2], 0);

        ep = (*(argv[3] + 1) == 'x')
             ? utility_strtox(argv[3] + 2, 0, 2)
             : utility_strtol(argv[3], 0);

        group_id = (*(argv[4] + 1) == 'x')
                   ? utility_strtox(argv[4] + 2, 0, 4)
                   : utility_strtol(argv[4], 0);

        scence_id = (*(argv[5] + 1) == 'x')
                    ? utility_strtox(argv[5] + 2, 0, 4)
                    : utility_strtol(argv[5], 0);

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_SCENES,
                            cmd, TRUE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, 0, 3)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = group_id & 0xFF;
            pt_data_req->cmdFormat[1] = (group_id >> 8) & 0xFF;
            pt_data_req->cmdFormat[2] = scence_id & 0xFF;

            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}
static int
_cli_cmd_zcl_door_lock(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr;
    uint8_t ep, cmd;
    uint8_t pincode_len = 8;
    uint8_t pin_code[] = {8, '1', '2', '3', '4', '5', '6', '7', '8'};
    do
    {
        if (argc < 5)
        {
            break;
        }

        if (memcmp("lock", argv[1], 4) == 0)
        {
            cmd = ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR;
        }
        else if (memcmp("unlock", argv[1], 6) == 0)
        {
            cmd = ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR;
        }
        addr = (*(argv[2] + 1) == 'x')
               ? utility_strtox(argv[2] + 2, 0, 4)
               : utility_strtol(argv[2], 0);

        ep = (*(argv[3] + 1) == 'x')
             ? utility_strtox(argv[3] + 2, 0, 2)
             : utility_strtol(argv[3], 0);

        pincode_len = strlen(argv[4]);
        memset(pin_code + 1, 0, pincode_len);
        pin_code[0] = pincode_len;
        memcpy(pin_code + 1, argv[4], pincode_len);


        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                            cmd,
                            TRUE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, pincode_len + 1)

        if (pt_data_req)
        {
            memcpy(pt_data_req->cmdFormat, pin_code, pincode_len + 1);
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}
static int
_cli_cmd_zcl_door_lock_set_pin_code(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr;
    uint8_t ep, pincode_len;
    char *pincode;

    do
    {
        if (argc < 4)
        {
            break;
        }


        pincode_len = strlen(argv[1]);
        pincode = argv[1];

        addr = (*(argv[2] + 1) == 'x')
               ? utility_strtox(argv[2] + 2, 0, 4)
               : utility_strtol(argv[2], 0);

        ep = (*(argv[3] + 1) == 'x')
             ? utility_strtox(argv[3] + 2, 0, 2)
             : utility_strtol(argv[3], 0);



        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
                            ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE,
                            TRUE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, 5 + pincode_len)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = 0x00;
            pt_data_req->cmdFormat[1] = 0x00;
            pt_data_req->cmdFormat[2] = 0x01;
            pt_data_req->cmdFormat[3] = 0x00;
            pt_data_req->cmdFormat[4] = pincode_len;
            memcpy(&pt_data_req->cmdFormat[5], pincode, pincode_len);
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}
static int
_cli_cmd_setpt(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr, amount;
    uint8_t ep, mode;

    do
    {
        if (argc < 5)
        {
            break;
        }

        addr = (*(argv[1] + 1) == 'x')
               ? utility_strtox(argv[1] + 2, 0, 4)
               : utility_strtol(argv[1], 0);

        ep = (*(argv[2] + 1) == 'x')
             ? utility_strtox(argv[2] + 2, 0, 2)
             : utility_strtol(argv[2], 0);

        mode = (*(argv[3] + 1) == 'x')
               ? utility_strtox(argv[3] + 2, 0, 1)
               : utility_strtol(argv[3], 0);

        amount = (*(argv[4] + 1) == 'x')
                 ? utility_strtox(argv[4] + 2, 0, 4)
                 : utility_strtol(argv[4], 0);



        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                            ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER,
                            TRUE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, 2)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = mode;
            pt_data_req->cmdFormat[1] = amount;
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}
static int
_cli_cmd_zcl_config_report(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    zcl_data_req_t *pt_data_req;
    uint16_t addr, cluster_id, attr_id, min_interval, max_interval;
    uint8_t ep, data_type, data_len;

    do
    {
        if (argc < 6)
        {
            break;
        }

        addr = (*(argv[1] + 1) == 'x')
               ? utility_strtox(argv[1] + 2, 0, 4)
               : utility_strtol(argv[1], 0);

        ep = (*(argv[2] + 1) == 'x')
             ? utility_strtox(argv[2] + 2, 0, 2)
             : utility_strtol(argv[2], 0);

        cluster_id = (*(argv[3] + 1) == 'x')
                     ? utility_strtox(argv[3] + 2, 0, 4)
                     : utility_strtol(argv[3], 0);

        attr_id = (*(argv[4] + 1) == 'x')
                  ? utility_strtox(argv[4] + 2, 0, 4)
                  : utility_strtol(argv[4], 0);

        data_type = (*(argv[5] + 1) == 'x')
                    ? utility_strtox(argv[5] + 2, 0, 2)
                    : utility_strtol(argv[5], 0);

        min_interval = (*(argv[6] + 1) == 'x')
                       ? utility_strtox(argv[6] + 2, 0, 2)
                       : utility_strtol(argv[6], 0);

        max_interval = (*(argv[7] + 1) == 'x')
                       ? utility_strtox(argv[7] + 2, 0, 2)
                       : utility_strtol(argv[7], 0);

        switch (data_type)//check if it is analog data type
        {
        case ZB_ZCL_ATTR_TYPE_U8:
        case ZB_ZCL_ATTR_TYPE_S8:
            data_len = 9;
            break;
        case ZB_ZCL_ATTR_TYPE_U16:
        case ZB_ZCL_ATTR_TYPE_S16:
            data_len = 10;
            break;
        case ZB_ZCL_ATTR_TYPE_U24:
        case ZB_ZCL_ATTR_TYPE_S24:
            data_len = 11;
            break;
        case ZB_ZCL_ATTR_TYPE_U32:
        case ZB_ZCL_ATTR_TYPE_S32:
        case ZB_ZCL_ATTR_TYPE_UTC_TIME:
            data_len = 12;
            break;
        case ZB_ZCL_ATTR_TYPE_U48:
        case ZB_ZCL_ATTR_TYPE_S48:
            data_len = 14;
            break;
        default:
            data_len = 8;
            break;
        }

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, ep, ZIGBEE_DEFAULT_ENDPOINT,
                            cluster_id,
                            ZB_ZCL_CMD_CONFIG_REPORT,
                            FALSE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, data_len)

        if (pt_data_req)
        {
            pt_data_req->cmdFormat[0] = 0x00;
            pt_data_req->cmdFormat[1] = attr_id & 0xFF;
            pt_data_req->cmdFormat[2] = (attr_id >> 8) & 0xFF;
            pt_data_req->cmdFormat[3] = data_type;
            pt_data_req->cmdFormat[4] = min_interval & 0xFF;
            pt_data_req->cmdFormat[5] = (min_interval >> 8) & 0xFF;
            pt_data_req->cmdFormat[6] = max_interval & 0xFF;
            pt_data_req->cmdFormat[7] = (max_interval >> 8) & 0xFF;
            if (data_len > 8)
            {
                memset(pt_data_req->cmdFormat + 8, 0x00, data_len - 8);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }

    } while (0);
    return 0;
}

//=============================================================================
//                  Public Function Definition
//=============================================================================

const sh_cmd_t g_cli_cmd_bind STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "bind",
    .cmd_exec = _cli_cmd_bind,
    .pDescription = "binding\n"
                    "  usage: bind [g/u] [src addr] [src ep] [dst addr] [dst ep] [cluster]\n"
                    "    g:group binding, u:unicast binding",
                    "    e.g. bind u 0x1234 1 0x0000 1 0x0006",
};
const sh_cmd_t g_cli_cmd_unbind STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "unbind",
    .cmd_exec = _cli_cmd_unbind,
    .pDescription = "binding\n"
                    "  usage: unbind [g/u] [src addr] [src ep] [dst addr] [dst ep] [cluster]\n"
                    "    g:group binding, u:unicast binding",
                    "    e.g. unbind u 0x1234 1 0x0000 1 0x0006",
};
const sh_cmd_t  g_cli_cmd_act_ep_req STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "ep",
    .cmd_exec       = _cli_cmd_zdo_act_ep_req,
    .pDescription   = "active endpoint request\n"
    "  usage: ep [addr]\n"
    "    e.g. ep 0x1234",
};
const sh_cmd_t  g_cli_cmd_simple_desc_req STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "simple",
    .cmd_exec       = _cli_cmd_zdo_simple_desc_req,
    .pDescription   = "simple descriptor request\n"
    "  usage: simple [addr] [ep]\n"
    "    e.g. simple 0x1234 2",
};
const sh_cmd_t  g_cli_cmd_ed_scan_req STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "edscan",
    .cmd_exec       = _cli_cmd_edscan_req,
    .pDescription   = "start energy detection\n"
    "  usage: edscan\n"
};
const sh_cmd_t g_cli_cmd_pj STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "pj",
    .cmd_exec = _cli_cmd_pj,
    .pDescription = "pj\n"
                    "  usage: pj [timeout]\n"
                    "    e.g. pj 180",
};
const sh_cmd_t g_cli_cmd_start STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "start",
    .cmd_exec = _cli_cmd_start,
    .pDescription = "start network\n"
                    "  usage: start [reset] [channel] [panid] [max child]\n"
                    "    e.g. start 0 21 0x1234 30",
};
const sh_cmd_t  g_cli_cmd_attr_read STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "ra",
    .cmd_exec       = _cli_cmd_zcl_attr_read,
    .pDescription   = "read attribute\n"
    "  usage: ra [addr] [dst ep] [cluster id] [attr id]\n"
    "    e.g. ra 0x1234 0x01 0x0003 0x0000\n",
};

const sh_cmd_t  g_cli_cmd_attr_write STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "wa",
    .cmd_exec       = _cli_cmd_zcl_attr_write,
    .pDescription   = "write attribute\n"
    "  usage: wa [addr] [dst ep] [cluster id] [attr id] [attr type] [value]\n"
    "    e.g. wa 0x1234 0x01 0x0003 0x0000 0x21 10 \n",
};

const sh_cmd_t  g_cli_cmd_zcl_group STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "group",
    .cmd_exec       = _cli_cmd_zcl_group,
    .pDescription   = "group\n"
    "  usage: group [action] [addr] [ep] [group id]\n"
    "    e.g. Add :  group a 0x1234 1 0x0001\n",
    "         Remove:group r 0x1234 1 0x0001",
};
const sh_cmd_t  g_cli_cmd_identify STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "id",
    .cmd_exec       = _cli_cmd_zcl_identify,
    .pDescription   = "id\n"
    "  usage: id [addr] [ep]\n"
    "    e.g. id 0x1234 1\n",
};

const sh_cmd_t g_cli_cmd_onoff STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "onoff",
    .cmd_exec = _cli_cmd_onoff,
    .pDescription = "onoff control\n"
                    "  usage: onoff [short addr] [ep] on/off/toggle \n"
                    "    e.g. onoff 0x1234 1 toggle",
};
const sh_cmd_t g_cli_cmd_level STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "level",
    .cmd_exec = _cli_cmd_level,
    .pDescription = "level control\n"
                    "  usage: level [short addr] [ep] [level] \n"
                    "    e.g. level 0x1234 1 128",
};
const sh_cmd_t  g_cli_cmd_zcl_scene STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "scene",
    .cmd_exec       = _cli_cmd_zcl_scene,
    .pDescription   = "scene\n"
    "  usage: scene [action] [addr] [ep] [group id] [scence id]\n"
    "    e.g. Store : scene s 0x1234 1 0x0001 1 \n"
    "         Remove: scene re 0x1234 1 0x0001 1\n"
    "         Recall: scene rc 0x1234 1 0x0001 1\n"
    "         View  : scene v 0x1234 1 0x0001 1\n",
};
const sh_cmd_t  g_cli_cmd_door_lock STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "doorlock",
    .cmd_exec       = _cli_cmd_zcl_door_lock,
    .pDescription   = "send doorlock command\n"
    "  usage: doorlock [lock/unlock] [short address] [end point] pincode\n"
    "    e.g. doorlock lock 0x1234 1 12345678\n",
};
const sh_cmd_t  g_cli_cmd_set_pin_code STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "pincode",
    .cmd_exec       = _cli_cmd_zcl_door_lock_set_pin_code,
    .pDescription   = "set pin code command\n"
    "  usage: pincode [pincode] [short address] [end point]\n"
    "    e.g. pincode 12345678 0x1234 1\n",
};
const sh_cmd_t  g_cli_cmd_setpt STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "setpt",
    .cmd_exec       = _cli_cmd_setpt,
    .pDescription   = "adjust heat/cool setpoint by amount\n"
    "  usage: setpt [short address] [ep] [mode] [amount]\n"
    "    e.g. setpt 0x1234 2 0 30\n",
    "    e.g. setpt 0x1234 2 0 -20\n",
};

const sh_cmd_t  g_cli_cmd_config_report STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "cr",
    .cmd_exec       = _cli_cmd_zcl_config_report,
    .pDescription   = "configure report\n"
    "  usage: cr [addr] [ep] [cluster id] [attribute id] [data type] [min_interval] [max interval]\n"
    "    e.g. cr 0x1234 2 0x0006 0x0000 0x10 0x1E 0x3D\n",
};
