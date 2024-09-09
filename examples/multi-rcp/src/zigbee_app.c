/**
 * @file zigbee_app.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-25
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
#include "queue.h"

#include "openthread_port.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"

#include <zigbee_platform.h>
#include "log.h"
#include "zigbee_app.h"
#include "zigbee_cmd.h"
#include "zigbee_data.h"
#include "zigbee_gw.h"

#include <cpc.h>
#include <cpc_api.h>
#include <cpc_user_interface.h>

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
static TaskHandle_t zb_app_taskHandle;
zb_app_event_t g_zb_app_evt_var;
static cpc_endpoint_handle_t cpc_zb_ep_handle;
static QueueHandle_t zb_app_handle;
static zcl_read_rsp_cb p_read_rsp_cb = NULL;
static zcl_write_rsp_cb p_write_rsp_cb = NULL;
static zcl_cfg_report_rsp_cb p_cfg_report_rsp_cb = NULL;
static zcl_report_attribute_cb p_report_attribute_cb = NULL;
static zcl_zone_status_change_notification_cb
    p_zone_status_change_notification_cb = NULL;

//=============================================================================
//                Functions
//=============================================================================
void zb_app_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(zb_app_taskHandle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(zb_app_taskHandle);
    }
}

static void __cpc_zb_write_done_evt(cpc_user_endpoint_id_t endpoint_id,
                                    void* buffer, void* arg, status_t status) {
    (void)endpoint_id;
    (void)buffer;

    if (arg)
        vPortFree(arg);

    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_CPC_WRITE_DONE);
}

static void __cpc_zb_read_evt(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_CPC_READ);
}

static void __cpc_zb_error_evt(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_CPC_ERROR);
}

static void __cpc_zb_ep_init(void) {
    uint32_t status;
    status = cpc_open_service_endpoint(&cpc_zb_ep_handle, CPC_ENDPOINT_ZIGBEE,
                                       0, 1);

    status = cpc_set_endpoint_option(&cpc_zb_ep_handle,
                                     CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                                     (void*)__cpc_zb_write_done_evt);

    status = cpc_set_endpoint_option(&cpc_zb_ep_handle,
                                     CPC_ENDPOINT_ON_IFRAME_RECEIVE,
                                     (void*)__cpc_zb_read_evt);

    status = cpc_set_endpoint_option(&cpc_zb_ep_handle, CPC_ENDPOINT_ON_ERROR,
                                     (void*)__cpc_zb_error_evt);

    if (status == 0)
        log_info("cpc zb ep init success");
}

void zigbee_app_read_otp_mac_addr(uint8_t* addr) {
    uint8_t temp[0x100];
    flash_read_sec_register((uint32_t)temp, 0x1100);
    memcpy(addr, temp + 8, 8);
}

static void __zb_app_proc(zb_app_event_t evt) {
    uint8_t ep_state;
    uint32_t rval = 0;
    uint8_t* read_buf;
    uint16_t len;

    uint32_t __tx_done = 1;
    uint8_t* ptr = NULL;
    _zb_app_data_t* cpc_data = NULL;
    static uint8_t tmp_buffer[400];
    static uint16_t idx = 0;

    if (ZB_APP_EVENT_ZBOSS_CB_IN & evt) {
        if (idx < 320) {
            while (xQueueReceive(zb_app_handle, (void*)&cpc_data, 0)
                   == pdPASS) {
                memcpy(&tmp_buffer[idx], cpc_data->pdata, cpc_data->dlen);
                idx += cpc_data->dlen;
                vPortFree(cpc_data);

                if (idx >= 380) {
                    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_ZBOSS_CB_IN);
                    break;
                }
                vTaskDelay(5);
            }
        } else {
            ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_ZBOSS_CB_IN);
        }
    }
    if (ZB_APP_EVENT_CPC_READ & evt) {
        rval = cpc_read(&cpc_zb_ep_handle, (void**)&read_buf, &len, 0, 1);

        if (rval) {
            log_error("zb read error %04lX", rval);
        } else {
            zigbee_gw_cmd_proc(read_buf, len);
            cpc_free_rx_buffer(read_buf);
        }
    }
    if (ZB_APP_EVENT_CPC_WRITE_DONE & evt) {
        __tx_done = 0;
    }
    if (ZB_APP_EVENT_CPC_ERROR & evt) {
        // ep_state = cpc_get_endpoint_state(&cpc_zb_ep_handle);

        // log_error("cpc zb ep error %d", ep_state);

        // if(ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
        //     cpc_system_reset(0);

        // cpc_close_endpoint(&cpc_zb_ep_handle);
        cpc_set_state(&cpc_zb_ep_handle, CPC_STATE_OPEN);

        cpc_zb_ep_handle.ref_count = 1u;
    }

    if ((__tx_done == 1) && (idx > 0)) {
        ptr = pvPortMalloc(idx);

        if (ptr != NULL) {
            memcpy(ptr, tmp_buffer, idx);
            rval = cpc_write(&cpc_zb_ep_handle, ptr, idx, 0, (void*)ptr);

            if (rval != 0) {
                log_error("cpc zb write error %04lX", rval);
                vPortFree(ptr);
                ptr = NULL;
            } else {
                __tx_done = 0;
            }
        }
        idx = 0;
    }
}

static void __zb_app_task(void* parameters_ptr) {
    zb_app_event_t sevent = ZB_APP_EVENT_NONE;

    for (;;) {
        ZIGBEE_APP_GET_NOTIFY(sevent);
        __zb_app_proc(sevent);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void zigbee_app_nwk_start(uint32_t channel_mask, uint32_t max_child,
                          uint16_t panId, uint32_t reset) {
    uint8_t ieeeAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    zigbee_app_read_otp_mac_addr(ieeeAddr);

    if ((ieeeAddr[0] == 0xFF) && (ieeeAddr[1] == 0xFF) && (ieeeAddr[2] == 0xFF)
        && (ieeeAddr[3] == 0xFF) && (ieeeAddr[4] == 0xFF)
        && (ieeeAddr[5] == 0xFF) && (ieeeAddr[6] == 0xFF)
        && (ieeeAddr[7] == 0xFF)) {
        flash_get_unique_id((uint32_t)ieeeAddr, 8);
    }

    log_info("15p4 MAC Address : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
             ieeeAddr[7], ieeeAddr[6], ieeeAddr[5], ieeeAddr[4], ieeeAddr[3],
             ieeeAddr[2], ieeeAddr[1], ieeeAddr[0]);

    zb_set_long_address(ieeeAddr);

    zb_set_nvram_erase_at_start(reset);
    zb_set_network_coordinator_role(ZIGBEE_CHANNEL_MASK(channel_mask));
    zb_set_max_children(max_child);
    zb_set_pan_id(panId);
    zb_zdo_set_aps_unsecure_join(ZB_TRUE);
    zb_bdb_set_legacy_device_support(ZB_TRUE);

    OT_THREAD_SAFE(
        otInstance* instance = otrGetInstance();

        if (otLinkRawIsEnabled(instance)) {
            log_info("Thread radion enabled, Enable 2 channel scan (%d, %d)",
                     otLinkGetChannel(instance), channel_mask);

            lmac15p4_auto_state_set(false);

            otLinkGetChannel(instance);

            lmac15p4_2ch_scan_set(true, (otLinkGetChannel(instance) - 11),
                                  (channel_mask - 11));
        } else { lmac15p4_2ch_scan_set(false, 0, (channel_mask - 11)); })

    zbStartRun();
}

static void dev_annce_cb(zb_zdo_device_annce_t* da) {
    uint8_t* pd = (uint8_t*)da;

    zigbee_gw_cmd_send((0x0013), 0x0000, 0, 0, &pd[1], 11);
}

static uint8_t zcl_cmd_handler(zb_uint8_t param) {
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

    log_info("Recv ZCL message 0x%04X -> 0x%04X", src_addr, dest_addr);
    log_info("Cluster %04x cmd %d seq %d", cmd_info->cluster_id,
             cmd_info->cmd_id, cmd_info->seq_number);
    log_info_hexdump("ZCL", (uint8_t*)pData, payload_size);

    if (cmd_info->is_common_command) {
        if (cmd_info->cmd_id == 0x01) // Read response
        {
            if (p_read_rsp_cb) {
                p_read_rsp_cb(cmd_info->cluster_id, src_addr, src_ep, pData,
                              payload_size);
            }
        } else if (cmd_info->cmd_id == 0x04) // Write response
        {
            if (p_write_rsp_cb) {
                p_write_rsp_cb(cmd_info->cluster_id, src_addr, src_ep, pData,
                               payload_size);
            }
        } else if (cmd_info->cmd_id == 0x07) // Config report response
        {
            if (p_cfg_report_rsp_cb) {
                p_cfg_report_rsp_cb(cmd_info->cluster_id, src_addr, src_ep,
                                    pData, payload_size);
            }
        } else if (cmd_info->cmd_id == 0x0a) // Report
        {
            // zigbee_app_zcl_report_attribute_cb_reg(_zcl_report_attribute_cb);
            // p_report_attribute_cb(cmd_info->cluster_id, src_addr, src_ep, pData, payload_size);
            uint8_t report_data[100];
            memcpy(report_data, &(cmd_info->cluster_id), sizeof(zb_uint16_t));
            memcpy(report_data + sizeof(zb_uint16_t), pData, payload_size);
            payload_size += sizeof(zb_uint16_t);
            zigbee_gw_cmd_send(0x00028800, src_addr, 0, src_ep, report_data,
                               payload_size);
        } else if (cmd_info->cmd_id == 0x0b) // defaut response
        {
            zigbee_gw_cmd_send(0x00018800, src_addr, 0, src_ep, pData,
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
        }
    }

    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_ZBOSS_CB_IN);
    return cmd_processed;
}

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

void zigbee_app_zcl_read_attr_command(zcl_attr_rw_t* pt_data_req) {
    uint16_t dstAddr = pt_data_req->dstAddr;
    zb_bufid_t buffer = 0;
    zb_uint8_t* cmd_ptr;
    ZB_THREAD_SAFE(do {
        buffer = zb_buf_get_any();

        if (buffer == 0)
            break;

        ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ(buffer, cmd_ptr,
                                          ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
        ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, pt_data_req->attrID);
        ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ(
            buffer, cmd_ptr, dstAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
            pt_data_req->dstEndpint, pt_data_req->srcEndPoint,
            ZB_AF_HA_PROFILE_ID, pt_data_req->clusterID, NULL);

        ZB_APP_NOTIFY(ZB_SYSTEM_EVENT_APP);
    } while (0););
}

void zigbee_app_zcl_write_attr_command(zcl_attr_rw_t* pt_data_req) {
    uint16_t dstAddr = pt_data_req->dstAddr;
    zb_bufid_t buffer = 0;
    zb_uint8_t* cmd_ptr;
    ZB_THREAD_SAFE(do {
        buffer = zb_buf_get_any();

        if (buffer == 0)
            break;

        ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ(buffer, cmd_ptr,
                                           ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
        ZB_ZCL_GENERAL_ADD_ID_WRITE_ATTR_REQ(cmd_ptr, pt_data_req->attrID,
                                             pt_data_req->attrType,
                                             pt_data_req->attrValue);
        ZB_ZCL_GENERAL_SEND_WRITE_ATTR_REQ(
            buffer, cmd_ptr, dstAddr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
            pt_data_req->dstEndpint, pt_data_req->srcEndPoint,
            ZB_AF_HA_PROFILE_ID, pt_data_req->clusterID, NULL);

        ZB_APP_NOTIFY(ZB_SYSTEM_EVENT_APP);
    } while (0););
}

void zigbee_app_zcl_send_command(zcl_data_req_t* pt_data_req) {
    uint16_t dstAddr = pt_data_req->dstAddr;
    zb_bufid_t buffer = 0;
    zb_uint8_t* ptr;
    ZB_THREAD_SAFE(do {
        buffer = zb_buf_get_any();

        if (buffer == 0)
            break;

        ptr = zb_zcl_start_command_header(
            buffer,
            ZB_ZCL_CONSTRUCT_FRAME_CONTROL(
                pt_data_req->specific, (pt_data_req->manuCode == 0) ? 0 : 1,
                pt_data_req->direction, pt_data_req->disableDefaultRsp),
            pt_data_req->manuCode, pt_data_req->cmd, NULL);
        memcpy(ptr, pt_data_req->cmdFormat, pt_data_req->cmdFormatLen);
        ptr += pt_data_req->cmdFormatLen;

        ZB_ZCL_FINISH_PACKET((buffer), ptr)
        ZB_ZCL_SEND_COMMAND_SHORT(
            buffer, dstAddr, pt_data_req->addrmode, pt_data_req->dstEndpint,
            pt_data_req->srcEndPoint, 0x0104, pt_data_req->clusterID, NULL);
        ZB_APP_NOTIFY(ZB_SYSTEM_EVENT_APP);
    } while (0););
}

void zigbee_app_init(void) {
    BaseType_t xReturned;
    xReturned = xTaskCreate(__zb_app_task, "ZB_APP", 512, NULL,
                            E_TASK_PRIORITY_NORMAL, &zb_app_taskHandle);
    if (xReturned != pdPASS) {
        log_error("ZigBee APP task create fail");
    }

    zb_app_handle = xQueueCreate(16, sizeof(_zb_app_data_t*));

    __cpc_zb_ep_init();

    log_info("simple_desc_gateway_ctx %p", &simple_desc_gateway_ctx);

    ZB_THREAD_SAFE(ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_gateway_ctx);

                   for (int i = 0; i < simple_desc_gateway_ctx.ep_count; i++) {
                       ZB_AF_SET_ENDPOINT_HANDLER(
                           simple_desc_gateway_ctx.ep_desc_list[i]->ep_id,
                           zcl_cmd_handler);
                   }

                   zb_zdo_register_device_annce_cb(dev_annce_cb);)
    zigbee_gw_init(zb_app_handle);
}
#endif