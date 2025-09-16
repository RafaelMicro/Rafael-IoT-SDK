/************************************************************************
 *
 * File Name  : ble_service_gaps.c
 * Description: This file contains the definitions and functions of BLE GAPS
 *
 *
 ************************************************************************/
#include "ble_service_gaps.h"
#include "ble_profile.h"

/** ble_svcs_gaps_handler
 * @note This callback receives the GAPS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_gaps_handler(ble_evt_att_param_t *p_param);

/**************************************************************************
 * GAPS(Server) Service Value Definitions
 **************************************************************************/
/** GAPS characteristic value.
 * @note Return the "Read data" when client send "Read Request".
*/
#define GAPS_DEVICE_NAME_DEFAULT             "BLE_DEMO"
#define GAPS_DEVICE_NAME_DEFAULT_LEN         (SIZE_STRING(GAPS_DEVICE_NAME_DEFAULT))

// Set GAPS Default Data.
ble_svcs_gaps_data_t svcs_gap_data =
{
    GAPS_DEVICE_NAME_DEFAULT,         // GAPS device name
    GAPS_DEVICE_NAME_DEFAULT_LEN,     // GAPS device name length
    BLE_APPEARANCE_UNKNOWN,           // GAPS appearance
    {
        0x0006,                       // minimum connection interval
        0x0008,                       // maximum connection interval
        0x0000,                       // peripheral latency
        0x0258,                       // Connection supervision timeout
    },
};


/**************************************************************************
 * GAPS UUID Definitions
 **************************************************************************/

/** GAPS UUID.
 * @note 16-bits UUID
 * @note UUID: 1800
*/

const uint16_t attr_uuid_gaps_primary_service[] =
{
    GATT_SERVICES_GENERIC_ACCESS,
};

/** GAPS characteristic DEVICE_NAME UUID.
 * @note 16-bits UUID
 * @note UUID: 2A00
*/

const uint16_t attr_uuid_gaps_charc_device_name[] =
{
    GATT_SPEC_CHARC_DEVICE_NAME,
};

/** GAPS characteristic APPEARANCE UUID.
 * @note 16-bits UUID
 * @note UUID: 2A01
*/

const uint16_t attr_uuid_gaps_charc_appearance[] =
{
    GATT_SPEC_CHARC_APPEARANCE,
};

/** GAPS characteristic PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS UUID.
 * @note 16-bits UUID
 * @note UUID: 2A04
*/

const uint16_t attr_uuid_gaps_charc_peripheral_preferred_connection_parameters[] =
{
    GATT_SPEC_CHARC_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,
};


/**************************************************************************
 * GAPS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * GAPS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_gaps_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_gaps_primary_service,
    sizeof(attr_uuid_gaps_primary_service),
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

const ble_att_param_t att_gaps_characteristic_device_name =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_gaps_charc_device_name,
    sizeof(attr_uuid_gaps_charc_device_name),
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

const ble_att_param_t att_gaps_device_name =
{
    (void *)attr_uuid_gaps_charc_device_name,
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
    ble_svcs_gaps_handler,       //registered callback function
};

const ble_att_param_t att_gaps_characteristic_appearance =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_gaps_charc_appearance,
    sizeof(attr_uuid_gaps_charc_appearance),
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

const ble_att_param_t att_gaps_appearance =
{
    (void *)attr_uuid_gaps_charc_appearance,
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
    ble_svcs_gaps_handler,       //registered callback function
};

const ble_att_param_t att_gaps_characteristic_peripheral_preferred_connection_parameters =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_gaps_charc_peripheral_preferred_connection_parameters,
    sizeof(attr_uuid_gaps_charc_peripheral_preferred_connection_parameters),
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

const ble_att_param_t att_gaps_peripheral_preferred_connection_parameters =
{
    (void *)attr_uuid_gaps_charc_peripheral_preferred_connection_parameters,
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
    ble_svcs_gaps_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << GAPS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_GAPS
// check MAX_NUM_CONN_GAPS if defined or set to default 1.
#define MAX_NUM_CONN_GAPS       1
#endif


// Length of appearance for decoded data to send read response to client.
#define BLE_GAPS_APPEARANCE_LEN            2

// Length of preferred peripheral connection parameters for decoded data to send read response to client.
#define BLE_GAPS_PREFERRED_CONNPARAM_LEN   8


// Service basic information
ble_svcs_common_info_t           gaps_basic_info[MAX_NUM_CONN_GAPS];

// GAPS information
ble_svcs_gaps_info_t             *gaps_info[MAX_NUM_CONN_GAPS];

// GAPS callback function
ble_svcs_evt_gaps_handler_t      gaps_callback[MAX_NUM_CONN_GAPS];

// GAPS registered total count
uint8_t                          gaps_count = 0;

// GAPS decoded buffer
uint8_t                          gaps_decoded_buffer[8];

/**************************************************************************
 * BLE Service << GAPS >> Public Function
 **************************************************************************/

/** Set GAPS Device Name
*/
ble_err_t ble_svcs_gaps_device_name_set(uint8_t *p_data, uint8_t length)
{
    uint8_t i;

    if ((length > GAPS_DEVICE_NAME_LENGH) || (length > GAPS_DEVICE_NAME_LENGH_MAX))
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    for (i = 0; i < length; i++)
    {
        svcs_gap_data.device_name[i] = p_data[i];
    }
    svcs_gap_data.device_name_len = length;

    return BLE_ERR_OK;
}


/** Set GAPS Appearance
*/
void ble_svcs_gaps_appearance_set(uint16_t appearance)
{
    svcs_gap_data.appearance = appearance;
}


/** Set GAPS Peripheral Preferred Connection Parameters
*/
ble_err_t ble_svcs_gaps_peripheral_conn_param_set(ble_gap_conn_param_t *p_param)
{
    if ( (p_param->min_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->min_conn_interval > BLE_CONN_INTERVAL_MAX) ||
            (p_param->max_conn_interval < BLE_CONN_INTERVAL_MIN || p_param->max_conn_interval > BLE_CONN_INTERVAL_MAX) ||
            (p_param->min_conn_interval > p_param->max_conn_interval) ||
            (p_param->periph_latency > BLE_CONN_LATENCY_MAX) ||
            (p_param->supv_timeout < BLE_CONN_SUPV_TIMEOUT_MIN || p_param->supv_timeout > BLE_CONN_SUPV_TIMEOUT_MAX) ||
            ((p_param->supv_timeout * 4) <= ((1 + p_param->periph_latency) * p_param->max_conn_interval )))
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    svcs_gap_data.periph_conn_param = *p_param;

    return BLE_ERR_OK;
}


/** GAPS Initialization
*/
ble_err_t ble_svcs_gaps_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_gaps_info_t *p_info, ble_svcs_evt_gaps_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "gaps_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_GAPS, gaps_basic_info, &config_index, &gaps_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set GAPS data
    gaps_info[config_index] = p_info;

    // Register GAPS callback function
    gaps_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_gaps_handles_get(host_id, BLE_GATT_ROLE_SERVER, gaps_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/** Get GAPS Handle Numbers
*/
ble_err_t ble_svcs_gaps_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_gaps_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_gaps_primary_service;

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
 * BLE Service << GAPS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void gaps_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_gaps_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle GAPS client GATT event
static void handle_gaps_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == gaps_info[index]->client_info.handles.hdl_device_name)
        {
            // received read response from server
            p_param->event = BLESERVICE_GAPS_DEVICE_NAME_READ_RSP_EVENT;
            gaps_evt_post(p_param, &gaps_callback[index]);
        }
        else if (p_param->handle_num == gaps_info[index]->client_info.handles.hdl_appearance)
        {
            // received read response from server
            p_param->event = BLESERVICE_GAPS_APPEARANCE_READ_RSP_EVENT;
            gaps_evt_post(p_param, &gaps_callback[index]);
        }
        else if (p_param->handle_num == gaps_info[index]->client_info.handles.hdl_peripheral_preferred_connection_parameters)
        {
            // received read response from server
            p_param->event = BLESERVICE_GAPS_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_READ_RSP_EVENT;
            gaps_evt_post(p_param, &gaps_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        break;

    default:
        break;
    }
}


// handle GAPS server GATT event
static void handle_gaps_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == gaps_info[index]->server_info.handles.hdl_device_name)
        {
            ble_svcs_auto_handle_read_req(p_param, svcs_gap_data.device_name, svcs_gap_data.device_name_len);
        }
        else if (p_param->handle_num == gaps_info[index]->server_info.handles.hdl_appearance)
        {
            // decoded uint16_t to uint8_t array
            gaps_decoded_buffer[0] = (uint8_t)(svcs_gap_data.appearance & 0xFF);
            gaps_decoded_buffer[1] = (uint8_t)((svcs_gap_data.appearance >> 8) & 0xFF);
            ble_svcs_auto_handle_read_req(p_param, (uint8_t *)gaps_decoded_buffer, BLE_GAPS_APPEARANCE_LEN);
        }
        else if (p_param->handle_num == gaps_info[index]->server_info.handles.hdl_peripheral_preferred_connection_parameters)
        {
            gaps_decoded_buffer[0] = (uint8_t) ((svcs_gap_data.periph_conn_param.min_conn_interval & 0x00FF) >> 0);
            gaps_decoded_buffer[1] = (uint8_t) ((svcs_gap_data.periph_conn_param.min_conn_interval & 0xFF00) >> 8);

            gaps_decoded_buffer[2] = (uint8_t) ((svcs_gap_data.periph_conn_param.max_conn_interval & 0x00FF) >> 0);
            gaps_decoded_buffer[3] = (uint8_t) ((svcs_gap_data.periph_conn_param.max_conn_interval & 0xFF00) >> 8);

            gaps_decoded_buffer[4] = (uint8_t) ((svcs_gap_data.periph_conn_param.periph_latency & 0x00FF) >> 0);
            gaps_decoded_buffer[5] = (uint8_t) ((svcs_gap_data.periph_conn_param.periph_latency & 0xFF00) >> 8);

            gaps_decoded_buffer[6] = (uint8_t) ((svcs_gap_data.periph_conn_param.supv_timeout & 0x00FF) >> 0);
            gaps_decoded_buffer[7] = (uint8_t) ((svcs_gap_data.periph_conn_param.supv_timeout & 0xFF00) >> 8);

            // received read or read by type request from client -> send read or read by type rsp with data back to client
            ble_svcs_auto_handle_read_req(p_param, gaps_decoded_buffer, BLE_GAPS_PREFERRED_CONNPARAM_LEN);
        }
        break;
    case OPCODE_ATT_READ_BLOB_REQUEST:
        if (p_param->handle_num == gaps_info[index]->server_info.handles.hdl_device_name)
        {
            ble_svcs_auto_handle_read_blob_req(p_param, svcs_gap_data.device_name, svcs_gap_data.device_name_len);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        break;

    default:
        break;
    }
}



/** ble_svcs_gaps_handler
 * @note This callback receives the GAPS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_gaps_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_GAPS, gaps_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle GAPS client GATT event
        handle_gaps_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle GAPS server GATT event
        handle_gaps_server(index, p_param);
    }
}

