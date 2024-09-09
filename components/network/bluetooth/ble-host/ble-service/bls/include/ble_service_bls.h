#ifndef _BLE_SERVICE_BLS_H_
#define _BLE_SERVICE_BLS_H_

/**************************************************************************//**
 * @file  ble_service_bls.h
 * @brief Provide the Definition of BLS.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * BLS Definitions
 **************************************************************************/
/** @defgroup service_bls_def BLE BLS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the BLS.
 * @}
**************************************************************************/

/**
 * @ingroup service_bls_def
 * @defgroup service_bls_UUIDDef BLE BLS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE BLS UUID Definitions.
*/
extern const uint16_t attr_uuid_bls_primary_service[];                     /**< BLS service UUID. */
extern const uint16_t attr_uuid_bls_charc_blood_pressure_measurement[];    /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT UUID. */
extern const uint16_t attr_uuid_bls_charc_intermediate_cuff_pressure[];    /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE UUID. */
extern const uint16_t attr_uuid_bls_charc_blood_pressure_feature[];        /**< BLS characteristic BLOOD_PRESSURE_FEATURE UUID. */
/** @} */

/**
 * @defgroup service_bls_ServiceChardef BLE BLS Service and Characteristic Definitions
 * @{
 * @ingroup service_bls_def
 * @details Here shows the definitions of the BLS service and characteristic.
 * @}
*/

/**
 * @ingroup service_bls_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_bls_primary_service;                                          /**< BLS primary service. */
extern const ble_att_param_t att_bls_characteristic_blood_pressure_measurement;                /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT. */
extern const ble_att_param_t att_bls_blood_pressure_measurement;                               /**< BLS BLOOD_PRESSURE_MEASUREMENT value. */
extern const ble_att_param_t att_bls_blood_pressure_measurement_client_charc_configuration;    /**< BLS BLOOD_PRESSURE_MEASUREMENT client characteristic configuration. */
extern const ble_att_param_t att_bls_characteristic_intermediate_cuff_pressure;                /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE. */
extern const ble_att_param_t att_bls_intermediate_cuff_pressure;                               /**< BLS INTERMEDIATE_CUFF_PRESSURE value. */
extern const ble_att_param_t att_bls_intermediate_cuff_pressure_client_charc_configuration;    /**< BLS INTERMEDIATE_CUFF_PRESSURE client characteristic configuration. */
extern const ble_att_param_t att_bls_characteristic_blood_pressure_feature;                    /**< BLS characteristic BLOOD_PRESSURE_FEATURE. */
extern const ble_att_param_t att_bls_blood_pressure_feature;                                   /**< BLS BLOOD_PRESSURE_FEATURE value. */
/** @} */


/** BLS Definition
 * @ingroup service_bls_ServiceChardef
*/
#define ATT_BLS_SERVICE                                                      \
    &att_bls_primary_service,                                                \
    &att_bls_characteristic_blood_pressure_measurement,                      \
    &att_bls_blood_pressure_measurement,                                     \
    &att_bls_blood_pressure_measurement_client_charc_configuration,          \
    &att_bls_characteristic_intermediate_cuff_pressure,                      \
    &att_bls_intermediate_cuff_pressure,                                     \
    &att_bls_intermediate_cuff_pressure_client_charc_configuration,          \
    &att_bls_characteristic_blood_pressure_feature,                          \
    &att_bls_blood_pressure_feature                                          \


/**************************************************************************
 * BLS Application Definitions
 **************************************************************************/
/** @defgroup app_bls_def BLE BLS Application Definitions
 * @{
 * @ingroup service_bls_def
 * @details Here shows the definitions of the BLS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_bls_def
 * @defgroup app_bls_eventDef BLE BLS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the BLS.
*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_INDICATE_CONFIRM_EVENT     0x01     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT indicate confirm event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_INDICATE_EVENT             0x02     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT indicate event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_READ_EVENT            0x03     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT cccd read event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_READ_RSP_EVENT        0x04     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT cccd read response event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_WRITE_EVENT           0x05     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT cccd write event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_MEASUREMENT_CCCD_WRITE_RSP_EVENT       0x06     /**< BLS characteristic BLOOD_PRESSURE_MEASUREMENT cccd write response event.*/
#define BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_NOTIFY_EVENT               0x07     /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE notify event.*/
#define BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_READ_EVENT            0x08     /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE cccd read event.*/
#define BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_READ_RSP_EVENT        0x09     /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE cccd read response event.*/
#define BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_WRITE_EVENT           0x0a     /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE cccd write event.*/
#define BLESERVICE_BLS_INTERMEDIATE_CUFF_PRESSURE_CCCD_WRITE_RSP_EVENT       0x0b     /**< BLS characteristic INTERMEDIATE_CUFF_PRESSURE cccd write response event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_FEATURE_READ_EVENT                     0x0c     /**< BLS characteristic BLOOD_PRESSURE_FEATURE read event.*/
#define BLESERVICE_BLS_BLOOD_PRESSURE_FEATURE_READ_RSP_EVENT                 0x0d     /**< BLS characteristic BLOOD_PRESSURE_FEATURE read response event.*/
/** @} */


#define BLEBLS_FEAT_BODY_MOVEMENT_DETECTION                            0x0001
#define BLEBLS_FEAT_CUFF_FIT_DETECTION                                 0x0002
#define BLEBLS_FEAT_IRREGULAR_PULSE_DETECTION                          0x0004
#define BLEBLS_FEAT_PULSE_RATE_RANGE_DETECTION                         0x0008
#define BLEBLS_FEAT_MEASUREMENT_POSITION_DETECTION                     0x0010
#define BLEBLS_FEAT_MULTIPLE_BOND                                      0x0020

/**
 * @ingroup app_bls_def
 * @defgroup app_bls_structureDef BLE BLS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the BLS.
 * @}
*/

/** BLS Handles Definition
 * @ingroup app_bls_structureDef
*/
typedef struct ble_svcs_bls_handles_s
{
    uint16_t hdl_blood_pressure_measurement;         /**< Handle of BLOOD_PRESSURE_MEASUREMENT. */
    uint16_t hdl_blood_pressure_measurement_cccd;    /**< Handle of BLOOD_PRESSURE_MEASUREMENT client characteristic configuration. */
    uint16_t hdl_intermediate_cuff_pressure;         /**< Handle of INTERMEDIATE_CUFF_PRESSURE. */
    uint16_t hdl_intermediate_cuff_pressure_cccd;    /**< Handle of INTERMEDIATE_CUFF_PRESSURE client characteristic configuration. */
    uint16_t hdl_blood_pressure_feature;             /**< Handle of BLOOD_PRESSURE_FEATURE. */
} ble_svcs_bls_handles_t;


/** BLS Data Definition
 * @ingroup app_bls_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_bls_data_s
{
    uint16_t blood_pressure_measurement_cccd;    /**< BLOOD_PRESSURE_MEASUREMENT cccd value */
    uint16_t intermediate_cuff_pressure_cccd;    /**< INTERMEDIATE_CUFF_PRESSURE cccd value */
} ble_svcs_bls_data_t;


/** BLS Application Data Structure Definition
 * @ingroup app_bls_structureDef
*/
typedef struct ble_svcs_bls_subinfo_s
{
    ble_svcs_bls_handles_t handles;    /**< BLS attribute handles */
    ble_svcs_bls_data_t    data;       /**< BLS attribute data */
} ble_svcs_bls_subinfo_t;

typedef struct ble_svcs_bls_info_s
{
    ble_gatt_role_t        role;           /**< BLE GATT role */
    ble_svcs_bls_subinfo_t client_info;    /**< Client Information */
    ble_svcs_bls_subinfo_t server_info;    /**< Server Information */
} ble_svcs_bls_info_t;


/**
 * @ingroup app_bls_def
 * @defgroup app_bls_App BLE BLS Definitions for Application
 * @{
 * @details Here shows the definitions of the BLS for application uses.
 * @}
*/

/** ble_svcs_evt_bls_handler_t
 * @ingroup app_bls_App
 * @note This callback receives the BLS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_bls_handler_t)(ble_evt_att_param_t *p_param);

/** BLS Initialization
*
* @ingroup app_bls_App
*
* @attention There is only one instance of BLS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to BLS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_bls_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bls_info_t *p_info, ble_svcs_evt_bls_handler_t callback);

/** Get BLS Handle Numbers
*
* @ingroup app_bls_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to BLS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_bls_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bls_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_BLS_H_ */

