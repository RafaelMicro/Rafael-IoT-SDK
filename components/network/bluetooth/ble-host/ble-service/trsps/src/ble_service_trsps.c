/************************************************************************
 *
 * File Name  : ble_service_trsps.c
 * Description: This file contains the definitions and functions of BLE TRSPS
 *
 *
 ************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_service_trsps.h"
#include "ble_profile.h"
#include "log.h"

/** ble_svcs_trsps_handler
 * @note This callback receives the TRSPS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_trsps_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * TRSPS UUID Definitions
 **************************************************************************/

/** TRSPS UUID.
 * @note 128-bits UUID
 * @note UUID: 00112233445566778899AABBCCDDEEFF
*/

const uint16_t attr_uuid_trsps_primary_service[] =
{
    0xEEFF, 0xCCDD,
    0xAABB, 0x8899,
    0x6677, 0x4455,
    0x2233, 0x0011,
};

/** TRSPS characteristic UDATR01 UUID.
 * @note 128-bits UUID
 * @note UUID: 101112131415161718191A1B1C1D1E1F
*/

const uint16_t attr_uuid_trsps_charc_udatr01[] =
{
    0x1E1F, 0x1C1D,
    0x1A1B, 0x1819,
    0x1617, 0x1415,
    0x1213, 0x1011,
};

/** TRSPS characteristic UDATNI01 UUID.
 * @note 128-bits UUID
 * @note UUID: 303132333435363738393A3B3C3D3E3F
*/

const uint16_t attr_uuid_trsps_charc_udatni01[] =
{
    0x3E3F, 0x3C3D,
    0x3A3B, 0x3839,
    0x3637, 0x3435,
    0x3233, 0x3031,
};

/** TRSPS characteristic UDATRW01 UUID.
 * @note 128-bits UUID
 * @note UUID: 505152535455565758595A5B5C5D5E5F
*/

const uint16_t attr_uuid_trsps_charc_udatrw01[] =
{
    0x5E5F, 0x5C5D,
    0x5A5B, 0x5859,
    0x5657, 0x5455,
    0x5253, 0x5051,
};


/**************************************************************************
 * TRSPS Service Value Definitions
 **************************************************************************/

/** TRSPS characteristic UDATR01 Value.
 * @note Return the "Read data" when client send "Read Request".
*/
#define ATTR_VALUE_TRSPS_UDATR01  "TRSPS UDATR01 Data"


/**************************************************************************
 * TRSPS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_trsps_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_trsps_primary_service,
    sizeof(attr_uuid_trsps_primary_service),
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

const ble_att_param_t att_trsps_characteristic_udatr01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatr01,
    sizeof(attr_uuid_trsps_charc_udatr01),
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

const ble_att_param_t att_trsps_udatr01 =
{
    (void *)attr_uuid_trsps_charc_udatr01,
    (void *)0,
    0,
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
        //ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_trsps_handler,       //registered callback function
};

const ble_att_param_t att_trsps_characteristic_udatni01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatni01,
    sizeof(attr_uuid_trsps_charc_udatni01),
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

const ble_att_param_t att_trsps_udatni01 =
{
    (void *)attr_uuid_trsps_charc_udatni01,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
        GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        //ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_trsps_handler,       //registered callback function
};

const ble_att_param_t att_trsps_udatni01_client_charc_configuration =
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
    ble_svcs_trsps_handler,       //registered callback function
};

const ble_att_param_t att_trsps_characteristic_udatrw01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatrw01,
    sizeof(attr_uuid_trsps_charc_udatrw01),
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

const ble_att_param_t att_trsps_udatrw01 =
{
    (void *)attr_uuid_trsps_charc_udatrw01,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
        //GATT_DECLARATIONS_PROPERTIES_NOTIFY |
        //GATT_DECLARATIONS_PROPERTIES_INDICATE |
        //GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES |
        //GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES |
        0x00
    ),
    (
        //ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_trsps_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << TRSPS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_TRSPS
// check MAX_NUM_CONN_TRSPS if defined or set to default 1.
#define MAX_NUM_CONN_TRSPS       1
#endif


// Service basic information
ble_svcs_common_info_t            trsps_basic_info[MAX_NUM_CONN_TRSPS];

// TRSPS information
ble_svcs_trsps_info_t             *trsps_info[MAX_NUM_CONN_TRSPS];

// TRSPS callback function
ble_svcs_evt_trsps_handler_t      trsps_callback[MAX_NUM_CONN_TRSPS];

// TRSPS registered total count
uint8_t                           trsps_count = 0;


/**************************************************************************
 * BLE Service << TRSPS >> Public Function
 **************************************************************************/
/* TRSPS Initialization */
ble_err_t ble_svcs_trsps_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_trsps_info_t *p_info, ble_svcs_evt_trsps_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "trsps_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_TRSPS, trsps_basic_info, &config_index, &trsps_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set TRSPS data
    trsps_info[config_index] = p_info;

    // Register TRSPS callback function
    trsps_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_trsps_handles_get(host_id, BLE_GATT_ROLE_SERVER, trsps_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get TRSPS Handle Numbers */
ble_err_t ble_svcs_trsps_handles_get(uint8_t host_id,  ble_gatt_role_t role, ble_svcs_trsps_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_trsps_primary_service;

        if (role == BLE_GATT_ROLE_SERVER)
        {
            ble_gatt_handle_table_param.p_handle_num_addr = (void *)&p_info->server_info.handles;
        }
        else if (role == BLE_GATT_ROLE_CLIENT)
        {
            ble_gatt_handle_table_param.p_handle_num_addr = (void *)&p_info->client_info.handles;
        }
        else
        {
            printf("Error role setting.\n");
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        status = ble_svcs_handles_mapping_get(&ble_gatt_handle_table_param);
    } while (0);

    return status;
}


/** Send Data to Server (Client ONLY)
*/
ble_err_t ble_svcs_trsps_client_send(uint8_t host_id, ble_gatt_write_t type, uint16_t handle_num, uint8_t *p_data, uint8_t length)
{
    ble_err_t status;
    ble_gatt_data_param_t p_param;

    p_param.host_id = host_id;
    p_param.handle_num = handle_num;
    p_param.length = length;
    p_param.p_data = p_data;

    if (type == BLEGATT_WRITE)
    {
        // write request
        status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &p_param);
    }
    else if (type == BLEGATT_WRITE_WITHOUT_RSP)
    {
        // write command
        status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_CMD, &p_param);
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    return status;
}


/** Get data from server by reading request (Client ONLY)
*/
ble_err_t ble_svcs_trsps_client_read(uint8_t host_id, uint16_t handle_num)
{
    ble_err_t status;
    ble_gatt_read_req_param_t *p_param;

    p_param = pvPortMalloc(sizeof(ble_gatt_read_req_param_t));
    if (p_param != NULL)
    {
        p_param->host_id = host_id;
        p_param->handle_num = handle_num;

        status = ble_cmd_gatt_read_req(p_param);
        if (status != BLE_ERR_OK) // send to BLE stack
        {
            log_info("<TYPE_BLE_GATT_READ_REQ> Send msg to BLE stack fail\n");
        }
        vPortFree(p_param);
    }
    else
    {
        log_info("<TYPE_BLE_GATT_READ_REQ> malloc fail\n");
        status = BLE_ERR_ALLOC_MEMORY_FAIL;
    }

    return status;
}


/** Send Data to Client (Server ONLY)
*/
ble_err_t ble_svcs_trsps_server_send(uint8_t host_id, ble_gatt_cccd_val_t cccd, uint16_t handle_num, uint8_t *p_data, uint8_t length)
{
    ble_err_t status;
    ble_gatt_data_param_t param;

    // set parameters
    param.host_id = host_id;
    param.handle_num = handle_num;
    param.length = length;
    param.p_data = p_data;

    if (cccd == BLEGATT_CCCD_INDICATION)
    {
        status = ble_svcs_data_send(TYPE_BLE_GATT_INDICATION, &param);
    }
    else if (cccd == BLEGATT_CCCD_NOTIFICATION)
    {
        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &param);
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    return status;
}


/**************************************************************************
 * BLE Service << TRSPS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void trsps_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_trsps_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle TRSPS client GATT event
static void handle_trsps_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == trsps_info[index]->client_info.handles.hdl_udatni01_cccd)
        {
            // received read response (cccd value) from server
            p_param->event = BLESERVICE_TRSPS_UDATNI01_CCCD_READ_RSP_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
        if (p_param->handle_num == trsps_info[index]->client_info.handles.hdl_udatni01_cccd)
        {
            // received write response from server -> cccd configure completed
            p_param->event = BLESERVICE_TRSPS_UDATNI01_CCCD_WRITE_RSP_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        else if (p_param->handle_num == trsps_info[index]->client_info.handles.hdl_udatrw01)
        {
            // received write response from server
            p_param->event = BLESERVICE_TRSPS_UDATRW01_WRITE_RSP_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == trsps_info[index]->client_info.handles.hdl_udatni01)
        {
            // received notification from server
            p_param->event = BLESERVICE_TRSPS_UDATNI01_NOTIFY_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == trsps_info[index]->client_info.handles.hdl_udatni01)
        {
            // received notification from server
            p_param->event = BLESERVICE_TRSPS_UDATNI01_INDICATE_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;

    default:
        break;
    }
}


// handle TRSPS server GATT event
static void handle_trsps_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatni01_cccd)
        {
            // received read or read by type request from client -> send read or read by type response
            ble_svcs_auto_handle_cccd_read_req(p_param, trsps_info[index]->server_info.data.udatni01_cccd);
        }
        else if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatr01)
        {
            // received read or read by type request from client -> send read or read by type rsp with data back to client
            ble_svcs_auto_handle_read_req(p_param, (uint8_t *)ATTR_VALUE_TRSPS_UDATR01, (sizeof(ATTR_VALUE_TRSPS_UDATR01) - 1));
        }
        else if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatrw01)
        {
            // received read or read by type request from client -> post to user to prepare read data back to client
            p_param->event = BLESERVICE_TRSPS_UDATRW01_READ_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatni01_cccd)
        {
            // received write request (cccd value) from client -> update server defined cccd value
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &trsps_info[index]->server_info.data.udatni01_cccd);
        }
        else if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatrw01)
        {
            // received write request from client -> post to user
            p_param->event = BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatrw01)
        {
            // received write without response from client
            p_param->event = BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == trsps_info[index]->server_info.handles.hdl_udatni01)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_TRSPS_UDATNI01_INDICATE_CONFIRM_EVENT;
            trsps_evt_post(p_param, &trsps_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_trsps_handler
 * @note This callback receives the TRSPS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_trsps_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_TRSPS, trsps_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle TRSPS client GATT event
        handle_trsps_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle TRSPS server GATT event
        handle_trsps_server(index, p_param);
    }
}
