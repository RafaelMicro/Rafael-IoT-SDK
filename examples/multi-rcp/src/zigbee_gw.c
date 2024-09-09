/**
 * @file zigbee_gw.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=============================================================================
//                Include
//=============================================================================
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
#include "mcu.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "zb_common.h"
#include "zb_mac_globals.h"
#include "zboss_api.h"

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
typedef void (*gw_cmd_app_func)(uint32_t, uint16_t, uint8_t*);
//=============================================================================
//                Private ENUM
//=============================================================================

#define GW_CMD_APP_CMD_RSP_GEN(id) (id | 0x8000)
#define GW_CMD_APP_CMD_OFFSET      0x10000
#define GW_CMD_APP_SRV_DEV_BASE    0x10000

typedef enum {
    GW_CMD_APP_SRV_DEV_GET_VER_INFO = 0,
    GW_CMD_APP_SRV_DEV_GET_MANUFACTURE_NAME,
    GW_CMD_APP_SRV_DEV_GET_MODEL_ID,
    GW_CMD_APP_SRV_DEV_GET_DATE_CODE,
    GW_CMD_APP_SRV_DEV_GET_SOFTWARE_ID,
} e_dev_info;

#define GW_CMD_APP_SRV_GENERAL_COMMAND_BASE 0x20000

typedef enum {
    GW_CMD_APP_SRV_DEV_READ_ATTRIBUTES = 0,
    GW_CMD_APP_SRV_DEV_WRITE_ATTRIBUTES,
    GW_CMD_APP_SRV_DEV_CONFIGURE_REPORT,
} e_dev_attribute;

#define GW_CMD_APP_SRV_IDENTIFY_BASE 0x40000

typedef enum {
    GW_CMD_APP_SRV_IDENTIFY = 0,
    GW_CMD_APP_SRV_IDENTIFY_QUERY,
    GW_CMD_APP_SRV_IDENTIFY_TRIGGER_EFFECT,
} e_dev_identify;

#define GW_CMD_APP_SRV_GROUP_MGMT_BASE 0x50000

typedef enum {
    GW_CMD_APP_SRV_GROUP_ADD = 0,
    GW_CMD_APP_SRV_GROUP_VIEW,
    GW_CMD_APP_SRV_GROUP_GET_MEMBERSHIP,
    GW_CMD_APP_SRV_GROUP_REMOVE,
    GW_CMD_APP_SRV_GROUP_REMOVE_ALL,
    GW_CMD_APP_SRV_GROUP_ADD_IF_IDENTIFYING,
} e_group_mgmt;

#define GW_CMD_APP_SRV_SCENE_MGMT_BASE 0x60000

typedef enum {
    GW_CMD_APP_SRV_SCENE_ADD = 0,
    GW_CMD_APP_SRV_SCENE_VIEW,
    GW_CMD_APP_SRV_SCENE_REMOVE,
    GW_CMD_APP_SRV_SCENE_REMOVE_ALL,
    GW_CMD_APP_SRV_SCENE_STORE,
    GW_CMD_APP_SRV_SCENE_RECALL,
    GW_CMD_APP_SRV_SCENE_GET_EMBERSHIP,
    GW_CMD_APP_SRV_SCENE_ENHANCED_ADD = 0x00000040,
    GW_CMD_APP_SRV_SCENE_ENHANCED_VIEW = 0x00000041,
    GW_CMD_APP_SRV_SCENE_COPY = 0x00000042,
} e_scene_mgmt;

#define GW_CMD_APP_SRV_ONOFF_CTRL_BASE 0x70000

typedef enum {
    GW_CMD_APP_SRV_ONOFF_ON = 0,
    GW_CMD_APP_SRV_ONOFF_OFF,
    GW_CMD_APP_SRV_ONOFF_TOGGLE,
    GW_CMD_APP_SRV_ONOFF_OFF_WITH_EFFECT,
    GW_CMD_APP_SRV_ONOFF_ON_WITH_RECALL_GLOBAL_SCENE,
    GW_CMD_APP_SRV_ONOFF_OFF_WITH_TIMED_OFF,
} e_on_off_ctrl;

#define GW_CMD_APP_SRV_LEVEL_CTRL_BASE 0x90000

typedef enum {
    GW_CMD_APP_SRV_LEVEL_MOVE_TO_LEVEL = 0,
    GW_CMD_APP_SRV_LEVEL_MOVE,
    GW_CMD_APP_SRV_LEVEL_STEP,
    GW_CMD_APP_SRV_LEVEL_STOP,
    GW_CMD_APP_SRV_LEVEL_MOVE_TO_LEVEL_WITH_ONOFF,
    GW_CMD_APP_SRV_LEVEL_MOVE_WITH_ONOFF,
    GW_CMD_APP_SRV_LEVEL_STEP_WITH_ONOFF,
} e_level_ctrl;

#define GW_CMD_APP_SRV_COLOR_CTRL_BASE 0x210000

typedef enum {
    GW_CMD_APP_SRV_COLOR_MOVE_TO_HUE = 0,
    GW_CMD_APP_SRV_COLOR_MOVE_HUE,
    GW_CMD_APP_SRV_COLOR_STEP_HUE,
    GW_CMD_APP_SRV_COLOR_MOVE_TO_SATURATION,
    GW_CMD_APP_SRV_COLOR_MOVE_SATURATION,
    GW_CMD_APP_SRV_COLOR_STEP_SATURATION,
    GW_CMD_APP_SRV_COLOR_MOVE_TO_HUE_AND_SATURATION,
    GW_CMD_APP_SRV_COLOR_MOVE_TO_COLOR,
    GW_CMD_APP_SRV_COLOR_MOVE_COLOR,
    GW_CMD_APP_SRV_COLOR_STEP_COLOR,
    GW_CMD_APP_SRV_COLOR_MOVE_TO_COLOR_TEMPERATURE = 0x0000000a,
    GW_CMD_APP_SRV_COLOR_MOVE_COLOR_TEMPERATURE = 0x0000004b,
    GW_CMD_APP_SRV_COLOR_STEP_COLOR_TEMPERATURE = 0x0000004c,
} e_color_ctrl;

#define GW_CMD_APP_SRV_DOOR_LOCK_BASE 0x240000

typedef enum {
    GW_CMD_APP_SRV_LOCK_DOOR = 0x00,
    GW_CMD_APP_SRV_UNLOCK_DOOR,
    GW_CMD_APP_SRV_TOGGLE,
    GW_CMD_APP_SRV_GET_LOG_RECORD,
    GW_CMD_APP_SRV_SET_PIN_CODE = 0x05,
    GW_CMD_APP_SRV_GET_PIN_CODE,
    GW_CMD_APP_SRV_CLEAR_PIN_CODE,
    GW_CMD_APP_SRV_CLEAR_ALL_PIN_CODES,
    GW_CMD_APP_SRV_SET_USER_STATUS,
    GW_CMD_APP_SRV_GET_USER_STATUS,
    GW_CMD_APP_SRV_SET_USER_TYPE = 0x14,
    GW_CMD_APP_SRV_GET_USER_TYPE,
} e_door_lock;

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct __attribute__((packed)) {
    uint8_t header[4];
    uint8_t len;
} gateway_cmd_hdr;

typedef struct __attribute__((packed)) {
    uint32_t command_id;
    uint16_t address;
    uint8_t address_mode;
    uint8_t parameter[];
} gateway_cmd_pd;

typedef struct __attribute__((packed)) {
    uint8_t cs;
} gateway_cmd_end;

typedef struct __attribute__((packed)) {
    uint16_t attr_id;
    uint8_t status;
    uint8_t data_type;
    uint8_t data[];
} read_attr_status_record_t;

typedef struct __attribute__((packed)) {
    uint8_t status;
} write_attr_status_record_t;

typedef struct __attribute__((packed)) {
    uint8_t status;
} cfg_report_status_record_t;

typedef struct __attribute__((packed)) {
    uint16_t attr_id;
    uint8_t attr_data_type;
    uint8_t data[];
} report_attribute_status_record_t;

typedef struct __attribute__((packed)) {
    uint16_t zone_status;
    uint8_t ext_status;
    uint8_t zone_id;
    uint16_t delay;
} zone_status_change_notification_record_t;

typedef struct __attribute__((packed)) {
    uint16_t cluster_id;
    read_attr_status_record_t att_status;

} read_cluster_attr_rsp_t;

typedef struct __attribute__((packed)) {
    // uint16_t cluster_id;
    write_attr_status_record_t att_status;
    uint16_t attr_id;
} write_cluster_attr_rsp_t;

typedef struct __attribute__((packed)) {
    uint16_t cluster_id;
    cfg_report_status_record_t att_status;
} cfg_report_rsp_t;

typedef struct __attribute__((packed)) {
    // uint16_t cluster_id;
    report_attribute_status_record_t att_status;
} report_attribute_t;

typedef struct __attribute__((packed)) {
    // uint16_t cluster_id;
    zone_status_change_notification_record_t att_status;
} zone_status_change_notification_t;

//=============================================================================
//                Private Global Variables
//=============================================================================
static QueueHandle_t g_cmd_queue;

//=============================================================================
//                Functions
//=============================================================================
static void _zcl_basic_read_rsp_cb(uint16_t cluster_id, uint16_t addr,
                                   uint8_t src_endp, uint8_t* pd,
                                   uint8_t pd_len) {
    read_attr_status_record_t* pt_record;
    uint8_t offset = 0, i;
    uint8_t *p_rsp_pd = NULL, rsp_pd_len = 0;
    uint32_t cmd_id;

    do {
        if (cluster_id != ZB_ZCL_CLUSTER_ID_BASIC) {
            break;
        }
        if (pd == NULL) {
            break;
        }
        pt_record = (read_attr_status_record_t*)(pd);

        switch (pt_record->attr_id) {
            case ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID:
                p_rsp_pd = pvPortMalloc(4);
                if (p_rsp_pd == NULL) {
                    break;
                }
                for (i = 0; i < 4; i++) {
                    pt_record = (read_attr_status_record_t*)(pd + offset);
                    offset += 5;
                    p_rsp_pd[i] = pt_record->data[0];
                }
                rsp_pd_len = 4;
                cmd_id = GW_CMD_APP_SRV_DEV_GET_VER_INFO;
                break;

            case ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID:

                rsp_pd_len = pt_record->data[0] + 1;
                p_rsp_pd = pvPortMalloc(rsp_pd_len);
                if (p_rsp_pd == NULL) {
                    break;
                }
                memcpy(p_rsp_pd, pt_record->data, rsp_pd_len);
                cmd_id = GW_CMD_APP_SRV_DEV_GET_MANUFACTURE_NAME;
                break;

            case ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID:
                rsp_pd_len = pt_record->data[0] + 1;
                p_rsp_pd = pvPortMalloc(rsp_pd_len);
                if (p_rsp_pd == NULL) {
                    break;
                }
                memcpy(p_rsp_pd, pt_record->data, rsp_pd_len);
                cmd_id = GW_CMD_APP_SRV_DEV_GET_MODEL_ID;
                break;

            case ZB_ZCL_ATTR_BASIC_DATE_CODE_ID:
                rsp_pd_len = pt_record->data[0] + 1;
                p_rsp_pd = pvPortMalloc(rsp_pd_len);
                if (p_rsp_pd == NULL) {
                    break;
                }
                memcpy(p_rsp_pd, pt_record->data, rsp_pd_len);
                cmd_id = GW_CMD_APP_SRV_DEV_GET_DATE_CODE;
                break;

            case ZB_ZCL_ATTR_BASIC_SW_BUILD_ID:
                rsp_pd_len = pt_record->data[0] + 1;
                p_rsp_pd = pvPortMalloc(rsp_pd_len);
                if (p_rsp_pd == NULL) {
                    break;
                }
                memcpy(p_rsp_pd, pt_record->data, rsp_pd_len);
                cmd_id = GW_CMD_APP_SRV_DEV_GET_SOFTWARE_ID;
                break;

            default: break;
        }
    } while (0);

    if (p_rsp_pd) {
        zigbee_gw_cmd_send(
            GW_CMD_APP_CMD_RSP_GEN(cmd_id | GW_CMD_APP_SRV_DEV_BASE), addr, 0,
            src_endp, p_rsp_pd, rsp_pd_len);
        vPortFree(p_rsp_pd);
    }
}

static void _zcl_basic_read_attribute_cb(uint16_t cluster_id, uint16_t addr,
                                         uint8_t src_endp, uint8_t* pd,
                                         uint8_t pd_len) {
    read_cluster_attr_rsp_t* pt_att_rsp = NULL;
    uint8_t rsp_pd_len = 0;
    do {

        if (pd == NULL) {
            break;
        }

        rsp_pd_len = pd_len + sizeof(cluster_id);
        pt_att_rsp = pvPortMalloc(rsp_pd_len);
        pt_att_rsp->cluster_id = cluster_id;
        memcpy(&(pt_att_rsp->att_status), pd,
               (rsp_pd_len - sizeof(cluster_id)));
    } while (0);

    if (pt_att_rsp) {
        zigbee_gw_cmd_send(
            GW_CMD_APP_CMD_RSP_GEN(GW_CMD_APP_SRV_GENERAL_COMMAND_BASE), addr,
            0, src_endp, (uint8_t*)pt_att_rsp, rsp_pd_len);
        vPortFree(pt_att_rsp);
    } else {
        log_error("Read attribute rsp malloc fail\n");
    }
}

static void _zcl_basic_write_attribute_cb(uint16_t cluster_id, uint16_t addr,
                                          uint8_t src_endp, uint8_t* pd,
                                          uint8_t pd_len) {
    write_cluster_attr_rsp_t* pt_att_rsp = NULL;
    uint8_t rsp_pd_len = 0;

    do {
        if (pd == NULL) {
            break;
        }
        rsp_pd_len = pd_len;
        pt_att_rsp = pvPortMalloc(rsp_pd_len);
        memcpy(pt_att_rsp, pd, pd_len);

    } while (0);

    if (pt_att_rsp) {
        zigbee_gw_cmd_send(
            GW_CMD_APP_CMD_RSP_GEN(GW_CMD_APP_SRV_DEV_WRITE_ATTRIBUTES
                                   | GW_CMD_APP_SRV_GENERAL_COMMAND_BASE),
            addr, 0, src_endp, (uint8_t*)pt_att_rsp, rsp_pd_len);
        vPortFree(pt_att_rsp);
    } else {
        log_error("Write attribute rsp malloc fail\n");
    }
}

static void _zcl_basic_cfg_report_rsp_cb(uint16_t cluster_id, uint16_t addr,
                                         uint8_t src_endp, uint8_t* pd,
                                         uint8_t pd_len) {
    cfg_report_rsp_t* pt_att_rsp = NULL;
    uint8_t rsp_pd_len = 0;
    do {

        if (pd == NULL) {
            break;
        }

        rsp_pd_len = pd_len + sizeof(cluster_id);
        pt_att_rsp = pvPortMalloc(rsp_pd_len);
        pt_att_rsp->cluster_id = cluster_id;
        memcpy(&(pt_att_rsp->att_status), pd,
               (rsp_pd_len - sizeof(cluster_id)));
    } while (0);

    if (pt_att_rsp) {
        zigbee_gw_cmd_send(
            GW_CMD_APP_CMD_RSP_GEN(GW_CMD_APP_SRV_DEV_CONFIGURE_REPORT
                                   | GW_CMD_APP_SRV_GENERAL_COMMAND_BASE),
            addr, 0, src_endp, (uint8_t*)pt_att_rsp, rsp_pd_len);
        vPortFree(pt_att_rsp);
    } else {
        log_error("config report rsp malloc fail\n");
    }
}

static void _cmd_dev_info_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_DEV_GET_VER_INFO:
            attr_data_len = 8;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID & 0xFF;
            p_attr_data[1] = (ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID >> 8) & 0xFF;

            p_attr_data[2] = ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID & 0xFF;
            p_attr_data[3] = (ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID >> 8)
                             & 0xFF;

            p_attr_data[4] = ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID & 0xFF;
            p_attr_data[5] = (ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID >> 8) & 0xFF;

            p_attr_data[6] = ZB_ZCL_ATTR_BASIC_HW_VERSION_ID & 0xFF;
            p_attr_data[7] = (ZB_ZCL_ATTR_BASIC_HW_VERSION_ID >> 8) & 0xFF;
            break;

        case GW_CMD_APP_SRV_DEV_GET_MANUFACTURE_NAME:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID & 0xFF;
            p_attr_data[1] = (ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID >> 8)
                             & 0xFF;

            break;

        case GW_CMD_APP_SRV_DEV_GET_MODEL_ID:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID & 0xFF;
            p_attr_data[1] = (ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID >> 8)
                             & 0xFF;
            break;

        case GW_CMD_APP_SRV_DEV_GET_DATE_CODE:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = ZB_ZCL_ATTR_BASIC_DATE_CODE_ID & 0xFF;
            p_attr_data[1] = (ZB_ZCL_ATTR_BASIC_DATE_CODE_ID >> 8) & 0xFF;

            break;

        case GW_CMD_APP_SRV_DEV_GET_SOFTWARE_ID:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = ZB_ZCL_ATTR_BASIC_SW_BUILD_ID & 0xFF;
            p_attr_data[1] = (ZB_ZCL_ATTR_BASIC_SW_BUILD_ID >> 8) & 0xFF;
            break;

        default: break;
    }

    do {
        if (p_attr_data == NULL || attr_data_len == 0) {
            break;
        }

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, pt_pd->address, address_mode,
                            pt_pd->parameter[0], ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_BASIC, ZB_ZCL_CMD_READ_ATTRIB,
                            FALSE, TRUE, ZCL_FRAME_CLIENT_SERVER_DIR, 0,
                            attr_data_len)

        if (pt_data_req) {
            memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);

            zigbee_app_zcl_read_rsp_cb_reg(_zcl_basic_read_rsp_cb);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_general_command_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint16_t cluster_id = 0x0000;

    switch (cmd_id) {

        case GW_CMD_APP_SRV_DEV_READ_ATTRIBUTES:
            cmd_id = ZB_ZCL_CMD_READ_ATTRIB;
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            cluster_id = (((((uint16_t)pt_pd->parameter[2]) << 8U) & 0xFF00U)
                          | (((uint16_t)pt_pd->parameter[1]) & 0xFFU));

            p_attr_data[0] = pt_pd->parameter[3];
            p_attr_data[1] = pt_pd->parameter[4];

            break;

        case GW_CMD_APP_SRV_DEV_WRITE_ATTRIBUTES:
            cmd_id = ZB_ZCL_CMD_WRITE_ATTRIB;

            attr_data_len = pkt[4] - 10;

            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            cluster_id = (((((uint16_t)pt_pd->parameter[2]) << 8U) & 0xFF00U)
                          | (((uint16_t)pt_pd->parameter[1]) & 0xFFU));

            memcpy(p_attr_data, pt_pd->parameter + 3, attr_data_len);

            break;

        case GW_CMD_APP_SRV_DEV_CONFIGURE_REPORT:
            cmd_id = ZB_ZCL_CMD_CONFIG_REPORT;
            attr_data_len = pkt[4] - 10 + 1;
            cluster_id = (((((uint16_t)pt_pd->parameter[2]) << 8U) & 0xFF00U)
                          | (((uint16_t)pt_pd->parameter[1]) & 0xFFU));

            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = 0x00;
            memcpy(p_attr_data + 1, pt_pd->parameter + 3, attr_data_len - 1);
            break;
        default: break;
    }

    do {
        if (p_attr_data == NULL || attr_data_len == 0) {
            break;
        }

        if (pt_pd->address_mode == 0)
            pt_pd->address_mode = ZB_APS_ADDR_MODE_16_ENDP_PRESENT;

        ZIGBEE_ZCL_DATA_REQ(pt_data_req, pt_pd->address, pt_pd->address_mode,
                            pt_pd->parameter[0], ZIGBEE_DEFAULT_ENDPOINT,
                            cluster_id, cmd_id, FALSE, TRUE,
                            ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            log_info("ZCL Data to %04X %d %04X %d", pt_pd->address,
                     pt_pd->parameter[0], cluster_id, cmd_id);
            memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
            if (cmd_id == ZB_ZCL_CMD_READ_ATTRIB) {
                zigbee_app_zcl_read_rsp_cb_reg(_zcl_basic_read_attribute_cb);
            } else if (cmd_id == ZB_ZCL_CMD_WRITE_ATTRIB) {
                zigbee_app_zcl_write_rsp_cb_reg(_zcl_basic_write_attribute_cb);
            } else if (cmd_id == ZB_ZCL_CMD_CONFIG_REPORT) {
                zigbee_app_zcl_cfg_report_rsp_cb_reg(
                    _zcl_basic_cfg_report_rsp_cb);
            }
        }

        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_identify_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_IDENTIFY:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = pt_pd->parameter[2];
            p_attr_data[1] = pt_pd->parameter[3];

            break;

        case GW_CMD_APP_SRV_IDENTIFY_QUERY: attr_data_len = 0; break;

        case GW_CMD_APP_SRV_IDENTIFY_TRIGGER_EFFECT:
            cmd_id = 0x40;
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = pt_pd->parameter[2];
            p_attr_data[1] = pt_pd->parameter[3];

        default: break;
    }

    do {
        ZIGBEE_ZCL_DATA_REQ(
            pt_data_req, pt_pd->address, address_mode, pt_pd->parameter[0],
            ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_IDENTIFY, cmd_id, TRUE,
            pt_pd->parameter[1], ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_group_mgmt_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_GROUP_ADD:
            attr_data_len = 3;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = pt_pd->parameter[1];
            p_attr_data[1] = pt_pd->parameter[2];
            p_attr_data[2] = 0;
            break;

        case GW_CMD_APP_SRV_GROUP_VIEW:
        case GW_CMD_APP_SRV_GROUP_REMOVE:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = pt_pd->parameter[1];
            p_attr_data[1] = pt_pd->parameter[2];
            break;

        case GW_CMD_APP_SRV_GROUP_GET_MEMBERSHIP: attr_data_len = 0; break;
        case GW_CMD_APP_SRV_GROUP_REMOVE_ALL:
            attr_data_len = 0;
            disable_default_rsp = pt_pd->parameter[1];
            break;
        case GW_CMD_APP_SRV_GROUP_ADD_IF_IDENTIFYING:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }

            p_attr_data[0] = pt_pd->parameter[2];
            p_attr_data[1] = pt_pd->parameter[3];

            disable_default_rsp = pt_pd->parameter[1];
            break;
        default: break;
    }

    do {
        ZIGBEE_ZCL_DATA_REQ(
            pt_data_req, pt_pd->address, address_mode, pt_pd->parameter[0],
            ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_GROUPS, cmd_id, TRUE,
            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_scene_mgmt_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_SCENE_ADD:
            attr_data_len = pkt[4] - 8;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;

        case GW_CMD_APP_SRV_SCENE_VIEW:
        case GW_CMD_APP_SRV_SCENE_REMOVE:
        case GW_CMD_APP_SRV_SCENE_STORE:
            attr_data_len = 3;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        case GW_CMD_APP_SRV_SCENE_REMOVE_ALL:
        case GW_CMD_APP_SRV_SCENE_GET_EMBERSHIP:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        case GW_CMD_APP_SRV_SCENE_RECALL:
            attr_data_len = 3;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[2], attr_data_len);
            disable_default_rsp = pt_pd->parameter[1];
            break;
        case GW_CMD_APP_SRV_SCENE_ENHANCED_ADD:
            attr_data_len = pkt[4] - 8;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;

        case GW_CMD_APP_SRV_SCENE_ENHANCED_VIEW:
            attr_data_len = 3;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;

        case GW_CMD_APP_SRV_SCENE_COPY:
            attr_data_len = 7;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;

        default: break;
    }

    do {
        ZIGBEE_ZCL_DATA_REQ(
            pt_data_req, pt_pd->address, address_mode, pt_pd->parameter[0],
            ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_SCENES, cmd_id, TRUE,
            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_level_ctrl_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_LEVEL_MOVE_TO_LEVEL:
        case GW_CMD_APP_SRV_LEVEL_MOVE_TO_LEVEL_WITH_ONOFF:
            attr_data_len = 5;
            break;
        case GW_CMD_APP_SRV_LEVEL_MOVE:
        case GW_CMD_APP_SRV_LEVEL_MOVE_WITH_ONOFF: attr_data_len = 4; break;
        case GW_CMD_APP_SRV_LEVEL_STEP:
        case GW_CMD_APP_SRV_LEVEL_STEP_WITH_ONOFF: attr_data_len = 6; break;
        case GW_CMD_APP_SRV_LEVEL_STOP: attr_data_len = 2; break;
        default: break;
    }

    p_attr_data = pvPortMalloc(attr_data_len);

    if (p_attr_data == NULL) {
        return;
    }
    disable_default_rsp = pt_pd->parameter[1];
    memcpy(p_attr_data, &pt_pd->parameter[2], attr_data_len);

    do {
        ZIGBEE_ZCL_DATA_REQ(pt_data_req, pt_pd->address, address_mode,
                            pt_pd->parameter[0], ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, cmd_id, TRUE,
                            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0,
                            attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_onoff_ctrl_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;

    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_ONOFF_ON:
        case GW_CMD_APP_SRV_ONOFF_OFF:
        case GW_CMD_APP_SRV_ONOFF_TOGGLE:
            attr_data_len = 0;
            disable_default_rsp = pt_pd->parameter[1];
            break;

        case GW_CMD_APP_SRV_ONOFF_OFF_WITH_EFFECT:
            attr_data_len = 2;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            disable_default_rsp = pt_pd->parameter[1];
            memcpy(p_attr_data, &pt_pd->parameter[2], attr_data_len);
            cmd_id = 0x40;
            break;

        case GW_CMD_APP_SRV_ONOFF_ON_WITH_RECALL_GLOBAL_SCENE:
            attr_data_len = 0;
            disable_default_rsp = pt_pd->parameter[1];
            cmd_id = 0x41;
            break;

        case GW_CMD_APP_SRV_ONOFF_OFF_WITH_TIMED_OFF:
            attr_data_len = 5;
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            disable_default_rsp = pt_pd->parameter[1];
            memcpy(p_attr_data, &pt_pd->parameter[2], attr_data_len);
            cmd_id = 0x42;
            break;
        default: break;
    }

    do {
        ZIGBEE_ZCL_DATA_REQ(
            pt_data_req, pt_pd->address, address_mode, pt_pd->parameter[0],
            ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_ON_OFF, cmd_id, TRUE,
            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_color_ctrl_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_COLOR_MOVE_TO_HUE:
        case GW_CMD_APP_SRV_COLOR_MOVE_TO_HUE_AND_SATURATION:
            attr_data_len = 6;
            break;
        case GW_CMD_APP_SRV_COLOR_MOVE_HUE:
        case GW_CMD_APP_SRV_COLOR_MOVE_SATURATION: attr_data_len = 4; break;
        case GW_CMD_APP_SRV_COLOR_STEP_HUE:
        case GW_CMD_APP_SRV_COLOR_MOVE_TO_SATURATION:
        case GW_CMD_APP_SRV_COLOR_STEP_SATURATION: attr_data_len = 5; break;
        case GW_CMD_APP_SRV_COLOR_STEP_COLOR:
        case GW_CMD_APP_SRV_COLOR_MOVE_TO_COLOR: attr_data_len = 8; break;
        case GW_CMD_APP_SRV_COLOR_MOVE_COLOR:
        case GW_CMD_APP_SRV_COLOR_MOVE_TO_COLOR_TEMPERATURE:
            attr_data_len = 6;
            break;
        case GW_CMD_APP_SRV_COLOR_MOVE_COLOR_TEMPERATURE:
            attr_data_len = 9;
            break;
        case GW_CMD_APP_SRV_COLOR_STEP_COLOR_TEMPERATURE:
            attr_data_len = 11;
            break;
        default: break;
    }

    p_attr_data = pvPortMalloc(attr_data_len);

    if (p_attr_data == NULL) {
        return;
    }
    disable_default_rsp = pt_pd->parameter[1];
    memcpy(p_attr_data, &pt_pd->parameter[2], attr_data_len);

    do {
        ZIGBEE_ZCL_DATA_REQ(pt_data_req, pt_pd->address, address_mode,
                            pt_pd->parameter[0], ZIGBEE_DEFAULT_ENDPOINT,
                            ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, cmd_id, TRUE,
                            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0,
                            attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _cmd_dev_door_lock_handle(uint32_t cmd_id, uint8_t* pkt) {
    gateway_cmd_hdr* pt_hdr = (gateway_cmd_hdr*)pkt;
    gateway_cmd_pd* pt_pd = (gateway_cmd_pd*)&pkt[5];

    zcl_data_req_t* pt_data_req;

    uint8_t disable_default_rsp = 1;
    uint8_t attr_data_len = 0;
    uint8_t* p_attr_data = NULL;
    uint32_t address_mode = 0;
    uint16_t user_id = 0;
    uint8_t user_status = 0;
    uint8_t user_type = 0;

    // log_info("%s\r\n", __FUNCTION__);

    address_mode = (pt_pd->address_mode == 0)
                       ? ZB_APS_ADDR_MODE_16_ENDP_PRESENT
                       : ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;

    switch (cmd_id) {
        case GW_CMD_APP_SRV_LOCK_DOOR:
        case GW_CMD_APP_SRV_UNLOCK_DOOR:
        case GW_CMD_APP_SRV_TOGGLE:
            disable_default_rsp = 0x01;
            attr_data_len = pt_pd->parameter[1] + 1;
            p_attr_data = pvPortMalloc(attr_data_len);
            if (p_attr_data == NULL) {
                break;
            }
            p_attr_data[0] = attr_data_len - 1;
            memcpy(&p_attr_data[1], &pt_pd->parameter[2], attr_data_len);
            // log_info("PIN Code: ");
            // for (int i = 0; i < attr_data_len; i++)
            // {
            //     log_info("%02x ", p_attr_data[i]);
            // }
            // log_info("\r\n");
            break;
        case GW_CMD_APP_SRV_SET_PIN_CODE:
            attr_data_len = pt_hdr->len - 8;
            user_id = pt_pd->parameter[1] | pt_pd->parameter[2] >> 8;
            user_status = pt_pd->parameter[3];
            user_type = pt_pd->parameter[4];

            // log_info("user id: %04x", user_id);
            // log_info("user status: %02x", user_status);
            // log_info("user type: %02x", user_type);
            // log_info("pin code len: %d\r\n", pt_pd->parameter[5]);
            // for (int i = 0; i < pt_pd->parameter[5]; i++)
            // {
            //     log_info("%c", pt_pd->parameter[6 + i]);
            // }
            // log_info("\r\n");
            p_attr_data = pvPortMalloc(attr_data_len);

            if (p_attr_data == NULL) {
                break;
            }
            disable_default_rsp = 0x01;
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        case GW_CMD_APP_SRV_GET_PIN_CODE:
        case GW_CMD_APP_SRV_CLEAR_PIN_CODE:
        case GW_CMD_APP_SRV_GET_USER_STATUS:
        case GW_CMD_APP_SRV_GET_USER_TYPE:
            attr_data_len = 2;
            disable_default_rsp = 0x01;
            user_id = pt_pd->parameter[1] | pt_pd->parameter[2] >> 8;
            // log_info("user id: %04x", user_id);
            p_attr_data = pvPortMalloc(attr_data_len);
            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        case GW_CMD_APP_SRV_CLEAR_ALL_PIN_CODES:
            attr_data_len = 0;
            disable_default_rsp = 0x01;
            break;
        case GW_CMD_APP_SRV_SET_USER_STATUS:
            attr_data_len = 3;
            disable_default_rsp = 0x01;
            user_id = pt_pd->parameter[1] | pt_pd->parameter[2] >> 8;
            user_status = pt_pd->parameter[3];
            // log_info("user id: %04x", user_id);
            // log_info("user status: %02x", user_status);
            p_attr_data = pvPortMalloc(attr_data_len);
            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        case GW_CMD_APP_SRV_SET_USER_TYPE:
            attr_data_len = 3;
            disable_default_rsp = 0x01;
            user_id = pt_pd->parameter[1] | pt_pd->parameter[2] >> 8;
            user_type = pt_pd->parameter[3];
            // log_info("user id: %04x", user_id);
            // log_info("user type: %02x", user_type);
            p_attr_data = pvPortMalloc(attr_data_len);
            if (p_attr_data == NULL) {
                break;
            }
            memcpy(p_attr_data, &pt_pd->parameter[1], attr_data_len);
            break;
        default: break;
    }

    do {
        ZIGBEE_ZCL_DATA_REQ(
            pt_data_req, pt_pd->address, address_mode, pt_pd->parameter[0],
            ZIGBEE_DEFAULT_ENDPOINT, ZB_ZCL_CLUSTER_ID_DOOR_LOCK, cmd_id, TRUE,
            disable_default_rsp, ZCL_FRAME_CLIENT_SERVER_DIR, 0, attr_data_len)

        if (pt_data_req) {
            if (attr_data_len > 0 && p_attr_data) {
                memcpy(pt_data_req->cmdFormat, p_attr_data, attr_data_len);
            }
            zigbee_app_zcl_send_command(pt_data_req);
            vPortFree(pt_data_req);
        }
        if (p_attr_data) {
            vPortFree(p_attr_data);
        }
    } while (0);
}

static void _gw_app_cmd_service_handle(uint32_t cmd_id, uint8_t* pkt) {
    log_info("cmd_id %04X, plen %d, %p", cmd_id, pkt[4], pkt);

    if ((cmd_id >= GW_CMD_APP_SRV_DEV_BASE)
        && (cmd_id < GW_CMD_APP_SRV_DEV_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_info_handle(cmd_id - GW_CMD_APP_SRV_DEV_BASE, pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_GENERAL_COMMAND_BASE)
               && (cmd_id < GW_CMD_APP_SRV_GENERAL_COMMAND_BASE
                                + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_general_command_handle(
            cmd_id - GW_CMD_APP_SRV_GENERAL_COMMAND_BASE, pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_IDENTIFY_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_IDENTIFY_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_identify_handle(cmd_id - GW_CMD_APP_SRV_IDENTIFY_BASE, pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_GROUP_MGMT_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_GROUP_MGMT_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_group_mgmt_handle(cmd_id - GW_CMD_APP_SRV_GROUP_MGMT_BASE,
                                   pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_SCENE_MGMT_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_SCENE_MGMT_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_scene_mgmt_handle(cmd_id - GW_CMD_APP_SRV_SCENE_MGMT_BASE,
                                   pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_ONOFF_CTRL_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_ONOFF_CTRL_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_onoff_ctrl_handle(cmd_id - GW_CMD_APP_SRV_ONOFF_CTRL_BASE,
                                   pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_LEVEL_CTRL_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_LEVEL_CTRL_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_level_ctrl_handle(cmd_id - GW_CMD_APP_SRV_LEVEL_CTRL_BASE,
                                   pkt);
    }

    else if ((cmd_id >= GW_CMD_APP_SRV_COLOR_CTRL_BASE)
             && (cmd_id
                 < GW_CMD_APP_SRV_COLOR_CTRL_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_color_ctrl_handle(cmd_id - GW_CMD_APP_SRV_COLOR_CTRL_BASE,
                                   pkt);
    } else if ((cmd_id >= GW_CMD_APP_SRV_DOOR_LOCK_BASE)
               && (cmd_id
                   < GW_CMD_APP_SRV_DOOR_LOCK_BASE + GW_CMD_APP_CMD_OFFSET)) {
        _cmd_dev_door_lock_handle(cmd_id - GW_CMD_APP_SRV_DOOR_LOCK_BASE, pkt);
    }
}

static uint8_t _gateway_checksum_calc(uint8_t* pBuf, uint8_t len) {
    uint8_t cs = 0;

    for (int i = 0; i < len; i++) {
        cs += pBuf[i];
    }
    return (~cs);
}

void zigbee_gw_cmd_act(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                       uint8_t src_endp, uint8_t* pParam, uint32_t len) {
    uint8_t* gateway_cmd_pkt;
    uint32_t pkt_len;
    uint8_t idx = 0;

    do {
        pkt_len = sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_pd) + len
                  + sizeof(gateway_cmd_end);

        if (src_endp != 0) {
            pkt_len += 1;
        }
        gateway_cmd_pkt = pvPortMalloc(sizeof(_zb_app_data_t) + pkt_len);

        if (gateway_cmd_pkt == NULL) {
            break;
        }

        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[0] = 0xFF;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[1] = 0xFC;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[2] = 0xFC;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[3] = 0xFF;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->len = sizeof(gateway_cmd_pd)
                                                     + len;

        if (src_endp != 0) {
            ((gateway_cmd_hdr*)(gateway_cmd_pkt))->len += 1;
        }

        idx += sizeof(gateway_cmd_hdr);

        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->command_id = cmd_id;
        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->address = addr;
        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->address_mode = addr_mode;

        if (src_endp != 0) {
            ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->parameter[0] = src_endp;
            memcpy(((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->parameter + 1,
                   pParam, len);
        } else {
            memcpy(((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->parameter,
                   pParam, len);
        }

        idx += sizeof(gateway_cmd_pd) + len;

        if (src_endp != 0) {
            idx += 1;
        }

        ((gateway_cmd_end*)(&gateway_cmd_pkt[idx]))->cs =
            _gateway_checksum_calc(
                (uint8_t*)&(((gateway_cmd_hdr*)(gateway_cmd_pkt))->len),
                (sizeof(gateway_cmd_pd) + len + 1));
        log_info_hexdump("GW_CMD", gateway_cmd_pkt, pkt_len);

        zigbee_gw_cmd_proc(gateway_cmd_pkt, pkt_len);

    } while (0);
}

void zigbee_gw_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                        uint8_t src_endp, uint8_t* pParam, uint32_t len) {
    _zb_app_data_t* zb_data = NULL;
    uint8_t* gateway_cmd_pkt;
    uint32_t pkt_len;
    uint32_t ep_len = 0;
    uint8_t idx = 0;

    do {
        pkt_len = sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_pd) + len
                  + sizeof(gateway_cmd_end);

        if (src_endp != 0) {
            pkt_len += 1;
        }
        zb_data = pvPortMalloc(sizeof(_zb_app_data_t) + pkt_len);

        if (zb_data == NULL) {
            break;
        }

        gateway_cmd_pkt = zb_data->pdata;

        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[0] = 0xFF;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[1] = 0xFC;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[2] = 0xFC;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->header[3] = 0xFF;
        ((gateway_cmd_hdr*)(gateway_cmd_pkt))->len = sizeof(gateway_cmd_pd)
                                                     + len;

        if (src_endp != 0) {
            ((gateway_cmd_hdr*)(gateway_cmd_pkt))->len += 1;
        }
        zb_data->dlen = pkt_len;

        idx += sizeof(gateway_cmd_hdr);

        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->command_id = cmd_id;
        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->address = addr;
        ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->address_mode = addr_mode;

        if (src_endp != 0) {
            ep_len = 1;
            ((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->parameter[0] = src_endp;
        }
        memcpy(((gateway_cmd_pd*)(&gateway_cmd_pkt[idx]))->parameter + ep_len,
               pParam, len);

        idx += sizeof(gateway_cmd_pd) + len + ep_len;

        ((gateway_cmd_end*)(&gateway_cmd_pkt[idx]))->cs =
            _gateway_checksum_calc(
                (uint8_t*)&(((gateway_cmd_hdr*)(gateway_cmd_pkt))->len),
                sizeof(gateway_cmd_pd) + len + 1 + ep_len);
        // log_info("sizeof(gateway_cmd_pd): %02x", sizeof(gateway_cmd_pd));
        // log_info("len: %02x", (sizeof(gateway_cmd_pd) + len + 1 + ep_len));
        log_info_hexdump("GW_TX", gateway_cmd_pkt, pkt_len);

        if (xQueueSend(g_cmd_queue, (void*)&zb_data, 0) != pdPASS) {
            log_error("q full!");
            vPortFree(zb_data);
        } else {
            ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_ZBOSS_CB_IN);
        }

    } while (0);
}

void zigbee_gw_cmd_proc(uint8_t* pBuf, uint32_t len) {
    gateway_cmd_hdr* pt_hd;
    gateway_cmd_pd* pt_pd;

    log_info_hexdump("GW_RX", pBuf, len);

    pt_hd = (gateway_cmd_hdr*)&pBuf[0];
    pt_pd = (gateway_cmd_pd*)&pBuf[5];

    if ((pt_pd->command_id < ZIGBEE_CMD_FINISH)) {
        zigbee_cmd_request(pt_pd->address, pt_pd->command_id, (pt_hd->len - 7),
                           pt_pd->parameter);
    } else if (pt_pd->command_id >= 0x10000) {
        _gw_app_cmd_service_handle(pt_pd->command_id, pBuf);
    }
}

void zigbee_gw_init(void* cmd_queue) { g_cmd_queue = (QueueHandle_t)cmd_queue; }
#endif