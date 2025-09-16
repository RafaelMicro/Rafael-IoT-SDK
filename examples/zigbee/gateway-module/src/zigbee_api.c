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
//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"

#include <zigbee_platform.h>
#include "log.h"
#include "zigbee_api.h"
#include "zigbee_cmd_nwk.h"
#include "zigbee_cmd_app.h"
#include "zigbee_cmd_ota.h"
#include "zigbee_zcl_msg_handler.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define ZB_TRACE_FILE_ID 294
#ifdef ZB_USE_SLEEP
#error Gateway cant be configured as sleepy device
#endif
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
static QueueHandle_t zb_app_handle;

static TimerHandle_t zb_time_timer;
static addr_list_t addr_table;

//=============================================================================
//                Functions
//=============================================================================

static void time_timer_handler(TimerHandle_t timer) {
    uint32_t cur_time = get_gw_time();
    if (cur_time != ZB_ZCL_TIME_TIME_DEFAULT_VALUE) {
        set_gw_time(cur_time + 1, false);
    }
}

void start_gw_timer(void) {
    zb_time_timer = xTimerCreate("Time", pdMS_TO_TICKS(1000), pdTRUE, (void*)0,
                                 time_timer_handler);
    xTimerStart(zb_time_timer, 0);
}

void zb_app_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(zb_app_taskHandle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(zb_app_taskHandle);
    }
}

void zigbee_app_read_otp_mac_addr(uint8_t* addr) {
    uint8_t temp[0x100];
    flash_read_sec_register((uint32_t)temp, 0x1100);
    memcpy(addr, temp + 8, 8);
}

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

            case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
            case ZB_BDB_SIGNAL_STEERING:
            case ZB_BDB_SIGNAL_DEVICE_REBOOT:
                if (z_ret == 0) {
                    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_JOINED);
                }

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

extern zb_ret_t next_data_ind_cb(zb_uint8_t index, zb_zcl_parsed_hdr_t* zcl_hdr,
    zb_uint32_t offset, zb_uint8_t size, zb_uint8_t** data);

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
    #if 1
    log_info("Channel 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26");
    log_info("ED scan %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
            status_and_rssi[0+1], status_and_rssi[1+1], status_and_rssi[2+1], status_and_rssi[3+1], status_and_rssi[4+1], status_and_rssi[5+1], status_and_rssi[6+1], status_and_rssi[7+1],
            status_and_rssi[8+1], status_and_rssi[9+1], status_and_rssi[10+1], status_and_rssi[11+1], status_and_rssi[12+1], status_and_rssi[13+1], status_and_rssi[14+1], status_and_rssi[15+1]);
    #endif
}
void zigbee_app_nwk_start(uint32_t channel, uint32_t max_child,
                          uint16_t panId, uint32_t reset) {
    uint8_t ieee_addr_invalid[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t ieee_addr[8];
    static uint8_t zb_started = 0;

    if(zb_started)
    {
        return;
    }

    zb_started = 1;
    zigbee_app_read_otp_mac_addr(ieee_addr);

    if (memcmp(ieee_addr, ieee_addr_invalid, 8) == 0) {
        flash_get_unique_id((uint32_t)ieee_addr, 8);
    }

    if(channel < 11) {
        channel = 11;
    }
    if(channel > 26) {
        channel = 26;
    }

    log_info("15p4 MAC Address : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
             ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
             ieee_addr[2], ieee_addr[1], ieee_addr[0]);

    zb_set_long_address(ieee_addr);

    zb_set_nvram_erase_at_start(reset);
    zb_set_network_coordinator_role(ZIGBEE_CHANNEL_MASK(channel));
    zb_set_max_children(max_child);
    zb_set_pan_id(panId);
    zb_zdo_set_aps_unsecure_join(ZB_TRUE);
    zb_bdb_set_legacy_device_support(ZB_TRUE);
    zb_zcl_ota_upgrade_init_server(ZIGBEE_DEFAULT_ENDPOINT, next_data_ind_cb);

    lmac15p4_channel_set((channel - 11));
    zbStartRun();
}

void dev_annce_cb(zb_zdo_device_annce_t* da) {
    uint8_t* pd = (uint8_t*)da;

    zigbee_gw_cmd_send(ZIGBEE_CMD_DEVICE_ANNCE_INDICATION, 0x0000, 0, 0, &pd[1], 11);
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

    if (ZB_U2B(ZG->addr.addr_map[i].used) && (ZG->addr.addr_map[i].lock_cnt > 0 && !ZG->addr.addr_map[i].pending_for_delete))
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
void zigbee_app_init(void) {
    BaseType_t xReturned;
    xReturned = xTaskCreate(app_main_loop, "app-zigbee", 512, NULL,
                            E_TASK_PRIORITY_LOWEST, &zb_app_taskHandle);
    if (xReturned != pdPASS) {
        log_error("ZigBee APP task create fail");
    }

    zb_app_handle = xQueueCreate(16, sizeof(_zb_app_data_t*));

    zigbee_gw_init(zb_app_handle);
}