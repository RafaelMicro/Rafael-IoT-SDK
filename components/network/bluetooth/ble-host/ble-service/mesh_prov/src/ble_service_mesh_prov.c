/************************************************************************
 *
 * File Name  : ble_service_mesh_prov.c
 * Description: This file contains the definitions and functions of BLE MESH_PROV
 *
 *
 ************************************************************************/
#include <string.h>
#include "ble_service_mesh_prov.h"
#include "ble_profile.h"
#include "log.h"

/** ble_svcs_mesh_prov_handler
 * @note This callback receives the MESH_PROV events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_mesh_prov_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * MESH_PROV UUID Definitions
 **************************************************************************/

/** MESH_PROV UUID.
 * @note 16-bits UUID
 * @note UUID: 180D
*/

const uint16_t attr_uuid_mesh_prov_primary_service[] =
{
    GATT_SERVICES_MESH_PROVISIONING,
};

/** MESH_PROV characteristic DATA_IN UUID.
 * @note 16-bits UUID
 * @note UUID: 2ADB
*/

const uint16_t attr_uuid_mesh_prov_charc_data_in[] =
{
    GATT_SPEC_CHARC_MESH_PROVISIONING_DATA_IN,
};

/** MESH_PROV characteristic DATA_OUT UUID.
 * @note 16-bits UUID
 * @note UUID: 2A38
*/

const uint16_t attr_uuid_mesh_prov_charc_data_out[] =
{
    GATT_SPEC_CHARC_MESH_PROVISIONING_DATA_OUT,
};


/**************************************************************************
 * MESH_PROV Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * MESH_PROV Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_mesh_prov_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_mesh_prov_primary_service,
    sizeof(attr_uuid_mesh_prov_primary_service),
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

const ble_att_param_t att_mesh_prov_characteristic_data_in =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_mesh_prov_charc_data_in,
    sizeof(attr_uuid_mesh_prov_charc_data_in),
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

const ble_att_param_t att_mesh_prov_data_in =
{
    (void *)attr_uuid_mesh_prov_charc_data_in,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
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
    ble_svcs_mesh_prov_handler,       //registered callback function
};

const ble_att_param_t att_mesh_prov_characteristic_data_out =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_mesh_prov_charc_data_out,
    sizeof(attr_uuid_mesh_prov_charc_data_out),
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

const ble_att_param_t att_mesh_prov_data_out =
{
    (void *)attr_uuid_mesh_prov_charc_data_out,
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
    ble_svcs_mesh_prov_handler,       //registered callback function
};

const ble_att_param_t att_mesh_prov_data_out_client_charc_configuration =
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
    ble_svcs_mesh_prov_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << MESH_PROV >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_MESH_PROV
// check MAX_NUM_CONN_MESH_PROV if defined or set to default 1.
#define MAX_NUM_CONN_MESH_PROV       1
#endif


// Service basic information
ble_svcs_common_info_t                mesh_prov_basic_info[MAX_NUM_CONN_MESH_PROV];

// MESH_PROV information
ble_svcs_mesh_prov_info_t             *mesh_prov_info[MAX_NUM_CONN_MESH_PROV];

// MESH_PROV callback function
ble_svcs_evt_mesh_prov_handler_t      mesh_prov_callback[MAX_NUM_CONN_MESH_PROV];

// MESH_PROV registered total count
uint8_t                               mesh_prov_count = 0;


/**************************************************************************
 * BLE Service << MESH_PROV >> Public Function
 **************************************************************************/
/* MESH_PROV Initialization */
ble_err_t ble_svcs_mesh_prov_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_mesh_prov_info_t *p_info, ble_svcs_evt_mesh_prov_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "mesh_prov_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_MESH_PROV, mesh_prov_basic_info, &config_index, &mesh_prov_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set MESH_PROV data
    mesh_prov_info[config_index] = p_info;

    // Register MESH_PROV callback function
    mesh_prov_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if (role == BLE_GATT_ROLE_SERVER)
    {
        status = ble_svcs_mesh_prov_handles_get(host_id, mesh_prov_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get MESH_PROV Handle Numbers */
ble_err_t ble_svcs_mesh_prov_handles_get(uint8_t host_id, ble_svcs_mesh_prov_info_t *p_info)
{
    ble_err_t status = BLE_ERR_OK;
    ble_gatt_handle_table_param_t *ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    ble_gatt_handle_table_param = pvPortMalloc(sizeof(ble_gatt_handle_table_param_t));

    if (ble_gatt_handle_table_param != NULL)
    {
        ble_gatt_handle_table_param->host_id = host_id;
        ble_gatt_handle_table_param->gatt_role = p_info->role;
        ble_gatt_handle_table_param->p_element = (ble_att_param_t *)&att_mesh_prov_primary_service;
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
 * BLE Service << MESH_PROV >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void mesh_prov_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_mesh_prov_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle MESH_PROV client GATT event
static void handle_mesh_prov_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_out_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_MESH_PROV_DATA_OUT_CCCD_READ_RSP_EVENT;
            mesh_prov_evt_post(p_param, &mesh_prov_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_out_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_MESH_PROV_DATA_OUT_CCCD_WRITE_RSP_EVENT;
            mesh_prov_evt_post(p_param, &mesh_prov_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_out)
        {
            // received notify from server
            p_param->event = BLESERVICE_MESH_PROV_DATA_OUT_NOTIFY_EVENT;
            mesh_prov_evt_post(p_param, &mesh_prov_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        break;

    default:
        break;
    }
}


// handle MESH_PROV server GATT event
static void handle_mesh_prov_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_out_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, mesh_prov_info[index]->data.data_out_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_out_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &mesh_prov_info[index]->data.data_out_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        if (p_param->handle_num == mesh_prov_info[index]->handles.hdl_data_in)
        {
            // received write without response from client
            p_param->event = BLESERVICE_MESH_PROV_DATA_IN_WRITE_WITHOUT_RSP_EVENT;
            mesh_prov_evt_post(p_param, &mesh_prov_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        break;

    default:
        break;
    }
}



/** ble_svcs_mesh_prov_handler
 * @note This callback receives the MESH_PROV events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_mesh_prov_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_MESH_PROV, mesh_prov_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle MESH_PROV client GATT event
        handle_mesh_prov_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle MESH_PROV server GATT event
        handle_mesh_prov_server(index, p_param);
    }
}
