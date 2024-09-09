/**
 * @file zigbee_cmd.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_CMD_H
#define __ZIGBEE_CMD_H

#include "stdint.h"

typedef enum {
    ZIGBEE_CMD_NWK_ADDRESS_REQUEST = 0x00000000,
    ZIGBEE_CMD_IEEE_ADDRESS_REQUEST,
    ZIGBEE_CMD_NODE_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_POWER_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_SIMPLE_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_ACTIVE_ENDPOINT_REQUEST,
    ZIGBEE_CMD_MATCH_DESCRIPTOR_REQUEST,
    ZIGBEE_CMD_BIND_REQUEST = 0x00000021,
    ZIGBEE_CMD_UNBIND_REQUEST,
    ZIGBEE_CMD_DEVICE_BINDING_INFORMATION_REQUEST = 0x00000033,
    ZIGBEE_CMD_DEVICE_LEAVE_REQUEST,
    ZIGBEE_CMD_DIRECT_JOIN_REQUEST,
    ZIGBEE_CMD_PERMIT_JOIN_REQUEST,
    ZIGBEE_CMD_NETWORK_UPDATE_REQUEST = 0x00000038,
    ZIGBEE_CMD_NETWORK_START_REQUEST = 0x00000039,
    ZIGBEE_CMD_NETWORK_RESET_REQUEST = 0x00000040,
    ZIGBEE_CMD_GATEWAY_EXT_ADDRESS_REQUEST = 0x00000041,
    ZIGBEE_CMD_GATEWAY_PERMITJOIN_STATUS_REQUEST = 0x00000042,
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

typedef void (*zigbee_cmd_func)(zigbee_cmd_req_t* pt_cmd_req);

void zigbee_cmd_request(uint16_t dst_addr, uint32_t t_index, uint32_t u32_len,
                        uint8_t* pu8_value);

#endif // __ZIGBEE_CMD_H