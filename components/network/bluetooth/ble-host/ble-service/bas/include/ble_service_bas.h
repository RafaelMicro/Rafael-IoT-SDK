#ifndef _BLE_SERVICE_BAS_H_
#define _BLE_SERVICE_BAS_H_


/**************************************************************************//**
 * @file  ble_service_bas.h
 * @brief Provide the Definition of BAS.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * BAS Definitions
 **************************************************************************/
/** @defgroup service_bas_def BLE BAS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the BAS.
 * @}
**************************************************************************/

/**
 * @ingroup service_bas_def
 * @defgroup service_bas_UUIDDef BLE BAS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE BAS UUID Definitions.
*/
extern const uint16_t attr_uuid_bas_primary_service[];        /**< BAS service UUID. */
extern const uint16_t attr_uuid_bas_charc_battery_level[];    /**< BAS characteristic BATTERY_LEVEL UUID. */
/** @} */

/**
 * @defgroup service_bas_ServiceChardef BLE BAS Service and Characteristic Definitions
 * @{
 * @ingroup service_bas_def
 * @details Here shows the definitions of the BAS service and characteristic.
 * @}
*/

/**
 * @ingroup service_bas_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_bas_primary_service;                             /**< BAS primary service. */
extern const ble_att_param_t att_bas_characteristic_battery_level;                /**< BAS characteristic BATTERY_LEVEL. */
extern const ble_att_param_t att_bas_battery_level;                               /**< BAS BATTERY_LEVEL value. */
extern const ble_att_param_t att_bas_battery_level_client_charc_configuration;    /**< BAS BATTERY_LEVEL client characteristic configuration. */
/** @} */


/** BAS Definition
 * @ingroup service_bas_ServiceChardef
*/
#define ATT_BAS_SERVICE                                        \
    &att_bas_primary_service,                                  \
    &att_bas_characteristic_battery_level,                     \
    &att_bas_battery_level,                                    \
    &att_bas_battery_level_client_charc_configuration          \


/**************************************************************************
 * BAS Application Definitions
 **************************************************************************/
/** @defgroup app_bas_def BLE BAS Application Definitions
 * @{
 * @ingroup service_bas_def
 * @details Here shows the definitions of the BAS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_bas_def
 * @defgroup app_bas_eventDef BLE BAS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the BAS.
*/
#define BLESERVICE_BAS_BATTERY_LEVEL_READ_EVENT               0x01     /**< BAS characteristic BATTERY_LEVEL read event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_READ_RSP_EVENT           0x02     /**< BAS characteristic BATTERY_LEVEL read response event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_NOTIFY_EVENT             0x03     /**< BAS characteristic BATTERY_LEVEL notify event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_CCCD_READ_EVENT          0x04     /**< BAS characteristic BATTERY_LEVEL cccd read event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_CCCD_READ_RSP_EVENT      0x05     /**< BAS characteristic BATTERY_LEVEL cccd read response event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_CCCD_WRITE_EVENT         0x06     /**< BAS characteristic BATTERY_LEVEL cccd write event.*/
#define BLESERVICE_BAS_BATTERY_LEVEL_CCCD_WRITE_RSP_EVENT     0x07     /**< BAS characteristic BATTERY_LEVEL cccd write response event.*/
/** @} */


/**
 * @ingroup app_bas_def
 * @defgroup app_bas_structureDef BLE BAS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the BAS.
 * @}
*/

/** BAS Handles Definition
 * @ingroup app_bas_structureDef
*/
typedef struct ble_svcs_bas_handles_s
{
    uint16_t hdl_battery_level;         /**< Handle of BATTERY_LEVEL. */
    uint16_t hdl_battery_level_cccd;    /**< Handle of BATTERY_LEVEL client characteristic configuration. */
} ble_svcs_bas_handles_t;


/** BAS Data Definition
 * @ingroup app_bas_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_bas_data_s
{
    uint16_t battery_level_cccd;    /**< BATTERY_LEVEL cccd value */
} ble_svcs_bas_data_t;


/** BAS Application Data Structure Definition
 * @ingroup app_bas_structureDef
*/
typedef struct ble_svcs_bas_info_s
{
    ble_gatt_role_t        role;       /**< BLE GATT role */
    ble_svcs_bas_handles_t handles;    /**< BAS attribute handles */
    ble_svcs_bas_data_t    data;       /**< BAS attribute data */
} ble_svcs_bas_info_t;


/**
 * @ingroup app_bas_def
 * @defgroup app_bas_App BLE BAS Definitions for Application
 * @{
 * @details Here shows the definitions of the BAS for application uses.
 * @}
*/

/** ble_svcs_evt_bas_handler_t
 * @ingroup app_bas_App
 * @note This callback receives the BAS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_bas_handler_t)(ble_evt_att_param_t *p_param);

/** BAS Initialization
*
* @ingroup app_bas_App
*
* @attention There is only one instance of BAS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to BAS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_bas_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_bas_info_t *p_info, ble_svcs_evt_bas_handler_t callback);

/** Get BAS Handle Numbers
*
* @ingroup app_bas_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to BAS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_bas_handles_get(uint8_t host_id, ble_svcs_bas_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_BAS_H_ */
