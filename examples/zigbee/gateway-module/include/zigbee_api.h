/**
 * @file zigbee_api.h
 * @brief
 *  
 * @version 0.1
 * 
 * @date 
 * 
 */

#ifndef __ZIGBEE_API_H
#define __ZIGBEE_API_H

#include <stdint.h>
#include <zigbee_platform.h>
#include "fota_define.h"
#include "hosal_gpio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "zb_common.h"

#define ZIGBEE_DEFAULT_ENDPOINT 1
#define ADDR_LIST_GROUP_SIZE 10

#define ZIGBEE_APP_NOTIFY_ISR(ebit)                                            \
    (g_zb_app_evt_var |= ebit);                                                \
    zb_app_signal()
#define ZIGBEE_APP_NOTIFY(ebit)                                                \
    vPortEnterCritical();                                                      \
    g_zb_app_evt_var |= ebit;                                                  \
    vPortExitCritical();                                                       \
    zb_app_signal()
#define ZIGBEE_APP_GET_NOTIFY(ebit)                                            \
    vPortEnterCritical();                                                      \
    ebit = g_zb_app_evt_var;                                                   \
    g_zb_app_evt_var = ZB_APP_EVENT_NONE;                                      \
    vPortExitCritical()

typedef void (*zcl_read_rsp_cb)(uint16_t cluster_id, uint16_t addr,
                                uint8_t src_endp, uint8_t* pd, uint8_t pd_len);
typedef void (*zcl_write_rsp_cb)(uint16_t cluster_id, uint16_t addr,
                                 uint8_t src_endp, uint8_t* pd, uint8_t pd_len);
typedef void (*zcl_cfg_report_rsp_cb)(uint16_t cluster_id, uint16_t addr,
                                      uint8_t src_endp, uint8_t* pd,
                                      uint8_t pd_len);
typedef void (*zcl_report_attribute_cb)(uint16_t cluster_id, uint16_t addr,
                                        uint8_t src_endp, uint8_t* pd,
                                        uint8_t pd_len);
typedef void (*zcl_zone_status_change_notification_cb)(uint16_t cluster_id,
                                                       uint16_t addr,
                                                       uint8_t src_endp,
                                                       uint8_t* pd,
                                                       uint8_t pd_len);

typedef enum {
    ZB_APP_EVENT_NONE = 0,
    ZB_APP_EVENT_ZBOSS_CB_IN = 0x00000001,
    ZB_APP_EVENT_UART_DATA_IN = 0x00000002,
    ZB_APP_EVENT_UART_TX_DONE = 0x00000004,
    ZB_APP_EVENT_JOINED = 0x00000008,
    ZB_APP_EVENT_ALL = 0xffffffff,
} zb_app_event_t;

typedef struct {
    uint16_t dlen;
    uint8_t pdata[];
} _zb_app_data_t;

typedef struct __attribute__((packed)) {
    uint8_t status;
    uint8_t addr_count;
    uint8_t group_count;
    uint16_t short_addr[ZB_IEEE_ADDR_TABLE_SIZE];
} addr_list_t;

typedef struct __attribute__((packed)) {
    uint8_t status;
    uint8_t addr_count;
    uint8_t group_id;
    uint16_t short_addr[ADDR_LIST_GROUP_SIZE];
} addr_list_by_group_id_t;

extern zb_app_event_t g_zb_app_evt_var;

zb_app_event_t g_zb_app_evt_var;
extern zb_af_device_ctx_t simple_desc_gateway_ctx;
void zigbee_app_init(void);
void app_main_loop(void* parameters_ptr);
void zb_app_signal(void);
void dev_annce_cb(zb_zdo_device_annce_t* da);
void zcl_send_cmd_cb(zb_uint8_t param);
void zigbee_app_nwk_start(uint32_t channel, uint32_t max_child, uint16_t panId, uint32_t reset);

void zigbee_app_zcl_read_rsp_cb_reg(void* cb);
void zigbee_app_zcl_write_rsp_cb_reg(void* cb);
void zigbee_app_zcl_cfg_report_rsp_cb_reg(void* cb);
void zigbee_app_zcl_report_attribute_cb_reg(void* cb);
void zigbee_app_zcl_zone_status_change_notification_reg(void* cb);
void zigbee_app_read_otp_mac_addr(uint8_t* addr);

void start_gw_timer(void);
void set_gw_time(uint32_t time, uint8_t sync_externally);
uint32_t get_gw_time(void);
void zigbee_app_mac_ed_scan_command(void);
void zigbee_app_addr_table_update(void);
void zigbee_app_get_address_by_group_idx(uint8_t group);
#endif // __ZIGBEE_API_H
