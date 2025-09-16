/************************************************************************
 *
 * File Name  : ble_service_hrs.c
 * Description: This file contains the definitions and functions of BLE HRS
 *
 *
 ************************************************************************/
#include "ble_service_hrs.h"
#include "ble_profile.h"

/** ble_svcs_hrs_handler
 * @note This callback receives the HRS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_hrs_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * HRS UUID Definitions
 **************************************************************************/

/** HRS UUID.
 * @note 16-bits UUID
 * @note UUID: 180D
*/

const uint16_t attr_uuid_hrs_primary_service[] =
{
    0x180D,
};

/** HRS characteristic HEART_RATE_MEASUREMENT UUID.
 * @note 16-bits UUID
 * @note UUID: 2A37
*/

const uint16_t attr_uuid_hrs_charc_heart_rate_measurement[] =
{
    0x2A37,
};

/** HRS characteristic BODY_SENSOR_LOCATION UUID.
 * @note 16-bits UUID
 * @note UUID: 2A38
*/

const uint16_t attr_uuid_hrs_charc_body_sensor_location[] =
{
    0x2A38,
};


/**************************************************************************
 * HRS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * HRS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_hrs_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_hrs_primary_service,
    sizeof(attr_uuid_hrs_primary_service),
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

const ble_att_param_t att_hrs_characteristic_heart_rate_measurement =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_hrs_charc_heart_rate_measurement,
    sizeof(attr_uuid_hrs_charc_heart_rate_measurement),
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

const ble_att_param_t att_hrs_heart_rate_measurement =
{
    (void *)attr_uuid_hrs_charc_heart_rate_measurement,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
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
    ble_svcs_hrs_handler,       //registered callback function
};

const ble_att_param_t att_hrs_heart_rate_measurement_client_charc_configuration =
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
    ble_svcs_hrs_handler,       //registered callback function
};

const ble_att_param_t att_hrs_characteristic_body_sensor_location =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_hrs_charc_body_sensor_location,
    sizeof(attr_uuid_hrs_charc_body_sensor_location),
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

const ble_att_param_t att_hrs_body_sensor_location =
{
    (void *)attr_uuid_hrs_charc_body_sensor_location,
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
    ble_svcs_hrs_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << HRS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_HRS
// check MAX_NUM_CONN_HRS if defined or set to default 1.
#define MAX_NUM_CONN_HRS       1
#endif


// Service basic information
ble_svcs_common_info_t          hrs_basic_info[MAX_NUM_CONN_HRS];

// HRS information
ble_svcs_hrs_info_t             *hrs_info[MAX_NUM_CONN_HRS];

// HRS callback function
ble_svcs_evt_hrs_handler_t      hrs_callback[MAX_NUM_CONN_HRS];

// HRS registered total count
uint8_t                         hrs_count = 0;


/**************************************************************************
 * BLE Service << HRS >> Public Function
 **************************************************************************/
/* HRS Initialization */
ble_err_t ble_svcs_hrs_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_hrs_info_t *p_info, ble_svcs_evt_hrs_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "hrs_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_HRS, hrs_basic_info, &config_index, &hrs_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set HRS data
    hrs_info[config_index] = p_info;

    // Register HRS callback function
    hrs_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_hrs_handles_get(host_id, BLE_GATT_ROLE_SERVER, hrs_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get HRS Handle Numbers */
ble_err_t ble_svcs_hrs_handles_get(uint8_t host_id,  ble_gatt_role_t role, ble_svcs_hrs_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_hrs_primary_service;

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
 * BLE Service << HRS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void hrs_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_hrs_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle HRS client GATT event
static void handle_hrs_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == hrs_info[index]->client_info.handles.hdl_heart_rate_measurement_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_READ_RSP_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
        }
        else if (p_param->handle_num == hrs_info[index]->client_info.handles.hdl_body_sensor_location)
        {
            // received read response from server
            p_param->event = BLESERVICE_HRS_BODY_SENSOR_LOCATION_READ_RSP_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == hrs_info[index]->client_info.handles.hdl_heart_rate_measurement_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_WRITE_RSP_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == hrs_info[index]->client_info.handles.hdl_heart_rate_measurement)
        {
            // received notify from server
            p_param->event = BLESERVICE_HRS_HEART_RATE_MEASUREMENT_NOTIFY_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        break;

    default:
        break;
    }
}


// handle HRS server GATT event
static void handle_hrs_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == hrs_info[index]->server_info.handles.hdl_heart_rate_measurement_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, hrs_info[index]->server_info.data.heart_rate_measurement_cccd);
        }
        else if (p_param->handle_num == hrs_info[index]->server_info.handles.hdl_body_sensor_location)
        {
            // received read from client
            p_param->event = BLESERVICE_HRS_BODY_SENSOR_LOCATION_READ_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == hrs_info[index]->server_info.handles.hdl_heart_rate_measurement_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &hrs_info[index]->server_info.data.heart_rate_measurement_cccd);

            p_param->event = BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_WRITE_EVENT;
            hrs_evt_post(p_param, &hrs_callback[index]);
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



/** ble_svcs_hrs_handler
 * @note This callback receives the HRS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_hrs_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_HRS, hrs_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle HRS client GATT event
        handle_hrs_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle HRS server GATT event
        handle_hrs_server(index, p_param);
    }
}
