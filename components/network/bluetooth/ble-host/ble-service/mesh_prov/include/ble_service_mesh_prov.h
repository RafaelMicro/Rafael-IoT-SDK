#ifndef _BLE_SERVICE_MESH_PROV_H_
#define _BLE_SERVICE_MESH_PROV_H_

/**************************************************************************//**
 * @file  ble_service_mesh_prov.h
 * @brief Provide the Definition of MESH_PROV.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * MESH_PROV Definitions
 **************************************************************************/
/** @defgroup service_mesh_prov_def BLE MESH_PROV Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the MESH_PROV.
 * @}
**************************************************************************/

/**
 * @ingroup service_mesh_prov_def
 * @defgroup service_mesh_prov_UUIDDef BLE MESH_PROV UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE MESH_PROV UUID Definitions.
*/
extern const uint16_t attr_uuid_mesh_prov_primary_service[];    /**< MESH_PROV service UUID. */
extern const uint16_t attr_uuid_mesh_prov_charc_data_in[];      /**< MESH_PROV characteristic DATA_IN UUID. */
extern const uint16_t attr_uuid_mesh_prov_charc_data_out[];     /**< MESH_PROV characteristic DATA_OUT UUID. */
/** @} */

/**
 * @defgroup service_mesh_prov_ServiceChardef BLE MESH_PROV Service and Characteristic Definitions
 * @{
 * @ingroup service_mesh_prov_def
 * @details Here shows the definitions of the MESH_PROV service and characteristic.
 * @}
*/

/**
 * @ingroup service_mesh_prov_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_mesh_prov_primary_service;                        /**< MESH_PROV primary service. */
extern const ble_att_param_t att_mesh_prov_characteristic_data_in;                 /**< MESH_PROV characteristic DATA_IN. */
extern const ble_att_param_t att_mesh_prov_data_in;                                /**< MESH_PROV DATA_IN value. */
extern const ble_att_param_t att_mesh_prov_characteristic_data_out;                /**< MESH_PROV characteristic DATA_OUT. */
extern const ble_att_param_t att_mesh_prov_data_out;                               /**< MESH_PROV DATA_OUT value. */
extern const ble_att_param_t att_mesh_prov_data_out_client_charc_configuration;    /**< MESH_PROV DATA_OUT client characteristic configuration. */
/** @} */


/** MESH_PROV Definition
 * @ingroup service_mesh_prov_ServiceChardef
*/
#define ATT_MESH_PROV_SERVICE                                   \
    &att_mesh_prov_primary_service,                             \
    &att_mesh_prov_characteristic_data_in,                      \
    &att_mesh_prov_data_in,                                     \
    &att_mesh_prov_characteristic_data_out,                     \
    &att_mesh_prov_data_out,                                    \
    &att_mesh_prov_data_out_client_charc_configuration          \


/**************************************************************************
 * MESH_PROV Application Definitions
 **************************************************************************/
/** @defgroup app_mesh_prov_def BLE MESH_PROV Application Definitions
 * @{
 * @ingroup service_mesh_prov_def
 * @details Here shows the definitions of the MESH_PROV for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_mesh_prov_def
 * @defgroup app_mesh_prov_eventDef BLE MESH_PROV Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the MESH_PROV.
*/
#define BLESERVICE_MESH_PROV_DATA_IN_WRITE_WITHOUT_RSP_EVENT     0x01     /**< MESH_PROV characteristic DATA_IN write without response event.*/
#define BLESERVICE_MESH_PROV_DATA_OUT_NOTIFY_EVENT               0x02     /**< MESH_PROV characteristic DATA_OUT notify event.*/
#define BLESERVICE_MESH_PROV_DATA_OUT_CCCD_READ_EVENT            0x03     /**< MESH_PROV characteristic DATA_OUT cccd read event.*/
#define BLESERVICE_MESH_PROV_DATA_OUT_CCCD_READ_RSP_EVENT        0x04     /**< MESH_PROV characteristic DATA_OUT cccd read response event.*/
#define BLESERVICE_MESH_PROV_DATA_OUT_CCCD_WRITE_EVENT           0x05     /**< MESH_PROV characteristic DATA_OUT cccd write event.*/
#define BLESERVICE_MESH_PROV_DATA_OUT_CCCD_WRITE_RSP_EVENT       0x06     /**< MESH_PROV characteristic DATA_OUT cccd write response event.*/
/** @} */


/**
 * @ingroup app_mesh_prov_def
 * @defgroup app_mesh_prov_structureDef BLE MESH_PROV Structure Definitions
 * @{
 * @details Here shows the structure definitions of the MESH_PROV.
 * @}
*/

/** MESH_PROV Handles Definition
 * @ingroup app_mesh_prov_structureDef
*/
typedef struct ble_svcs_mesh_prov_handles_s
{
    uint16_t hdl_data_in;          /**< Handle of DATA_IN. */
    uint16_t hdl_data_out;         /**< Handle of DATA_OUT. */
    uint16_t hdl_data_out_cccd;    /**< Handle of DATA_OUT client characteristic configuration. */
} ble_svcs_mesh_prov_handles_t;


/** MESH_PROV Data Definition
 * @ingroup app_mesh_prov_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_mesh_prov_data_s
{
    uint16_t data_out_cccd;    /**< DATA_OUT cccd value */
} ble_svcs_mesh_prov_data_t;


/** MESH_PROV Application Data Structure Definition
 * @ingroup app_mesh_prov_structureDef
*/
typedef struct ble_svcs_mesh_prov_info_s
{
    ble_gatt_role_t              role;       /**< BLE GATT role */
    ble_svcs_mesh_prov_handles_t handles;    /**< MESH_PROV attribute handles */
    ble_svcs_mesh_prov_data_t    data;       /**< MESH_PROV attribute data */
} ble_svcs_mesh_prov_info_t;


/**
 * @ingroup app_mesh_prov_def
 * @defgroup app_mesh_prov_App BLE MESH_PROV Definitions for Application
 * @{
 * @details Here shows the definitions of the MESH_PROV for application uses.
 * @}
*/

/** ble_svcs_evt_mesh_prov_handler_t
 * @ingroup app_mesh_prov_App
 * @note This callback receives the MESH_PROV events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_mesh_prov_handler_t)(ble_evt_att_param_t *p_param);

/** MESH_PROV Initialization
*
* @ingroup app_mesh_prov_App
*
* @attention There is only one instance of MESH_PROV shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to MESH_PROV information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_mesh_prov_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_mesh_prov_info_t *p_info, ble_svcs_evt_mesh_prov_handler_t callback);

/** Get MESH_PROV Handle Numbers
*
* @ingroup app_mesh_prov_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to MESH_PROV information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_mesh_prov_handles_get(uint8_t host_id, ble_svcs_mesh_prov_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_MESH_PROV_H_ */
