/**
 * @file zigbee_cmd_nwk.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_CMD_NWK_H
#define __ZIGBEE_CMD_NWK_H

#include "stdint.h"

typedef enum {
    /* Device Management */
    ZIGBEE_CMD_NWK_ADDRESS_REQUEST = 0x00000000,
    ZIGBEE_CMD_IEEE_ADDRESS_REQUEST,
    ZIGBEE_CMD_NODE_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_POWER_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_SIMPLE_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_ACTIVE_ENDPOINT_REQUEST,
    ZIGBEE_CMD_DEVICE_ANNCE_INDICATION = 0x00000013,
    ZIGBEE_CMD_DEVICE_LEAVE_INDICATION = 0x00000014,
    /* Binding Management */
    ZIGBEE_CMD_BIND_REQUEST = 0x00000021,
    ZIGBEE_CMD_UNBIND_REQUEST,
    ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST = 0x00000033,
    /* Network Management */
    ZIGBEE_CMD_DEVICE_LEAVE_REQUEST= 0x00000034,
    ZIGBEE_CMD_PERMIT_JOIN_REQUEST = 0x00000036,
    ZIGBEE_CMD_PERMIT_JOIN_TIMEOUT_NOTIFY = 0x00008037,
    ZIGBEE_CMD_NETWORK_UPDATE_REQUEST = 0x00000038,
    ZIGBEE_CMD_NETWORK_START_REQUEST = 0x00000039,
    ZIGBEE_CMD_CHANNEL_ENERGY_SCAN_REQUEST = 0x0000003A,
    ZIGBEE_CMD_NETWORK_ADDR_TABLE_UPDATE_REQUEST = 0x0000003B,
    ZIGBEE_CMD_GET_NETWORK_ADDR_VIA_GROUP_IDX_REQUEST = 0x0000003C,
    ZIGBEE_CMD_GATEWAY_RESET_REQUEST = 0x00000040,
    ZIGBEE_CMD_GATEWAY_EXT_ADDRESS_REQUEST = 0x00000041,
    ZIGBEE_CMD_GATEWAY_PERMITJOIN_STATUS_REQUEST = 0x00000042,
    ZIGBEE_CMD_GATEWAY_PANID_CHANNEL_REQUEST = 0x00000043,
    ZIGBEE_CMD_GATEWAY_INSTALL_CODE_SET_REQUEST = 0x00000044,
    ZIGBEE_CMD_GATEWAY_INSTALL_CODE_REMOVE_REQUEST = 0x00000045,
    ZIGBEE_CMD_GATEWAY_INSTALL_CODE_REMOVE_ALL_REQUEST = 0x00000046,
    ZIGBEE_CMD_GATEWAY_STANDARD_TIME_SET = 0x00000047,
    ZIGBEE_CMD_FINISH,
} zigbee_cmd_t;

typedef struct {
    uint32_t cmd_type;
    uint32_t cmd_index;
    uint16_t cmd_length;
    uint16_t cmd_dst_addr;
    uint32_t cmd_value[];
} zigbee_cmd_req_t;

typedef struct {
    uint32_t Status;
    uint32_t cmd_type;
    uint32_t cmd_index;
    uint16_t cmd_length;
    uint16_t cmd_dst_addr;
    uint32_t cmd_value[];
} zigbee_cmd_cfm_t;

typedef struct permit_join_status {
    uint8_t association_permit;
    uint8_t remaining_time;
} permit_join_status_t;

typedef struct install_code_add{
    uint8_t  ic_type;
    uint8_t  ieeeAddr[8];
    uint8_t  ic[19];
} install_code_add_t;

typedef void (*zigbee_cmd_func)(zigbee_cmd_req_t* pt_cmd_req);

void zigbee_cmd_request(uint16_t dst_addr, uint32_t t_index, uint32_t u32_len,
                        uint8_t* pu8_value);

#endif // __ZIGBEE_CMD_NWK_H