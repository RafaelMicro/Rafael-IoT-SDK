/************************************************************************
 *
 * File Name  : ble_service_matter.c
 * Description: This file contains the definitions and functions of BLE MATTER
 *
 *
 ************************************************************************/
#include "ble_service_matter.h"
#include "ble_profile.h"

/** ble_svcs_matter_handler
 * @note This callback receives the MATTER events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_matter_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * MATTER UUID Definitions
 **************************************************************************/

/** MATTER UUID.
 * @note 16-bits UUID
 * @note UUID: FFF6
*/

const uint16_t attr_uuid_matter_primary_service[] =
{
    0xFFF6,
};

/** MATTER characteristic CLIENT_TX_BUFFER UUID.
 * @note 128-bits UUID
 * @note UUID: 18EE2EF5263D4559959F4F9C429F9D11
*/

const uint16_t attr_uuid_matter_charc_client_tx_buffer[] =
{
    0x9D11, 0x429F,
    0x4F9C, 0x959F,
    0x4559, 0x263D,
    0x2EF5, 0x18EE,
};

/** MATTER characteristic CLIENT_RX_BUFFER UUID.
 * @note 128-bits UUID
 * @note UUID: 18EE2EF5263D4559959F4F9C429F9D12
*/

const uint16_t attr_uuid_matter_charc_client_rx_buffer[] =
{
    0x9D12, 0x429F,
    0x4F9C, 0x959F,
    0x4559, 0x263D,
    0x2EF5, 0x18EE,
};

/** MATTER characteristic ADDITIONAL_COMMISSIONING_RELATED_DATA UUID.
 * @note 128-bits UUID
 * @note UUID: 64630238877245F2B87D748A83218F04
*/

const uint16_t attr_uuid_matter_charc_additional_commissioning_related_data[] =
{
    0x8F04, 0x8321,
    0x748A, 0xB87D,
    0x45F2, 0x8772,
    0x0238, 0x6463,
};


/**************************************************************************
 * MATTER Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * MATTER Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_matter_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_matter_primary_service,
    sizeof(attr_uuid_matter_primary_service),
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

const ble_att_param_t att_matter_characteristic_client_tx_buffer =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_matter_charc_client_tx_buffer,
    sizeof(attr_uuid_matter_charc_client_tx_buffer),
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

const ble_att_param_t att_matter_client_tx_buffer =
{
    (void *)attr_uuid_matter_charc_client_tx_buffer,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
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
    ble_svcs_matter_handler,       //registered callback function
};

const ble_att_param_t att_matter_characteristic_client_rx_buffer =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_matter_charc_client_rx_buffer,
    sizeof(attr_uuid_matter_charc_client_rx_buffer),
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

const ble_att_param_t att_matter_client_rx_buffer =
{
    (void *)attr_uuid_matter_charc_client_rx_buffer,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        //GATT_DECLARATIONS_PROPERTIES_WRITE |
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
    ble_svcs_matter_handler,       //registered callback function
};

const ble_att_param_t att_matter_client_rx_buffer_client_charc_configuration =
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
    ble_svcs_matter_handler,       //registered callback function
};

const ble_att_param_t att_matter_characteristic_additional_commissioning_related_data =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_matter_charc_additional_commissioning_related_data,
    sizeof(attr_uuid_matter_charc_additional_commissioning_related_data),
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

const ble_att_param_t att_matter_additional_commissioning_related_data =
{
    (void *)attr_uuid_matter_charc_additional_commissioning_related_data,
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
    ble_svcs_matter_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << MATTER >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_MATTER
    // check MAX_NUM_CONN_MATTER if defined or set to default 1.
    #define MAX_NUM_CONN_MATTER       1
#endif


// Service basic information
ble_svcs_common_info_t             matter_basic_info[MAX_NUM_CONN_MATTER];

// MATTER information
ble_svcs_matter_info_t             *matter_info[MAX_NUM_CONN_MATTER];

// MATTER callback function
ble_svcs_evt_matter_handler_t      matter_callback[MAX_NUM_CONN_MATTER];

// MATTER registered total count
uint8_t                            matter_count = 0;


/**************************************************************************
 * BLE Service << MATTER >> Public Function
 **************************************************************************/
/* MATTER Initialization */
ble_err_t ble_svcs_matter_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_matter_info_t *p_info, ble_svcs_evt_matter_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "matter_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_MATTER, matter_basic_info, &config_index, &matter_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set MATTER data
    matter_info[config_index] = p_info;

    // Register MATTER callback function
    matter_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_matter_handles_get(host_id, BLE_GATT_ROLE_SERVER, matter_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get MATTER Handle Numbers */
ble_err_t ble_svcs_matter_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_matter_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_matter_primary_service;

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
 * BLE Service << MATTER >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void matter_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_matter_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle MATTER client GATT event
static void handle_matter_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == matter_info[index]->client_info.handles.hdl_client_rx_buffer_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_MATTER_CLIENT_RX_BUFFER_CCCD_READ_RSP_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        else if (p_param->handle_num == matter_info[index]->client_info.handles.hdl_additional_commissioning_related_data)
        {
            // received read response from server
            p_param->event = BLESERVICE_MATTER_ADDITIONAL_COMMISSIONING_RELATED_DATA_READ_RSP_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == matter_info[index]->client_info.handles.hdl_client_tx_buffer)
        {
            // received write response from server
            p_param->event = BLESERVICE_MATTER_CLIENT_TX_BUFFER_WRITE_RSP_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        else if (p_param->handle_num == matter_info[index]->client_info.handles.hdl_client_rx_buffer_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_MATTER_CLIENT_RX_BUFFER_CCCD_WRITE_RSP_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == matter_info[index]->client_info.handles.hdl_client_rx_buffer)
        {
            // received indicate from server
            p_param->event = BLESERVICE_MATTER_CLIENT_RX_BUFFER_INDICATE_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;

    default:
        break;
    }
}


// handle MATTER server GATT event
static void handle_matter_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == matter_info[index]->server_info.handles.hdl_client_rx_buffer_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, matter_info[index]->server_info.data.client_rx_buffer_cccd);
        }
        else if (p_param->handle_num == matter_info[index]->server_info.handles.hdl_additional_commissioning_related_data)
        {
            // received read from client
            p_param->event = BLESERVICE_MATTER_ADDITIONAL_COMMISSIONING_RELATED_DATA_READ_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == matter_info[index]->server_info.handles.hdl_client_tx_buffer)
        {
            // received write from client
            p_param->event = BLESERVICE_MATTER_CLIENT_TX_BUFFER_WRITE_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        else if (p_param->handle_num == matter_info[index]->server_info.handles.hdl_client_rx_buffer_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &matter_info[index]->server_info.data.client_rx_buffer_cccd);
            p_param->event = BLESERVICE_MATTER_CLIENT_RX_BUFFER_CCCD_WRITE_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == matter_info[index]->server_info.handles.hdl_client_rx_buffer)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_MATTER_CLIENT_RX_BUFFER_INDICATE_CONFIRM_EVENT;
            matter_evt_post(p_param, &matter_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_matter_handler
 * @note This callback receives the MATTER events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_matter_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_MATTER, matter_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle MATTER client GATT event
        handle_matter_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle MATTER server GATT event
        handle_matter_server(index, p_param);
    }
}
