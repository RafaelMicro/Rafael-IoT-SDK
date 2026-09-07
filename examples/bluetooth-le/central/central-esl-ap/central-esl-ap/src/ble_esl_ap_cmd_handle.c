/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file 
 * @author 
 * @brief
 * @version 0.1
 * @date 2022-03-31
 *
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"
#include <stdio.h>
#include <string.h>

/* Utility Library APIs */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart_handler.h"
#include "ble_scan.h"
#include "ble_gap.h"
#include "ble_common.h"
#include "ble_esl_ap_cmd.h"
#include "ble_app.h"
#include "ble_profile.h"
#include "ble_service_esls.h"
#include "ble_service_ots.h"
#include "ble_extended_advertising.h"
#include "ble_fota.h"

extern QueueHandle_t g_app_fota_cmd_q;
//=============================================================================
//                Private Definitions of const value
//=============================================================================

#define GET_AP_ERROR_CODE(ble_err) ((~ble_err) + 1)
//=============================================================================
//                Private ENUM
//=============================================================================



typedef enum
{
    BLE_CONTROL_SCAN_CONTROL = 0x01,
    BLE_CONTROL_CREATE_CONNECTION,
    BLE_CONTROL_ENHANCE_CREATE_CONNECTION,
    BLE_CONTROL_CANCEL_CREATE_CONNECTION,
    BLE_CONTROL_DISCONNECT_CONNECTION,
    BLE_CONTROL_LONG_TERM_KEY_CONFIGURATION,
    BLE_CONTROL_SYNC_CONFIGURATION,
    BLE_CONTROL_SYNC_CONTROL,
    BLE_CONTROL_SET_PERIODIC_ADV_INFO_TRANSFER,
    AP_CONTROL_ESL_AP_CONFIGURATION = 0x20,
    AP_CONTROL_ESL_TAG_CONFIGURATION,
    AP_CONTROL_ESL_IMAGE_INFO,
    AP_CONTROL_ESL_IMAGE_TRANSFER,
    AP_CONTROL_ESL_PING,
    AP_CONTROL_UNASSOCIATE_FROM_AP,
    AP_CONTROL_SERVICE_RESET,
    AP_CONTROL_FACTORY_RESET,
    AP_CONTROL_UPDATE_COMPLETE,
    AP_CONTROL_READ_SENSOR_DATA,
    AP_CONTROL_REFRESH_DISPLAY,
    AP_CONTROL_DISPLAY_IMAGE,
    AP_CONTROL_DISPLAY_TIMED_IMAGE,
    AP_CONTROL_LED_CONTROL,
    AP_CONTROL_LED_TIMED_CONTROL,
    AP_CONTROL_OTA_IMAGE,
    AP_CONTROL_OTA_CONTROL,
} ap_cmd_code_t;

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct __attribute__((packed))
{
    uint8_t  header[4];
    uint16_t len;
    uint8_t  parameter[];
}
esl_ap_cmd_hdr;

typedef struct __attribute__((packed))
{
    uint8_t command_id;
    uint8_t parameter[];
}
esl_ap_cmd_pd;

typedef struct __attribute__((packed))
{
    uint8_t cs;
}
esl_ap_cmd_end;

typedef struct __attribute__((packed)) ble_scan_control_param_s
{
    uint16_t interval;
    uint16_t window;
    uint8_t  control;
} ble_scan_control_param_t;

typedef struct __attribute__((packed)) ble_create_conn_param_s
{
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval;
    uint16_t conn_timeout;
    uint8_t  addr_type;
    uint8_t  address[6];
} ble_create_conn_param_t;

typedef struct __attribute__((packed)) ble_enhance_create_conn_param_s
{
    uint16_t scan_interval;
    uint16_t scan_window;
    uint16_t conn_interval;
    uint16_t conn_timeout;
    uint8_t  addr_type;
    uint8_t  address[6];
    uint8_t  group_id;
} ble_enhance_create_conn_param_t;

typedef struct __attribute__((packed)) ble_sync_configuration_param_s
{
    uint16_t period_interval;
    uint8_t  num_of_subevent;
    uint8_t  subevent_interval;
    uint8_t  response_slot_delay;
    uint8_t  slot_spacing;
    uint8_t  num_of_response_slot;
} ble_sync_configuration_param_t;

typedef struct __attribute__((packed)) ble_sync_control_param_s
{
    uint8_t  control;
} ble_sync_control_param_t;

typedef struct __attribute__((packed)) ble_set_periodic_adv_info_transfer_param_s
{
    uint16_t skip;
    uint16_t sync_timeout;
    uint16_t service_data;
} ble_set_periodic_adv_info_transfer_param_t;

typedef struct __attribute__((packed)) app_esl_ap_configuration_param_s
{
    uint8_t sync_key[16];
    uint8_t sync_nonce[8];
    uint8_t response_key[16];
    uint8_t response_nonce[8];
} app_esl_ap_configuration_param_t;

typedef struct __attribute__((packed)) app_esl_tag_configuration_param_s
{
    uint8_t group_id;
    uint8_t esl_id;
} app_esl_tag_configuration_param_t;

typedef struct __attribute__((packed)) app_esl_image_info_param_s
{
    uint8_t  image_idx;
    uint8_t  part;
    uint16_t image_length;
} app_esl_image_info_param_t;

#if 0
typedef struct __attribute__((packed)) app_esl_image_transfer_param_s
{
    uint8_t  image_data[];
} app_esl_image_transfer_param_t;
#endif

typedef struct __attribute__((packed)) app_esl_ping_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_ping_t ping[];
} app_esl_ping_param_t;

typedef struct __attribute__((packed)) app_unassociate_from_ap_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_unassociate_from_ap_t  unassociate_from_ap[];
} app_unassociate_from_ap_param_t;

typedef struct __attribute__((packed)) app_service_reset_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_service_reset_t service_reset[];
} app_service_reset_param_t;

typedef struct __attribute__((packed)) app_factory_reset_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_factory_reset_t  factory_reset[];
} app_factory_reset_param_t;

typedef struct __attribute__((packed)) app_update_complete_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_update_complete_t update_complete[];
} app_update_complete_param_t;

typedef struct __attribute__((packed)) app_read_sensor_data_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_read_sensor_data_t read_sensor_data[];
} app_read_sensor_data_param_t;

typedef struct __attribute__((packed)) app_refresh_display_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_refresh_display_t refresh_display[];
} app_refresh_display_param_t;

typedef struct __attribute__((packed)) app_display_image_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_display_image_t display_image[];
} app_display_image_param_t;

typedef struct __attribute__((packed)) app_display_timed_image_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_display_timed_image_t display_timed_image[];
} app_display_timed_image_param_t;

typedef struct __attribute__((packed)) app_led_control_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_led_control_t led_control[];
} app_led_control_param_t;

typedef struct __attribute__((packed)) app_led_timed_control_param_s
{
    uint8_t over_encrypted_adv;
    uint8_t group_id;
    ble_esl_cmd_led_timed_control_t led_timed_control[];
} app_led_timed_control_param_t;

typedef struct __attribute__((packed)) app_ota_image_param_s
{
    uint32_t address;
    uint8_t image_data[];
} app_ota_image_param_t;

typedef struct __attribute__((packed)) app_ota_control_param_s
{
    uint8_t ota_cmd;
    uint8_t ota_cmd_payload[];
} app_ota_control_param_t;
//=============================================================================
//                Private Function Declaration
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================


//=============================================================================
//                Functions
//=============================================================================
static uint8_t _ap_checksum_calc(uint8_t *pBuf, uint8_t len)
{
    uint8_t cs = 0;

    for (int i = 0; i < len; i++)
    {
        cs += pBuf[i];
    }
    return (~cs);
}

static uint8_t esl_ap_cmd_null(uint8_t *p_parm, uint8_t parm_len)
{
    return AP_CMD_UNKNOWN_TYPE;
}

static void esl_ap_send_cmd_complete(uint8_t command, uint8_t err_code)
{
    event_command_complete_param_t *p_command_complete;
    p_command_complete = pvPortMalloc(sizeof(event_command_complete_param_t));

    if (p_command_complete != NULL)
    {
        p_command_complete->command = command;
        p_command_complete->error_code = err_code;
        ble_esl_ap_event_send(AP_EVENT_CMD_COMPLETE, (uint8_t*)p_command_complete, sizeof(event_command_complete_param_t));
        vPortFree(p_command_complete);
    }
}

static uint8_t ble_control_scan_control(uint8_t *p_parm, uint8_t parm_len)
{
    ble_scan_control_param_t *p_scan_control;
    ap_cmd_error_t app_err_code;
    ble_err_t status;
    p_scan_control = (ble_scan_control_param_t *)p_parm;

    do
    {
        if (p_scan_control->control == 1)
        {
            ble_scan_param_t scan_param;
            ble_gap_addr_t addr_param;

            status = ble_cmd_device_addr_get(&addr_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }

            scan_param.scan_type = SCAN_TYPE_ACTIVE;
            scan_param.own_addr_type = addr_param.addr_type;
            scan_param.scan_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED;
            scan_param.scan_interval = p_scan_control->interval;       // 100*0.625ms=62.5ms;
            scan_param.scan_window = p_scan_control->window;       // 100*0.625ms=62.5ms;
            status = ble_cmd_scan_param_set(&scan_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }

            status = ble_cmd_scan_enable();
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            status = ble_cmd_scan_disable();
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_SCAN_CONTROL, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_create_connection(uint8_t *p_parm, uint8_t parm_len)
{
    ble_create_conn_param_t *p_create_conn;
    ble_gap_create_conn_param_t create_conn;
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    p_create_conn = (ble_create_conn_param_t *)p_parm;

    do
    {
        create_conn.host_id = 0;
        create_conn.own_addr_type = 0;//addr_param.addr_type;
        create_conn.scan_interval = p_create_conn->scan_interval;
        create_conn.scan_window = p_create_conn->scan_window;
        create_conn.init_filter_policy = INIT_FILTER_POLICY_ACCEPT_ALL;
        create_conn.peer_addr.addr_type = p_create_conn->addr_type;
        create_conn.peer_addr.addr[5] = p_create_conn->address[5];
        create_conn.peer_addr.addr[4] = p_create_conn->address[4];
        create_conn.peer_addr.addr[3] = p_create_conn->address[3];
        create_conn.peer_addr.addr[2] = p_create_conn->address[2];
        create_conn.peer_addr.addr[1] = p_create_conn->address[1];
        create_conn.peer_addr.addr[0] = p_create_conn->address[0];

        create_conn.conn_param.min_conn_interval = p_create_conn->conn_interval; //*1.25ms 
        create_conn.conn_param.max_conn_interval = p_create_conn->conn_interval; //*1.25ms
        create_conn.conn_param.periph_latency = 0; 
        create_conn.conn_param.supv_timeout = p_create_conn->conn_timeout; //*10ms = 1s

        status = ble_cmd_conn_create(&create_conn);
        if (status == BLE_ERR_OK)
        {
            printf("create connection to %02x:%02x:%02x:%02x:%02x:%02x ...\n", 
                   create_conn.peer_addr.addr[5], create_conn.peer_addr.addr[4], create_conn.peer_addr.addr[3],
                   create_conn.peer_addr.addr[2], create_conn.peer_addr.addr[1], create_conn.peer_addr.addr[0]);
        }
        else
        {
            printf("connection create fail %d\n", status);
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_CREATE_CONNECTION, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_enhance_create_connection(uint8_t *p_parm, uint8_t parm_len)
{
    ble_enhance_create_conn_param_t *p_enhance_create_conn;
    ble_gap_extend_create_conn_v2_param_t create_conn;
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    p_enhance_create_conn = (ble_enhance_create_conn_param_t *)p_parm;

    do
    {
        create_conn.host_id = 0;
        create_conn.adv_handle = ESL_AP_ADV_HANDLE;
        create_conn.own_addr_type = 0;//addr_param.addr_type;
        create_conn.scan_interval = p_enhance_create_conn->scan_interval;
        create_conn.scan_window = p_enhance_create_conn->scan_window;
        create_conn.init_filter_policy = INIT_FILTER_POLICY_ACCEPT_ALL;

        create_conn.peer_addr.addr_type = p_enhance_create_conn->addr_type;
        create_conn.peer_addr.addr[5] = p_enhance_create_conn->address[5];
        create_conn.peer_addr.addr[4] = p_enhance_create_conn->address[4];
        create_conn.peer_addr.addr[3] = p_enhance_create_conn->address[3];
        create_conn.peer_addr.addr[2] = p_enhance_create_conn->address[2];
        create_conn.peer_addr.addr[1] = p_enhance_create_conn->address[1];
        create_conn.peer_addr.addr[0] = p_enhance_create_conn->address[0];
        create_conn.conn_param.min_conn_interval = p_enhance_create_conn->conn_interval; //*1.25ms 
        create_conn.conn_param.max_conn_interval = p_enhance_create_conn->conn_interval; //*1.25ms
        create_conn.conn_param.periph_latency = 0; 
        create_conn.conn_param.supv_timeout = p_enhance_create_conn->conn_timeout; //*10ms = 1s
        create_conn.subevent = p_enhance_create_conn->group_id;

        status = ble_cmd_extend_conn_create_v2(&create_conn);
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_ENHANCE_CREATE_CONNECTION, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_cancel_create_connection(uint8_t *p_parm, uint8_t parm_len)
{
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    status = ble_cmd_conn_create_cancel();

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_CANCEL_CREATE_CONNECTION, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_disconnect_connection(uint8_t *p_parm, uint8_t parm_len)
{
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    status = ble_cmd_conn_terminate(0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_DISCONNECT_CONNECTION, app_err_code);
    return app_err_code;
}


static uint8_t ble_control_long_term_key_configuration(uint8_t *p_parm, uint8_t parm_len)
{
    return AP_CMD_UNKNOWN_TYPE;
}

static uint8_t ble_control_sync_configuration(uint8_t *p_parm, uint8_t parm_len)
{
    ble_sync_configuration_param_t *p_sync_configuration;
    ble_extended_adv_param_set_t *p_extend_adv_param;
    ble_adv_set_random_address_set_t *p_adv_set_addr;
    ble_periodic_adv_param_set_param_t *p_padv_param;
    ble_gap_addr_t own_addr;
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    p_sync_configuration = (ble_sync_configuration_param_t *)p_parm;

    do
    {
        p_extend_adv_param = pvPortMalloc(sizeof(ble_extended_adv_param_set_t));
        if (p_extend_adv_param == NULL)
        {
            printf("p_extend_adv_param mallc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_extend_adv_param->adv_handle = ESL_AP_ADV_HANDLE;
        p_extend_adv_param->adv_event_properties = 0;
        p_extend_adv_param->primary_adv_interval_min = 0x280;
        p_extend_adv_param->primary_adv_interval_max = 0x280;
        p_extend_adv_param->primary_adv_ch_map = ADV_CHANNEL_ALL;

        ble_gap_device_address_get(&own_addr);
        p_extend_adv_param->own_address_type = own_addr.addr_type;
        p_extend_adv_param->peer_address_type = 0x01;
        p_extend_adv_param->peer_address[0] = 0x88;
        p_extend_adv_param->peer_address[1] = 0x88;
        p_extend_adv_param->peer_address[2] = 0x88;
        p_extend_adv_param->peer_address[3] = 0x88;
        p_extend_adv_param->peer_address[4] = 0x88;
        p_extend_adv_param->peer_address[5] = 0xCC;
        p_extend_adv_param->adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;
        p_extend_adv_param->primary_adv_phy = BLE_PHY_1M;
        p_extend_adv_param->secondary_adv_max_skip = 0x00;
        p_extend_adv_param->secondary_adv_phy = BLE_PHY_1M;
        p_extend_adv_param->adv_sid = 0x00;
        p_extend_adv_param->scan_req_notification_enable = 0x01;

        status = ble_cmd_extended_adv_parameter_set(p_extend_adv_param);
        vPortFree(p_extend_adv_param);
        if (status != BLE_ERR_OK)
        {
            printf("extended ADV parameter set fail %d\n", status); 
            break;
        }

        p_adv_set_addr = pvPortMalloc(sizeof(ble_adv_set_random_address_set_t));
        if (p_adv_set_addr == NULL)
        {
            printf("p_adv_set_addr mallc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        p_adv_set_addr->adv_handle = ESL_AP_ADV_HANDLE;
        memcpy(p_adv_set_addr->addr, DEVICE_ADDR.addr, BLE_ADDR_LEN);
        status = ble_cmd_adv_set_random_addr_set(p_adv_set_addr);
        vPortFree(p_adv_set_addr);
        if (status != BLE_ERR_OK)
        {
            printf("adv set random address set fail %d\n", status); 
            break;
        }

        p_padv_param = pvPortMalloc(sizeof(ble_periodic_adv_param_set_param_t));
        if (p_padv_param == NULL)
        {
            printf("p_padv_param malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }  
        p_padv_param->adv_handle = ESL_AP_ADV_HANDLE;
        p_padv_param->padv_interval_min = p_sync_configuration->period_interval;// padv_interval*1.25ms  
        p_padv_param->padv_interval_max = p_sync_configuration->period_interval;// padv_interval*1.25ms
        p_padv_param->padv_properties = 0;   
        p_padv_param->num_subevents = p_sync_configuration->num_of_subevent;// 15;
        p_padv_param->subevent_interval =  p_sync_configuration->subevent_interval; //subevent_interval*1.25ms
        p_padv_param->rsp_slot_delay =  p_sync_configuration->response_slot_delay;  //rsp_slot_delay*1.25ms
        p_padv_param->rsp_slot_spacing =p_sync_configuration->slot_spacing;//rsp_slot_spacing* 0.125ms
        p_padv_param->num_rsp_slot = p_sync_configuration->num_of_response_slot;
        status = ble_cmd_periodic_adv_parameter_set(p_padv_param); 
        vPortFree(p_padv_param);

        if (status != BLE_ERR_OK)
        {
            printf("periodic ADV parameter set fail %d\n", status);
            break;
        }


    } while (0);
   
    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_SYNC_CONFIGURATION, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_sync_control(uint8_t *p_parm, uint8_t parm_len)
{
    ble_sync_control_param_t *p_sync_control;
    ble_periodic_adv_enable_set_param_t *p_padv_eanble; 
    ble_extended_adv_enable_set_t *p_eadv_enable;
    ap_cmd_error_t app_err_code;
    ble_err_t status;

    p_sync_control = (ble_sync_control_param_t *)p_parm;

    do
    {
        p_padv_eanble =pvPortMalloc(sizeof(ble_periodic_adv_enable_set_param_t));
        if (p_padv_eanble == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_padv_eanble->adv_handle = ESL_AP_ADV_HANDLE;
        status = ble_cmd_periodic_adv_enable_set(p_padv_eanble);
        vPortFree(p_padv_eanble);

        if (status != BLE_ERR_OK)
        {
            printf("periodic ADV enable fail %d\n", status);
            break;
        }

        p_eadv_enable = pvPortMalloc(sizeof(ble_extended_adv_enable_set_t));
        if (p_eadv_enable == NULL)
        {
            printf("p_eadv_enable malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        p_eadv_enable->enable = 0x01;
        p_eadv_enable->adv_handle = ESL_AP_ADV_HANDLE;
        p_eadv_enable->duration = 100;
        p_eadv_enable->max_extended_adv_events = 0;
        status = ble_cmd_extended_adv_enable_set(BLE_HOSTID_RESERVED, p_eadv_enable);
        vPortFree(p_eadv_enable);
        if (status != BLE_ERR_OK)
        {
            printf("extended ADV enable fail %d\n", status);
        }

    } while (0);
   
    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_SYNC_CONTROL, app_err_code);
    return app_err_code;
}

static uint8_t ble_control_set_periodic_adv_info_transfer(uint8_t *p_parm, uint8_t parm_len)
{
    ble_periodic_adv_set_info_transfer_set_param_t info_transfer;
    ble_periodic_adv_sync_transfer_param_t sync_transfer;
    ble_set_periodic_adv_info_transfer_param_t *p_padv_info_transfer;
    ap_cmd_error_t app_err_code;
    ble_err_t status;
    
    p_padv_info_transfer = (ble_set_periodic_adv_info_transfer_param_t *)p_parm;
    do
    {
        sync_transfer.host_id = 0;
        sync_transfer.mode = PERIODIC_ADV_EVENT_WITH_DUPLICATE_FILTER;
        sync_transfer.skip = p_padv_info_transfer->skip;  //  0x0123;
        sync_transfer.sync_timeout = p_padv_info_transfer->sync_timeout; //0x300;
        status = ble_cmd_periodic_adv_sync_transfer_param_set(&sync_transfer);
        if (status != BLE_ERR_OK)
        {
            printf("periodic_adv_sync transfer param %d\n", status);
        }

        info_transfer.adv_handle = ESL_AP_ADV_HANDLE;
        info_transfer.service_data = p_padv_info_transfer->service_data ;//0x002A;
        info_transfer.host_id = 0;
        status = ble_cmd_periodic_adv_set_info_transfer_set(&info_transfer);

        if (status != BLE_ERR_OK)
        {
            printf("periodic_adv_set_info_transfer_set %d\n", status);
        }

    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(BLE_CONTROL_SET_PERIODIC_ADV_INFO_TRANSFER, app_err_code);
    return app_err_code;
}

uint8_t (* const process_ble_control[])(uint8_t *p_data, uint8_t length) =
{
    ble_control_scan_control,                     /* 0x01 scan control */
    ble_control_create_connection,                /* 0x02 create connection */
    ble_control_enhance_create_connection,        /* 0x03 enhance create connection */
    ble_control_cancel_create_connection,         /* 0x04 cancel create connection  */
    ble_control_disconnect_connection,            /* 0x05 disconnect connection  */
    ble_control_long_term_key_configuration,      /* 0x06 long term key configuration */
    ble_control_sync_configuration,               /* 0x07 configure synchronization information*/
    ble_control_sync_control,                     /* 0x08 enable or disable synchronization*/
    ble_control_set_periodic_adv_info_transfer,   /* 0x09 set periodic advertising set info transfer and periodic advertising sync transfer parameter*/
    esl_ap_cmd_null,                              /* 0x0a */
    esl_ap_cmd_null,                              /* 0x0b */
    esl_ap_cmd_null,                              /* 0x0c */
    esl_ap_cmd_null,                              /* 0x0d */
    esl_ap_cmd_null,                              /* 0x0e */
    esl_ap_cmd_null,                              /* 0x0f */
};


static uint8_t app_control_esl_ap_configuration(uint8_t *p_parm, uint8_t parm_len)
{
    app_esl_ap_configuration_param_t *p_esl_ap_config;
    ap_cmd_error_t app_err_code = AP_CMD_UNKNOWN_TYPE;
    
    if (parm_len < sizeof(app_esl_ap_configuration_param_t))
    {
        app_err_code = AP_CMD_INVALID_PARAMETER;
    }
    else
    {
        p_esl_ap_config = (app_esl_ap_configuration_param_t *)p_parm;

        memcpy(esl_ap_sync_key, p_esl_ap_config->sync_key, sizeof(esl_ap_sync_key));
        memcpy(esl_ap_nonce, p_esl_ap_config->sync_nonce, sizeof(esl_ap_nonce));
        memcpy(esl_esl_rsp_key, p_esl_ap_config->response_key, sizeof(esl_esl_rsp_key));
        memcpy(esl_rsp_nonce, p_esl_ap_config->response_nonce, sizeof(esl_rsp_nonce));
        app_err_code = AP_CMD_SUCCESS;
    }

    esl_ap_send_cmd_complete(AP_CONTROL_ESL_AP_CONFIGURATION, app_err_code);

    return app_err_code;
}

static uint8_t app_control_esl_tag_configuration(uint8_t *p_parm, uint8_t parm_len)
{
    app_esl_tag_configuration_param_t *p_esl_tag_config;
    ap_cmd_error_t app_err_code;
    ap_config_esl_device_t config_device;
    ble_err_t status;

    p_esl_tag_config = (app_esl_tag_configuration_param_t *)p_parm;

    do
    {
        config_device.esl_id = ((p_esl_tag_config->group_id << 8) | p_esl_tag_config->esl_id);
        config_device.p_ap_sync_key = pvPortMalloc(sizeof(esl_ap_sync_key) + sizeof(esl_ap_nonce));
        if (config_device.p_ap_sync_key != NULL)
        {
            memcpy(config_device.p_ap_sync_key, esl_ap_sync_key, sizeof(esl_ap_sync_key));
            memcpy(config_device.p_ap_sync_key + sizeof(esl_ap_sync_key), esl_ap_nonce, sizeof(esl_ap_nonce));
        }
        else
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }

        config_device.p_esl_rsp_key = pvPortMalloc(sizeof(esl_esl_rsp_key) + sizeof(esl_rsp_nonce));
        if (config_device.p_esl_rsp_key != NULL)
        {
            memcpy(config_device.p_esl_rsp_key, esl_esl_rsp_key, sizeof(esl_esl_rsp_key));
            memcpy(config_device.p_esl_rsp_key + sizeof(esl_esl_rsp_key), esl_rsp_nonce, sizeof(esl_rsp_nonce));
        }
        else
        {
            if (config_device.p_ap_sync_key != NULL)
            {
                vPortFree(config_device.p_ap_sync_key);
            }
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        config_device.current_time = xTaskGetTickCount();

        status = ap_config_esl_device(&config_device);

        if (config_device.p_ap_sync_key != NULL)
        {
            vPortFree(config_device.p_ap_sync_key);
        }

        if (config_device.p_esl_rsp_key != NULL)
        {
            vPortFree(config_device.p_esl_rsp_key);
        }
    } while (0);
   
    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_ESL_TAG_CONFIGURATION, app_err_code);
    return app_err_code;
}

static uint8_t app_control_esl_image_info(uint8_t *p_parm, uint8_t parm_len)
{
    app_esl_image_info_param_t *p_image_info;
    event_image_info_rsp_param_t *p_image_info_rsp;
    ots_oacp_t ocap_cmd;
    ots_olcp_t olcp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    uint16_t retry_cnt = 6000, curr_cnt = 0;

    p_image_info = (app_esl_image_info_param_t *)p_parm;

    do
    {
        if (ble_app_link_info[APP_ESL_AP_C_HOST_ID].state != STATE_CONNECTED)
        {
            status = BLE_ERR_INVALID_STATE;
            break;
        }

        olcp_cmd.opcode = OLCP_OPCODE_GO_TO;
        olcp_cmd.param.go_to.object_id[0] = p_image_info->image_idx;
        olcp_cmd.param.go_to.object_id[1] = p_image_info->part;
        olcp_cmd.param.go_to.object_id[2] = 0x00;
        olcp_cmd.param.go_to.object_id[3] = 0x00;
        olcp_cmd.param.go_to.object_id[4] = 0x00;
        olcp_cmd.param.go_to.object_id[5] = 0xFF;

        curr_cnt = 0;
        while(object_list_control_point_cmd_send((uint8_t*)&olcp_cmd, sizeof(ots_olcp_go_to_t) + sizeof(olcp_cmd.opcode)) != BLE_ERR_OK)
        {            
            curr_cnt++;
            if(curr_cnt > retry_cnt)
            {
                status = BLE_BUSY;
                break;
            }
            vTaskDelay(2);
        }

        if (status == BLE_BUSY)
        {
            break;
        }

        ocap_cmd.opcode = OACP_OPCODE_WRITE;
        ocap_cmd.param.write.length = p_image_info->image_length;
        ocap_cmd.param.write.offset = 0;
        ocap_cmd.param.write.mode = 1;
        ocap_cmd.param.write.rfu = 0;
        ocap_cmd.param.write.rfu_1 = 0;
        ocap_cmd.opcode = OACP_OPCODE_WRITE;
        curr_cnt = 0;
        while (object_action_control_point_cmd_send((uint8_t*)&ocap_cmd, sizeof(ots_oacp_calculate_write_t) + sizeof(ocap_cmd.opcode)) != BLE_ERR_OK);
        {            
            curr_cnt++;
            if(curr_cnt > retry_cnt)
            {
                status = BLE_BUSY;
                break;
            }
            vTaskDelay(2);
        }

        if (status == BLE_BUSY)
        {
            break;
        }
        status = esl_update_l2cap_data_info(p_image_info->image_length);
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);

    esl_ap_send_cmd_complete(AP_CONTROL_ESL_IMAGE_INFO, app_err_code);

    p_image_info_rsp = pvPortMalloc(sizeof(event_image_info_rsp_param_t));

    if (p_image_info_rsp != NULL)
    {
        p_image_info_rsp->error_code = app_err_code;
        p_image_info_rsp->mtu_size = g_l2cap_data.mtu_size;
        ble_esl_ap_event_send(AP_EVENT_ESL_IMAGE_INFO_RESPONSE, (uint8_t *)p_image_info_rsp, sizeof(event_image_info_rsp_param_t));
        vPortFree(p_image_info_rsp);
    }
    return app_err_code;
}

static uint8_t app_control_esl_image_transfer(uint8_t *p_parm, uint8_t parm_len)
{
    //app_esl_image_transfer_param_t *p_image_transfer;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;

    //p_image_transfer = (app_esl_image_transfer_param_t *)p_parm;

    status = esl_send_l2cap_data(p_parm, parm_len);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_ESL_IMAGE_TRANSFER, app_err_code);
    return app_err_code;
}

static uint8_t app_control_ping(uint8_t *p_parm, uint8_t parm_len)
{
    app_esl_ping_param_t *p_ping;
    ble_esl_cmd_ping_t ping_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_ping = (app_esl_ping_param_t *)p_parm;
    parse_len = sizeof(app_esl_ping_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_ping->ping[num].opcode != ESL_CMD_PING)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_ping_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_ping->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
           else
            {
                for( i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_ping->ping[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_ping->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_ping_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_ping->ping, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_ping->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            ping_cmd.esl_id = p_ping->ping[0].esl_id;
            ping_cmd.opcode = p_ping->ping[0].opcode;
            status = esl_control_point_cmd_send((uint8_t*)&ping_cmd, sizeof(ping_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_ping->group_id, p_ping->ping[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_ESL_PING, app_err_code);
    return app_err_code;
}

static uint8_t app_control_unassociate_from_ap(uint8_t *p_parm, uint8_t parm_len)
{
    app_unassociate_from_ap_param_t *p_unassociate_from_ap;
    ble_esl_cmd_unassociate_from_ap_t unassociate_from_ap_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_unassociate_from_ap = (app_unassociate_from_ap_param_t *)p_parm;
    parse_len = sizeof(app_unassociate_from_ap_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_unassociate_from_ap->unassociate_from_ap[num].opcode != ESL_CMD_UNASSOCIATE_FROM_AP)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_unassociate_from_ap_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_unassociate_from_ap->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_unassociate_from_ap->unassociate_from_ap[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_unassociate_from_ap->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_unassociate_from_ap_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_unassociate_from_ap->unassociate_from_ap, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_unassociate_from_ap->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }

            unassociate_from_ap_cmd.esl_id = p_unassociate_from_ap->unassociate_from_ap[0].esl_id;
            unassociate_from_ap_cmd.opcode = p_unassociate_from_ap->unassociate_from_ap[0].opcode;
            status = esl_control_point_cmd_send((uint8_t*)&unassociate_from_ap_cmd, sizeof(unassociate_from_ap_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_unassociate_from_ap->group_id, p_unassociate_from_ap->unassociate_from_ap[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_UNASSOCIATE_FROM_AP, app_err_code);
    return app_err_code;
}

static uint8_t app_control_service_reset(uint8_t *p_parm, uint8_t parm_len)
{
    app_service_reset_param_t *p_service_reset;
    ble_esl_cmd_service_reset_t service_reset_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_service_reset = (app_service_reset_param_t *)p_parm;
    parse_len = sizeof(app_service_reset_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_service_reset->service_reset[num].opcode != ESL_CMD_SERVICE_RESET)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_service_reset_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_service_reset->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_service_reset->service_reset[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_service_reset->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_service_reset_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_service_reset->service_reset, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_service_reset->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }

            service_reset_cmd.esl_id = p_service_reset->service_reset[0].esl_id;
            service_reset_cmd.opcode = p_service_reset->service_reset[0].opcode;
            status = esl_control_point_cmd_send((uint8_t*)&service_reset_cmd, sizeof(service_reset_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_service_reset->group_id, p_service_reset->service_reset[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_SERVICE_RESET, app_err_code);
    return app_err_code;
}

static uint8_t app_control_factory_reset(uint8_t *p_parm, uint8_t parm_len)
{
    app_factory_reset_param_t *p_factory_reset;
    ble_esl_cmd_factory_reset_t factory_reset_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_factory_reset = (app_factory_reset_param_t *)p_parm;
    parse_len = sizeof(app_factory_reset_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_factory_reset->factory_reset[num].opcode != ESL_CMD_FACTORY_RESET)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_factory_reset_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_factory_reset->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_factory_reset->factory_reset[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_factory_reset->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_factory_reset_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_factory_reset->factory_reset, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_factory_reset->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            factory_reset_cmd.esl_id = p_factory_reset->factory_reset[0].esl_id;
            factory_reset_cmd.opcode = p_factory_reset->factory_reset[0].opcode;
            status = esl_control_point_cmd_send((uint8_t*)&factory_reset_cmd, sizeof(factory_reset_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_factory_reset->group_id, p_factory_reset->factory_reset[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_FACTORY_RESET, app_err_code);
    return app_err_code;
}

static uint8_t app_control_update_complete(uint8_t *p_parm, uint8_t parm_len)
{
    app_update_complete_param_t *p_update_complete;
    ble_esl_cmd_update_complete_t update_complete_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_update_complete = (app_update_complete_param_t *)p_parm;
    parse_len = sizeof(app_update_complete_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_update_complete->update_complete[num].opcode != ESL_CMD_UPDATE_COMPLETE)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_update_complete_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_update_complete->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_update_complete->update_complete[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_update_complete->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_update_complete_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_update_complete->update_complete, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_update_complete->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            update_complete_cmd.esl_id = p_update_complete->update_complete[0].esl_id;
            update_complete_cmd.opcode = p_update_complete->update_complete[0].opcode;
            status = esl_control_point_cmd_send((uint8_t*)&update_complete_cmd, sizeof(update_complete_cmd));

        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_UPDATE_COMPLETE, app_err_code);
    return app_err_code;
}

static uint8_t app_control_read_sensor_data(uint8_t *p_parm, uint8_t parm_len)
{
    app_read_sensor_data_param_t *p_read_sensor_data;
    ble_esl_cmd_read_sensor_data_t read_sensor_data_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_read_sensor_data = (app_read_sensor_data_param_t *)p_parm;
    parse_len = sizeof(app_read_sensor_data_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_read_sensor_data->read_sensor_data[num].opcode != ESL_CMD_READ_SENSOR_DATA)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_read_sensor_data_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_read_sensor_data->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_read_sensor_data->read_sensor_data[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_read_sensor_data->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_read_sensor_data_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_read_sensor_data->read_sensor_data, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_read_sensor_data->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            memcpy((uint8_t*)&read_sensor_data_cmd.esl_id, (uint8_t*)&p_read_sensor_data->read_sensor_data[0].esl_id, sizeof(read_sensor_data_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&read_sensor_data_cmd, sizeof(read_sensor_data_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_read_sensor_data->group_id, p_read_sensor_data->read_sensor_data[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_READ_SENSOR_DATA, app_err_code);
    return app_err_code;
}

static uint8_t app_control_refresh_display(uint8_t *p_parm, uint8_t parm_len)
{
    app_refresh_display_param_t *p_refresh_display;
    ble_esl_cmd_refresh_display_t refresh_display_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_refresh_display = (app_refresh_display_param_t *)p_parm;
    parse_len = sizeof(app_refresh_display_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_refresh_display->refresh_display[num].opcode != ESL_CMD_REFRESH_DISPLAY)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_refresh_display_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_refresh_display->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_refresh_display->refresh_display[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_refresh_display->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_refresh_display_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_refresh_display->refresh_display, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_refresh_display->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            memcpy((uint8_t*)&refresh_display_cmd.esl_id, (uint8_t*)&p_refresh_display->refresh_display[0].esl_id, sizeof(refresh_display_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&refresh_display_cmd, sizeof(refresh_display_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_refresh_display->group_id, p_refresh_display->refresh_display[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_REFRESH_DISPLAY, app_err_code);
    return app_err_code;
}

static uint8_t app_control_display_image(uint8_t *p_parm, uint8_t parm_len)
{
    app_display_image_param_t *p_display_image;
    ble_esl_cmd_display_image_t display_image_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_display_image = (app_display_image_param_t *)p_parm;
    parse_len = sizeof(app_display_image_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_display_image->display_image[num].opcode != ESL_CMD_DISPLAY_IMAGE)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_display_image_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_display_image->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_display_image->display_image[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_display_image->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_display_image_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_display_image->display_image, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_display_image->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }

            memcpy((uint8_t*)&display_image_cmd.esl_id, (uint8_t*)&p_display_image->display_image[0].esl_id, sizeof(display_image_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&display_image_cmd, sizeof(display_image_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_display_image->group_id, p_display_image->display_image[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_DISPLAY_IMAGE, app_err_code);
    return app_err_code;
}

static uint8_t app_control_display_timed_image(uint8_t *p_parm, uint8_t parm_len)
{
    app_display_timed_image_param_t *p_display_timed_image;
    ble_esl_cmd_display_timed_image_t display_timed_image_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_display_timed_image = (app_display_timed_image_param_t *)p_parm;
    parse_len = sizeof(app_display_timed_image_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_display_timed_image->display_timed_image[num].opcode != ESL_CMD_DISPLAY_TIMED_IMAGE)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_display_timed_image_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_display_timed_image->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_display_timed_image->display_timed_image[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_display_timed_image->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_display_timed_image_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_display_timed_image->display_timed_image, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_display_timed_image->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            memcpy((uint8_t*)&display_timed_image_cmd.esl_id, (uint8_t*)&p_display_timed_image->display_timed_image[0].esl_id, sizeof(display_timed_image_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&display_timed_image_cmd, sizeof(display_timed_image_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_display_timed_image->group_id, p_display_timed_image->display_timed_image[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_DISPLAY_TIMED_IMAGE, app_err_code);
    return app_err_code;
}

static uint8_t app_control_led_control(uint8_t *p_parm, uint8_t parm_len)
{
    app_led_control_param_t *p_led_control;
    ble_esl_cmd_led_control_t led_control_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_led_control = (app_led_control_param_t *)p_parm;
    parse_len = sizeof(app_led_control_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_led_control->led_control[num].opcode != ESL_CMD_LED_CONTROL)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_led_control_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_led_control->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_led_control->led_control[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_led_control->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_led_control_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_led_control->led_control, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_led_control->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            memcpy((uint8_t*)&led_control_cmd.esl_id, (uint8_t*)&p_led_control->led_control[0].esl_id, sizeof(led_control_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&led_control_cmd, sizeof(led_control_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_led_control->group_id, p_led_control->led_control[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_LED_CONTROL, app_err_code);
    return app_err_code;
}

static uint8_t app_control_led_timed_control(uint8_t *p_parm, uint8_t parm_len)
{
    app_led_timed_control_param_t *p_led_timed_control;
    ble_esl_cmd_led_timed_control_t led_timed_control_cmd;
    esl_control_point_cmd_send_t cp_cmd;
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_OK;
    uint8_t parse_len = 0, num = 0, *p_esl_id_list, i;

    p_led_timed_control = (app_led_timed_control_param_t *)p_parm;
    parse_len = sizeof(app_led_timed_control_param_t);

    do
    {
        while (parse_len < parm_len)
        {
            if (p_led_timed_control->led_timed_control[num].opcode != ESL_CMD_LED_TIMED_CONTROL)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break;
            }
            num++;
            parse_len += sizeof(ble_esl_cmd_led_timed_control_t);
        }

        if (status != BLE_ERR_OK)
        {
            break;
        }

        if (p_led_timed_control->over_encrypted_adv == true)
        {
            p_esl_id_list = pvPortMalloc(sizeof(uint8_t) * num);
            if (p_esl_id_list == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                break;
            }
            else
            {
                for (i = 0; i < num; i++)
                {
                    p_esl_id_list[i] = p_led_timed_control->led_timed_control[i].esl_id;
                }
            }

            cp_cmd.group_num = 1;
            cp_cmd.group_cmd[0].group_id = p_led_timed_control->group_id;
            cp_cmd.group_cmd[0].cmd_num = num;
            cp_cmd.group_cmd[0].cmd_len = (cp_cmd.group_cmd[0].cmd_num * sizeof(ble_esl_cmd_led_timed_control_t));
            cp_cmd.group_cmd[0].p_cmd = pvPortMalloc(cp_cmd.group_cmd[0].cmd_len);
            if (cp_cmd.group_cmd[0].p_cmd != NULL)
            {
                memcpy(cp_cmd.group_cmd[0].p_cmd, p_led_timed_control->led_timed_control, cp_cmd.group_cmd[0].cmd_len);
                status = esl_control_point_cmd_send_over_encrypt_adv(&cp_cmd);
                vPortFree(cp_cmd.group_cmd[0].p_cmd);
                if (status == BLE_ERR_OK)
                {
                    esl_add_control_point_esl_id_list(p_led_timed_control->group_id, num, p_esl_id_list);
                }
            }
            else
            {
                status = BLE_ERR_ALLOC_MEMORY_FAIL;
            }
            vPortFree(p_esl_id_list);
        }
        else
        {
            if (num > 1)
            {
                status = BLE_ERR_INVALID_PARAMETER;
                break; 
            }

            if (esl_control_point_cmd_processing() == TRUE)
            {
                status = BLE_BUSY;
                break; 
            }
            memcpy((uint8_t*)&led_timed_control_cmd.esl_id, (uint8_t*)&p_led_timed_control->led_timed_control[0].esl_id, sizeof(led_timed_control_cmd));
            status = esl_control_point_cmd_send((uint8_t*)&led_timed_control_cmd, sizeof(led_timed_control_cmd));
            if (status == BLE_ERR_OK)
            {
                esl_record_control_point_esl_id(p_led_timed_control->group_id, p_led_timed_control->led_timed_control[0].esl_id);
            }
        }
    } while (0);

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_LED_TIMED_CONTROL, app_err_code);
    return app_err_code;
}

static uint8_t app_control_ota_image(uint8_t *p_parm, uint8_t parm_len)
{
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_info_link0_t *p_profile_info;
    app_ota_image_param_t *p_ota_image;
    fota_data_param_t *p_fota_data;
    ble_gatt_data_param_t param;

    p_ota_image = (app_ota_image_param_t *)p_parm;

    p_fota_data = pvPortMalloc(sizeof(fota_data_param_t) + parm_len - sizeof(app_ota_image_param_t));

    if (p_fota_data != NULL)
    {
        p_fota_data->data_addr = p_ota_image->address;
        p_fota_data->data_len = parm_len - sizeof(app_ota_image_param_t);
        memcpy(p_fota_data->data, p_ota_image->image_data, p_fota_data->data_len);
        p_profile_info = (ble_info_link0_t *)ble_app_link_info[0].profile_info;

        param.host_id = APP_ESL_AP_C_HOST_ID;
        param.handle_num = p_profile_info->svcs_info_fotas.client_info.handles.hdl_data;
        param.length = sizeof(fota_data_param_t) + parm_len - sizeof(app_ota_image_param_t);
        param.p_data = (uint8_t *)p_fota_data;
        status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_CMD, &param);
        vPortFree(p_fota_data);
    }
    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_OTA_IMAGE, app_err_code);


    return app_err_code;
}

static uint8_t app_control_ota_control(uint8_t *p_parm, uint8_t parm_len)
{
    ap_cmd_error_t app_err_code;
    ble_err_t status = BLE_ERR_UNKNOW_TYPE;
    ble_info_link0_t *p_profile_info;
    app_ota_control_param_t *p_ota_control;
    fota_cmd_param_t *p_fota_cmd;
    ble_gatt_data_param_t param;
    app_fota_cmd_queue_t app_fota_cmd;
    p_ota_control = (app_ota_control_param_t *)p_parm;

    p_fota_cmd = pvPortMalloc(sizeof(fota_cmd_param_t) + parm_len - sizeof(app_ota_control_param_t));
    if (p_fota_cmd != NULL)
    {
        p_profile_info = (ble_info_link0_t *)ble_app_link_info[0].profile_info;
        p_fota_cmd->cmd_id = p_ota_control->ota_cmd;
        if ((parm_len - sizeof(app_ota_control_param_t)) > 0)
        {
            memcpy((uint8_t *)&p_fota_cmd->cmd_param.start_cmd, p_ota_control->ota_cmd_payload, (parm_len - sizeof(app_ota_control_param_t)));
        }

        app_fota_cmd.cmd_id = p_ota_control->ota_cmd;
        if (xQueueSendToBack(g_app_fota_cmd_q, &app_fota_cmd, 2) != pdTRUE)
        {
            status = BLE_BUSY;
        }
        else
        {
            param.host_id = APP_ESL_AP_C_HOST_ID;
            param.handle_num = p_profile_info->svcs_info_fotas.client_info.handles.hdl_command;
            param.length = sizeof(p_ota_control->ota_cmd) + parm_len - sizeof(app_ota_control_param_t);
            param.p_data = (uint8_t *)p_fota_cmd;
            status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &param);
        }
        vPortFree(p_fota_cmd);
    }
    else
    {
        status = BLE_ERR_ALLOC_MEMORY_FAIL;
    }

    app_err_code = GET_AP_ERROR_CODE(status);
    esl_ap_send_cmd_complete(AP_CONTROL_OTA_CONTROL, app_err_code);
    return app_err_code;
}

uint8_t (* const process_application_control[])(uint8_t *p_data, uint8_t length) =
{
    app_control_esl_ap_configuration,    /* 0x20 */
    app_control_esl_tag_configuration,   /* 0x21 */
    app_control_esl_image_info,          /* 0x22 */
    app_control_esl_image_transfer,      /* 0x23 */
    app_control_ping,                    /* 0x24 */
    app_control_unassociate_from_ap,     /* 0x25 */
    app_control_service_reset,           /* 0x26 */
    app_control_factory_reset,           /* 0x27 */
    app_control_update_complete,         /* 0x28 */
    app_control_read_sensor_data,        /* 0x29 */
    app_control_refresh_display,         /* 0x2a */
    app_control_display_image,           /* 0x2b */
    app_control_display_timed_image,     /* 0x2c */
    app_control_led_control,             /* 0x2d */
    app_control_led_timed_control,       /* 0x2e */
    app_control_ota_image,               /* 0x2f */
    app_control_ota_control,             /* 0x30 */
    esl_ap_cmd_null,                     /* 0x31 */
};

void esl_ap_send_adv_report_event(uint8_t complete_number)
{
    event_image_transfer_complete_event_param_t *p_image_transfer_complete;
    p_image_transfer_complete = pvPortMalloc(sizeof(event_image_transfer_complete_event_param_t));

    if (p_image_transfer_complete != NULL)
    {
        p_image_transfer_complete->complete_num = complete_number;
        ble_esl_ap_event_send(AP_EVENT_ESL_IMAGE_TRANSFER_COMPLETE, (uint8_t *)p_image_transfer_complete, sizeof(event_image_transfer_complete_event_param_t));
        vPortFree(p_image_transfer_complete);
    }
}

void ble_esl_ap_event_send(uint8_t event_code, uint8_t *p_param, uint16_t param_len)
{
    uint8_t *p_ap_event_pkt, idx = 0, i;
    uint32_t pkt_len;
    uint32_t opcode_size = 0, sub_hdr_len = 0;

    do
    {
        pkt_len = sizeof(esl_ap_cmd_hdr) + sizeof(esl_ap_cmd_pd) + param_len + sizeof(esl_ap_cmd_end);

        p_ap_event_pkt = pvPortMalloc(pkt_len);

        if (p_ap_event_pkt == NULL)
        {
            break;
        }

        ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->header[0] = 0xFF;
        ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->header[1] = 0xFD;
        ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->header[2] = 0xFD;
        ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->header[3] = 0xFF;
        ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->len = sizeof(esl_ap_cmd_pd) + param_len;

        idx += sizeof(esl_ap_cmd_hdr);

        ((esl_ap_cmd_pd *)(&p_ap_event_pkt[idx]))->command_id = event_code;

        memcpy(((esl_ap_cmd_pd *)(&p_ap_event_pkt[idx]))->parameter, p_param, param_len);

        idx += sizeof(esl_ap_cmd_pd);
        idx += (param_len);
        ((esl_ap_cmd_end *)(&p_ap_event_pkt[idx]))->cs = _ap_checksum_calc((uint8_t *) & (((esl_ap_cmd_hdr *)(p_ap_event_pkt))->len),
                ((esl_ap_cmd_hdr *)(p_ap_event_pkt))->len + 2/*len 2Byte*/);

        uart_handler_send(p_ap_event_pkt, pkt_len);
#if 0
        printf("------------------------      GW >>>> ------------------------\n");
        for (i = 0; i < pkt_len; i++)
        {
            printf("%02x ", p_ap_event_pkt[i]);
        }
        printf("\n");
#endif
    } while (0);

}

void ble_esl_ap_cmd_handle(uint8_t command, uint8_t *p_parm, uint8_t parm_len)
{
    ap_cmd_error_t err_code;
    
    if ((command >= BLE_CONTROL_SCAN_CONTROL) && (command <= BLE_CONTROL_SET_PERIODIC_ADV_INFO_TRANSFER))
    {
        err_code = process_ble_control[command - BLE_CONTROL_SCAN_CONTROL](p_parm, parm_len);
    }
    else if ((command >= AP_CONTROL_ESL_AP_CONFIGURATION) && (command <= AP_CONTROL_OTA_CONTROL))
    {
        err_code = process_application_control[command - AP_CONTROL_ESL_AP_CONFIGURATION](p_parm, parm_len);
    }


}

uint8_t ble_esl_ap_cmd_proc(ble_tlv_t *p_ble_tlv)
{
    uint32_t param_len, i;
    uint32_t opcode, opcode_size;
    esl_ap_cmd_hdr *p_cmd_hdr;
    esl_ap_cmd_pd *p_pd;
#if 0
    printf("------------------------ >>>> GW      ------------------------\n");
    for (i = 0; i < p_ble_tlv->length; i++)
    {
        printf("%02x ", p_ble_tlv->value[i]);
    }
    printf("\n");
#endif
    p_cmd_hdr = (esl_ap_cmd_hdr *)p_ble_tlv->value;
    p_pd = (esl_ap_cmd_pd *)&p_cmd_hdr->parameter[0];

    param_len = p_cmd_hdr->len - sizeof(esl_ap_cmd_pd);

    ble_esl_ap_cmd_handle(p_pd->command_id, p_pd->parameter, param_len);

    return true;

}

uart_handler_data_sts_t ble_esl_ap_cmd_parser(uint8_t *p_buf, uint16_t len, uint16_t *p_data_len, uint16_t *p_offset)
{
    //+-------------+-----------+---------------+------------+---------------+--------+
    //|  Header(4)  | Length(1) | Command ID(4) | Address(2) |  Parameter(N) | CS (1) |
    //+-------------+-----------+---------------+------------+---------------+--------+
    //| FF FC FC FF |           |               |            |               |        |
    //+-------------+-----------+---------------+------------+---------------+--------+
    uart_handler_data_sts_t t_return = UART_DATA_INVALID;

    uint16_t i = 0;
    uint16_t idx = 0;
    uint8_t cs = 0;

    uint8_t find = 0;

    esl_ap_cmd_hdr *p_hdr = NULL;
    esl_ap_cmd_end *p_end = NULL;
    do
    {
        /* find tag */
        if (len < 4)
        {
            break;
        }

        for (i = 0; i < len; i++)
        {
            if ((p_buf[i] == 0xFF) && (p_buf[i + 1] == 0xFC) &&
                    (p_buf[i + 2] == 0xFC) && (p_buf[i + 3] == 0xFF))
            {
                if (p_offset)
                {
                    idx = i;
                }
                find = 1;
                break;
            }
        }
        
        if (!find)
        {
            break;
        }
        
        if ((len - idx) < (sizeof(esl_ap_cmd_hdr) + sizeof(esl_ap_cmd_end)))
        {
            break;
        }
        p_hdr = (esl_ap_cmd_hdr *)(p_buf + idx);

        if (len < (idx + sizeof(esl_ap_cmd_hdr) + p_hdr->len +  sizeof(esl_ap_cmd_end)))
        {
            break;
        }

        p_end = (esl_ap_cmd_end *)&p_buf[idx + sizeof(esl_ap_cmd_hdr) + p_hdr->len];
        cs = _ap_checksum_calc(&p_buf[idx + 4], (p_hdr->len + sizeof(p_hdr->len)));
        if (cs != p_end->cs)
        {
            t_return = UART_DATA_CS_ERROR;
            printf(">> %s: checksum error:0x%x, correct:0x%x\n", __FUNCTION__, p_end->cs, cs);
            break;
        }
        else if (cs == p_end->cs)   /* show receved command data */
        {
            *p_data_len = sizeof(esl_ap_cmd_hdr) + p_hdr->len + sizeof(esl_ap_cmd_end);
            *p_offset = idx;
            t_return = UART_DATA_VALID_CRC_OK;
            break;
        }
        *p_data_len = sizeof(esl_ap_cmd_hdr) + p_hdr->len + sizeof(esl_ap_cmd_end);
        *p_offset = idx;
        t_return = UART_DATA_VALID;

    } while (0);

    return t_return;
}
