#ifndef _BLE_SERVICE_FOTAS_H_
#define _BLE_SERVICE_FOTAS_H_

/**************************************************************************//**
 * @file  ble_service_fotas.h
 * @brief Provide the Definition of FOTAS.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * FOTAS Definitions
 **************************************************************************/
/** @defgroup service_fotas_def BLE FOTAS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the FOTAS.
 * @}
**************************************************************************/

/**
 * @ingroup service_fotas_def
 * @defgroup service_fotas_UUIDDef BLE FOTAS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE FOTAS UUID Definitions.
*/
extern const uint16_t attr_uuid_fotas_primary_service[];    /**< FOTAS service UUID. */
extern const uint16_t attr_uuid_fotas_charc_data[];         /**< FOTAS characteristic DATA UUID. */
extern const uint16_t attr_uuid_fotas_charc_command[];      /**< FOTAS characteristic COMMAND UUID. */
/** @} */

/**
 * @defgroup service_fotas_ServiceChardef BLE FOTAS Service and Characteristic Definitions
 * @{
 * @ingroup service_fotas_def
 * @details Here shows the definitions of the FOTAS service and characteristic.
 * @}
*/

/**
 * @ingroup service_fotas_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_fotas_primary_service;                       /**< FOTAS primary service. */
extern const ble_att_param_t att_fotas_characteristic_data;                   /**< FOTAS characteristic DATA. */
extern const ble_att_param_t att_fotas_data;                                  /**< FOTAS DATA value. */
extern const ble_att_param_t att_fotas_data_client_charc_configuration;       /**< FOTAS DATA client characteristic configuration. */
extern const ble_att_param_t att_fotas_data_user_description;                 /**< FOTAS DATA user description. */
extern const ble_att_param_t att_fotas_characteristic_command;                /**< FOTAS characteristic COMMAND. */
extern const ble_att_param_t att_fotas_command;                               /**< FOTAS COMMAND value. */
extern const ble_att_param_t att_fotas_command_client_charc_configuration;    /**< FOTAS COMMAND client characteristic configuration. */
extern const ble_att_param_t att_fotas_command_user_description;              /**< FOTAS COMMAND user description. */
/** @} */


/** FOTAS Definition
 * @ingroup service_fotas_ServiceChardef
*/
#define ATT_FOTAS_SERVICE                                   \
    &att_fotas_primary_service,                             \
    &att_fotas_characteristic_data,                         \
    &att_fotas_data,                                        \
    &att_fotas_data_client_charc_configuration,             \
    &att_fotas_data_user_description,                       \
    &att_fotas_characteristic_command,                      \
    &att_fotas_command,                                     \
    &att_fotas_command_client_charc_configuration,          \
    &att_fotas_command_user_description                     \


/**************************************************************************
 * FOTAS Application Definitions
 **************************************************************************/
/** @defgroup app_fotas_def BLE FOTAS Application Definitions
 * @{
 * @ingroup service_fotas_def
 * @details Here shows the definitions of the FOTAS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_fotas_def
 * @defgroup app_fotas_eventDef BLE FOTAS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the FOTAS.
*/
#define BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT           0x01     /**< FOTAS characteristic DATA write without response event.*/
#define BLESERVICE_FOTAS_DATA_NOTIFY_EVENT                      0x02     /**< FOTAS characteristic DATA notify event.*/
#define BLESERVICE_FOTAS_DATA_DESCRIPTION_READ_EVENT            0x03     /**< FOTAS characteristic DATA user description read event.*/
#define BLESERVICE_FOTAS_DATA_DESCRIPTION_READ_RSP_EVENT        0x04     /**< FOTAS characteristic DATA user description read response event.*/
#define BLESERVICE_FOTAS_DATA_CCCD_READ_EVENT                   0x05     /**< FOTAS characteristic DATA cccd read event.*/
#define BLESERVICE_FOTAS_DATA_CCCD_READ_RSP_EVENT               0x06     /**< FOTAS characteristic DATA cccd read response event.*/
#define BLESERVICE_FOTAS_DATA_CCCD_WRITE_EVENT                  0x07     /**< FOTAS characteristic DATA cccd write event.*/
#define BLESERVICE_FOTAS_DATA_CCCD_WRITE_RSP_EVENT              0x08     /**< FOTAS characteristic DATA cccd write response event.*/
#define BLESERVICE_FOTAS_COMMAND_READ_EVENT                     0x09     /**< FOTAS characteristic COMMAND read event.*/
#define BLESERVICE_FOTAS_COMMAND_READ_RSP_EVENT                 0x0a     /**< FOTAS characteristic COMMAND read response event.*/
#define BLESERVICE_FOTAS_COMMAND_WRITE_EVENT                    0x0b     /**< FOTAS characteristic COMMAND write event.*/
#define BLESERVICE_FOTAS_COMMAND_WRITE_RSP_EVENT                0x0c     /**< FOTAS characteristic COMMAND write response event.*/
#define BLESERVICE_FOTAS_COMMAND_INDICATE_CONFIRM_EVENT         0x0d     /**< FOTAS characteristic COMMAND indicate confirm event.*/
#define BLESERVICE_FOTAS_COMMAND_INDICATE_EVENT                 0x0e     /**< FOTAS characteristic COMMAND indicate event.*/
#define BLESERVICE_FOTAS_COMMAND_DESCRIPTION_READ_EVENT         0x0f     /**< FOTAS characteristic COMMAND user description read event.*/
#define BLESERVICE_FOTAS_COMMAND_DESCRIPTION_READ_RSP_EVENT     0x10     /**< FOTAS characteristic COMMAND user description read response event.*/
#define BLESERVICE_FOTAS_COMMAND_CCCD_READ_EVENT                0x11     /**< FOTAS characteristic COMMAND cccd read event.*/
#define BLESERVICE_FOTAS_COMMAND_CCCD_READ_RSP_EVENT            0x12     /**< FOTAS characteristic COMMAND cccd read response event.*/
#define BLESERVICE_FOTAS_COMMAND_CCCD_WRITE_EVENT               0x13     /**< FOTAS characteristic COMMAND cccd write event.*/
#define BLESERVICE_FOTAS_COMMAND_CCCD_WRITE_RSP_EVENT           0x14     /**< FOTAS characteristic COMMAND cccd write response event.*/
/** @} */


/**
 * @ingroup app_fotas_def
 * @defgroup app_fotas_structureDef BLE FOTAS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the FOTAS.
 * @}
*/

/** FOTAS Handles Definition
 * @ingroup app_fotas_structureDef
*/
typedef struct ble_svcs_fotas_handles_s
{
    uint16_t hdl_data;                        /**< Handle of DATA. */
    uint16_t hdl_data_cccd;                   /**< Handle of DATA client characteristic configuration. */
    uint16_t hdl_data_user_description;       /**< Handle of DATA user description. */
    uint16_t hdl_command;                     /**< Handle of COMMAND. */
    uint16_t hdl_command_cccd;                /**< Handle of COMMAND client characteristic configuration. */
    uint16_t hdl_command_user_description;    /**< Handle of COMMAND user description. */
} ble_svcs_fotas_handles_t;


/** FOTAS Data Definition
 * @ingroup app_fotas_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_fotas_data_s
{
    uint16_t data_cccd;       /**< DATA cccd value */
    uint16_t command_cccd;    /**< COMMAND cccd value */
} ble_svcs_fotas_data_t;


/** FOTAS Application Data Structure Definition
 * @ingroup app_fotas_structureDef
*/
typedef struct ble_svcs_fotas_subinfo_s
{
    ble_svcs_fotas_handles_t handles;    /**< FOTAS attribute handles */
    ble_svcs_fotas_data_t    data;       /**< FOTAS attribute data */
} ble_svcs_fotas_subinfo_t;


typedef struct ble_svcs_fotas_info_s
{
    ble_gatt_role_t           role;         /**< BLE GATT role */
    ble_svcs_fotas_subinfo_t  client_info;
    ble_svcs_fotas_subinfo_t  server_info;
} ble_svcs_fotas_info_t;


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/

/**
 * @ingroup app_fotas_def
 * @defgroup app_fotas_App BLE FOTAS Definitions for Application
 * @{
 * @details Here shows the definitions of the FOTAS for application uses.
 * @}
*/

/** ble_svcs_evt_fotas_handler_t
 * @ingroup app_fotas_App
 * @note This callback receives the FOTAS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_fotas_handler_t)(ble_evt_att_param_t *p_param);

/** FOTAS Initialization
*
* @ingroup app_fotas_App
*
* @attention There is only one instance of FOTAS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to FOTAS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_fotas_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_fotas_info_t *p_info, ble_svcs_evt_fotas_handler_t callback);

/** Get FOTAS Handle Numbers
*
* @ingroup app_fotas_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to FOTAS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_fotas_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_fotas_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_FOTAS_H_ */
