#ifndef _BLE_SERVICE_ESLS_H_
#define _BLE_SERVICE_ESLS_H_

/**************************************************************************//**
 * @file  ble_service_esls.h
 * @brief Provide the Definition of ESLS.
*****************************************************************************/

#include "ble_service_common.h"
#include "ble_att_gatt.h"


/**************************************************************************
 * ESLS Definitions
 **************************************************************************/
/** @defgroup service_esls_def BLE ESLS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the ESLS.
 * @}
**************************************************************************/

/**
 * @ingroup service_esls_def
 * @defgroup service_esls_UUIDDef BLE ESLS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE ESLS UUID Definitions.
*/
extern const uint16_t attr_uuid_esls_primary_service[];                    /**< ESLS service UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_address[];                  /**< ESLS characteristic ESL_ADDRESS UUID. */
extern const uint16_t attr_uuid_esls_charc_ap_sync_key_material[];         /**< ESLS characteristic AP_SYNC_KEY_MATERIAL UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_rsp_key_material[];         /**< ESLS characteristic ESL_RSP_KEY_MATERIAL UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_current_absolute_time[];    /**< ESLS characteristic ESL_CURRENT_ABSOLUTE_TIME UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_display_information[];      /**< ESLS characteristic ESL_DISPLAY_INFORMATION UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_image_information[];        /**< ESLS characteristic ESL_IMAGE_INFORMATION UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_sensor_information[];       /**< ESLS characteristic ESL_SENSOR_INFORMATION UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_led_information[];          /**< ESLS characteristic ESL_LED_INFORMATION UUID. */
extern const uint16_t attr_uuid_esls_charc_esl_control_point[];            /**< ESLS characteristic ESL_CONTROL_POINT UUID. */
/** @} */

/**
 * @defgroup service_esls_ServiceChardef BLE ESLS Service and Characteristic Definitions
 * @{
 * @ingroup service_esls_def
 * @details Here shows the definitions of the ESLS service and characteristic.
 * @}
*/

/**
 * @ingroup service_esls_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_esls_primary_service;                                 /**< ESLS primary service. */
extern const ble_att_param_t att_esls_characteristic_esl_address;                      /**< ESLS characteristic ESL_ADDRESS. */
extern const ble_att_param_t att_esls_esl_address;                                     /**< ESLS ESL_ADDRESS value. */
extern const ble_att_param_t att_esls_characteristic_ap_sync_key_material;             /**< ESLS characteristic AP_SYNC_KEY_MATERIAL. */
extern const ble_att_param_t att_esls_ap_sync_key_material;                            /**< ESLS AP_SYNC_KEY_MATERIAL value. */
extern const ble_att_param_t att_esls_characteristic_esl_rsp_key_material;             /**< ESLS characteristic ESL_RSP_KEY_MATERIAL. */
extern const ble_att_param_t att_esls_esl_rsp_key_material;                            /**< ESLS ESL_RSP_KEY_MATERIAL value. */
extern const ble_att_param_t att_esls_characteristic_esl_current_absolute_time;        /**< ESLS characteristic ESL_CURRENT_ABSOLUTE_TIME. */
extern const ble_att_param_t att_esls_esl_current_absolute_time;                       /**< ESLS ESL_CURRENT_ABSOLUTE_TIME value. */
extern const ble_att_param_t att_esls_characteristic_esl_display_information;          /**< ESLS characteristic ESL_DISPLAY_INFORMATION. */
extern const ble_att_param_t att_esls_esl_display_information;                         /**< ESLS ESL_DISPLAY_INFORMATION value. */
extern const ble_att_param_t att_esls_characteristic_esl_image_information;            /**< ESLS characteristic ESL_IMAGE_INFORMATION. */
extern const ble_att_param_t att_esls_esl_image_information;                           /**< ESLS ESL_IMAGE_INFORMATION value. */
extern const ble_att_param_t att_esls_characteristic_esl_sensor_information;           /**< ESLS characteristic ESL_SENSOR_INFORMATION. */
extern const ble_att_param_t att_esls_esl_sensor_information;                          /**< ESLS ESL_SENSOR_INFORMATION value. */
extern const ble_att_param_t att_esls_characteristic_esl_led_information;              /**< ESLS characteristic ESL_LED_INFORMATION. */
extern const ble_att_param_t att_esls_esl_led_information;                             /**< ESLS ESL_LED_INFORMATION value. */
extern const ble_att_param_t att_esls_characteristic_esl_control_point;                /**< ESLS characteristic ESL_CONTROL_POINT. */
extern const ble_att_param_t att_esls_esl_control_point;                               /**< ESLS ESL_CONTROL_POINT value. */
extern const ble_att_param_t att_esls_esl_control_point_client_charc_configuration;    /**< ESLS ESL_CONTROL_POINT client characteristic configuration. */
/** @} */


/** ESLS Definition
 * @ingroup service_esls_ServiceChardef
*/
#define ATT_ESLS_SERVICE                                            \
    &att_esls_primary_service,                                      \
    &att_esls_characteristic_esl_address,                           \
    &att_esls_esl_address,                                          \
    &att_esls_characteristic_ap_sync_key_material,                  \
    &att_esls_ap_sync_key_material,                                 \
    &att_esls_characteristic_esl_rsp_key_material,                  \
    &att_esls_esl_rsp_key_material,                                 \
    &att_esls_characteristic_esl_current_absolute_time,             \
    &att_esls_esl_current_absolute_time,                            \
    &att_esls_characteristic_esl_display_information,               \
    &att_esls_esl_display_information,                              \
    &att_esls_characteristic_esl_image_information,                 \
    &att_esls_esl_image_information,                                \
    &att_esls_characteristic_esl_sensor_information,                \
    &att_esls_esl_sensor_information,                               \
    &att_esls_characteristic_esl_led_information,                   \
    &att_esls_esl_led_information,                                  \
    &att_esls_characteristic_esl_control_point,                     \
    &att_esls_esl_control_point,                                    \
    &att_esls_esl_control_point_client_charc_configuration          \


/**************************************************************************
 * ESLS Application Definitions
 **************************************************************************/
/** @defgroup app_esls_def BLE ESLS Application Definitions
 * @{
 * @ingroup service_esls_def
 * @details Here shows the definitions of the ESLS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_esls_def
 * @defgroup app_esls_eventDef BLE ESLS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the ESLS.
*/
#define BLESERVICE_ESLS_ESL_ADDRESS_WRITE_EVENT                       0x01     /**< ESLS characteristic ESL_ADDRESS write event.*/
#define BLESERVICE_ESLS_ESL_ADDRESS_WRITE_RSP_EVENT                   0x02     /**< ESLS characteristic ESL_ADDRESS write response event.*/
#define BLESERVICE_ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVENT              0x03     /**< ESLS characteristic AP_SYNC_KEY_MATERIAL write event.*/
#define BLESERVICE_ESLS_AP_SYNC_KEY_MATERIAL_WRITE_RSP_EVENT          0x04     /**< ESLS characteristic AP_SYNC_KEY_MATERIAL write response event.*/
#define BLESERVICE_ESLS_ESL_RSP_KEY_MATERIAL_WRITE_EVENT              0x05     /**< ESLS characteristic ESL_RSP_KEY_MATERIAL write event.*/
#define BLESERVICE_ESLS_ESL_RSP_KEY_MATERIAL_WRITE_RSP_EVENT          0x06     /**< ESLS characteristic ESL_RSP_KEY_MATERIAL write response event.*/
#define BLESERVICE_ESLS_ESL_CURRENT_ABSOLUTE_TIME_WRITE_EVENT         0x07     /**< ESLS characteristic ESL_CURRENT_ABSOLUTE_TIME write event.*/
#define BLESERVICE_ESLS_ESL_CURRENT_ABSOLUTE_TIME_WRITE_RSP_EVENT     0x08     /**< ESLS characteristic ESL_CURRENT_ABSOLUTE_TIME write response event.*/
#define BLESERVICE_ESLS_ESL_DISPLAY_INFORMATION_READ_EVENT            0x09     /**< ESLS characteristic ESL_DISPLAY_INFORMATION read event.*/
#define BLESERVICE_ESLS_ESL_DISPLAY_INFORMATION_READ_RSP_EVENT        0x0a     /**< ESLS characteristic ESL_DISPLAY_INFORMATION read response event.*/
#define BLESERVICE_ESLS_ESL_IMAGE_INFORMATION_READ_EVENT              0x0b     /**< ESLS characteristic ESL_IMAGE_INFORMATION read event.*/
#define BLESERVICE_ESLS_ESL_IMAGE_INFORMATION_READ_RSP_EVENT          0x0c     /**< ESLS characteristic ESL_IMAGE_INFORMATION read response event.*/
#define BLESERVICE_ESLS_ESL_SENSOR_INFORMATION_READ_EVENT             0x0d     /**< ESLS characteristic ESL_SENSOR_INFORMATION read event.*/
#define BLESERVICE_ESLS_ESL_SENSOR_INFORMATION_READ_RSP_EVENT         0x0e     /**< ESLS characteristic ESL_SENSOR_INFORMATION read response event.*/
#define BLESERVICE_ESLS_ESL_LED_INFORMATION_READ_EVENT                0x0f     /**< ESLS characteristic ESL_LED_INFORMATION read event.*/
#define BLESERVICE_ESLS_ESL_LED_INFORMATION_READ_RSP_EVENT            0x10     /**< ESLS characteristic ESL_LED_INFORMATION read response event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_EVENT                 0x11     /**< ESLS characteristic ESL_CONTROL_POINT write event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_WITHOUT_RSP_EVENT     0x12     /**< ESLS characteristic ESL_CONTROL_POINT write without response event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_NOTIFY_EVENT                0x13     /**< ESLS characteristic ESL_CONTROL_POINT notify event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_READ_EVENT             0x14     /**< ESLS characteristic ESL_CONTROL_POINT cccd read event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_READ_RSP_EVENT         0x15     /**< ESLS characteristic ESL_CONTROL_POINT cccd read response event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_WRITE_EVENT            0x16     /**< ESLS characteristic ESL_CONTROL_POINT cccd write event.*/
#define BLESERVICE_ESLS_ESL_CONTROL_POINT_CCCD_WRITE_RSP_EVENT        0x17     /**< ESLS characteristic ESL_CONTROL_POINT cccd write response event.*/
/** @} */

typedef uint8_t esl_cmd_opcode_t;
#define ESL_CMD_PING                    0x00
#define ESL_CMD_UNASSOCIATE_FROM_AP     0x01
#define ESL_CMD_SERVICE_RESET           0x02
#define ESL_CMD_FACTORY_RESET           0x03
#define ESL_CMD_UPDATE_COMPLETE         0x04
#define ESL_CMD_READ_SENSOR_DATA        0x10
#define ESL_CMD_REFRESH_DISPLAY         0x11
#define ESL_CMD_DISPLAY_IMAGE           0x20
#define ESL_CMD_DISPLAY_TIMED_IMAGE     0x60
#define ESL_CMD_LED_CONTROL             0xB0
#define ESL_CMD_LED_TIMED_CONTROL       0xF0

typedef uint8_t esl_rsp_err_t;
#define ESL_RSP_ERROR_CODE_RFU                        0x00
#define ESL_RSP_ERROR_CODE_UNSPECIFIED_ERROR          0x01
#define ESL_RSP_ERROR_CODE_INVALID_OPCODE             0x02
#define ESL_RSP_ERROR_CODE_INVALID_STATE              0x03
#define ESL_RSP_ERROR_CODE_INVALID_IMAGE_INDEX        0x04
#define ESL_RSP_ERROR_CODE_IMAGE_NOT_AVAILABLE        0x05
#define ESL_RSP_ERROR_CODE_INVALID_PARAMETERS         0x06
#define ESL_RSP_ERROR_CODE_CAPACITY_LIMIT             0x07
#define ESL_RSP_ERROR_CODE_INSUFFICIENT_BATTERY       0x08
#define ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES     0x09
#define ESL_RSP_ERROR_CODE_RERTY                      0x0A
#define ESL_RSP_ERROR_CODE_QUEUE_FULL                 0x0B
#define ESL_RSP_ERROR_CODE_IMPLAUSIBLE_ABSOLUTE_TIME  0x0C

typedef uint8_t esl_rsp_opcode_t;
#define ESL_RSP_ERROR                0x00
#define ESL_RSP_LED_STATE            0x01
#define ESL_RSP_BASIC_STATE          0x10
#define ESL_RSP_DISPLAY_STATE        0x11
#define ESL_RSP_SENSOR_VALUE         0x0E
#define ESL_RSP_VENDOR_SPECIFIC_RSP  0x0F


/**
 * @ingroup app_esls_def
 * @defgroup app_esls_structureDef BLE ESLS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the ESLS.
 * @}
*/

/** ESLS Handles Definition
 * @ingroup app_esls_structureDef
*/
typedef struct ble_svcs_esls_handles_s
{
    uint16_t hdl_esl_address;                  /**< Handle of ESL_ADDRESS. */
    uint16_t hdl_ap_sync_key_material;         /**< Handle of AP_SYNC_KEY_MATERIAL. */
    uint16_t hdl_esl_rsp_key_material;         /**< Handle of ESL_RSP_KEY_MATERIAL. */
    uint16_t hdl_esl_current_absolute_time;    /**< Handle of ESL_CURRENT_ABSOLUTE_TIME. */
    uint16_t hdl_esl_display_information;      /**< Handle of ESL_DISPLAY_INFORMATION. */
    uint16_t hdl_esl_image_information;        /**< Handle of ESL_IMAGE_INFORMATION. */
    uint16_t hdl_esl_sensor_information;       /**< Handle of ESL_SENSOR_INFORMATION. */
    uint16_t hdl_esl_led_information;          /**< Handle of ESL_LED_INFORMATION. */
    uint16_t hdl_esl_control_point;            /**< Handle of ESL_CONTROL_POINT. */
    uint16_t hdl_esl_control_point_cccd;       /**< Handle of ESL_CONTROL_POINT client characteristic configuration. */
} ble_svcs_esls_handles_t;


/** ESLS Data Definition
 * @ingroup app_esls_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_esls_data_s
{
    uint16_t esl_control_point_cccd;    /**< ESL_CONTROL_POINT cccd value */
} ble_svcs_esls_data_t;


/** ESLS Application Data Structure Definition
 * @ingroup app_esls_structureDef
*/
typedef struct ble_svcs_esls_subinfo_s
{
    ble_svcs_esls_handles_t handles;    /**< ESLS attribute handles */
    ble_svcs_esls_data_t    data;       /**< ESLS attribute data */
} ble_svcs_esls_subinfo_t;

typedef struct ble_svcs_esls_info_s
{
    ble_gatt_role_t         role;           /**< BLE GATT role */
    ble_svcs_esls_subinfo_t client_info;    /**< Client Information */
    ble_svcs_esls_subinfo_t server_info;    /**< Server Information */
} ble_svcs_esls_info_t;


typedef struct ble_svcs_esls_esl_address_s
{
    uint16_t  esl_id:8,
              group_id:7,
              rfu:1;
} ble_svcs_esls_esl_address_t;

typedef struct esls_charc_ap_sync_key_material_s
{
    uint8_t   ap_sync_key[24];
} esls_charc_ap_sync_key_material_t;

typedef struct esls_charc_esl_rsp_key_material_s
{
    uint8_t   esl_rsp_key[24];
} esls_charc_esl_rsp_key_material_t;

typedef struct esls_charc_current_absolute_time_s
{
    uint32_t   time;
} esls_charc_current_absolute_time_t;

typedef struct __attribute__((packed)) esls_ctrl_point_rsp_err_param_s
{
    esl_rsp_opcode_t opcode;
    esl_rsp_err_t    error_code;
} esls_ctrl_point_rsp_err_param_t; 


typedef struct __attribute__((packed)) esls_ctrl_point_rsp_led_state_param_s
{
    esl_rsp_opcode_t opcode;
    uint8_t          led_index;
} esls_ctrl_point_rsp_led_state_param_t; 

typedef struct __attribute__((packed)) esls_ctrl_point_rsp_basic_state_param_s
{
    esl_rsp_opcode_t opcode;
    uint16_t         service_need:1,
                     synchronized:1,
                     active_led:1,
                     pending_led_update:1,
                     pending_display_update:1,
                     rfu:11;
} esls_ctrl_point_rsp_basic_state_param_t; 

typedef struct __attribute__((packed)) esls_ctrl_point_rsp_display_state_param_s
{
    esl_rsp_opcode_t opcode;
    uint8_t          display_index;
    uint8_t          image_index;
} esls_ctrl_point_rsp_display_state_param_t; 

typedef struct __attribute__((packed)) esls_ctrl_point_rsp_sensor_state_param_s
{
    esl_rsp_opcode_t opcode;
    uint8_t          sensor_index;
    uint8_t          sensor_data[];
} esls_ctrl_point_rsp_sensor_state_param_t; 

typedef struct __attribute__((packed)) esls_ctrl_point_rsp_vendor_state_param_s
{
    esl_rsp_opcode_t opcode;
    uint8_t          vendor_data[];
} esls_ctrl_point_rsp_vendor_state_param_t; 

typedef union __attribute__((packed)) esls_ctrl_point_rsp_param_s
{
    esl_rsp_opcode_t opcode;
    esls_ctrl_point_rsp_err_param_t           err;
    esls_ctrl_point_rsp_led_state_param_t     led_state;
    esls_ctrl_point_rsp_basic_state_param_t   basic_state;
    esls_ctrl_point_rsp_display_state_param_t display_state;
    esls_ctrl_point_rsp_sensor_state_param_t  sensor_state;
    esls_ctrl_point_rsp_vendor_state_param_t  vendor_state;
} esls_ctrl_point_rsp_param_t; 


typedef struct __attribute__((packed)) esls_padv_rsp_param_s
{
    uint8_t len;
    uint8_t esl_tag;
    uint8_t rsp[]; //esls_ctrl_point_rsp_param_t
} esls_padv_rsp_param_t; 

typedef struct __attribute__((packed)) ble_esl_cmd_read_sensor_data_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t sensor_index;
} ble_esl_cmd_read_sensor_data_t;

typedef struct __attribute__((packed)) ble_esl_cmd_refresh_display_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t display_index;
} ble_esl_cmd_refresh_display_t;

typedef struct __attribute__((packed)) ble_esl_cmd_display_image_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t display_index;
    uint8_t image_index;
} ble_esl_cmd_display_image_t;

typedef struct __attribute__((packed)) ble_esl_cmd_display_timed_image_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t display_index;
    uint8_t image_index;
    uint32_t absolute_time;
} ble_esl_cmd_display_timed_image_t;

typedef struct __attribute__((packed)) ble_esl_cmd_led_control_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t led_index;
    uint8_t color_red:2,
            color_green:2,
            color_blue:2,
            brightness:2;
    uint8_t flashing_pattern[7];
    uint16_t repeat_type:1,
             repeat_duration:15;        
} ble_esl_cmd_led_control_t;

typedef struct __attribute__((packed)) ble_esl_cmd_led_timed_control_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
    uint8_t led_index;
    uint8_t color_red:2,
            color_green:2,
            color_blue:2,
            brightness:2;
    uint8_t flashing_pattern[7];
    uint16_t repeat_type:1,
             repeat_duration:15;
    uint32_t absolute_time;
} ble_esl_cmd_led_timed_control_t;

typedef struct __attribute__((packed)) ble_esl_ctrl_point_cmd_header_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_ctrl_point_cmd_header_t;

typedef struct __attribute__((packed)) ble_esl_cmd_ping_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_cmd_ping_t;

typedef struct __attribute__((packed)) ble_esl_cmd_unassociate_from_ap_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_cmd_unassociate_from_ap_t;

typedef struct __attribute__((packed)) ble_esl_cmd_service_reset_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_cmd_service_reset_t;

typedef struct __attribute__((packed)) ble_esl_cmd_factory_reset_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_cmd_factory_reset_t;

typedef struct __attribute__((packed)) ble_esl_cmd_update_complete_s
{
    esl_cmd_opcode_t opcode;
    uint8_t esl_id;
}ble_esl_cmd_update_complete_t;

typedef union __attribute__((packed)) ble_esl_control_point_cmd_s
{

    ble_esl_ctrl_point_cmd_header_t   cmd_header;
    ble_esl_cmd_ping_t                ping;
    ble_esl_cmd_unassociate_from_ap_t unassoc_from_ap;
    ble_esl_cmd_service_reset_t       service_reset;
    ble_esl_cmd_factory_reset_t       factory_reset;
    ble_esl_cmd_update_complete_t     update_complete;
    ble_esl_cmd_read_sensor_data_t    read_sensor_data;
    ble_esl_cmd_refresh_display_t     refresh_display;
    ble_esl_cmd_display_image_t       display_image;
    ble_esl_cmd_display_timed_image_t display_timed_image;
    ble_esl_cmd_led_control_t         led_ctrl;
    ble_esl_cmd_led_timed_control_t   led_timed_ctrl;
} ble_esl_control_point_cmd_t;


typedef struct  __attribute__((packed)) esls_padv_enc_data_param_s
{
    uint8_t len;
    uint8_t esl_tag;
    uint8_t group_id:7,
            rfu:1;
    uint8_t  cmd[]; //ble_esl_control_point_cmd_t
} esls_padv_enc_data_param_t;

typedef struct __attribute__((packed)) esls_padv_rsp_enc_hdr_param_s
{
     uint8_t length;
     uint8_t type;
     uint8_t randomizer[5];
     uint8_t enc_data[]; // esls_padv_enc_data_param_t
} esls_padv_enc_hdr_param_t; 


/**
 * @ingroup app_esls_def
 * @defgroup app_esls_App BLE ESLS Definitions for Application
 * @{
 * @details Here shows the definitions of the ESLS for application uses.
 * @}
*/

/** ble_svcs_evt_esls_handler_t
 * @ingroup app_esls_App
 * @note This callback receives the ESLS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_esls_handler_t)(ble_evt_att_param_t *p_param);

/** ESLS Initialization
*
* @ingroup app_esls_App
*
* @attention There is only one instance of ESLS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to ESLS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_esls_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_esls_info_t *p_info, ble_svcs_evt_esls_handler_t callback);

/** Get ESLS Handle Numbers
*
* @ingroup app_esls_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to ESLS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_esls_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_esls_info_t *p_info);

#endif //_BLE_SERVICE_ESLS_H_

