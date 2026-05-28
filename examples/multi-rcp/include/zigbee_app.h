/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file zigee_app.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * 
 */

#ifndef __ZIGBEE_APP_H
#define __ZIGBEE_APP_H

#include "FreeRTOS.h"
#include "stdint.h"
#include "zb_common.h"
#include "flashctl.h"

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

#define ZIGBEE_DEFAULT_ENDPOINT 0x01
#define ADDR_LIST_GROUP_SIZE 10

#define ZIGBEE_ZCL_ATTR_RW_REQ(data_req_name, daddr, dep, sep, cid, aid, at,   \
                               len)                                            \
    do {                                                                       \
        data_req_name = pvPortMalloc(sizeof(zcl_attr_rw_t) + len);             \
        if (data_req_name == NULL)                                             \
            break;                                                             \
        data_req_name->dstAddr = daddr;                                        \
        data_req_name->dstEndpint = dep;                                       \
        data_req_name->srcEndPoint = sep;                                      \
        data_req_name->clusterID = cid;                                        \
        data_req_name->attrID = aid;                                           \
        data_req_name->attrType = at;                                          \
        data_req_name->attrLen = len;                                          \
    } while (0);

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

    ZB_APP_EVENT_CPC_READ = 0x00000002,
    ZB_APP_EVENT_CPC_WRITE_DONE = 0x00000004,
    ZB_APP_EVENT_CPC_ERROR = 0x00000008,

    ZB_APP_EVENT_ALL = 0xffffffff,
} zb_app_event_t;

typedef struct {
    uint16_t dlen;
    uint8_t pdata[];
} _zb_app_data_t;

typedef struct {
    uint32_t dstAddr     : 16;
    uint32_t dstEndpint  : 8;
    uint32_t srcEndPoint : 8;

    uint32_t clusterID : 16;
    uint32_t attrID    : 16;

    uint32_t attrType : 8;
    uint32_t attrLen  : 8;
    uint32_t          : 16;

    uint8_t attrValue[];
} zcl_attr_rw_t;

typedef struct {
    uint32_t ep          : 8;
    uint32_t clusterRole : 8;
    uint32_t checkAccess : 8;
    uint32_t             : 8;

    uint32_t clusterID : 16;
    uint32_t attrId    : 16;

    uint8_t value[];
} zcl_attr_set_t;

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

void zigbee_app_nwk_start(uint32_t channel_mask, uint32_t max_child,
                          uint16_t panId, uint32_t reset);

void zb_app_signal(void);

void zigbee_app_zcl_read_rsp_cb_reg(void* cb);
void zigbee_app_zcl_write_rsp_cb_reg(void* cb);
void zigbee_app_zcl_cfg_report_rsp_cb_reg(void* cb);
void zigbee_app_zcl_report_attribute_cb_reg(void* cb);
void zigbee_app_zcl_zone_status_change_notification_reg(void* cb);
void zigbee_app_read_otp_mac_addr(uint8_t* addr);
void set_gw_time(uint32_t time, uint8_t sync_externally);
uint32_t get_gw_time(void);
void zigbee_app_mac_ed_scan_command(void);
void zigbee_app_addr_table_update(void);
void zigbee_app_get_address_by_group_idx(uint8_t group);

#endif // __ZIGBEE_APP_H