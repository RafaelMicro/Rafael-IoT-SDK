/**
 * @file zigbee_cmd.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"
#include "zboss_api_nwk.h"

#include <cpc_user_interface.h>
#include <zigbee_platform.h>
#include "log.h"
#include "zigbee_app.h"
#include "zigbee_cmd.h"
#include "zigbee_gw.h"
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

//=============================================================================
//                Private Global Variables
//=============================================================================
static void __cmd_network_address_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_ieee_address_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_node_descriptor_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_power_descriptor_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_simple_descriptor_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_active_endpoint_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_match_descriptor_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_bind_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_unbind_request(zigbee_cmd_req_t* pt_cmd_req);
static void
__cmd_device_binding_information_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_device_leave_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_direct_join_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_permit_join_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_network_update_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_network_start_request(zigbee_cmd_req_t* pt_cmd_req);
static void __cmd_gateway_ext_address_request(zigbee_cmd_req_t* pt_cmd_req);
static void
__cmd_gateway_permitjoin_status_request(zigbee_cmd_req_t* pt_cmd_req);

static zigbee_cmd_func g_zb_cmd_func[] = {
    [ZIGBEE_CMD_NWK_ADDRESS_REQUEST] = __cmd_network_address_request,
    [ZIGBEE_CMD_IEEE_ADDRESS_REQUEST] = __cmd_ieee_address_request,
    [ZIGBEE_CMD_NODE_DESCRIPTOR_REQUEST] = __cmd_node_descriptor_request,
    [ZIGBEE_CMD_POWER_DESCRIPTOR_REQUEST] = __cmd_power_descriptor_request,
    [ZIGBEE_CMD_SIMPLE_DESCRIPTOR_REQUEST] = __cmd_simple_descriptor_request,
    [ZIGBEE_CMD_ACTIVE_ENDPOINT_REQUEST] = __cmd_active_endpoint_request,
    [ZIGBEE_CMD_MATCH_DESCRIPTOR_REQUEST] = __cmd_match_descriptor_request,
    [ZIGBEE_CMD_BIND_REQUEST] = __cmd_bind_request,
    [ZIGBEE_CMD_UNBIND_REQUEST] = __cmd_unbind_request,
    [ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST] =
        __cmd_device_binding_information_request,
    [ZIGBEE_CMD_DEVICE_LEAVE_REQUEST] = __cmd_device_leave_request,
    [ZIGBEE_CMD_DIRECT_JOIN_REQUEST] = __cmd_direct_join_request,
    [ZIGBEE_CMD_PERMIT_JOIN_REQUEST] = __cmd_permit_join_request,
    [ZIGBEE_CMD_NETWORK_UPDATE_REQUEST] = __cmd_network_update_request,
    [ZIGBEE_CMD_NETWORK_START_REQUEST] = __cmd_network_start_request,
    [ZIGBEE_CMD_GATEWAY_EXT_ADDRESS_REQUEST] =
        __cmd_gateway_ext_address_request,
    [ZIGBEE_CMD_GATEWAY_PERMITJOIN_STATUS_REQUEST] =
        __cmd_gateway_permitjoin_status_request,
};

static uint16_t g_tsn_adrr_tbl[0xFF] = {0};
//=============================================================================
//                Functions
//=============================================================================

extern void nwk_permit_timeout(zb_uint8_t param);

static void _network_address_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_nwk_addr_resp_head_t* resp;
    zb_ieee_addr_t ieee_addr;
    zb_uint16_t nwk_addr;
    zb_address_ieee_ref_t addr_ref;

    uint8_t nwk_addr_rsp_pd[11];

    resp = (zb_zdo_nwk_addr_resp_head_t*)zb_buf_begin(buf);

    nwk_addr_rsp_pd[0] = resp->status;
    memcpy(&nwk_addr_rsp_pd[1], resp->ieee_addr, 8);
    memcpy(&nwk_addr_rsp_pd[9], (uint8_t*)&resp->nwk_addr, 2);
    zb_buf_free(param);

    zigbee_gw_cmd_send((ZIGBEE_CMD_NWK_ADDRESS_REQUEST | 0x8000), 0x0000, 0, 0,
                       nwk_addr_rsp_pd, sizeof(nwk_addr_rsp_pd));
}

static void __cmd_network_address_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_nwk_addr_req_param_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_get_tail(buf, sizeof(zb_zdo_nwk_addr_req_param_t));

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        req_param->dst_addr = pt_cmd_req->cmd_dst_addr;
        ;
        memcpy(req_param->ieee_addr, pdata, 8);
        req_param->request_type = 0;
        req_param->start_index = 0;

        zb_zdo_nwk_addr_req(buf, _network_address_response);
    } while (0);
}

static void _ieee_address_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_nwk_addr_resp_head_t* resp;
    zb_ieee_addr_t ieee_addr;
    zb_uint16_t nwk_addr;
    zb_address_ieee_ref_t addr_ref;

    uint8_t nwk_addr_rsp_pd[11];

    resp = (zb_zdo_nwk_addr_resp_head_t*)zb_buf_begin(buf);

    nwk_addr_rsp_pd[0] = resp->status;
    memcpy(&nwk_addr_rsp_pd[1], resp->ieee_addr, 8);
    memcpy(&nwk_addr_rsp_pd[9], (uint8_t*)&resp->nwk_addr, 2);
    zb_buf_free(param);

    zigbee_gw_cmd_send((ZIGBEE_CMD_IEEE_ADDRESS_REQUEST | 0x8000),
                       resp->nwk_addr, 0, 0, nwk_addr_rsp_pd,
                       sizeof(nwk_addr_rsp_pd));

    // int32_t temp;
    // RfMcu_MemoryGet(0x03A8, (uint8_t *)&temp, 4);
    // temp = (temp >> 8) & 0x3F;
    // printf("check: %d\r\n", temp);
}

static void __cmd_ieee_address_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_ieee_addr_req_param_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_get_tail(buf, sizeof(zb_zdo_ieee_addr_req_param_t));
        req_param->dst_addr = pt_cmd_req->cmd_dst_addr;

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        memcpy((uint8_t*)&req_param->nwk_addr, pdata, 2);
        req_param->request_type = 0;
        req_param->start_index = 0;

        zb_zdo_ieee_addr_req(buf, _ieee_address_response);
    } while (0);
}

static void _node_descriptor_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_node_desc_resp_t* resp;
    uint8_t* prsp_pd = NULL;
    uint8_t p_fail_pd[3];

    do {
        resp = (zb_zdo_node_desc_resp_t*)zb_buf_begin(buf);
        if (resp->hdr.status != 0x00) {
            p_fail_pd[0] = resp->hdr.status;
            break;
        }
        prsp_pd = pvPortMalloc(sizeof(zb_af_node_desc_t) + 1 + 2);
        if (!prsp_pd) {
            p_fail_pd[0] = 0x7F;
            break;
        }

        prsp_pd[0] = resp->hdr.status;
        memcpy(&prsp_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        memcpy(&prsp_pd[3], (uint8_t*)&resp->node_desc,
               sizeof(zb_af_node_desc_t));
        zb_buf_free(param);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send((ZIGBEE_CMD_NODE_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, prsp_pd,
                           sizeof(zb_af_node_desc_t) + 1 + 2);
        vPortFree(prsp_pd);
    } else {
        memcpy(&p_fail_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        zigbee_gw_cmd_send((ZIGBEE_CMD_NODE_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, p_fail_pd,
                           sizeof(p_fail_pd));
    }
}

static void __cmd_node_descriptor_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_node_desc_req_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_initial_alloc(buf, sizeof(zb_zdo_node_desc_req_t));

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        memcpy((uint8_t*)&req_param->nwk_addr, pdata, 2);

        zb_zdo_node_desc_req(buf, _node_descriptor_response);
    } while (0);
}

static void _power_descriptor_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_power_desc_resp_t* resp;
    uint8_t* prsp_pd = NULL;
    uint8_t p_fail_pd[3];

    do {
        resp = (zb_zdo_power_desc_resp_t*)zb_buf_begin(buf);
        if (resp->hdr.status != 0x00) {
            p_fail_pd[0] = resp->hdr.status;
            break;
        }
        prsp_pd = pvPortMalloc(sizeof(zb_af_node_power_desc_t) + 1 + 2);
        if (!prsp_pd) {
            p_fail_pd[0] = 0x7F;
            break;
        }

        prsp_pd[0] = resp->hdr.status;
        memcpy(&prsp_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        memcpy(&prsp_pd[3], (uint8_t*)&resp->power_desc,
               sizeof(zb_af_node_power_desc_t));
        zb_buf_free(param);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send((ZIGBEE_CMD_POWER_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, prsp_pd,
                           sizeof(zb_af_node_power_desc_t) + 1 + 2);
        vPortFree(prsp_pd);
    } else {
        memcpy(&p_fail_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        zigbee_gw_cmd_send((ZIGBEE_CMD_POWER_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, p_fail_pd,
                           sizeof(p_fail_pd));
    }
}

static void __cmd_power_descriptor_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_power_desc_req_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_initial_alloc(buf, sizeof(zb_zdo_node_desc_req_t));

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        memcpy((uint8_t*)&req_param->nwk_addr, pdata, 2);

        zb_zdo_power_desc_req(buf, _power_descriptor_response);
    } while (0);
}

static void _simple_descriptor_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_simple_desc_resp_t* resp;
    uint8_t* prsp_pd = NULL;
    uint8_t p_fail_pd[4];
    uint8_t total_cluster_count, i;
    uint32_t pkt_len;

    // log_info("%s\r\n", __FUNCTION__);

    do {
        resp = (zb_zdo_simple_desc_resp_t*)zb_buf_begin(buf);
        if (resp->hdr.status != 0x00) {
            p_fail_pd[0] = resp->hdr.status;
            break;
        }

        total_cluster_count = resp->simple_desc.app_input_cluster_count
                              + resp->simple_desc.app_output_cluster_count;

        // pkt_len = sizeof(zb_af_simple_desc_1_1_t) + (total_cluster_count * sizeof(uint16_t)) + 1 + 2 + 1;
        pkt_len =
            sizeof(zb_af_simple_desc_1_1_t)
            + ((total_cluster_count)
               * sizeof(uint16_t)); // -4 Take into accout app_cluster_list
        prsp_pd = pvPortMalloc(pkt_len);
        if (!prsp_pd) {
            p_fail_pd[0] = 0x7F;
            break;
        }

        prsp_pd[0] = resp->hdr.status;
        memcpy(&prsp_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        prsp_pd[3] = sizeof(zb_af_simple_desc_1_1_t)
                     + ((total_cluster_count) * sizeof(uint16_t))
                     - (sizeof(uint16_t)
                        * 2); // -4 Take into accout app_cluster_list

        // log_info("descriptro length: %d\r\n", prsp_pd[3]);

        zb_copy_simple_desc((zb_af_simple_desc_1_1_t*)&prsp_pd[4],
                            &resp->simple_desc);
        zb_buf_free(param);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send((ZIGBEE_CMD_SIMPLE_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, prsp_pd, pkt_len);
        vPortFree(prsp_pd);
    } else {
        memcpy(&p_fail_pd[1], (uint8_t*)&resp->hdr.nwk_addr, 2);
        zigbee_gw_cmd_send((ZIGBEE_CMD_SIMPLE_DESCRIPTOR_REQUEST | 0x8000),
                           resp->hdr.nwk_addr, 0, 0, p_fail_pd,
                           sizeof(p_fail_pd));
    }
}

static void __cmd_simple_descriptor_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_simple_desc_req_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_initial_alloc(buf, sizeof(zb_zdo_simple_desc_req_t));

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        memcpy((uint8_t*)req_param, pdata, sizeof(zb_zdo_simple_desc_req_t));

        zb_zdo_simple_desc_req(buf, _simple_descriptor_response);
    } while (0);
}

static void _active_endpoint_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    zb_zdo_ep_resp_t* resp;
    uint8_t* prsp_pd = NULL;
    zb_uint8_t* ep_list;
    uint8_t p_fail_pd[4];

    do {
        resp = (zb_zdo_ep_resp_t*)zb_buf_begin(buf);
        ep_list = (uint8_t*)resp + sizeof(zb_zdo_ep_resp_t);
        if (resp->status != 0x00) {
            p_fail_pd[0] = resp->status;
            break;
        }
        prsp_pd = pvPortMalloc((resp->ep_count * sizeof(uint8_t)) + 1 + 2 + 1);
        if (!prsp_pd) {
            p_fail_pd[0] = 0x7F;
            break;
        }

        prsp_pd[0] = resp->status;
        memcpy(&prsp_pd[1], (uint8_t*)&resp->nwk_addr, 2);
        prsp_pd[3] = resp->ep_count;
        memcpy(&prsp_pd[4], ep_list, resp->ep_count);
        zb_buf_free(param);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send((ZIGBEE_CMD_ACTIVE_ENDPOINT_REQUEST | 0x8000),
                           resp->nwk_addr, 0, 0, prsp_pd,
                           (resp->ep_count * sizeof(uint8_t)) + 1 + 2 + 1);
        vPortFree(prsp_pd);
    } else {
        memcpy(&p_fail_pd[1], (uint8_t*)&resp->nwk_addr, 2);
        p_fail_pd[3] = 0;

        zigbee_gw_cmd_send((ZIGBEE_CMD_ACTIVE_ENDPOINT_REQUEST | 0x8000),
                           resp->nwk_addr, 0, 0, p_fail_pd, sizeof(p_fail_pd));
    }
}

static void __cmd_active_endpoint_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_active_ep_req_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = zb_buf_initial_alloc(buf, sizeof(zb_zdo_active_ep_req_t));

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        memcpy((uint8_t*)req_param, pdata, sizeof(zb_zdo_active_ep_req_t));

        zb_zdo_active_ep_req(buf, _active_endpoint_response);
    } while (0);
}

static void __cmd_match_descriptor_request(zigbee_cmd_req_t* pt_cmd_req) {
    log_info("%p", pt_cmd_req);
}

static void _bind_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd;

    zb_zdo_bind_resp_t* resp;

    do {

        resp = (zb_zdo_bind_resp_t*)zb_buf_begin(buf);
        p_fail_pd = resp->status;

        zb_buf_free(param);
    } while (0);
    zigbee_gw_cmd_send((ZIGBEE_CMD_BIND_REQUEST | 0x8000),
                       g_tsn_adrr_tbl[resp->tsn], 0, 0, &p_fail_pd,
                       sizeof(p_fail_pd));
}

static void __cmd_bind_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_bind_req_param_t* req_param = NULL;
    zb_bufid_t buf;
    uint8_t* pdata;
    uint8_t tsn;
    do {
        pdata = (uint8_t*)pt_cmd_req->cmd_value;
        buf = zb_buf_get_out();

        if (!buf) {
            break;
        }
        req_param = ZB_BUF_GET_PARAM(buf, zb_zdo_bind_req_param_t);
        memcpy((uint8_t*)req_param, pdata, sizeof(zb_zdo_bind_req_param_t));

        memcpy(req_param->src_address, pdata, 8);
        req_param->src_endp = pdata[8];
        memcpy((uint8_t*)&req_param->cluster_id, &pdata[9], 2);
        req_param->dst_addr_mode = pdata[11];
        if (req_param->dst_addr_mode
            == ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT) {
            memcpy((uint8_t*)&req_param->dst_address.addr_short, &pdata[12], 2);
        } else if (req_param->dst_addr_mode
                   == ZB_APS_ADDR_MODE_64_ENDP_PRESENT) {
            memcpy((uint8_t*)&req_param->dst_address.addr_long, &pdata[12], 8);
        } else {
            pdata[0] = 1;
            zigbee_gw_cmd_send((ZIGBEE_CMD_BIND_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
            break;
        }
        if (req_param->dst_addr_mode == 0x03) {
            req_param->dst_endp = pdata[20];
        }
        req_param->req_dst_addr = pt_cmd_req->cmd_dst_addr;

        tsn = zb_zdo_bind_req(buf, _bind_response);

        if (tsn < 0xFF) {
            g_tsn_adrr_tbl[tsn] = pt_cmd_req->cmd_dst_addr;
        } else {
            pdata[0] = tsn;
            zigbee_gw_cmd_send((ZIGBEE_CMD_BIND_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
        }
    } while (0);
}

static void _unbind_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd;

    zb_zdo_bind_resp_t* resp;

    do {

        resp = (zb_zdo_bind_resp_t*)zb_buf_begin(buf);
        p_fail_pd = resp->status;

        zb_buf_free(param);
    } while (0);
    zigbee_gw_cmd_send((ZIGBEE_CMD_UNBIND_REQUEST | 0x8000),
                       g_tsn_adrr_tbl[resp->tsn], 0, 0, &p_fail_pd,
                       sizeof(p_fail_pd));
}

static void __cmd_unbind_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_bind_req_param_t* req_param = NULL;
    zb_bufid_t buf;
    uint8_t* pdata;
    uint8_t tsn;
    do {
        pdata = (uint8_t*)pt_cmd_req->cmd_value;
        buf = zb_buf_get_out();

        if (!buf) {
            break;
        }
        req_param = ZB_BUF_GET_PARAM(buf, zb_zdo_bind_req_param_t);
        memcpy((uint8_t*)req_param, pdata, sizeof(zb_zdo_bind_req_param_t));

        memcpy(req_param->src_address, pdata, 8);
        req_param->src_endp = pdata[8];
        memcpy((uint8_t*)&req_param->cluster_id, &pdata[9], 2);
        req_param->dst_addr_mode = pdata[11];
        if (req_param->dst_addr_mode
            == ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT) {
            memcpy((uint8_t*)&req_param->dst_address.addr_short, &pdata[12], 2);
        } else if (req_param->dst_addr_mode
                   == ZB_APS_ADDR_MODE_64_ENDP_PRESENT) {
            memcpy((uint8_t*)&req_param->dst_address.addr_long, &pdata[12], 8);
        } else {
            pdata[0] = 1;
            zigbee_gw_cmd_send((ZIGBEE_CMD_UNBIND_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
            break;
        }
        if (req_param->dst_addr_mode == 0x03) {
            req_param->dst_endp = pdata[20];
        }
        req_param->req_dst_addr = pt_cmd_req->cmd_dst_addr;

        tsn = zb_zdo_unbind_req(buf, _unbind_response);

        if (tsn < 0xFF) {
            g_tsn_adrr_tbl[tsn] = pt_cmd_req->cmd_dst_addr;
        } else {
            pdata[0] = tsn;
            zigbee_gw_cmd_send((ZIGBEE_CMD_UNBIND_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
        }
    } while (0);
}

static void _device_binding_information_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd[4];
    uint8_t* prsp_pd = NULL;

    zb_zdo_mgmt_bind_resp_t* resp;
    zb_zdo_binding_table_record_t* pt_binding_table;

    do {
        resp = (zb_zdo_mgmt_bind_resp_t*)zb_buf_begin(buf);

        if (resp->status != 0x00) {
            p_fail_pd[0] = resp->status;
            break;
        }
        prsp_pd = pvPortMalloc(resp->binding_table_list_count
                                   * sizeof(zb_zdo_binding_table_record_t)
                               + sizeof(p_fail_pd));

        if (!prsp_pd) {
            p_fail_pd[0] = 0x7F;
            break;
        }

        pt_binding_table =
            (zb_zdo_binding_table_record_t*)((zb_uint8_t*)resp
                                             + sizeof(zb_zdo_mgmt_bind_resp_t));

        prsp_pd[0] = resp->status;
        prsp_pd[1] = resp->binding_table_entries;
        prsp_pd[2] = resp->start_index;
        prsp_pd[3] = resp->binding_table_list_count;
        memcpy(&prsp_pd[4], (uint8_t*)pt_binding_table,
               (resp->binding_table_list_count
                * sizeof(zb_zdo_binding_table_record_t)));
        zb_buf_free(buf);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send(
            (ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST | 0x8000),
            g_tsn_adrr_tbl[resp->tsn], 0, 0, prsp_pd,
            (resp->binding_table_list_count
                 * sizeof(zb_zdo_binding_table_record_t)
             + sizeof(p_fail_pd)));
        vPortFree(prsp_pd);
    } else {
        p_fail_pd[1] = 0;
        p_fail_pd[2] = 0;
        p_fail_pd[3] = 0;
        zigbee_gw_cmd_send(
            (ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST | 0x8000),
            g_tsn_adrr_tbl[resp->tsn], 0, 0, p_fail_pd, sizeof(p_fail_pd));
    }
}

static void
__cmd_device_binding_information_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_bufid_t buf;
    zb_zdo_mgmt_bind_param_t* req_params;
    uint8_t* pdata;
    uint8_t tsn;
    do {
        pdata = (uint8_t*)pt_cmd_req->cmd_value;
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }
        req_params = ZB_BUF_GET_PARAM(buf, zb_zdo_mgmt_bind_param_t);
        req_params->start_index = pdata[0];
        req_params->dst_addr = pt_cmd_req->cmd_dst_addr;
        tsn = zb_zdo_mgmt_bind_req(buf, _device_binding_information_response);
        if (tsn < 0xFF) {
            g_tsn_adrr_tbl[tsn] = pt_cmd_req->cmd_dst_addr;
        } else {
            pdata[0] = tsn;
            zigbee_gw_cmd_send(
                (ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST | 0x8000),
                pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
        }
    } while (0);
}

static void _device_leave_response(zb_uint8_t param) {
    zb_bufid_t buf = param;
    uint8_t p_fail_pd;

    zb_zdo_mgmt_leave_res_t* resp;

    do {

        resp = (zb_zdo_mgmt_leave_res_t*)zb_buf_begin(buf);
        p_fail_pd = resp->status;

        zb_buf_free(param);
    } while (0);

    zigbee_gw_cmd_send((ZIGBEE_CMD_DEVICE_LEAVE_REQUEST | 0x8000),
                       g_tsn_adrr_tbl[resp->tsn], 0, 0, &p_fail_pd,
                       sizeof(p_fail_pd));
}

static void __cmd_device_leave_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_bufid_t buf;
    zb_zdo_mgmt_leave_param_t* req_params;
    uint8_t* pdata;
    uint8_t tsn;
    do {
        pdata = (uint8_t*)pt_cmd_req->cmd_value;
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }
        req_params = ZB_BUF_GET_PARAM(buf, zb_zdo_mgmt_leave_param_t);
        ZB_BZERO(req_params, sizeof(zb_zdo_mgmt_leave_param_t));

        memcpy(req_params->device_address, pdata, 8);
        req_params->remove_children = pdata[8];
        req_params->rejoin = pdata[9];
        if (pt_cmd_req->cmd_dst_addr == 0x0000) {
            req_params->dst_addr = zb_address_short_by_ieee(
                req_params->device_address);
            // log_info("target: %04x\r\n", req_params->dst_addr);
        } else {
            req_params->dst_addr = pt_cmd_req->cmd_dst_addr;
        }
        tsn = zdo_mgmt_leave_req(buf, _device_leave_response);

        if (tsn < 0xFF) {
            g_tsn_adrr_tbl[tsn] = pt_cmd_req->cmd_dst_addr;
        } else {
            pdata[0] = tsn;
            zigbee_gw_cmd_send((ZIGBEE_CMD_DEVICE_LEAVE_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
        }
    } while (0);
}

static void __cmd_direct_join_request(zigbee_cmd_req_t* pt_cmd_req) {
    log_info("%p", pt_cmd_req);
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

static void __cmd_permit_join_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_mgmt_permit_joining_req_param_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = ZB_BUF_GET_PARAM(buf,
                                     zb_zdo_mgmt_permit_joining_req_param_t);

        pdata = (uint8_t*)pt_cmd_req->cmd_value;

        req_param->dest_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
        req_param->permit_duration = pdata[0];
        req_param->tc_significance = 1;

        zb_zdo_mgmt_permit_joining_req(buf, _permit_join_response);

    } while (0);
}

static void _network_update_notify(zb_uint8_t param) {
    zb_uint8_t tsn;
    zb_ret_t ret;
    zb_uint16_t pkt_len;
    zb_zdo_mgmt_nwk_update_notify_param_t* resp;

    uint8_t p_fail_pd;
    uint8_t* prsp_pd = NULL;

    do {
        resp = ZB_BUF_GET_PARAM(param, zb_zdo_mgmt_nwk_update_notify_param_t);

        if (resp->hdr.status != 0x00) {
            p_fail_pd = resp->hdr.status;
            break;
        }

        pkt_len = 1 + 4 + 2 + 2 + 1 + resp->hdr.scanned_channels_list_count;

        prsp_pd = pvPortMalloc(pkt_len);
        if (prsp_pd) {
            p_fail_pd = 0x7F;
            break;
        }

        memcpy(prsp_pd, (uint8_t*)&resp->hdr.status, 1 + 4 + 2 + 2 + 1);
        if (resp->hdr.scanned_channels_list_count > 0) {
            memcpy(&prsp_pd[10], resp->energy_values,
                   resp->hdr.scanned_channels_list_count);
        }

        zb_buf_free(param);
    } while (0);

    if (prsp_pd) {
        zigbee_gw_cmd_send((ZIGBEE_CMD_NETWORK_UPDATE_REQUEST | 0x8000),
                           g_tsn_adrr_tbl[resp->hdr.tsn], 0, 0, prsp_pd,
                           pkt_len);
    } else {
        zigbee_gw_cmd_send((ZIGBEE_CMD_NETWORK_UPDATE_REQUEST | 0x8000),
                           g_tsn_adrr_tbl[resp->hdr.tsn], 0, 0, &p_fail_pd,
                           sizeof(p_fail_pd));
    }
}

static void __cmd_network_update_request(zigbee_cmd_req_t* pt_cmd_req) {
    zb_zdo_mgmt_nwk_update_req_t* req_param;
    zb_bufid_t buf;
    uint8_t* pdata;
    uint8_t tsn;
    pdata = (uint8_t*)pt_cmd_req->cmd_value;

    do {
        buf = zb_buf_get_out();
        if (!buf) {
            break;
        }

        req_param = ZB_BUF_GET_PARAM(buf, zb_zdo_mgmt_nwk_update_req_t);
        memcpy((uint8_t*)&req_param->hdr, pdata, 5);
        req_param->scan_count = pdata[5];
        req_param->manager_addr = pdata[7] | (pdata[8] << 8);
        req_param->dst_addr = pt_cmd_req->cmd_dst_addr;

        tsn = zb_zdo_mgmt_nwk_update_req(buf, _network_update_notify);
        if (tsn < 0xFF) {
            g_tsn_adrr_tbl[tsn] = pt_cmd_req->cmd_dst_addr;
        } else {
            pdata[0] = tsn;
            zigbee_gw_cmd_send((ZIGBEE_CMD_NETWORK_UPDATE_REQUEST | 0x8000),
                               pt_cmd_req->cmd_dst_addr, 0, 0, pdata, 1);
            zb_buf_free(buf);
        }

    } while (0);
}

static void __cmd_network_start_request(zigbee_cmd_req_t* pt_cmd_req) {
    uint8_t* pdata;
    uint8_t channel, reset;
    uint16_t pan_id;
    uint8_t status = 0;

    pdata = (uint8_t*)pt_cmd_req->cmd_value;

    channel = pdata[0];
    memcpy((uint8_t*)&pan_id, &pdata[1], 2);
    reset = pdata[3];

    zigbee_app_nwk_start(channel, 32, pan_id, reset);

    zigbee_gw_cmd_send((ZIGBEE_CMD_NETWORK_START_REQUEST | 0x8000), 0x0000, 0,
                       0, &status, 1);
}

static void __cmd_gateway_ext_address_request(zigbee_cmd_req_t* pt_cmd_req) {
    uint8_t* pdata = NULL;
    uint8_t temp[0x100];
    uint8_t status = STATUS_SUCCESS;
    uint8_t extAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    memcpy(extAddr, ZB_PIBCACHE_EXTENDED_ADDRESS(), 8);

    log_info("ext addr: %02x %02x %02x %02x %02x %02x %02x %02x", extAddr[7],
             extAddr[6], extAddr[5], extAddr[4], extAddr[3], extAddr[2],
             extAddr[1], extAddr[0]);

    temp[0] = status;
    memcpy(&temp[1], extAddr, 8);

    zigbee_gw_cmd_send((ZIGBEE_CMD_GATEWAY_EXT_ADDRESS_REQUEST | 0x8000),
                       0x0000, 0, 0, temp, 9);
}

static void
__cmd_gateway_permitjoin_status_request(zigbee_cmd_req_t* pt_cmd_req) {
    permit_join_status_t status;
    zb_time_t time, current;

    status.association_permit = ZB_PIBCACHE_ASSOCIATION_PERMIT();
    status.remaining_time = 0;
    zb_schedule_get_alarm_time(nwk_permit_timeout, 0, &time);
    log_info("association permit: %s",
             status.association_permit ? "true" : "false");
    current = ZB_TIMER_GET();
    if ((status.association_permit == ZB_TRUE_U) && ZB_TIME_GE(time, current)) {
        status.remaining_time = (time - current) / ZB_TIME_ONE_SECOND;
        log_info("remaining time: %d s", status.remaining_time);
    }
    zigbee_gw_cmd_send((ZIGBEE_CMD_GATEWAY_PERMITJOIN_STATUS_REQUEST | 0x8000),
                       0x0000, 0, 0, (void*)&status,
                       sizeof(permit_join_status_t));
}

static void __cmd_handle(zigbee_cmd_req_t* pt_cmd_req) {
    if (g_zb_cmd_func[pt_cmd_req->cmd_index]) {
        ZB_THREAD_SAFE(g_zb_cmd_func[pt_cmd_req->cmd_index](pt_cmd_req););
    }
}

void zigbee_cmd_request(uint16_t dst_addr, uint32_t t_index, uint32_t u32_len,
                        uint8_t* pu8_value) {
    zigbee_cmd_req_t* pt_cmd_req = NULL;

    do {
        pt_cmd_req = pvPortMalloc(sizeof(zigbee_cmd_req_t) + u32_len);

        if (pt_cmd_req == NULL) {
            log_error("zb cmd malloc fail");
            break;
        }
        memset(pt_cmd_req, 0x00, sizeof(zigbee_cmd_req_t) + u32_len);

        pt_cmd_req->cmd_index = t_index;
        pt_cmd_req->cmd_length = u32_len;
        pt_cmd_req->cmd_dst_addr = dst_addr;
        memcpy(pt_cmd_req->cmd_value, pu8_value, u32_len);

        __cmd_handle(pt_cmd_req);
    } while (0);

    if (pt_cmd_req != NULL)
        vPortFree(pt_cmd_req);
}
#endif