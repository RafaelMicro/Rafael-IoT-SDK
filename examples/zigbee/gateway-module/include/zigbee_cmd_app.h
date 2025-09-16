/**
 * @file zigbee_cmd_app.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_GW_H
#define __ZIGBEE_GW_H

#include "stdint.h"

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
    GW_CMD_APP_SRV_DEV_READ_CUSTOM_ATTRIBUTES,
    GW_CMD_APP_SRV_DEV_WRITE_CUSTOM_ATTRIBUTES,
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
    GW_CMD_APP_SRV_ONOFF_ON_WITH_TIMED_OFF,
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
    GW_CMD_APP_SRV_COLOR_STOP_MOVE_STEP = 0x00000047,
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
} e_door_lock;

#define GW_CMD_APP_SRV_CUSTOM_BASE 0xFC000000


void zigbee_gw_init(void* cmd_queue);
void zigbee_gw_cmd_proc(uint8_t* pBuf, uint16_t len);
void zigbee_gw_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                        uint8_t src_endp, uint8_t* pParam, uint32_t len);
void zigbee_gw_cmd_act(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                       uint8_t src_endp, uint8_t* pParam, uint32_t len);
// void _zcl_report_attribute_cb(uint16_t cluster_id, uint16_t addr, uint8_t src_endp, uint8_t *pd, uint8_t pd_len);
// void _zcl_zone_status_change_notificatin_cb(uint16_t cluster_id, uint16_t addr, uint8_t src_endp, uint8_t *pd, uint8_t pd_len);

#endif // __ZIGBEE_GW_H