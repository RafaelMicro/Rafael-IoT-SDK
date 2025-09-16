/************************************************************************
 *
 * File Name  : ble_service_atcmd.c
 * Description: This file contains the definitions and functions of BLE ATCMD
 *
 *
 ************************************************************************/
#include "ble_service_atcmd.h"
#include "ble_profile.h"

/** ble_svcs_atcmd_handler
 * @note This callback receives the ATCMD events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_atcmd_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * ATCMD UUID Definitions
 **************************************************************************/

/** ATCMD UUID.
 * @note 128-bits UUID
 * @note UUID: 00112233445566778899AABBCCDDEEFF
*/

const uint16_t attr_uuid_atcmd_primary_service[] =
{
    0xEEFF, 0xCCDD,
    0xAABB, 0x8899,
    0x6677, 0x4455,
    0x2233, 0x0011,
};

/** ATCMD characteristic CHARAC01 UUID.
 * @note 128-bits UUID
 * @note UUID: 101112131415161718191A1B1C1D1E1F
*/

const uint16_t attr_uuid_atcmd_charc_charac01[] =
{
    0x1E1F, 0x1C1D,
    0x1A1B, 0x1819,
    0x1617, 0x1415,
    0x1213, 0x1011,
};

/** ATCMD characteristic CHARAC02 UUID.
 * @note 128-bits UUID
 * @note UUID: 202122232425262728292A2B2C2D2E2F
*/

const uint16_t attr_uuid_atcmd_charc_charac02[] =
{
    0x2E2F, 0x2C2D,
    0x2A2B, 0x2829,
    0x2627, 0x2425,
    0x2223, 0x2021,
};

/** ATCMD characteristic CHARAC03 UUID.
 * @note 128-bits UUID
 * @note UUID: 303132333435363738393A3B3C3D3E3F
*/

const uint16_t attr_uuid_atcmd_charc_charac03[] =
{
    0x3E3F, 0x3C3D,
    0x3A3B, 0x3839,
    0x3637, 0x3435,
    0x3233, 0x3031,
};

/** ATCMD characteristic CHARAC04 UUID.
 * @note 128-bits UUID
 * @note UUID: 404142434445464748494A4B4C4D4E4F
*/

const uint16_t attr_uuid_atcmd_charc_charac04[] =
{
    0x4E4F, 0x4C4D,
    0x4A4B, 0x4849,
    0x4647, 0x4445,
    0x4243, 0x4041,
};


/**************************************************************************
 * ATCMD Service Value Definitions
 **************************************************************************/

/** CHARAC01 user description definition.
 * @note Return the "description value" when central send "Read Request".
*/
#define ATTR_VALUE_ATCMD_CHARAC01_USER_DESCRIPTION  "user description"

/** CHARAC01 presentation format definition.
 * @note Return the "description value" when central send "Read Request".
*/
#define ATTR_VALUE_ATCMD_CHARAC01_PRESENTATION_FORMAT  "presentation format"

/** CHARAC01 report reference definition.
 * @note Return the "description value" when central send "Read Request".
*/
#define ATTR_VALUE_ATCMD_CHARAC01_REPORT_REFERENCE  "report"


/**************************************************************************
 * ATCMD Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_atcmd_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_atcmd_primary_service,
    sizeof(attr_uuid_atcmd_primary_service),
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

const ble_att_param_t att_atcmd_characteristic_charac01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_atcmd_charc_charac01,
    sizeof(attr_uuid_atcmd_charc_charac01),
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

const ble_att_param_t att_atcmd_charac01 =
{
    (void *)attr_uuid_atcmd_charc_charac01,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        GATT_DECLARATIONS_PROPERTIES_READ |
        GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
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
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_charac01_client_charc_configuration =
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
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_charac01_user_description =
{
    (void *)attr_uuid_type_charc_user_description,
    (void *)ATTR_VALUE_ATCMD_CHARAC01_USER_DESCRIPTION,
    sizeof(ATTR_VALUE_ATCMD_CHARAC01_USER_DESCRIPTION),
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
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_charac01_presentation_format =
{
    (void *)attr_uuid_type_charc_presentation_format,
    (void *)ATTR_VALUE_ATCMD_CHARAC01_PRESENTATION_FORMAT,
    sizeof(ATTR_VALUE_ATCMD_CHARAC01_PRESENTATION_FORMAT),
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
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_charac01_report_reference =
{
    (void *)attr_uuid_type_report_reference,
    (void *)ATTR_VALUE_ATCMD_CHARAC01_REPORT_REFERENCE,
    sizeof(ATTR_VALUE_ATCMD_CHARAC01_REPORT_REFERENCE),
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
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_characteristic_charac02 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_atcmd_charc_charac02,
    sizeof(attr_uuid_atcmd_charc_charac02),
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

const ble_att_param_t att_atcmd_charac02 =
{
    (void *)attr_uuid_atcmd_charc_charac02,
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
        //ATT_TYPE_FORMAT_16UUID |            //otherwise, 128bit UUID
        //ATT_VALUE_BOND_ENABLE |
        //ATT_PERMISSION_ENC_READ |
        ATT_PERMISSION_ENC_WRITE |
        //ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_characteristic_charac03 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_atcmd_charc_charac03,
    sizeof(attr_uuid_atcmd_charc_charac03),
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

const ble_att_param_t att_atcmd_charac03 =
{
    (void *)attr_uuid_atcmd_charc_charac03,
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_atcmd_handler,       //registered callback function
};

const ble_att_param_t att_atcmd_characteristic_charac04 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_atcmd_charc_charac04,
    sizeof(attr_uuid_atcmd_charc_charac04),
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

const ble_att_param_t att_atcmd_charac04 =
{
    (void *)attr_uuid_atcmd_charc_charac04,
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
        ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_atcmd_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << ATCMD >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_ATCMD
// check MAX_NUM_CONN_ATCMD if defined or set to default 1.
#define MAX_NUM_CONN_ATCMD       1
#endif


// Service basic information
ble_svcs_common_info_t            atcmd_basic_info[MAX_NUM_CONN_ATCMD];

// ATCMD information
ble_svcs_atcmd_info_t             *atcmd_info[MAX_NUM_CONN_ATCMD];

// ATCMD callback function
ble_svcs_evt_atcmd_handler_t      atcmd_callback[MAX_NUM_CONN_ATCMD];

// ATCMD registered total count
uint8_t                           atcmd_count = 0;


/**************************************************************************
 * BLE Service << ATCMD >> Public Function
 **************************************************************************/
/** ATCMD Initialization
*
* @attention There is only one instance of ATCMD shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to ATCMD information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_STATUS_ERR_INVALID_HOSTID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_STATUS_ERR_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_atcmd_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_atcmd_info_t *p_info, ble_svcs_evt_atcmd_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "atcmd_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_ATCMD, atcmd_basic_info, &config_index, &atcmd_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set ATCMD data
    atcmd_info[config_index] = p_info;

    // Register ATCMD callback function
    atcmd_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if (role == BLE_GATT_ROLE_SERVER)
    {
        status = ble_svcs_atcmd_handles_get(host_id, BLE_GATT_ROLE_SERVER, atcmd_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/** Get ATCMD Handle Numbers
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to ATCMD information.
*
* @retval BLE_STATUS_ERR_INVALID_HOSTID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_atcmd_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_atcmd_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_atcmd_primary_service;

        if (role == BLE_GATT_ROLE_SERVER)
        {
            ble_gatt_handle_table_param.p_handle_num_addr = (void *)&p_info->info.handles;
        }
        else if (role == BLE_GATT_ROLE_CLIENT)
        {
            ble_gatt_handle_table_param.p_handle_num_addr = (void *)&p_info->info.handles;
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
 * BLE Service << ATCMD >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void atcmd_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_atcmd_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle ATCMD client GATT event
static void handle_atcmd_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_CCCD_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_user_description)
        {
            // received user description read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_DESCRIPTION_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_presentation_format)
        {
            // received presentation format read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_FORMAT_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_report_reference)
        {
            // received report reference read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_REPORT_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac02)
        {
            // received read response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC02_READ_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received write response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_WRITE_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_CCCD_WRITE_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac02)
        {
            // received write response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC02_WRITE_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac03)
        {
            // received write response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC03_WRITE_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac04)
        {
            // received write response from server
            p_param->event = BLESERVICE_ATCMD_CHARAC04_WRITE_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received notify from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_NOTIFY_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received indicate from server
            p_param->event = BLESERVICE_ATCMD_CHARAC01_INDICATE_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_ERROR_RESPONSE:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac02)
        {
            p_param->event = BLESERVICE_ATCMD_CHARAC02_ERROR_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            p_param->event = BLESERVICE_ATCMD_CHARAC01_RESTORE_BOND_DATA_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    default:
        break;
    }
}


// handle ATCMD server GATT event
static void handle_atcmd_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received read from client
            p_param->event = BLESERVICE_ATCMD_CHARAC01_READ_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, atcmd_info[index]->info.data.charac01_cccd);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac02)
        {
            // received read from client
            p_param->event = BLESERVICE_ATCMD_CHARAC02_READ_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received write from client
            p_param->event = BLESERVICE_ATCMD_CHARAC01_WRITE_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &atcmd_info[index]->info.data.charac01_cccd);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac02)
        {
            // received write from client
            p_param->event = BLESERVICE_ATCMD_CHARAC02_WRITE_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac03)
        {
            // received write from client
            p_param->event = BLESERVICE_ATCMD_CHARAC03_WRITE_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        else if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac04)
        {
            // received write from client
            p_param->event = BLESERVICE_ATCMD_CHARAC04_WRITE_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received write without response from client
            p_param->event = BLESERVICE_ATCMD_CHARAC01_WRITE_WITHOUT_RSP_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_ATCMD_CHARAC01_INDICATE_CONFIRM_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == atcmd_info[index]->info.handles.hdl_charac01_cccd)
        {
            p_param->event = BLESERVICE_ATCMD_CHARAC01_RESTORE_BOND_DATA_EVENT;
            atcmd_evt_post(p_param, &atcmd_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_atcmd_handler
 * @note This callback receives the ATCMD events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_atcmd_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_ATCMD, atcmd_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle ATCMD client GATT event
        handle_atcmd_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle ATCMD server GATT event
        handle_atcmd_server(index, p_param);
    }
}
