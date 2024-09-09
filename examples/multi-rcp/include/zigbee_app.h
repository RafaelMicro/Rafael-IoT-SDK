/**
 * @file zigee_app.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_APP_H
#define __ZIGBEE_APP_H

#include "FreeRTOS.h"
#include "stdint.h"

#define ZIGBEE_CHANNEL_MASK(n)    (1l << n)
#define ZIGBEE_CHANNEL_ALL_MASK() (0x7FFF800l)

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
/*** Frame Client/Server Directions ***/
#define ZCL_FRAME_CLIENT_SERVER_DIR 0x00
#define ZCL_FRAME_SERVER_CLIENT_DIR 0x01

#define ZIGBEE_ZCL_DATA_REQ(data_req_name, daddr, daddrmode, dep, sep, cid, c, \
                            sp, disdefrsp, dir, mcode, len)                    \
    do {                                                                       \
        data_req_name = pvPortMalloc(sizeof(zcl_data_req_t) + len);            \
        if (data_req_name == NULL)                                             \
            break;                                                             \
        data_req_name->dstAddr = daddr;                                        \
        data_req_name->addrmode = daddrmode;                                   \
        data_req_name->dstEndpint = dep;                                       \
        data_req_name->srcEndPoint = sep;                                      \
        data_req_name->clusterID = cid;                                        \
        data_req_name->cmd = c;                                                \
        data_req_name->specific = sp;                                          \
        data_req_name->disableDefaultRsp = disdefrsp;                          \
        data_req_name->direction = dir;                                        \
        data_req_name->manuCode = mcode;                                       \
        data_req_name->cmdFormatLen = len;                                     \
    } while (0);

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

    uint32_t clusterID         : 16;
    uint32_t cmd               : 8;
    uint32_t specific          : 1;
    uint32_t disableDefaultRsp : 1;
    uint32_t direction         : 1;
    uint32_t addrmode          : 3;
    uint32_t                   : 2;

    uint32_t manuCode     : 16;
    uint32_t cmdFormatLen : 16;

    uint8_t cmdFormat[];

} zcl_data_req_t;

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

extern zb_app_event_t g_zb_app_evt_var;

void zigbee_app_nwk_start(uint32_t channel_mask, uint32_t max_child,
                          uint16_t panId, uint32_t reset);

void zb_app_signal(void);
void zigbee_app_zcl_send_command(zcl_data_req_t* pt_data_req);
void zigbee_app_zcl_read_attr_command(zcl_attr_rw_t* pt_data_req);
void zigbee_app_zcl_write_attr_command(zcl_attr_rw_t* pt_data_req);

void zigbee_app_zcl_read_rsp_cb_reg(void* cb);
void zigbee_app_zcl_write_rsp_cb_reg(void* cb);
void zigbee_app_zcl_cfg_report_rsp_cb_reg(void* cb);
void zigbee_app_zcl_report_attribute_cb_reg(void* cb);
void zigbee_app_zcl_zone_status_change_notification_reg(void* cb);
void zigbee_app_read_otp_mac_addr(uint8_t* addr);

#endif // __ZIGBEE_APP_H