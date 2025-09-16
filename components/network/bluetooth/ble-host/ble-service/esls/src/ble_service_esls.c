/************************************************************************
 *
 * File Name  : ble_service_esls.c
 * Description: This file contains the definitions and functions of BLE ESLS
 *
 *
 ************************************************************************/
#include "ble_service_esls.h"
#include "ble_profile.h"

/** ble_svcs_esls_handler
 * @note This callback receives the ESLS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_esls_handler(ble_evt_att_param_t *p_param);


/**************************************************************************
 * ESLS UUID Definitions
 **************************************************************************/

/** ESLS UUID.
 * @note 16-bits UUID
 * @note UUID: 1857
*/

const uint16_t attr_uuid_esls_primary_service[] =
{
    0x1857,
};

/** ESLS characteristic ESL_ADDRESS UUID.
 * @note 16-bits UUID
 * @note UUID: 2bf6
*/

const uint16_t attr_uuid_esls_charc_esl_address[] =
{
    0x2bf6,
};

/** ESLS characteristic AP_SYNC_KEY_MATERIAL UUID.
 * @note 16-bits UUID
 * @note UUID: 2bf7
*/

const uint16_t attr_uuid_esls_charc_ap_sync_key_material[] =
{
    0x2bf7,
};

/** ESLS characteristic ESL_RSP_KEY_MATERIAL UUID.
 * @note 16-bits UUID
 * @note UUID: 2bf8
*/

const uint16_t attr_uuid_esls_charc_esl_rsp_key_material[] =
{
    0x2bf8,
};

/** ESLS characteristic ESL_CURRENT_ABSOLUTE_TIME UUID.
 * @note 16-bits UUID
 * @note UUID: 2bf9
*/

const uint16_t attr_uuid_esls_charc_esl_current_absolute_time[] =
{
    0x2bf9,
};

/** ESLS characteristic ESL_DISPLAY_INFORMATION UUID.
 * @note 16-bits UUID
 * @note UUID: 2bfa
*/

const uint16_t attr_uuid_esls_charc_esl_display_information[] =
{
    0x2bfa,
};

/** ESLS characteristic ESL_IMAGE_INFORMATION UUID.
 * @note 16-bits UUID
 * @note UUID: 2bfb
*/

const uint16_t attr_uuid_esls_charc_esl_image_information[] =
{
    0x2bfb,
};

/** ESLS characteristic ESL_SENSOR_INFORMATION UUID.
 * @note 16-bits UUID
 * @note UUID: 2bfc
*/

const uint16_t attr_uuid_esls_charc_esl_sensor_information[] =
{
    0x2bfc,
};

/** ESLS characteristic ESL_LED_INFORMATION UUID.
 * @note 16-bits UUID
 * @note UUID: 2bfd
*/

const uint16_t attr_uuid_esls_charc_esl_led_information[] =
{
    0x2bfd,
};

/** ESLS characteristic ESL_CONTROL_POINT UUID.
 * @note 16-bits UUID
 * @note UUID: 2bfe
*/

const uint16_t attr_uuid_esls_charc_esl_control_point[] =
{
    0x2bfe,
};


/**************************************************************************
 * ESLS Service Value Definitions
 **************************************************************************/


/**************************************************************************
 * ESLS Service/ Characteristic Definitions
 **************************************************************************/

const ble_att_param_t att_esls_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_esls_primary_service,
    sizeof(attr_uuid_esls_primary_service),
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

const ble_att_param_t att_esls_characteristic_esl_address =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_address,
    sizeof(attr_uuid_esls_charc_esl_address),
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

const ble_att_param_t att_esls_esl_address =
{
    (void *)attr_uuid_esls_charc_esl_address,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_ap_sync_key_material =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_ap_sync_key_material,
    sizeof(attr_uuid_esls_charc_ap_sync_key_material),
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

const ble_att_param_t att_esls_ap_sync_key_material =
{
    (void *)attr_uuid_esls_charc_ap_sync_key_material,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_rsp_key_material =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_rsp_key_material,
    sizeof(attr_uuid_esls_charc_esl_rsp_key_material),
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

const ble_att_param_t att_esls_esl_rsp_key_material =
{
    (void *)attr_uuid_esls_charc_esl_rsp_key_material,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_current_absolute_time =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_current_absolute_time,
    sizeof(attr_uuid_esls_charc_esl_current_absolute_time),
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

const ble_att_param_t att_esls_esl_current_absolute_time =
{
    (void *)attr_uuid_esls_charc_esl_current_absolute_time,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_display_information =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_display_information,
    sizeof(attr_uuid_esls_charc_esl_display_information),
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

const ble_att_param_t att_esls_esl_display_information =
{
    (void *)attr_uuid_esls_charc_esl_display_information,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_image_information =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_image_information,
    sizeof(attr_uuid_esls_charc_esl_image_information),
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

const ble_att_param_t att_esls_esl_image_information =
{
    (void *)attr_uuid_esls_charc_esl_image_information,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_sensor_information =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_sensor_information,
    sizeof(attr_uuid_esls_charc_esl_sensor_information),
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

const ble_att_param_t att_esls_esl_sensor_information =
{
    (void *)attr_uuid_esls_charc_esl_sensor_information,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_led_information =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_led_information,
    sizeof(attr_uuid_esls_charc_esl_led_information),
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

const ble_att_param_t att_esls_esl_led_information =
{
    (void *)attr_uuid_esls_charc_esl_led_information,
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
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_characteristic_esl_control_point =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_esls_charc_esl_control_point,
    sizeof(attr_uuid_esls_charc_esl_control_point),
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

const ble_att_param_t att_esls_esl_control_point =
{
    (void *)attr_uuid_esls_charc_esl_control_point,
    (void *)0,
    0,
    (
        //GATT_DECLARATIONS_PROPERTIES_BROADCAST |
        //GATT_DECLARATIONS_PROPERTIES_READ |
        GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE |
        GATT_DECLARATIONS_PROPERTIES_WRITE |
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
        ATT_PERMISSION_AUTHE_WRITE |
        //ATT_PERMISSION_AUTHO_READ |
        //ATT_PERMISSION_AUTHO_WRITE |
        0x00
    ),
    ble_svcs_esls_handler,       //registered callback function
};

const ble_att_param_t att_esls_esl_control_point_client_charc_configuration =
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
    ble_svcs_esls_handler,       //registered callback function
};


/**************************************************************************
 * BLE Service << ESLS >> Local Variable
 **************************************************************************/
#ifndef MAX_NUM_CONN_ESLS
    // check MAX_NUM_CONN_ESLS if defined or set to default 1.
    #define MAX_NUM_CONN_ESLS       1
#endif


// Service basic information
ble_svcs_common_info_t           esls_basic_info[MAX_NUM_CONN_ESLS];

// ESLS information
ble_svcs_esls_info_t             *esls_info[MAX_NUM_CONN_ESLS];

// ESLS callback function
ble_svcs_evt_esls_handler_t      esls_callback[MAX_NUM_CONN_ESLS];

// ESLS registered total count
uint8_t                          esls_count = 0;


/**************************************************************************
 * BLE Service << ESLS >> Public Function
 **************************************************************************/
/* ESLS Initialization */
ble_err_t ble_svcs_esls_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_esls_info_t *p_info, ble_svcs_evt_esls_handler_t callback)
{
    ble_err_t status;
    uint8_t config_index;

    if (p_info == NULL)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    // init service client basic information and get "config_index" & "esls_count"
    status = ble_svcs_common_init(host_id, role, MAX_NUM_CONN_ESLS, esls_basic_info, &config_index, &esls_count);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    // Set service role
    p_info->role = role;

    // Set ESLS data
    esls_info[config_index] = p_info;

    // Register ESLS callback function
    esls_callback[config_index] = callback;

    // Get handles at initialization if role is set to BLE_GATT_ROLE_SERVER
    if ((role & BLE_GATT_ROLE_SERVER) != 0)
    {
        status = ble_svcs_esls_handles_get(host_id, BLE_GATT_ROLE_SERVER, esls_info[config_index]);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    }

    return BLE_ERR_OK;
}


/* Get ESLS Handle Numbers */
ble_err_t ble_svcs_esls_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_esls_info_t *p_info)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    do
    {
        ble_gatt_handle_table_param.host_id = host_id;
        ble_gatt_handle_table_param.gatt_role = p_info->role;
        ble_gatt_handle_table_param.p_element = (ble_att_param_t *)&att_esls_primary_service;

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
 * BLE Service << ESLS >> General Callback Function
 **************************************************************************/
// post the event to the callback function
static void esls_evt_post(ble_evt_att_param_t *p_param, ble_svcs_evt_esls_handler_t *p_callback)
{
    // check callback is null or not
    if (*p_callback == NULL)
    {
        return;
    }
    // post event to user
    (*p_callback)(p_param);
}


// handle ESLS client GATT event
static void handle_esls_client(uint8_t index, ble_evt_att_param_t *p_param)
{
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_RESPONSE:
        if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_display_information)
        {
            // received read response from server
            p_param->event = BLESERVICE_ESLS_ESL_DISPLAY_INFORMATION_READ_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_image_information)
        {
            // received read response from server
            p_param->event = BLESERVICE_ESLS_ESL_IMAGE_INFORMATION_READ_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_sensor_information)
        {
            // received read response from server
            p_param->event = BLESERVICE_ESLS_ESL_SENSOR_INFORMATION_READ_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_led_information)
        {
            // received read response from server
            p_param->event = BLESERVICE_ESLS_ESL_LED_INFORMATION_READ_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_control_point_cccd)
        {
            // received cccd read response from server
            p_param->event = BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_READ_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_RESPONSE:
    case OPCODE_ATT_RESTORE_BOND_DATA_COMMAND:
        if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_address)
        {
            // received write response from server
            p_param->event = BLESERVICE_ESLS_ESL_ADDRESS_WRITE_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_ap_sync_key_material)
        {
            // received write response from server
            p_param->event = BLESERVICE_ESLS_AP_SYNC_KEY_MATERIAL_WRITE_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_rsp_key_material)
        {
            // received write response from server
            p_param->event = BLESERVICE_ESLS_ESL_RSP_KEY_MATERIAL_WRITE_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_current_absolute_time)
        {
            // received write response from server
            p_param->event = BLESERVICE_ESLS_ESL_CURRENT_ABSOLUTE_TIME_WRITE_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_control_point_cccd)
        {
            // received cccd write response from server
            p_param->event = BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_WRITE_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_NOTIFICATION:
        if (p_param->handle_num == esls_info[index]->client_info.handles.hdl_esl_control_point)
        {
            // received notify from server
            p_param->event = BLESERVICE_ESLS_ESL_CONTROL_POINT_NOTIFY_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_INDICATION:
        break;

    default:
        break;
    }
}


// handle ESLS server GATT event
static void handle_esls_server(uint8_t index, ble_evt_att_param_t *p_param)
{

    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_REQUEST:
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
        if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_display_information)
        {
            // received read from client
            p_param->event = BLESERVICE_ESLS_ESL_DISPLAY_INFORMATION_READ_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_image_information)
        {
            // received read from client
            p_param->event = BLESERVICE_ESLS_ESL_IMAGE_INFORMATION_READ_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_sensor_information)
        {
            // received read from client
            p_param->event = BLESERVICE_ESLS_ESL_SENSOR_INFORMATION_READ_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_led_information)
        {
            // received read from client
            p_param->event = BLESERVICE_ESLS_ESL_LED_INFORMATION_READ_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_control_point_cccd)
        {
            // received cccd read from client
            ble_svcs_auto_handle_cccd_read_req(p_param, esls_info[index]->server_info.data.esl_control_point_cccd);
        }
        break;
    case OPCODE_ATT_WRITE_REQUEST:

        if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_address)
        {
            // received write from client
            p_param->event = BLESERVICE_ESLS_ESL_ADDRESS_WRITE_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_ap_sync_key_material)
        {
            // received write from client
            p_param->event = BLESERVICE_ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_rsp_key_material)
        {
            // received write from client
            p_param->event = BLESERVICE_ESLS_ESL_RSP_KEY_MATERIAL_WRITE_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_current_absolute_time)
        {
            // received write from client
            p_param->event = BLESERVICE_ESLS_ESL_CURRENT_ABSOLUTE_TIME_WRITE_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_control_point_cccd)
        {
            // received cccd write from client
            ble_svcs_handle_cccd_write_req(p_param->data, p_param->length, &esls_info[index]->server_info.data.esl_control_point_cccd);
        }
        else if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_control_point)
        {
            // received write without response from client
            p_param->event = BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        break;
    case OPCODE_ATT_WRITE_COMMAND:
        if (p_param->handle_num == esls_info[index]->server_info.handles.hdl_esl_control_point)
        {
            // received write without response from client
            p_param->event = BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_WITHOUT_RSP_EVENT;
            esls_evt_post(p_param, &esls_callback[index]);
        }
        break;
    case OPCODE_ATT_HANDLE_VALUE_CONFIRMATION:
        break;

    default:
        break;
    }
}



/** ble_svcs_esls_handler
 * @note This callback receives the ESLS events.
 * Each of these events can be associated with parameters.
*/

void ble_svcs_esls_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_ESLS, esls_basic_info, &index) != BLE_ERR_OK)
    {
        // Host id has not registered so there is no callback function -> do nothing
        return;
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        // handle ESLS client GATT event
        handle_esls_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        // handle ESLS server GATT event
        handle_esls_server(index, p_param);
    }
}
