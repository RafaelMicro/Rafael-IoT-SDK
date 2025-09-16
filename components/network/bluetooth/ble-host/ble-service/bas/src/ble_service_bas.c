/************************************************************************
 *
 * File Name  : ble_service_bas.c
 * Description: This file contains the definitions and functions of BLE BAS
 *
 *
 ************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_service_bas.h"
#include "ble_profile.h"
#include "log.h"

/** ble_svcs_bas_handler
 * @note This callback receives the BAS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_bas_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * BAS UUID Definitions
 **************************************************************************/

/** BAS UUID.
 * @note 16-bits UUID
 * @note UUID: 180F
*/

const uint16_t attr_uuid_bas_primary_service[] =
{
    0x180F,
};

/** BAS characteristic BATTERY_LEVEL UUID.
 * @note 16-bits UUID
 * @note UUID: 2A19
*/

const uint16_t attr_uuid_bas_charc_battery_level[] =
{
    0x2A19,
};


/**************************************************************************
 * BAS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * BAS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_bas_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_bas_primary_service,
    sizeof(attr_uuid_bas_primary_service),
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
        //GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        //GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    attr_null_access,                       //This function should be set to attr_null_access when att_len or p_uuid_value is a null value.
};

const ble_att_param_t att_bas_characteristic_battery_level =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_bas_charc_battery_level,
    sizeof(attr_uuid_bas_charc_battery_level),
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
        //GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        //GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    attr_null_access,                       //This function should be set to attr_null_access when att_len or p_uuid_value is a null value.
};

const ble_att_param_t att_bas_battery_level =
{
    (void *)attr_uuid_bas_charc_battery_level,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
        GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        //GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_bas_handler,       //registered callback function
};

const ble_att_param_t att_bas_battery_level_client_charc_configuration =
{
    (void *)attr_uuid_type_client_charc_configuration,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
        //GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        //GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_bas_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << BAS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_BAS
// check MAX_NUM_CONN_BAS if defined or set to default 1.
#define MAX_NUM_CONN_BAS       1
#endif


// Service basic information
ble_svcs_common_info_t          bas_basic_info[MAX_NUM_CONN_BAS];

// BAS information
ble_svcs_bas_info_t             *bas_info[MAX_NUM_CONN_BAS];

// BAS callback function
ble_svcs_evt_bas_handler_t      bas_callback[MAX_NUM_CONN_BAS];

// BAS registered total count
uint8_t                         bas_count = 0;


/**************************************************************************
 * BLE Service << BAS >> Public Function
 **************************************************************************/
/* BAS Initialization */
ble_err_t ble_svcs_bas_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bas_info_t *p_info, ble_svcs_evt_bas_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "bas_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_BAS, bas_basic_info, &config_index, &bas_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set BAS data
    bas_info[config_index] = p_info;

    // Register BAS callback function
    bas_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if (role == BLE_GATT_ROLE_SERVER)
    {
        status = ble_svcs_bas_handles_get(host_id, bas_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get BAS Handle Numbers */
ble_err_t ble_svcs_bas_handles_get(uint8_t host_id, ble_svcs_bas_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t *ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    ble_gatt_handle_table_param = pvPortMalloc(sizeof(ble_gatt_handle_table_param_t));
    if (ble_gatt_handle_table_param != NULL)
    {
        ble_gatt_handle_table_param->host_id = host_id;
        ble_gatt_handle_table_param->gatt_role = p_info->role;
        ble_gatt_handle_table_param->p_element = (ble_att_param_t *)&att_bas_primary_service;
        ble_gatt_handle_table_param->p_handle_num_addr = (void *)&p_info->handles;

        status = ble_cmd_gatt_att_handle_mapping_get(ble_gatt_handle_table_param);
        if (status != BLE_ERR_OK) // send to BLE stack
        {
            log_info("<TYPE_BLE_GATT_ATT_HANDLE_MAPPING_GET> Send msg to BLE stack fail\n");
        }
        vPortFree(ble_gatt_handle_table_param);
    }
    else
    {
        log_info("<TYPE_BLE_GATT_ATT_HANDLE_MAPPING_GET> malloc fail\n");
        status = BLE_ERR_ALLOC_MEMORY_FAIL;
    }

    return status;
}

/**************************************************************************
 * BLE Service << BAS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void bas_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_bas_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle BAS client GATT event
static void handle_bas_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level)
        {
            // received read response from server
            p_param->event = BLESERVICE_BAS_BATTERY_LEVEL_READ_RSP_EVENT;
            bas_evt_post(p_param, &bas_callback[index]);
        }
        else if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_BAS_BATTERY_LEVEL_CCCD_READ_RSP_EVENT;
            bas_evt_post(p_param, &bas_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_BAS_BATTERY_LEVEL_CCCD_WRITE_RSP_EVENT;
            bas_evt_post(p_param, &bas_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level)
        {
            // received notify from server
            p_param->event = BLESERVICE_BAS_BATTERY_LEVEL_NOTIFY_EVENT;
            bas_evt_post(p_param, &bas_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        break;

    default:
        break;
    }
}


// handle BAS server GATT event
static void handle_bas_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level)
        {
            // received read from client
            p_param->event = BLESERVICE_BAS_BATTERY_LEVEL_READ_EVENT;
            bas_evt_post(p_param, &bas_callback[index]);
        }
        else if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, bas_info[index]->data.battery_level_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == bas_info[index]->handles.hdl_battery_level_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &bas_info[index]->data.battery_level_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        break;

    default:
        break;
    }
}



/** ble_svcs_bas_handler
 * @note This callback receives the BAS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_bas_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_BAS, bas_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle BAS client GATT event
        handle_bas_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle BAS server GATT event
        handle_bas_server(index, p_param);
    }
}
