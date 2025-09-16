/************************************************************************
 *
 * File Name  : ble_service_ots.c
 * Description: This file contains the definitions and functions of BLE OTS
 *
 *
 ************************************************************************/
#include "ble_service_ots.h"
#include "ble_profile.h"

/** ble_svcs_ots_handler
 * @note This callback receives the OTS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_ots_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * OTS UUID Definitions
 **************************************************************************/

/** OTS UUID.
 * @note 16-bits UUID
 * @note UUID: 1825
*/

const uint16_t attr_uuid_ots_primary_service[] =
{
    0x1825,
};

/** OTS characteristic OTS_FEATURE UUID.
 * @note 16-bits UUID
 * @note UUID: 2abd
*/

const uint16_t attr_uuid_ots_charc_ots_feature[] =
{
    0x2abd,
};

/** OTS characteristic OBJECT_NAME UUID.
 * @note 16-bits UUID
 * @note UUID: 2abe
*/

const uint16_t attr_uuid_ots_charc_object_name[] =
{
    0x2abe,
};

/** OTS characteristic OBJECT_TYPE UUID.
 * @note 16-bits UUID
 * @note UUID: 2abf
*/

const uint16_t attr_uuid_ots_charc_object_type[] =
{
    0x2abf,
};

/** OTS characteristic OBJECT_SIZE UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac0
*/

const uint16_t attr_uuid_ots_charc_object_size[] =
{
    0x2ac0,
};

/** OTS characteristic OBJECT_FIRST_CREATED UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac1
*/

const uint16_t attr_uuid_ots_charc_object_first_created[] =
{
    0x2ac1,
};

/** OTS characteristic OBJECT_LAST_MODIFIED UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac2
*/

const uint16_t attr_uuid_ots_charc_object_last_modified[] =
{
    0x2ac2,
};

/** OTS characteristic OBJECT_ID UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac3
*/

const uint16_t attr_uuid_ots_charc_object_id[] =
{
    0x2ac3,
};

/** OTS characteristic OBJECT_PROPERTIES UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac4
*/

const uint16_t attr_uuid_ots_charc_object_properties[] =
{
    0x2ac4,
};

/** OTS characteristic OBJECT_ACTION_CONTROL_POINT UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac5
*/

const uint16_t attr_uuid_ots_charc_object_action_control_point[] =
{
    0x2ac5,
};

/** OTS characteristic OBJECT_LIST_CONTROL_POINT UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac6
*/

const uint16_t attr_uuid_ots_charc_object_list_control_point[] =
{
    0x2ac6,
};

/** OTS characteristic OBJECT_LIST_FILTER UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac7
*/

const uint16_t attr_uuid_ots_charc_object_list_filter[] =
{
    0x2ac7,
};

/** OTS characteristic OBJECT_CHANGED UUID.
 * @note 16-bits UUID
 * @note UUID: 2ac8
*/

const uint16_t attr_uuid_ots_charc_object_changed[] =
{
    0x2ac8,
};

const uint8_t ots_features[] = 
{
    0xA0, //OACP features, support write & truncation of objects
    0x02,
    0x00,
    0x00,
    0x01, //OLCP features
    0x00,
    0x00,
    0x00
};

/**************************************************************************
 * OTS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * OTS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_ots_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_ots_primary_service,
    sizeof(attr_uuid_ots_primary_service),
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

const ble_att_param_t att_ots_characteristic_ots_feature =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_ots_feature,
    sizeof(attr_uuid_ots_charc_ots_feature),
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

const ble_att_param_t att_ots_ots_feature =
{
    (void *)attr_uuid_ots_charc_ots_feature,
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
        ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_name =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_name,
    sizeof(attr_uuid_ots_charc_object_name),
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

const ble_att_param_t att_ots_object_name =
{
    (void *)attr_uuid_ots_charc_object_name,
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
        ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_type =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_type,
    sizeof(attr_uuid_ots_charc_object_type),
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

const ble_att_param_t att_ots_object_type =
{
    (void *)attr_uuid_ots_charc_object_type,
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
        ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_size =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_size,
    sizeof(attr_uuid_ots_charc_object_size),
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

const ble_att_param_t att_ots_object_size =
{
    (void *)attr_uuid_ots_charc_object_size,
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
        ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_first_created =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_first_created,
    sizeof(attr_uuid_ots_charc_object_first_created),
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

const ble_att_param_t att_ots_object_first_created =
{
    (void *)attr_uuid_ots_charc_object_first_created,
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
        ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_last_modified =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_last_modified,
    sizeof(attr_uuid_ots_charc_object_last_modified),
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

const ble_att_param_t att_ots_object_last_modified =
{
    (void *)attr_uuid_ots_charc_object_last_modified,
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
        ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_id =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_id,
    sizeof(attr_uuid_ots_charc_object_id),
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

const ble_att_param_t att_ots_object_id =
{
    (void *)attr_uuid_ots_charc_object_id,
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
        ATT_PERMISSION_AUTHE_READ |
        //ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_properties =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_properties,
    sizeof(attr_uuid_ots_charc_object_properties),
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

const ble_att_param_t att_ots_object_properties =
{
    (void *)attr_uuid_ots_charc_object_properties,
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
        ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_action_control_point =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_action_control_point,
    sizeof(attr_uuid_ots_charc_object_action_control_point),
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

const ble_att_param_t att_ots_object_action_control_point =
{
    (void *)attr_uuid_ots_charc_object_action_control_point,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_object_action_control_point_client_charc_configuration =
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_list_control_point =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_list_control_point,
    sizeof(attr_uuid_ots_charc_object_list_control_point),
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

const ble_att_param_t att_ots_object_list_control_point =
{
    (void *)attr_uuid_ots_charc_object_list_control_point,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        //GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_object_list_control_point_client_charc_configuration =
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_list_filter =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_list_filter,
    sizeof(attr_uuid_ots_charc_object_list_filter),
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

const ble_att_param_t att_ots_object_list_filter =
{
    (void *)attr_uuid_ots_charc_object_list_filter,
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
        ATT_PERMISSION_AUTHE_READ |
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_characteristic_object_changed =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_ots_charc_object_changed,
    sizeof(attr_uuid_ots_charc_object_changed),
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

const ble_att_param_t att_ots_object_changed =
{
    (void *)attr_uuid_ots_charc_object_changed,
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
    ble_svcs_ots_handler,       //registered callback function
};

const ble_att_param_t att_ots_object_changed_client_charc_configuration =
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_ots_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << OTS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_OTS
    // check MAX_NUM_CONN_OTS if defined or set to default 1.
    #define MAX_NUM_CONN_OTS       1
#endif


// Service basic information
ble_svcs_common_info_t          ots_basic_info[MAX_NUM_CONN_OTS];

// OTS information
ble_svcs_ots_info_t             *ots_info[MAX_NUM_CONN_OTS];

// OTS callback function
ble_svcs_evt_ots_handler_t      ots_callback[MAX_NUM_CONN_OTS];

// OTS registered total count
uint8_t                         ots_count = 0;


/**************************************************************************
 * BLE Service << OTS >> Public Function
 **************************************************************************/
/* OTS Initialization */
ble_err_t ble_svcs_ots_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_ots_info_t *p_info, ble_svcs_evt_ots_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "ots_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_OTS, ots_basic_info, &config_index, &ots_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set OTS data
    ots_info[config_index] = p_info;

    // Register OTS callback function
    ots_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_ots_handles_get(host_id, BLE_GATT_ROLE_SERVER, ots_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get OTS Handle Numbers */
ble_err_t ble_svcs_ots_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_ots_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_ots_primary_service;

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
 * BLE Service << OTS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void ots_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_ots_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle OTS client GATT event
static void handle_ots_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_ots_feature)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OTS_FEATURE_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_name)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_NAME_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_type)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_TYPE_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_size)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_SIZE_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_first_created)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_FIRST_CREATED_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_last_modified)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LAST_MODIFIED_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_id)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_ID_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_properties)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_PROPERTIES_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_action_control_point_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_control_point_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_filter)
        {
            // received read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_FILTER_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_changed_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_OTS_OBJECT_CHANGED_CCCD_READ_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_name)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_NAME_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_first_created)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_FIRST_CREATED_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_last_modified)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LAST_MODIFIED_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_properties)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_PROPERTIES_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_action_control_point)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_action_control_point_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_control_point)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_control_point_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_filter)
        {
            // received write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_FILTER_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_changed_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_OTS_OBJECT_CHANGED_CCCD_WRITE_RSP_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_action_control_point)
        {
            // received indicate from server
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_list_control_point)
        {
            // received indicate from server
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->client_info.handles.hdl_object_changed)
        {
            // received indicate from server
            p_param->event = BLESERVICE_OTS_OBJECT_CHANGED_INDICATE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        break;

    default:
        break;
    }
}


// handle OTS server GATT event
static void handle_ots_server(uint8_t index, ble_evt_att_param_t *p_param)
{
    printf("OTS opcode %d handle 0x%04x\n", p_param->opcode, p_param->handle_num);
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:

        if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_ots_feature)
        {
            // received read from client
            ble_svcs_auto_handle_read_req(p_param, (uint8_t *)ots_features, sizeof(ots_features));
            //p_param->event = BLESERVICE_OTS_OTS_FEATURE_READ_EVENT;
            //ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_name)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_NAME_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_type)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_TYPE_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_size)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_SIZE_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_first_created)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_FIRST_CREATED_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_last_modified)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_LAST_MODIFIED_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_id)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_ID_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_properties)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_PROPERTIES_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_action_control_point_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, ots_info[index]->server_info.data.object_action_control_point_cccd);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_control_point_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, ots_info[index]->server_info.data.object_list_control_point_cccd);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_filter)
        {
            // received read from client
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_FILTER_READ_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_changed_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, ots_info[index]->server_info.data.object_changed_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:
        if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_name)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_NAME_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_first_created)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_FIRST_CREATED_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_last_modified)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_LAST_MODIFIED_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_properties)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_PROPERTIES_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_action_control_point)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_action_control_point_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &ots_info[index]->server_info.data.object_action_control_point_cccd);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_control_point)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_control_point_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &ots_info[index]->server_info.data.object_list_control_point_cccd);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_filter)
        {
            // received write from client
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_FILTER_WRITE_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_changed_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &ots_info[index]->server_info.data.object_changed_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_action_control_point)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_CONFIRM_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_list_control_point)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_CONFIRM_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        else if (p_param->handle_num == ots_info[index]->server_info.handles.hdl_object_changed)
        {
            // received indicate confirm from client
            p_param->event = BLESERVICE_OTS_OBJECT_CHANGED_INDICATE_CONFIRM_EVENT;
            ots_evt_post(p_param, &ots_callback[index]);
        }
        break;

    default:
        break;
    }
}



/** ble_svcs_ots_handler
 * @note This callback receives the OTS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_ots_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_OTS, ots_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle OTS client GATT event
        handle_ots_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle OTS server GATT event
        handle_ots_server(index, p_param);
    }
}
