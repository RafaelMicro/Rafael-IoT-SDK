/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file zigbee_app.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-25
 * 
 * 
 */
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include "openthread_port.h"
#include <openthread/link_raw.h>
#include <openthread/link.h>

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"

#include <zigbee_platform.h>
#include "log.h"
#include "zigbee_app.h"
#include "zigbee_cmd_nwk.h"
#include "zigbee_data.h"
#include "zigbee_cmd_app.h"
#include "zigbee_cmd_ota.h"

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

static TimerHandle_t zb_time_timer;

static addr_list_t addr_table;
//=============================================================================
//                Functions
//=============================================================================
void set_gw_time(uint32_t time, uint8_t sync_externally) {
    if (sync_externally)
        g_attr_last_set_time = time;
    g_attr_time = time;
    g_attr_standard_time = time;
    g_attr_local_time = time;
}

uint32_t get_gw_time(void) { return g_attr_time; }

static void time_timer_handler(TimerHandle_t timer) {
    uint32_t cur_time = get_gw_time();
    if (cur_time != ZB_ZCL_TIME_TIME_DEFAULT_VALUE) {
        set_gw_time(cur_time + 1, false);
    }
}

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
        uint8_t ep_state;
        ep_state = cpc_get_endpoint_state(&cpc_zb_ep_handle);

        log_warn("EP Zigbee Error %d!", ep_state);
        if (ep_state == CPC_STATE_ERROR_FAULT)
            cpc_system_reset(1);

        cpc_set_state(&cpc_zb_ep_handle, CPC_STATE_OPEN);

        vPortEnterCritical();
        cpc_zb_ep_handle.ref_count = 1;
        vPortExitCritical();
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

extern zb_ret_t next_data_ind_cb(zb_uint8_t index, zb_zcl_parsed_hdr_t* zcl_hdr,
                                 zb_uint32_t offset, zb_uint8_t size,
                                 zb_uint8_t** data);

void zboss_signal_handler(zb_uint8_t param) {
    zb_zdo_app_signal_hdr_t* sg_p = NULL;
    zb_zdo_app_signal_t sig = zb_get_app_signal(param, &sg_p);
    zb_ret_t z_ret = ZB_GET_APP_SIGNAL_STATUS(param);
    zb_zdo_signal_device_annce_params_t* dev_annce_params;

    log_info(">>zdo_signal_handler: status %d signal %d", z_ret, sig);
    do {
        if (z_ret != 0) {
            break;
        }
        switch (sig) {
            case ZB_ZDO_SIGNAL_SKIP_STARTUP: zboss_start_continue(); break;
            case ZB_BDB_SIGNAL_STEERING: {
                log_info("Successfull steering, start f&b target\n");
            } break;

            case ZB_COMMON_SIGNAL_CAN_SLEEP: {

            } break;

            case ZB_NLME_STATUS_INDICATION: {

            } break;
            case ZB_ZDO_SIGNAL_PERMIT_JOIN: {
                zb_zdo_signal_permit_join_params_t* request =
                    ZB_ZDO_SIGNAL_GET_PARAMS(
                        sg_p, zb_zdo_signal_permit_join_params_t);
                if (request->permit_duration == 0) {
                    zigbee_gw_cmd_send(ZIGBEE_CMD_PERMIT_JOIN_TIMEOUT_NOTIFY, 0x0000, 0, 0, NULL, 0);
                }
            } break;
            case ZB_ZDO_SIGNAL_LEAVE_INDICATION: {
                zb_zdo_signal_leave_indication_params_t* ind_params = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_zdo_signal_leave_indication_params_t);
                if(ind_params->rejoin == 0) {
                    uint16_t short_addr = zb_address_short_by_ieee(ind_params->device_addr);
                    if(short_addr != ZB_UNKNOWN_SHORT_ADDR) {
                        zigbee_gw_cmd_send(ZIGBEE_CMD_DEVICE_LEAVE_INDICATION, 0x0000, 0, 0, (uint8_t*) &short_addr, 2);
                    }
                }
            } break;
            default: break;
        }
    } while (0);

    if (sig == ZB_BDB_SIGNAL_DEVICE_FIRST_START
        || sig == ZB_BDB_SIGNAL_STEERING) {
        if (z_ret == 0 && sig == ZB_BDB_SIGNAL_DEVICE_FIRST_START) {
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        }
    } else if (sig == ZB_BDB_SIGNAL_DEVICE_REBOOT) {
    }

    if (param) {
        zb_buf_free(param);
    }
}
void zigbee_app_mac_ed_scan_command(void) {
    uint8_t status_and_rssi[17];
    uint16_t i, sample_count=300;
    uint8_t rssi_sample[sample_count];
    uint8_t min_value;
    uint8_t channel_num;
    extern uint32_t zboss_start_run;

    memset(status_and_rssi, 0, 17);
    if(zboss_start_run==1)
    {
        log_info("ED scan skipped when zigbee is running....");
        status_and_rssi[0] = 0xFF;  //failed to read the energy level
        zigbee_gw_cmd_send((ZIGBEE_CMD_CHANNEL_ENERGY_SCAN_REQUEST| 0x8000), 0x0000, 0, 0, status_and_rssi, 17);
        return;   
    }
    log_info("Enter ed scan....");

    lmac15p4_auto_state_set(true);
    for(channel_num=11; channel_num<=26; channel_num++)
    {
        ZB_TRANSCEIVER_SET_CHANNEL(0, channel_num);
        
        for(i=0;i<sample_count;i++)
            ZB_TRANSCEIVER_GET_ENERGY_LEVEL(&rssi_sample[i]);
        
        min_value = rssi_sample[0];
        for (i = 1; i < sample_count; i++) 
        {
            if (rssi_sample[i] < min_value) 
                min_value = rssi_sample[i];
        }

        status_and_rssi[(channel_num-11+1)] = min_value;
    }

    zigbee_gw_cmd_send((ZIGBEE_CMD_CHANNEL_ENERGY_SCAN_REQUEST| 0x8000), 0x0000, 0, 0, status_and_rssi, 17);
    #if 0
    log_info("Channel 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26");
    log_info("ED scan %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
            status_and_rssi[0+1], status_and_rssi[1+1], status_and_rssi[2+1], status_and_rssi[3+1], status_and_rssi[4+1], status_and_rssi[5+1], status_and_rssi[6+1], status_and_rssi[7+1],
            status_and_rssi[8+1], status_and_rssi[9+1], status_and_rssi[10+1], status_and_rssi[11+1], status_and_rssi[12+1], status_and_rssi[13+1], status_and_rssi[14+1], status_and_rssi[15+1]);
    #endif
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

    uint8_t ota_ep = get_endpoint_by_cluster(ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                                             ZB_ZCL_CLUSTER_SERVER_ROLE);
    zb_set_nvram_erase_at_start(reset);
    zb_set_network_coordinator_role(ZIGBEE_CHANNEL_MASK(channel_mask));
    zb_set_max_children(max_child);
    zb_set_pan_id(panId);
    zb_zdo_set_aps_unsecure_join(ZB_TRUE);
    zb_zcl_ota_upgrade_init_server(ota_ep, next_data_ind_cb);
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

    zigbee_gw_cmd_send(ZIGBEE_CMD_DEVICE_ANNCE_INDICATION, 0x0000, 0, 0, &pd[1], 11);
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
    // log_info_hexdump("ZCL", (uint8_t*)pData, payload_size);

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
void zcl_send_cmd_cb(zb_uint8_t param)
{
  zb_zcl_command_send_status_t *cmd_send_status = ZB_BUF_GET_PARAM(param, zb_zcl_command_send_status_t);
  zb_uint16_t short_addr = 0;
  
  if (cmd_send_status->dst_addr.addr_type == ZB_ZCL_ADDR_TYPE_SHORT)
  {
    short_addr = cmd_send_status->dst_addr.u.short_addr;
  }
  else
  {
    short_addr = zb_address_short_by_ieee(cmd_send_status->dst_addr.u.ieee_addr);
  }

  zigbee_gw_cmd_send(0x00028005, short_addr, 0, cmd_send_status->dst_endpoint,
    (uint8_t *)&cmd_send_status->status, 1);

  zb_buf_free(param);
}
void zigbee_app_addr_table_update(void)
{
  zb_ushort_t i;
  uint8_t addr_idx=0;
  
  addr_table.status=0xFF;
  addr_table.addr_count=0;
  addr_table.group_count=0;
  memset(&addr_table, 0, 3+ZB_IEEE_ADDR_TABLE_SIZE*2);

  for (i=0; i<ZB_IEEE_ADDR_TABLE_SIZE; i++)
  {
    zb_address_map_t *ent;

    ent = &ZG->addr.addr_map[i];

    if (ZB_U2B(ZG->addr.addr_map[i].used) && (CPC_TX_QUEUE_ITEM_MAX_COUNT  > 0 && !ZG->addr.addr_map[i].pending_for_delete))
    {
      if (ent->redirect_type == ZB_ADDR_REDIRECT_NONE && ent->addr != 0x0000)
      {
        addr_table.short_addr[addr_idx] = ent->addr;
        addr_idx += 1;
      }
    }
  }
  log_info("addr count %d", addr_idx);
  addr_table.addr_count = addr_idx;
  addr_table.status = 0;
  addr_table.group_count = (addr_table.addr_count + ADDR_LIST_GROUP_SIZE - 1) / ADDR_LIST_GROUP_SIZE;

  zigbee_gw_cmd_send((ZIGBEE_CMD_NETWORK_ADDR_TABLE_UPDATE_REQUEST| 0x8000), 0x0000, 0, 0, (uint8_t *)&addr_table, 3);
}

void zigbee_app_get_address_by_group_idx(uint8_t group)
{
    addr_list_by_group_id_t addr_list_response;
    uint8_t j=0;
    uint8_t tmp[3+ADDR_LIST_GROUP_SIZE*2];
    memset(&addr_list_response, 0xFF, 3+ADDR_LIST_GROUP_SIZE*2);
    if(addr_table.status == 0)
    {
        if (group < 1 || group > addr_table.group_count)
        {
            addr_list_response.status = 0xFF;
            zigbee_gw_cmd_send((ZIGBEE_CMD_GET_NETWORK_ADDR_VIA_GROUP_IDX_REQUEST| 0x8000), 0x0000, 0, 0, (uint8_t *)&addr_list_response, 3+ADDR_LIST_GROUP_SIZE*2);
            return;
        }
        
        addr_list_response.status = 0;
        addr_list_response.group_id = group;

        uint16_t start_idx = (group - 1) * ADDR_LIST_GROUP_SIZE;
        uint16_t end_idx = start_idx + ADDR_LIST_GROUP_SIZE;
        if (end_idx > addr_table.addr_count)
        {
            end_idx = addr_table.addr_count;
            addr_list_response.addr_count = end_idx-start_idx;
        }
        
        //log_info("Group %d (index %d ~ %d):", group, start_idx, end_idx - 1);
        for (uint16_t i = start_idx; i < end_idx; i++)
        {
            //log_info("--> 0x%04X", addr_table.short_addr[i]);
            addr_list_response.short_addr[j] = addr_table.short_addr[i];
            j++;
        }
        memcpy(tmp, &addr_list_response, 3+ADDR_LIST_GROUP_SIZE*2);
        zigbee_gw_cmd_send((ZIGBEE_CMD_GET_NETWORK_ADDR_VIA_GROUP_IDX_REQUEST| 0x8000), 0x0000, 0, 0, (uint8_t *)&addr_list_response, 3+ADDR_LIST_GROUP_SIZE*2);
    }
    else
    {
        addr_list_response.status = 0xFF;
        zigbee_gw_cmd_send((ZIGBEE_CMD_GET_NETWORK_ADDR_VIA_GROUP_IDX_REQUEST| 0x8000), 0x0000, 0, 0, (uint8_t *)&addr_list_response, 3+ADDR_LIST_GROUP_SIZE*2);
    }
}
static void __zb_app_task(void* parameters_ptr) {
    zb_app_event_t sevent = ZB_APP_EVENT_NONE;

    __cpc_zb_ep_init();
    ZB_THREAD_SAFE(
        ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_gateway_ctx);
        register_app_cb(zigbee_gw_ota_cb);
        register_zcl_cb(zcl_send_cmd_cb);
        for (int i = 0; i < simple_desc_gateway_ctx.ep_count; i++) {
            ZB_AF_SET_ENDPOINT_HANDLER(
                simple_desc_gateway_ctx.ep_desc_list[i]->ep_id,
                zcl_cmd_handler);
        }
        zb_zdo_register_device_annce_cb(dev_annce_cb);
    )
    zb_time_timer = xTimerCreate("Time", pdMS_TO_TICKS(1000), pdTRUE, (void*)0,
                                 time_timer_handler);
    xTimerStart(zb_time_timer, 0);

    for (;;) {
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0) {
            ZIGBEE_APP_GET_NOTIFY(sevent);
            __zb_app_proc(sevent);
        }
    }
}
void zigbee_app_init(void) {
    BaseType_t xReturned;
    xReturned = xTaskCreate(__zb_app_task, "app-zigbee", 512, NULL,
                            E_TASK_PRIORITY_LOWEST, &zb_app_taskHandle);
    if (xReturned != pdPASS) {
        log_error("ZigBee APP task create fail");
    }

    zb_app_handle = xQueueCreate(32, sizeof(_zb_app_data_t*));


    zigbee_gw_init(zb_app_handle);
}
#endif