/************************************************************************
 *
 * File Name  : ble_service_bls.c
 * Description: This file contains the definitions and functions of BLE BLS
 *
 *
 ************************************************************************/
#include "ble_service_bls.h"
#include "ble_profile.h"

/** ble_svcs_bls_handler
 * @note This callback receives the BLS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_bls_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * BLS UUID Definitions
 **************************************************************************/

/** BLS UUID.
 * @note 16-bits UUID
 * @note UUID: 1810
*/

const uint16_t attr_uuid_bls_primary_service[] =
{
    0x1810,
};

/** BLS characteristic BLOOD_PRESSURE_MEASUREMENT UUID.
 * @note 16-bits UUID
 * @note UUID: 2A35
*/

const uint16_t attr_uuid_bls_charc_blood_pressure_measurement[] =
{
    0x2A35,
};

/** BLS characteristic INTERMEDIATE_CUFF_PRESSURE UUID.
 * @note 16-bits UUID
 * @note UUID: 2A36
*/

const uint16_t attr_uuid_bls_charc_intermediate_cuff_pressure[] =
{
    0x2A36,
};

/** BLS characteristic BLOOD_PRESSURE_FEATURE UUID.
 * @note 16-bits UUID
 * @note UUID: 2A49
*/

const uint16_t attr_uuid_bls_charc_blood_pressure_feature[] =
{
    0x2A49,
};


/**************************************************************************
 * BLS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * BLS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_bls_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_bls_primary_service,
    sizeof(attr_uuid_bls_primary_service),
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

const ble_att_param_t att_bls_characteristic_blood_pressure_measurement =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_bls_charc_blood_pressure_measurement,
    sizeof(attr_uuid_bls_charc_blood_pressure_measurement),
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

const ble_att_param_t att_bls_blood_pressure_measurement =
{
    (void *)attr_uuid_bls_charc_blood_pressure_measurement,
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
    ble_svcs_bls_handler,       //registered callback function
};

const ble_att_param_t att_bls_blood_pressure_measurement_client_charc_configuration =
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
        ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_bls_handler,       //registered callback function
};

const ble_att_param_t att_bls_characteristic_intermediate_cuff_pressure =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_bls_charc_intermediate_cuff_pressure,
    sizeof(attr_uuid_bls_charc_intermediate_cuff_pressure),
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

const ble_att_param_t att_bls_intermediate_cuff_pressure =
{
    (void *)attr_uuid_bls_charc_intermediate_cuff_pressure,
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
    ble_svcs_bls_handler,       //registered callback function
};

const ble_att_param_t att_bls_intermediate_cuff_pressure_client_charc_configuration =
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
        ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_bls_handler,       //registered callback function
};

const ble_att_param_t att_bls_characteristic_blood_pressure_feature =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_bls_charc_blood_pressure_feature,
    sizeof(attr_uuid_bls_charc_blood_pressure_feature),
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

const ble_att_param_t att_bls_blood_pressure_feature =
{
    (void *)attr_uuid_bls_charc_blood_pressure_feature,
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
        ATT_PERMISSION_ENC_READ |
        //ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_bls_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << BLS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_BLS
// check MAX_NUM_CONN_BLS if defined or set to default 1.
#define MAX_NUM_CONN_BLS       1
#endif


// Service basic information
ble_svcs_common_info_t          bls_basic_info[MAX_NUM_CONN_BLS];

// BLS information
ble_svcs_bls_info_t             *bls_info[MAX_NUM_CONN_BLS];

// BLS callback function
ble_svcs_evt_bls_handler_t      bls_callback[MAX_NUM_CONN_BLS];

// BLS registered total count
uint8_t                         bls_count = 0;


/**************************************************************************
 * BLE Service << BLS >> Public Function
 **************************************************************************/
/* BLS Initialization */
ble_err_t ble_svcs_bls_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bls_info_t *p_info, ble_svcs_evt_bls_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "bls_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_BLS, bls_basic_info, &config_index, &bls_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set BLS data
    bls_info[config_index] = p_info;

    // Register BLS callback function
    bls_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_bls_handles_get(host_id, BLE_GATT_ROLE_SERVER, bls_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get BLS Handle Numbers */
ble_err_t ble_svcs_bls_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bls_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_bls_primary_service;

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
 * BLE Service << BLS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void bls_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_bls_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle BLS client GATT event
static void handle_bls_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_blood_pressure_measurement_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_READ_RSP_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        else if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_intermediate_cuff_pressure_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_READ_RSP_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        else if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_blood_pressure_feature)
        {
            // received read response from server
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_FEATURE_READ_RSP_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_blood_pressure_measurement_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_WRITE_RSP_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        else if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_intermediate_cuff_pressure_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_WRITE_RSP_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_intermediate_cuff_pressure)
        {
            // received notify from server
            p_param->event = BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_NOTIFY_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == bls_info[index]->client_info.handles.hdl_blood_pressure_measurement)
        {
            // received indicate from server
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_INDICATE_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;

    default:
        break;
    }
}


// handle BLS server GATT event
static void handle_bls_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_blood_pressure_measurement_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, bls_info[index]->server_info.data.blood_pressure_measurement_cccd);
        }
        else if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_intermediate_cuff_pressure_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, bls_info[index]->server_info.data.intermediate_cuff_pressure_cccd);
        }
        else if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_blood_pressure_feature)
        {
            // received read from client
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_FEATURE_READ_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_blood_pressure_measurement_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &bls_info[index]->server_info.data.blood_pressure_measurement_cccd);
        }
        else if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_intermediate_cuff_pressure_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &bls_info[index]->server_info.data.intermediate_cuff_pressure_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == bls_info[index]->server_info.handles.hdl_blood_pressure_measurement)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_INDICATE_CONFIRM_EVENT;
            bls_evt_post(p_param, &bls_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_bls_handler
 * @note This callback receives the BLS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_bls_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_BLS, bls_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle BLS client GATT event
        handle_bls_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle BLS server GATT event
        handle_bls_server(index, p_param);
    }
}
