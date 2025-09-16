#ifndef _BLE_SERVICE_OTS_H_
#define _BLE_SERVICE_OTS_H_

/**************************************************************************//**
 * @file  ble_service_ots.h
 * @brief Provide the Definition of OTS.
*****************************************************************************/

#include "ble_service_common.h"
#include "ble_att_gatt.h"


/**************************************************************************
 * OTS Definitions
 **************************************************************************/
/** @defgroup service_ots_def BLE OTS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the OTS.
 * @}
**************************************************************************/

/**
 * @ingroup service_ots_def
 * @defgroup service_ots_UUIDDef BLE OTS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE OTS UUID Definitions.
*/
extern const uint16_t attr_uuid_ots_primary_service[];                      /**< OTS service UUID. */
extern const uint16_t attr_uuid_ots_charc_ots_feature[];                    /**< OTS characteristic OTS_FEATURE UUID. */
extern const uint16_t attr_uuid_ots_charc_object_name[];                    /**< OTS characteristic OBJECT_NAME UUID. */
extern const uint16_t attr_uuid_ots_charc_object_type[];                    /**< OTS characteristic OBJECT_TYPE UUID. */
extern const uint16_t attr_uuid_ots_charc_object_size[];                    /**< OTS characteristic OBJECT_SIZE UUID. */
extern const uint16_t attr_uuid_ots_charc_object_first_created[];           /**< OTS characteristic OBJECT_FIRST_CREATED UUID. */
extern const uint16_t attr_uuid_ots_charc_object_last_modified[];           /**< OTS characteristic OBJECT_LAST_MODIFIED UUID. */
extern const uint16_t attr_uuid_ots_charc_object_id[];                      /**< OTS characteristic OBJECT_ID UUID. */
extern const uint16_t attr_uuid_ots_charc_object_properties[];              /**< OTS characteristic OBJECT_PROPERTIES UUID. */
extern const uint16_t attr_uuid_ots_charc_object_action_control_point[];    /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT UUID. */
extern const uint16_t attr_uuid_ots_charc_object_list_control_point[];      /**< OTS characteristic OBJECT_LIST_CONTROL_POINT UUID. */
extern const uint16_t attr_uuid_ots_charc_object_list_filter[];             /**< OTS characteristic OBJECT_LIST_FILTER UUID. */
extern const uint16_t attr_uuid_ots_charc_object_changed[];                 /**< OTS characteristic OBJECT_CHANGED UUID. */
/** @} */

/**
 * @defgroup service_ots_ServiceChardef BLE OTS Service and Characteristic Definitions
 * @{
 * @ingroup service_ots_def
 * @details Here shows the definitions of the OTS service and characteristic.
 * @}
*/

/**
 * @ingroup service_ots_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_ots_primary_service;                                           /**< OTS primary service. */
extern const ble_att_param_t att_ots_characteristic_ots_feature;                                /**< OTS characteristic OTS_FEATURE. */
extern const ble_att_param_t att_ots_ots_feature;                                               /**< OTS OTS_FEATURE value. */
extern const ble_att_param_t att_ots_characteristic_object_name;                                /**< OTS characteristic OBJECT_NAME. */
extern const ble_att_param_t att_ots_object_name;                                               /**< OTS OBJECT_NAME value. */
extern const ble_att_param_t att_ots_characteristic_object_type;                                /**< OTS characteristic OBJECT_TYPE. */
extern const ble_att_param_t att_ots_object_type;                                               /**< OTS OBJECT_TYPE value. */
extern const ble_att_param_t att_ots_characteristic_object_size;                                /**< OTS characteristic OBJECT_SIZE. */
extern const ble_att_param_t att_ots_object_size;                                               /**< OTS OBJECT_SIZE value. */
extern const ble_att_param_t att_ots_characteristic_object_first_created;                       /**< OTS characteristic OBJECT_FIRST_CREATED. */
extern const ble_att_param_t att_ots_object_first_created;                                      /**< OTS OBJECT_FIRST_CREATED value. */
extern const ble_att_param_t att_ots_characteristic_object_last_modified;                       /**< OTS characteristic OBJECT_LAST_MODIFIED. */
extern const ble_att_param_t att_ots_object_last_modified;                                      /**< OTS OBJECT_LAST_MODIFIED value. */
extern const ble_att_param_t att_ots_characteristic_object_id;                                  /**< OTS characteristic OBJECT_ID. */
extern const ble_att_param_t att_ots_object_id;                                                 /**< OTS OBJECT_ID value. */
extern const ble_att_param_t att_ots_characteristic_object_properties;                          /**< OTS characteristic OBJECT_PROPERTIES. */
extern const ble_att_param_t att_ots_object_properties;                                         /**< OTS OBJECT_PROPERTIES value. */
extern const ble_att_param_t att_ots_characteristic_object_action_control_point;                /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT. */
extern const ble_att_param_t att_ots_object_action_control_point;                               /**< OTS OBJECT_ACTION_CONTROL_POINT value. */
extern const ble_att_param_t att_ots_object_action_control_point_client_charc_configuration;    /**< OTS OBJECT_ACTION_CONTROL_POINT client characteristic configuration. */
extern const ble_att_param_t att_ots_characteristic_object_list_control_point;                  /**< OTS characteristic OBJECT_LIST_CONTROL_POINT. */
extern const ble_att_param_t att_ots_object_list_control_point;                                 /**< OTS OBJECT_LIST_CONTROL_POINT value. */
extern const ble_att_param_t att_ots_object_list_control_point_client_charc_configuration;      /**< OTS OBJECT_LIST_CONTROL_POINT client characteristic configuration. */
extern const ble_att_param_t att_ots_characteristic_object_list_filter;                         /**< OTS characteristic OBJECT_LIST_FILTER. */
extern const ble_att_param_t att_ots_object_list_filter;                                        /**< OTS OBJECT_LIST_FILTER value. */
extern const ble_att_param_t att_ots_characteristic_object_changed;                             /**< OTS characteristic OBJECT_CHANGED. */
extern const ble_att_param_t att_ots_object_changed;                                            /**< OTS OBJECT_CHANGED value. */
extern const ble_att_param_t att_ots_object_changed_client_charc_configuration;                 /**< OTS OBJECT_CHANGED client characteristic configuration. */
/** @} */


/** OTS Definition
 * @ingroup service_ots_ServiceChardef
*/
#define ATT_OTS_SERVICE                                                       \
    &att_ots_primary_service,                                                 \
    &att_ots_characteristic_ots_feature,                                      \
    &att_ots_ots_feature,                                                     \
    &att_ots_characteristic_object_name,                                      \
    &att_ots_object_name,                                                     \
    &att_ots_characteristic_object_type,                                      \
    &att_ots_object_type,                                                     \
    &att_ots_characteristic_object_size,                                      \
    &att_ots_object_size,                                                     \
    &att_ots_characteristic_object_first_created,                             \
    &att_ots_object_first_created,                                            \
    &att_ots_characteristic_object_last_modified,                             \
    &att_ots_object_last_modified,                                            \
    &att_ots_characteristic_object_id,                                        \
    &att_ots_object_id,                                                       \
    &att_ots_characteristic_object_properties,                                \
    &att_ots_object_properties,                                               \
    &att_ots_characteristic_object_action_control_point,                      \
    &att_ots_object_action_control_point,                                     \
    &att_ots_object_action_control_point_client_charc_configuration,          \
    &att_ots_characteristic_object_list_control_point,                        \
    &att_ots_object_list_control_point,                                       \
    &att_ots_object_list_control_point_client_charc_configuration,            \
    &att_ots_characteristic_object_list_filter,                               \
    &att_ots_object_list_filter,                                              \
    &att_ots_characteristic_object_changed,                                   \
    &att_ots_object_changed,                                                  \
    &att_ots_object_changed_client_charc_configuration                        \


/**************************************************************************
 * OTS Application Definitions
 **************************************************************************/
/** @defgroup app_ots_def BLE OTS Application Definitions
 * @{
 * @ingroup service_ots_def
 * @details Here shows the definitions of the OTS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_ots_def
 * @defgroup app_ots_eventDef BLE OTS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the OTS.
*/
#define BLESERVICE_OTS_OTS_FEATURE_READ_EVENT                                 0x01     /**< OTS characteristic OTS_FEATURE read event.*/
#define BLESERVICE_OTS_OTS_FEATURE_READ_RSP_EVENT                             0x02     /**< OTS characteristic OTS_FEATURE read response event.*/
#define BLESERVICE_OTS_OBJECT_NAME_READ_EVENT                                 0x03     /**< OTS characteristic OBJECT_NAME read event.*/
#define BLESERVICE_OTS_OBJECT_NAME_READ_RSP_EVENT                             0x04     /**< OTS characteristic OBJECT_NAME read response event.*/
#define BLESERVICE_OTS_OBJECT_NAME_WRITE_EVENT                                0x05     /**< OTS characteristic OBJECT_NAME write event.*/
#define BLESERVICE_OTS_OBJECT_NAME_WRITE_RSP_EVENT                            0x06     /**< OTS characteristic OBJECT_NAME write response event.*/
#define BLESERVICE_OTS_OBJECT_TYPE_READ_EVENT                                 0x07     /**< OTS characteristic OBJECT_TYPE read event.*/
#define BLESERVICE_OTS_OBJECT_TYPE_READ_RSP_EVENT                             0x08     /**< OTS characteristic OBJECT_TYPE read response event.*/
#define BLESERVICE_OTS_OBJECT_SIZE_READ_EVENT                                 0x09     /**< OTS characteristic OBJECT_SIZE read event.*/
#define BLESERVICE_OTS_OBJECT_SIZE_READ_RSP_EVENT                             0x0a     /**< OTS characteristic OBJECT_SIZE read response event.*/
#define BLESERVICE_OTS_OBJECT_FIRST_CREATED_READ_EVENT                        0x0b     /**< OTS characteristic OBJECT_FIRST_CREATED read event.*/
#define BLESERVICE_OTS_OBJECT_FIRST_CREATED_READ_RSP_EVENT                    0x0c     /**< OTS characteristic OBJECT_FIRST_CREATED read response event.*/
#define BLESERVICE_OTS_OBJECT_FIRST_CREATED_WRITE_EVENT                       0x0d     /**< OTS characteristic OBJECT_FIRST_CREATED write event.*/
#define BLESERVICE_OTS_OBJECT_FIRST_CREATED_WRITE_RSP_EVENT                   0x0e     /**< OTS characteristic OBJECT_FIRST_CREATED write response event.*/
#define BLESERVICE_OTS_OBJECT_LAST_MODIFIED_READ_EVENT                        0x0f     /**< OTS characteristic OBJECT_LAST_MODIFIED read event.*/
#define BLESERVICE_OTS_OBJECT_LAST_MODIFIED_READ_RSP_EVENT                    0x10     /**< OTS characteristic OBJECT_LAST_MODIFIED read response event.*/
#define BLESERVICE_OTS_OBJECT_LAST_MODIFIED_WRITE_EVENT                       0x11     /**< OTS characteristic OBJECT_LAST_MODIFIED write event.*/
#define BLESERVICE_OTS_OBJECT_LAST_MODIFIED_WRITE_RSP_EVENT                   0x12     /**< OTS characteristic OBJECT_LAST_MODIFIED write response event.*/
#define BLESERVICE_OTS_OBJECT_ID_READ_EVENT                                   0x13     /**< OTS characteristic OBJECT_ID read event.*/
#define BLESERVICE_OTS_OBJECT_ID_READ_RSP_EVENT                               0x14     /**< OTS characteristic OBJECT_ID read response event.*/
#define BLESERVICE_OTS_OBJECT_PROPERTIES_READ_EVENT                           0x15     /**< OTS characteristic OBJECT_PROPERTIES read event.*/
#define BLESERVICE_OTS_OBJECT_PROPERTIES_READ_RSP_EVENT                       0x16     /**< OTS characteristic OBJECT_PROPERTIES read response event.*/
#define BLESERVICE_OTS_OBJECT_PROPERTIES_WRITE_EVENT                          0x17     /**< OTS characteristic OBJECT_PROPERTIES write event.*/
#define BLESERVICE_OTS_OBJECT_PROPERTIES_WRITE_RSP_EVENT                      0x18     /**< OTS characteristic OBJECT_PROPERTIES write response event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_WRITE_EVENT                0x19     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT write event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_WRITE_RSP_EVENT            0x1a     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT write response event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_CONFIRM_EVENT     0x1b     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT indicate confirm event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_EVENT             0x1c     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT indicate event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_READ_EVENT            0x1d     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT cccd read event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_READ_RSP_EVENT        0x1e     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT cccd read response event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_WRITE_EVENT           0x1f     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT cccd write event.*/
#define BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_CCCD_WRITE_RSP_EVENT       0x20     /**< OTS characteristic OBJECT_ACTION_CONTROL_POINT cccd write response event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_WRITE_EVENT                  0x21     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT write event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_WRITE_RSP_EVENT              0x22     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT write response event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_CONFIRM_EVENT       0x23     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT indicate confirm event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_EVENT               0x24     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT indicate event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_READ_EVENT              0x25     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT cccd read event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_READ_RSP_EVENT          0x26     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT cccd read response event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_WRITE_EVENT             0x27     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT cccd write event.*/
#define BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_CCCD_WRITE_RSP_EVENT         0x28     /**< OTS characteristic OBJECT_LIST_CONTROL_POINT cccd write response event.*/
#define BLESERVICE_OTS_OBJECT_LIST_FILTER_READ_EVENT                          0x29     /**< OTS characteristic OBJECT_LIST_FILTER read event.*/
#define BLESERVICE_OTS_OBJECT_LIST_FILTER_READ_RSP_EVENT                      0x2a     /**< OTS characteristic OBJECT_LIST_FILTER read response event.*/
#define BLESERVICE_OTS_OBJECT_LIST_FILTER_WRITE_EVENT                         0x2b     /**< OTS characteristic OBJECT_LIST_FILTER write event.*/
#define BLESERVICE_OTS_OBJECT_LIST_FILTER_WRITE_RSP_EVENT                     0x2c     /**< OTS characteristic OBJECT_LIST_FILTER write response event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_INDICATE_CONFIRM_EVENT                  0x2d     /**< OTS characteristic OBJECT_CHANGED indicate confirm event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_INDICATE_EVENT                          0x2e     /**< OTS characteristic OBJECT_CHANGED indicate event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_CCCD_READ_EVENT                         0x2f     /**< OTS characteristic OBJECT_CHANGED cccd read event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_CCCD_READ_RSP_EVENT                     0x30     /**< OTS characteristic OBJECT_CHANGED cccd read response event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_CCCD_WRITE_EVENT                        0x31     /**< OTS characteristic OBJECT_CHANGED cccd write event.*/
#define BLESERVICE_OTS_OBJECT_CHANGED_CCCD_WRITE_RSP_EVENT                    0x32     /**< OTS characteristic OBJECT_CHANGED cccd write response event.*/
/** @} */

typedef uint8_t oacp_opcode_t;
#define OACP_OPCODE_CREATE             0x01
#define OACP_OPCODE_DELETE             0x02
#define OACP_OPCODE_CALCULATE_CHECKSUM 0x03
#define OACP_OPCODE_EXECUTE            0x04
#define OACP_OPCODE_READ               0x05
#define OACP_OPCODE_WRITE              0x06
#define OACP_OPCODE_ABORT              0x07
#define OACP_OPCODE_RESPONSE           0x60

typedef uint8_t oacp_result_code_t;
#define OACP_RESULT_CODE_SUCCESS                  0x01
#define OACP_RESULT_CODE_OPCODE_NOT_SUPPORT       0x02
#define OACP_RESULT_CODE_INVALID_PARAMETER        0x03
#define OACP_RESULT_CODE_INSUFFICIENT_RESOURCES   0x04
#define OACP_RESULT_CODE_INVALID_OBJECT           0x05
#define OACP_RESULT_CODE_CHANNEL_UNAVAILABLE      0x06
#define OACP_RESULT_CODE_UNSUPPORTED_TYPE         0x07
#define OACP_RESULT_CODE_PROCEDURE_NOT_PERMITTED  0x08
#define OACP_RESULT_CODE_OBJECT_LOCKED            0x09
#define OACP_RESULT_CODE_OPERATION_FAILED         0x0A

typedef uint8_t olcp_opcode_t;
#define OLCP_OPCODE_FIRST              0x01
#define OLCP_OPCODE_LAST               0x02
#define OLCP_OPCODE_PREVIOUS           0x03
#define OLCP_OPCODE_NEXT               0x04
#define OLCP_OPCODE_GO_TO              0x05
#define OLCP_OPCODE_ORDER              0x06
#define OLCP_OPCODE_REQ_NUM_OF_OBJECTS 0x07
#define OLCP_OPCODE_CLEAR_MARKING      0x08
#define OLCP_OPCODE_RESPONSE           0x70

typedef uint8_t olcp_result_code_t;
#define OLCP_RESULT_CODE_SUCCESS              0x01
#define OLCP_RESULT_CODE_OPCODE_NOT_SUPPORT   0x02
#define OLCP_RESULT_CODE_INVALID_PARAMETER    0x03
#define OLCP_RESULT_CODE_OPERATION_FAILED     0x04
#define OLCP_RESULT_CODE_OUT_OF_BOUNDS        0x05
#define OLCP_RESULT_CODE_TOO_MANY_OBJECTS     0x06
#define OLCP_RESULT_CODE_NO_OBJECT            0x07
#define OLCP_RESULT_CODE_OBJECT_ID_NOT_FOUND  0x08

#define BLESERVICE_OTS_PSM                    0x0025
/**
 * @ingroup app_ots_def
 * @defgroup app_ots_structureDef BLE OTS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the OTS.
 * @}
*/

/** OTS Handles Definition
 * @ingroup app_ots_structureDef
*/
typedef struct ble_svcs_ots_handles_s
{
    uint16_t hdl_ots_feature;                         /**< Handle of OTS_FEATURE. */
    uint16_t hdl_object_name;                         /**< Handle of OBJECT_NAME. */
    uint16_t hdl_object_type;                         /**< Handle of OBJECT_TYPE. */
    uint16_t hdl_object_size;                         /**< Handle of OBJECT_SIZE. */
    uint16_t hdl_object_first_created;                /**< Handle of OBJECT_FIRST_CREATED. */
    uint16_t hdl_object_last_modified;                /**< Handle of OBJECT_LAST_MODIFIED. */
    uint16_t hdl_object_id;                           /**< Handle of OBJECT_ID. */
    uint16_t hdl_object_properties;                   /**< Handle of OBJECT_PROPERTIES. */
    uint16_t hdl_object_action_control_point;         /**< Handle of OBJECT_ACTION_CONTROL_POINT. */
    uint16_t hdl_object_action_control_point_cccd;    /**< Handle of OBJECT_ACTION_CONTROL_POINT client characteristic configuration. */
    uint16_t hdl_object_list_control_point;           /**< Handle of OBJECT_LIST_CONTROL_POINT. */
    uint16_t hdl_object_list_control_point_cccd;      /**< Handle of OBJECT_LIST_CONTROL_POINT client characteristic configuration. */
    uint16_t hdl_object_list_filter;                  /**< Handle of OBJECT_LIST_FILTER. */
    uint16_t hdl_object_changed;                      /**< Handle of OBJECT_CHANGED. */
    uint16_t hdl_object_changed_cccd;                 /**< Handle of OBJECT_CHANGED client characteristic configuration. */
} ble_svcs_ots_handles_t;


/** OTS Data Definition
 * @ingroup app_ots_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_ots_data_s
{
    uint16_t object_action_control_point_cccd;    /**< OBJECT_ACTION_CONTROL_POINT cccd value */
    uint16_t object_list_control_point_cccd;      /**< OBJECT_LIST_CONTROL_POINT cccd value */
    uint16_t object_changed_cccd;                 /**< OBJECT_CHANGED cccd value */
} ble_svcs_ots_data_t;


/** OTS Application Data Structure Definition
 * @ingroup app_ots_structureDef
*/
typedef struct ble_svcs_ots_subinfo_s
{
    ble_svcs_ots_handles_t handles;    /**< OTS attribute handles */
    ble_svcs_ots_data_t    data;       /**< OTS attribute data */
} ble_svcs_ots_subinfo_t;

typedef struct ble_svcs_ots_info_s
{
    ble_gatt_role_t        role;           /**< BLE GATT role */
    ble_svcs_ots_subinfo_t client_info;    /**< Client Information */
    ble_svcs_ots_subinfo_t server_info;    /**< Server Information */
} ble_svcs_ots_info_t;

typedef struct __attribute__((packed)) ots_oacp_create_s
{
    uint32_t size;
    uint8_t gatt_uuid[16];
}ots_oacp_create_t;

typedef struct __attribute__((packed)) ots_oacp_calculate_checksum_s
{
    uint32_t offset;
    uint32_t length;
}ots_oacp_calculate_checksum_t;

typedef struct __attribute__((packed)) ots_oacp_calculate_read_s
{
    uint32_t offset;
    uint32_t length;
}ots_oacp_calculate_read_t;

typedef struct __attribute__((packed)) ots_oacp_calculate_write_s
{
    uint32_t offset;
    uint32_t length;
    uint8_t  rfu:1,
             mode:1,
             rfu_1:6;
}ots_oacp_calculate_write_t;


typedef struct __attribute__((packed)) ots_oacp_s
{
    uint8_t opcode;
    union
    {
        ots_oacp_create_t create;
        ots_oacp_calculate_checksum_t calculate_checksum;
        ots_oacp_calculate_read_t read;
        ots_oacp_calculate_write_t write;
    }param;

}ots_oacp_t;

typedef struct __attribute__((packed)) ots_oacp_response_s
{
    oacp_opcode_t       op_code;
    oacp_opcode_t       req_op_code;
    oacp_result_code_t  result_code;
    uint8_t             rsp_param[];
}ots_oacp_response_t;

typedef struct __attribute__((packed)) ots_olcp_go_to_s
{
    uint8_t  object_id[6];
}ots_olcp_go_to_t;

typedef struct __attribute__((packed)) ots_olcp_order_s
{
    uint8_t  list_short_order;
}ots_olcp_order_t;

typedef struct __attribute__((packed)) ots_olcp_s
{
    uint8_t opcode;
    union
    {
        ots_olcp_go_to_t go_to;
        ots_olcp_order_t list_sort_order;
    }param;
}ots_olcp_t;

typedef struct __attribute__((packed)) ots_olcp_response_s
{
    olcp_opcode_t       op_code;
    olcp_opcode_t       req_op_code;
    olcp_result_code_t  result_code;
    uint8_t             rsp_param[];
}ots_olcp_response_t;

/**
 * @ingroup app_ots_def
 * @defgroup app_ots_App BLE OTS Definitions for Application
 * @{
 * @details Here shows the definitions of the OTS for application uses.
 * @}
*/

/** ble_svcs_evt_ots_handler_t
 * @ingroup app_ots_App
 * @note This callback receives the OTS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_ots_handler_t)(ble_evt_att_param_t *p_param);

/** OTS Initialization
*
* @ingroup app_ots_App
*
* @attention There is only one instance of OTS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to OTS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_ots_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_ots_info_t *p_info, ble_svcs_evt_ots_handler_t callback);

/** Get OTS Handle Numbers
*
* @ingroup app_ots_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to OTS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_ots_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_ots_info_t *p_info);

#endif //_BLE_SERVICE_OTS_H_

