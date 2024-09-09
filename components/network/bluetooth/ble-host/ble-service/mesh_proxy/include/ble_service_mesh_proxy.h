#ifndef _BLE_SERVICE_MESH_PROXY_H_
#define _BLE_SERVICE_MESH_PROXY_H_

/**************************************************************************//**
 * @file  ble_service_mesh_proxy.h
 * @brief Provide the Definition of MESH_PROXY.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * MESH_PROXY Definitions
 **************************************************************************/
/** @defgroup service_mesh_proxy_def BLE MESH_PROXY Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the MESH_PROXY.
 * @}
**************************************************************************/

/**
 * @ingroup service_mesh_proxy_def
 * @defgroup service_mesh_proxy_UUIDDef BLE MESH_PROXY UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE MESH_PROXY UUID Definitions.
*/
extern const uint16_t attr_uuid_mesh_proxy_primary_service[];    /**< MESH_PROXY service UUID. */
extern const uint16_t attr_uuid_mesh_proxy_charc_data_in[];      /**< MESH_PROXY characteristic DATA_IN UUID. */
extern const uint16_t attr_uuid_mesh_proxy_charc_data_out[];     /**< MESH_PROXY characteristic DATA_OUT UUID. */
/** @} */

/**
 * @defgroup service_mesh_proxy_ServiceChardef BLE MESH_PROXY Service and Characteristic Definitions
 * @{
 * @ingroup service_mesh_proxy_def
 * @details Here shows the definitions of the MESH_PROXY service and characteristic.
 * @}
*/

/**
 * @ingroup service_mesh_proxy_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_mesh_proxy_primary_service;                        /**< MESH_PROXY primary service. */
extern const ble_att_param_t att_mesh_proxy_characteristic_data_in;                 /**< MESH_PROXY characteristic DATA_IN. */
extern const ble_att_param_t att_mesh_proxy_data_in;                                /**< MESH_PROXY DATA_IN value. */
extern const ble_att_param_t att_mesh_proxy_characteristic_data_out;                /**< MESH_PROXY characteristic DATA_OUT. */
extern const ble_att_param_t att_mesh_proxy_data_out;                               /**< MESH_PROXY DATA_OUT value. */
extern const ble_att_param_t att_mesh_proxy_data_out_client_charc_configuration;    /**< MESH_PROXY DATA_OUT client characteristic configuration. */
/** @} */


/** MESH_PROXY Definition
 * @ingroup service_mesh_proxy_ServiceChardef
*/
#define ATT_MESH_PROXY_SERVICE                                   \
    &att_mesh_proxy_primary_service,                             \
    &att_mesh_proxy_characteristic_data_in,                      \
    &att_mesh_proxy_data_in,                                     \
    &att_mesh_proxy_characteristic_data_out,                     \
    &att_mesh_proxy_data_out,                                    \
    &att_mesh_proxy_data_out_client_charc_configuration          \


/**************************************************************************
 * MESH_PROXY Application Definitions
 **************************************************************************/
/** @defgroup app_mesh_proxy_def BLE MESH_PROXY Application Definitions
 * @{
 * @ingroup service_mesh_proxy_def
 * @details Here shows the definitions of the MESH_PROXY for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_mesh_proxy_def
 * @defgroup app_mesh_proxy_eventDef BLE MESH_PROXY Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the MESH_PROXY.
*/
#define BLESERVICE_MESH_PROXY_DATA_IN_WRITE_WITHOUT_RSP_EVENT     0x01     /**< MESH_PROXY characteristic DATA_IN write without response event.*/
#define BLESERVICE_MESH_PROXY_DATA_OUT_NOTIFY_EVENT               0x02     /**< MESH_PROXY characteristic DATA_OUT notify event.*/
#define BLESERVICE_MESH_PROXY_DATA_OUT_CCCD_READ_EVENT            0x03     /**< MESH_PROXY characteristic DATA_OUT cccd read event.*/
#define BLESERVICE_MESH_PROXY_DATA_OUT_CCCD_READ_RSP_EVENT        0x04     /**< MESH_PROXY characteristic DATA_OUT cccd read response event.*/
#define BLESERVICE_MESH_PROXY_DATA_OUT_CCCD_WRITE_EVENT           0x05     /**< MESH_PROXY characteristic DATA_OUT cccd write event.*/
#define BLESERVICE_MESH_PROXY_DATA_OUT_CCCD_WRITE_RSP_EVENT       0x06     /**< MESH_PROXY characteristic DATA_OUT cccd write response event.*/
/** @} */


/**
 * @ingroup app_mesh_proxy_def
 * @defgroup app_mesh_proxy_structureDef BLE MESH_PROXY Structure Definitions
 * @{
 * @details Here shows the structure definitions of the MESH_PROXY.
 * @}
*/

/** MESH_PROXY Handles Definition
 * @ingroup app_mesh_proxy_structureDef
*/
typedef struct ble_svcs_mesh_proxy_handles_s
{
    uint16_t hdl_data_in;          /**< Handle of DATA_IN. */
    uint16_t hdl_data_out;         /**< Handle of DATA_OUT. */
    uint16_t hdl_data_out_cccd;    /**< Handle of DATA_OUT client characteristic configuration. */
} ble_svcs_mesh_proxy_handles_t;


/** MESH_PROXY Data Definition
 * @ingroup app_mesh_proxy_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_mesh_proxy_data_s
{
    uint16_t data_out_cccd;    /**< DATA_OUT cccd value */
} ble_svcs_mesh_proxy_data_t;


/** MESH_PROXY Application Data Structure Definition
 * @ingroup app_mesh_proxy_structureDef
*/
typedef struct ble_svcs_mesh_proxy_info_s
{
    ble_gatt_role_t               role;       /**< BLE GATT role */
    ble_svcs_mesh_proxy_handles_t handles;    /**< MESH_PROXY attribute handles */
    ble_svcs_mesh_proxy_data_t    data;       /**< MESH_PROXY attribute data */
} ble_svcs_mesh_proxy_info_t;


/**
 * @ingroup app_mesh_proxy_def
 * @defgroup app_mesh_proxy_App BLE MESH_PROXY Definitions for Application
 * @{
 * @details Here shows the definitions of the MESH_PROXY for application uses.
 * @}
*/

/** ble_svcs_evt_mesh_proxy_handler_t
 * @ingroup app_mesh_proxy_App
 * @note This callback receives the MESH_PROXY events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_mesh_proxy_handler_t)(ble_evt_att_param_t *p_param);

/** MESH_PROXY Initialization
*
* @ingroup app_mesh_proxy_App
*
* @attention There is only one instance of MESH_PROXY shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to MESH_PROXY information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_mesh_proxy_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_mesh_proxy_info_t *p_info, ble_svcs_evt_mesh_proxy_handler_t callback);

/** Get MESH_PROXY Handle Numbers
*
* @ingroup app_mesh_proxy_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to MESH_PROXY information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_mesh_proxy_handles_get(uint8_t host_id, ble_svcs_mesh_proxy_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_MESH_PROXY_H_ */
