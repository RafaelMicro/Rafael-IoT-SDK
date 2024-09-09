#ifndef _BLE_SERVICE_HRS_H_
#define _BLE_SERVICE_HRS_H_

/**************************************************************************//**
 * @file  ble_service_hrs.h
 * @brief Provide the Definition of HRS.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * HRS Definitions
 **************************************************************************/
/** @defgroup service_hrs_def BLE HRS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the HRS.
 * @}
**************************************************************************/

/**
 * @ingroup service_hrs_def
 * @defgroup service_hrs_UUIDDef BLE HRS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE HRS UUID Definitions.
*/
extern const uint16_t attr_uuid_hrs_primary_service[];                 /**< HRS service UUID. */
extern const uint16_t attr_uuid_hrs_charc_heart_rate_measurement[];    /**< HRS characteristic HEART_RATE_MEASUREMENT UUID. */
extern const uint16_t attr_uuid_hrs_charc_body_sensor_location[];      /**< HRS characteristic BODY_SENSOR_LOCATION UUID. */
/** @} */

/**
 * @defgroup service_hrs_ServiceChardef BLE HRS Service and Characteristic Definitions
 * @{
 * @ingroup service_hrs_def
 * @details Here shows the definitions of the HRS service and characteristic.
 * @}
*/

/**
 * @ingroup service_hrs_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_hrs_primary_service;                                      /**< HRS primary service. */
extern const ble_att_param_t att_hrs_characteristic_heart_rate_measurement;                /**< HRS characteristic HEART_RATE_MEASUREMENT. */
extern const ble_att_param_t att_hrs_heart_rate_measurement;                               /**< HRS HEART_RATE_MEASUREMENT value. */
extern const ble_att_param_t att_hrs_heart_rate_measurement_client_charc_configuration;    /**< HRS HEART_RATE_MEASUREMENT client characteristic configuration. */
extern const ble_att_param_t att_hrs_characteristic_body_sensor_location;                  /**< HRS characteristic BODY_SENSOR_LOCATION. */
extern const ble_att_param_t att_hrs_body_sensor_location;                                 /**< HRS BODY_SENSOR_LOCATION value. */
/** @} */


/** HRS Definition
 * @ingroup service_hrs_ServiceChardef
*/
#define ATT_HRS_SERVICE                                                  \
    &att_hrs_primary_service,                                            \
    &att_hrs_characteristic_heart_rate_measurement,                      \
    &att_hrs_heart_rate_measurement,                                     \
    &att_hrs_heart_rate_measurement_client_charc_configuration,          \
    &att_hrs_characteristic_body_sensor_location,                        \
    &att_hrs_body_sensor_location                                        \


/**************************************************************************
 * HRS Application Definitions
 **************************************************************************/
/** @defgroup app_hrs_def BLE HRS Application Definitions
 * @{
 * @ingroup service_hrs_def
 * @details Here shows the definitions of the HRS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_hrs_def
 * @defgroup app_hrs_eventDef BLE HRS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the HRS.
*/
#define BLESERVICE_HRS_HEART_RATE_MEASUREMENT_NOTIFY_EVENT             0x01     /**< HRS characteristic HEART_RATE_MEASUREMENT notify event.*/
#define BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_READ_EVENT          0x02     /**< HRS characteristic HEART_RATE_MEASUREMENT cccd read event.*/
#define BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_READ_RSP_EVENT      0x03     /**< HRS characteristic HEART_RATE_MEASUREMENT cccd read response event.*/
#define BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_WRITE_EVENT         0x04     /**< HRS characteristic HEART_RATE_MEASUREMENT cccd write event.*/
#define BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_WRITE_RSP_EVENT     0x05     /**< HRS characteristic HEART_RATE_MEASUREMENT cccd write response event.*/
#define BLESERVICE_HRS_BODY_SENSOR_LOCATION_READ_EVENT                 0x06     /**< HRS characteristic BODY_SENSOR_LOCATION read event.*/
#define BLESERVICE_HRS_BODY_SENSOR_LOCATION_READ_RSP_EVENT             0x07     /**< HRS characteristic BODY_SENSOR_LOCATION read response event.*/
/** @} */


/**
 * @ingroup app_hrs_def
 * @defgroup app_hrs_structureDef BLE HRS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the HRS.
 * @}
*/

/** HRS Handles Definition
 * @ingroup app_hrs_structureDef
*/
typedef struct ble_svcs_hrs_handles_s
{
    uint16_t hdl_heart_rate_measurement;         /**< Handle of HEART_RATE_MEASUREMENT. */
    uint16_t hdl_heart_rate_measurement_cccd;    /**< Handle of HEART_RATE_MEASUREMENT client characteristic configuration. */
    uint16_t hdl_body_sensor_location;           /**< Handle of BODY_SENSOR_LOCATION. */
} ble_svcs_hrs_handles_t;


/** HRS Data Definition
 * @ingroup app_hrs_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_hrs_data_s
{
    uint8_t  heart_rate_measurement[4];      /**< HEART_RATE_MEASUREMENT Heart rate measurement value: [0]: HRS Flag; [1]: Heart Rate Data [3][2]: Heart Rate RR-Interval */
    uint16_t heart_rate_measurement_cccd;    /**< HEART_RATE_MEASUREMENT cccd value */
    uint8_t   body_sensor_location;          /**< HEART_RATE_MEASUREMENT Body sensor location value */
} ble_svcs_hrs_data_t;


/** HRS Application Data Structure Definition
 * @ingroup app_hrs_structureDef
*/
typedef struct ble_svcs_hrs_subinfo_s
{
    ble_svcs_hrs_handles_t handles;    /**< HRS attribute handles */
    ble_svcs_hrs_data_t    data;       /**< HRS attribute data */
} ble_svcs_hrs_subinfo_t;


typedef struct ble_svcs_hrs_info_s
{
    ble_gatt_role_t           role;         /**< BLE GATT role */
    ble_svcs_hrs_subinfo_t    client_info;
    ble_svcs_hrs_subinfo_t    server_info;
} ble_svcs_hrs_info_t;


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/

/**
 * @ingroup app_hrs_def
 * @defgroup app_hrs_App BLE HRS Definitions for Application
 * @{
 * @details Here shows the definitions of the HRS for application uses.
 * @}
*/

/** ble_svcs_evt_hrs_handler_t
 * @ingroup app_hrs_App
 * @note This callback receives the HRS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_hrs_handler_t)(ble_evt_att_param_t *p_param);

/** HRS Initialization
*
* @ingroup app_hrs_App
*
* @attention There is only one instance of HRS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to HRS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_hrs_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_hrs_info_t *p_info, ble_svcs_evt_hrs_handler_t callback);

/** Get HRS Handle Numbers
*
* @ingroup app_hrs_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to HRS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_hrs_handles_get(uint8_t host_id,  ble_gatt_role_t role, ble_svcs_hrs_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_HRS_H_ */