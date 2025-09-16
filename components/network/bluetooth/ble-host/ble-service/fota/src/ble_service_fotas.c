/************************************************************************
 *
 * File Name  : ble_service_fotas.c
 * Description: This file contains the definitions and functions of BLE FOTAS
 *
 *
 ************************************************************************/
#include "ble_service_fotas.h"
#include "ble_profile.h"

/** ble_svcs_fotas_handler
 * @note This callback receives the FOTAS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_fotas_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * FOTAS UUID Definitions
 **************************************************************************/

/** FOTAS UUID.
 * @note 128-bits UUID
 * @note UUID: 09102132435465768798a9bacbdcedfe
*/

const uint16_t attr_uuid_fotas_primary_service[] =
{
    0xedfe, 0xcbdc,
    0xa9ba, 0x8798,
    0x6576, 0x4354,
    0x2132, 0x0910,
};

/** FOTAS characteristic DATA UUID.
 * @note 128-bits UUID
 * @note UUID: 01112131415161718191a1b1c1d1e1f1
*/

const uint16_t attr_uuid_fotas_charc_data[] =
{
    0xe1f1, 0xc1d1,
    0xa1b1, 0x8191,
    0x6171, 0x4151,
    0x2131, 0x0111,
};

/** FOTAS characteristic COMMAND UUID.
 * @note 128-bits UUID
 * @note UUID: 02122232425262728292a2b2c2d2e2f2
*/

const uint16_t attr_uuid_fotas_charc_command[] =
{
    0xe2f2, 0xc2d2,
    0xa2b2, 0x8292,
    0x6272, 0x4252,
    0x2232, 0x0212,
};


/**************************************************************************
 * FOTAS Service Value Definitions
 **************************************************************************/

/** DATA user description definition.
 * @note Return the "description value" when central send "Read Request".
*/
#define ATTR_VALUE_FOTAS_DATA_USER_DESCRIPTION  "FOTA_DATA"

/** COMMAND user description definition.
 * @note Return the "description value" when central send "Read Request".
*/
#define ATTR_VALUE_FOTAS_COMMAND_USER_DESCRIPTION  "FOTA_CMD"


/**************************************************************************
 * FOTAS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_fotas_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_fotas_primary_service,
    sizeof(attr_uuid_fotas_primary_service),
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

const ble_att_param_t att_fotas_characteristic_data =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_fotas_charc_data,
    sizeof(attr_uuid_fotas_charc_data),
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

const ble_att_param_t att_fotas_data =
{
    (void *)attr_uuid_fotas_charc_data,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
        GATT_DECLARATIONS_PROPERTIES_NOTIFY |
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
    ble_svcs_fotas_handler,       //registered callback function
};

const ble_att_param_t att_fotas_data_client_charc_configuration =
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
    ble_svcs_fotas_handler,       //registered callback function
};

const ble_att_param_t att_fotas_data_user_description =
{
    (void *)attr_uuid_type_charc_user_description,
    (void *)ATTR_VALUE_FOTAS_DATA_USER_DESCRIPTION,
    sizeof(ATTR_VALUE_FOTAS_DATA_USER_DESCRIPTION),
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
    ble_svcs_fotas_handler,       //registered callback function
};

const ble_att_param_t att_fotas_characteristic_command =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_fotas_charc_command,
    sizeof(attr_uuid_fotas_charc_command),
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

const ble_att_param_t att_fotas_command =
{
    (void *)attr_uuid_fotas_charc_command,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
        //GATT_DECLARATIONS_PROPERTIES_NOTIFY |
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
    ble_svcs_fotas_handler,       //registered callback function
};

const ble_att_param_t att_fotas_command_client_charc_configuration =
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
    ble_svcs_fotas_handler,       //registered callback function
};

const ble_att_param_t att_fotas_command_user_description =
{
    (void *)attr_uuid_type_charc_user_description,
    (void *)ATTR_VALUE_FOTAS_COMMAND_USER_DESCRIPTION,
    sizeof(ATTR_VALUE_FOTAS_COMMAND_USER_DESCRIPTION),
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
    ble_svcs_fotas_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << FOTAS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_FOTAS
// check MAX_NUM_CONN_FOTAS if defined or set to default 1.
#define MAX_NUM_CONN_FOTAS       1
#endif


// Service basic information
ble_svcs_common_info_t            fotas_basic_info[MAX_NUM_CONN_FOTAS];

// FOTAS information
ble_svcs_fotas_info_t             *fotas_info[MAX_NUM_CONN_FOTAS];

// FOTAS callback function
ble_svcs_evt_fotas_handler_t      fotas_callback[MAX_NUM_CONN_FOTAS];

// FOTAS registered total count
uint8_t                           fotas_count = 0;


/**************************************************************************
 * BLE Service << FOTAS >> Public Function
 **************************************************************************/
/* FOTAS Initialization */
ble_err_t ble_svcs_fotas_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_fotas_info_t *p_info, ble_svcs_evt_fotas_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "fotas_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_FOTAS, fotas_basic_info, &config_index, &fotas_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set FOTAS data
    fotas_info[config_index] = p_info;

    // Register FOTAS callback function
    fotas_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_fotas_handles_get(host_id, BLE_GATT_ROLE_SERVER, fotas_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get FOTAS Handle Numbers */
ble_err_t ble_svcs_fotas_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_fotas_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_fotas_primary_service;

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

/**************************************************************************
 * BLE Service << FOTAS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void fotas_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_fotas_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle FOTAS client GATT event
static void handle_fotas_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_data_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_FOTAS_DATA_CCCD_READ_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_data_user_description)
        {
            // received user description read response from server
            p_param->event = BLESERVICE_FOTAS_DATA_DESCRIPTION_READ_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command)
        {
            // received read response from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_READ_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_CCCD_READ_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command_user_description)
        {
            // received user description read response from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_DESCRIPTION_READ_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_data_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_FOTAS_DATA_CCCD_WRITE_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command)
        {
            // received write response from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_WRITE_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_CCCD_WRITE_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_data)
        {
            // received notify from server
            p_param->event = BLESERVICE_FOTAS_DATA_NOTIFY_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == fotas_info[index]->client_info.handles.hdl_command)
        {
            // received indicate from server
            p_param->event = BLESERVICE_FOTAS_COMMAND_INDICATE_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;

    default:
        break;
    }
}


// handle FOTAS server GATT event
static void handle_fotas_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_data_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, fotas_info[index]->server_info.data.data_cccd);
        }
        else if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_command)
        {
            // received read from client
            p_param->event = BLESERVICE_FOTAS_COMMAND_READ_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_command_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, fotas_info[index]->server_info.data.command_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_data_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &fotas_info[index]->server_info.data.data_cccd);
        }
        else if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_command)
        {
            // received write from client
            p_param->event = BLESERVICE_FOTAS_COMMAND_WRITE_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        else if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_command_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &fotas_info[index]->server_info.data.command_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_data)
        {
            // received write without response from client
            p_param->event = BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == fotas_info[index]->server_info.handles.hdl_command)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_FOTAS_COMMAND_INDICATE_CONFIRM_EVENT;
            fotas_evt_post(p_param, &fotas_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_fotas_handler
 * @note This callback receives the FOTAS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_fotas_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_FOTAS, fotas_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle FOTAS client GATT event
        handle_fotas_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle FOTAS server GATT event
        handle_fotas_server(index, p_param);
    }
}
