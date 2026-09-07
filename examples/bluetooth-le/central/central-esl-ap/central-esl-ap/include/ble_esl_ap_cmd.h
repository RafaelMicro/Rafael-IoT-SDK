/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file uart_handler.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-03-24
 *
 *
 */

#ifndef __BLE_ESL_AP_CMD_H__
#define __BLE_ESL_AP_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "uart_handler.h"
#include "ble_service_esls.h"
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

//=============================================================================
//                Public Definitions of const value
//=============================================================================


//=============================================================================
//                Public ENUM
//=============================================================================

typedef enum
{
    AP_CMD_SUCCESS,
    AP_CMD_UNKNOWN_TYPE,
    AP_CMD_NOT_INIT,
    AP_CMD_DUPLICATE_INIT,
    AP_CMD_DATA_MALLOC_FAIL,
    AP_CMD_QUEUE_MALLOC_FAIL,
    AP_CMD_THREAD_MALLOC_FAIL,
    AP_CMD_SEMAPHORE_MALLOC_FAIL,
    AP_CMD_WRONG_CONFIGURATION,
    AP_CMD_BUSY,
    AP_CMD_SENDTO_POINTER_NULL,
    AP_CMD_SENDTO_FAIL,
    AP_CMD_RECVFROM_POINTER_NULL,
    AP_CMD_RECVFROM_NO_DATA,
    AP_CMD_RECVFROM_FAIL,
    AP_CMD_RECVFROM_LEN_NOT_ENOUGH,
    AP_CMD_ALLOC_MEMORY_FAIL,
    AP_CMD_TIMER_OP,
    AP_CMD_INVALID_STATE,
    AP_CMD_INVALID_PARAMETER,
    AP_CMD_CMD_NOT_SUPPORTED,
    AP_CMD_INVALID_HOST_ID,
    AP_CMD_INVALID_HANDLE,
    AP_CMD_DB_PARSING_IN_PROGRESS,
    AP_CMD_SEQUENTIAL_PROTOCOL_VIOLATION,
    AP_CMD_HCI_TX_CMD_QUEUE_FULL,
    AP_CMD_HCI_TX_DATA_QUEUE_FULL,
    AP_CMD_RESOURCE_NOT_ENOUGH,
    AP_CMD_L2CAP_DATA_CHANNEL_NOT_EXIST,
    AP_CMD_L2CAP_DATA_DEST_ID_UNMATCH
} ap_cmd_error_t;

typedef enum
{
    AP_EVENT_CMD_COMPLETE = 0x01,
    AP_EVENT_ADVERTISING_REPORT,
    AP_EVENT_CONNECTION_COMPLETE,
    AP_EVENT_DISCONNECTION_COMPLETE,
    AP_EVENT_BASIC_RESPONSE,
    AP_EVENT_SENSOR_DATA_RESPONSE,
    AP_EVENT_DISPLAY_STATE_RESPONSE,
    AP_EVENT_LED_STATE_RESPONSE,
    AP_EVENT_ERROR_RESPONSE,
    AP_EVENT_OTA_CONTROL_RESPONSE,
    AP_EVENT_OTA_DOWNLOAD_RESPONSE,
    AP_EVENT_ESL_IMAGE_INFO_RESPONSE,
    AP_EVENT_ESL_IMAGE_TRANSFER_COMPLETE,
} ap_event_code_t;

//=============================================================================
//                Public Struct
//=============================================================================

typedef struct __attribute__((packed)) event_command_complete_param_s
{
    uint8_t  command;
    uint8_t  error_code;
} event_command_complete_param_t;

typedef struct __attribute__((packed)) event_advertising_report_param_s
{
    uint8_t  address_type;
    uint8_t  address[6];
} event_advertising_report_param_t;

typedef struct __attribute__((packed)) event_connection_complete_param_s
{
    uint8_t  status;
    uint8_t  address_type;
    uint8_t  address[6];
    uint16_t interval;
    uint16_t lantency;
    uint16_t timeout;
} event_connection_complete_param_t;

typedef struct __attribute__((packed)) event_disconnection_complete_param_s
{
    uint8_t  status;
    uint8_t  reason;
} event_disconnection_complete_param_t;

typedef struct __attribute__((packed)) event_basic_state_rsp_param_s
{
    uint8_t          group_id;
    uint8_t          esl_id;
    uint16_t         service_need:1,
                     synchronized:1,
                     active_led:1,
                     pending_led_update:1,
                     pending_display_update:1,
                     rfu:11;
} event_basic_state_rsp_param_t; 

typedef struct __attribute__((packed)) event_sensor_value_rsp_param_s
{
    uint8_t          group_id;
    uint8_t          esl_id;
    uint8_t          sensor_index;
    uint8_t          sensor_data[];
} event_sensor_value_rsp_param_t; 

typedef struct __attribute__((packed)) event_display_state_rsp_param_s
{
    uint8_t          group_id;
    uint8_t          esl_id;
    uint8_t          display_index;
    uint8_t          image_index;
} event_display_state_rsp_param_t; 

typedef struct __attribute__((packed)) event_led_state_rsp_param_s
{
    uint8_t          group_id;
    uint8_t          esl_id;
    uint8_t          led_index;
} event_led_state_rsp_param_t;

typedef struct __attribute__((packed)) event_err_rsp_param_s
{
    uint8_t          group_id;
    uint8_t          esl_id;
    esl_rsp_err_t    error_code;
} event_err_rsp_param_t;

typedef struct __attribute__((packed)) event_ota_control_rsp_param_s
{
    uint8_t          ota_command;
    uint8_t          command_error_code;
    uint8_t          rsp_data[];
} event_ota_control_rsp_param_t;

typedef struct __attribute__((packed)) event_ota_download_rsp_param_s
{
    uint8_t          notify_code;
    uint8_t          notify_data[];
} event_ota_download_rsp_param_t;

typedef struct __attribute__((packed)) event_image_info_rsp_param_s
{
    uint8_t          error_code;
    uint8_t          mtu_size;
} event_image_info_rsp_param_t;

typedef struct __attribute__((packed)) event_image_transfer_complete_event_param_s
{
    uint8_t          complete_num;
} event_image_transfer_complete_event_param_t;


//=============================================================================
//                Public Function Declaration
//=============================================================================


uint8_t ble_esl_ap_cmd_proc(ble_tlv_t *p_ble_tlv);
void ble_esl_ap_event_send(uint8_t event_code, uint8_t *p_param, uint16_t param_len);
ble_err_t esl_update_l2cap_data_info(uint16_t data_len);
ble_err_t esl_send_l2cap_data(uint8_t *p_l2cap_data, uint16_t l2cap_data_len);
void esl_ap_send_image_transfer_complete_event(uint8_t complete_number);
uart_handler_data_sts_t ble_esl_ap_cmd_parser(uint8_t *p_buf, uint16_t len, uint16_t *p_data_len, uint16_t *p_offset);

#ifdef __cplusplus
};
#endif
#endif /* __BLE_ESL_AP_CMD_H__ */
