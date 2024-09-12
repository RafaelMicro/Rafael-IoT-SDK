#ifndef _BLE_SERVICE_ATCMD_H_
#define _BLE_SERVICE_ATCMD_H_

/**************************************************************************//**
 * @file  ble_service_atcmd.h
 * @brief Provide the Definition of ATCMD.
*****************************************************************************/

#include "ble_service_common.h"


/**************************************************************************
 * ATCMD Definitions
 **************************************************************************/
/** @defgroup service_atcmd_def BLE ATCMD Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the ATCMD.
 * @}
**************************************************************************/

/**
 * @ingroup service_atcmd_def
 * @defgroup service_atcmd_UUIDDef BLE ATCMD UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE ATCMD UUID Definitions.
*/
extern const uint16_t attr_uuid_atcmd_primary_service[];    /**< ATCMD service UUID. */
extern const uint16_t attr_uuid_atcmd_charc_charac01[];     /**< ATCMD characteristic CHARAC01 UUID. */
extern const uint16_t attr_uuid_atcmd_charc_charac02[];     /**< ATCMD characteristic CHARAC02 UUID. */
extern const uint16_t attr_uuid_atcmd_charc_charac03[];     /**< ATCMD characteristic CHARAC03 UUID. */
extern const uint16_t attr_uuid_atcmd_charc_charac04[];     /**< ATCMD characteristic CHARAC04 UUID. */
/** @} */

/**
 * @defgroup service_atcmd_ServiceChardef BLE ATCMD Service and Characteristic Definitions
 * @{
 * @ingroup service_atcmd_def
 * @details Here shows the definitions of the ATCMD service and characteristic.
 * @}
*/

/**
 * @ingroup service_atcmd_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_atcmd_primary_service;                        /**< ATCMD primary service. */
extern const ble_att_param_t att_atcmd_characteristic_charac01;                /**< ATCMD characteristic CHARAC01. */
extern const ble_att_param_t att_atcmd_charac01;                               /**< ATCMD CHARAC01 value. */
extern const ble_att_param_t att_atcmd_charac01_client_charc_configuration;    /**< ATCMD CHARAC01 client characteristic configuration. */
extern const ble_att_param_t att_atcmd_charac01_user_description;              /**< ATCMD CHARAC01 user description. */
extern const ble_att_param_t att_atcmd_charac01_presentation_format;           /**< ATCMD CHARAC01 presentation format. */
extern const ble_att_param_t att_atcmd_charac01_report_reference;              /**< ATCMD CHARAC01 report reference. */
extern const ble_att_param_t att_atcmd_characteristic_charac02;                /**< ATCMD characteristic CHARAC02. */
extern const ble_att_param_t att_atcmd_charac02;                               /**< ATCMD CHARAC02 value. */
extern const ble_att_param_t att_atcmd_characteristic_charac03;                /**< ATCMD characteristic CHARAC03. */
extern const ble_att_param_t att_atcmd_charac03;                               /**< ATCMD CHARAC03 value. */
extern const ble_att_param_t att_atcmd_characteristic_charac04;                /**< ATCMD characteristic CHARAC04. */
extern const ble_att_param_t att_atcmd_charac04;                               /**< ATCMD CHARAC04 value. */
/** @} */


/** ATCMD Definition
 * @ingroup service_atcmd_ServiceChardef
*/
#define ATT_ATCMD_SERVICE                                    \
    &att_atcmd_primary_service,                              \
    &att_atcmd_characteristic_charac01,                      \
    &att_atcmd_charac01,                                     \
    &att_atcmd_charac01_client_charc_configuration,          \
    &att_atcmd_charac01_user_description,                    \
    &att_atcmd_charac01_presentation_format,                 \
    &att_atcmd_charac01_report_reference,                    \
    &att_atcmd_characteristic_charac02,                      \
    &att_atcmd_charac02,                                     \
    &att_atcmd_characteristic_charac03,                      \
    &att_atcmd_charac03,                                     \
    &att_atcmd_characteristic_charac04,                      \
    &att_atcmd_charac04                                      \


/**************************************************************************
 * ATCMD Application Definitions
 **************************************************************************/
/** @defgroup app_atcmd_def BLE ATCMD Application Definitions
 * @{
 * @ingroup service_atcmd_def
 * @details Here shows the definitions of the ATCMD for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_atcmd_def
 * @defgroup app_atcmd_eventDef BLE ATCMD Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the ATCMD.
*/
#define BLESERVICE_ATCMD_CHARAC01_READ_EVENT                     0x01     /**< ATCMD characteristic CHARAC01 read event.*/
#define BLESERVICE_ATCMD_CHARAC01_READ_RSP_EVENT                 0x02     /**< ATCMD characteristic CHARAC01 read response event.*/
#define BLESERVICE_ATCMD_CHARAC01_WRITE_EVENT                    0x03     /**< ATCMD characteristic CHARAC01 write event.*/
#define BLESERVICE_ATCMD_CHARAC01_WRITE_RSP_EVENT                0x04     /**< ATCMD characteristic CHARAC01 write response event.*/
#define BLESERVICE_ATCMD_CHARAC01_WRITE_WITHOUT_RSP_EVENT        0x05     /**< ATCMD characteristic CHARAC01 write without response event.*/
#define BLESERVICE_ATCMD_CHARAC01_NOTIFY_EVENT                   0x06     /**< ATCMD characteristic CHARAC01 notify event.*/
#define BLESERVICE_ATCMD_CHARAC01_INDICATE_CONFIRM_EVENT         0x07     /**< ATCMD characteristic CHARAC01 indicate confirm event.*/
#define BLESERVICE_ATCMD_CHARAC01_INDICATE_EVENT                 0x08     /**< ATCMD characteristic CHARAC01 indicate event.*/
#define BLESERVICE_ATCMD_CHARAC01_CCCD_READ_EVENT                0x09     /**< ATCMD characteristic CHARAC01 cccd read event.*/
#define BLESERVICE_ATCMD_CHARAC01_CCCD_READ_RSP_EVENT            0x0a     /**< ATCMD characteristic CHARAC01 cccd read response event.*/
#define BLESERVICE_ATCMD_CHARAC01_CCCD_WRITE_EVENT               0x0b     /**< ATCMD characteristic CHARAC01 cccd write event.*/
#define BLESERVICE_ATCMD_CHARAC01_CCCD_WRITE_RSP_EVENT           0x0c     /**< ATCMD characteristic CHARAC01 cccd write response event.*/
#define BLESERVICE_ATCMD_CHARAC01_DESCRIPTION_READ_EVENT         0x0d     /**< ATCMD characteristic CHARAC01 user description read event.*/
#define BLESERVICE_ATCMD_CHARAC01_DESCRIPTION_READ_RSP_EVENT     0x0e     /**< ATCMD characteristic CHARAC01 user description read response event.*/
#define BLESERVICE_ATCMD_CHARAC01_FORMAT_READ_EVENT              0x0f     /**< ATCMD characteristic CHARAC01 presentation format read event.*/
#define BLESERVICE_ATCMD_CHARAC01_FORMAT_READ_RSP_EVENT          0x10     /**< ATCMD characteristic CHARAC01 presentation format read response event.*/
#define BLESERVICE_ATCMD_CHARAC01_REPORT_READ_EVENT              0x11     /**< ATCMD characteristic CHARAC01 report reference read event.*/
#define BLESERVICE_ATCMD_CHARAC01_REPORT_READ_RSP_EVENT          0x12     /**< ATCMD characteristic CHARAC01 report reference read response event.*/
#define BLESERVICE_ATCMD_CHARAC02_READ_EVENT                     0x13     /**< ATCMD characteristic CHARAC02 read event.*/
#define BLESERVICE_ATCMD_CHARAC02_READ_RSP_EVENT                 0x14     /**< ATCMD characteristic CHARAC02 read response event.*/
#define BLESERVICE_ATCMD_CHARAC02_WRITE_EVENT                    0x15     /**< ATCMD characteristic CHARAC02 write event.*/
#define BLESERVICE_ATCMD_CHARAC02_WRITE_RSP_EVENT                0x16     /**< ATCMD characteristic CHARAC02 write response event.*/
#define BLESERVICE_ATCMD_CHARAC03_WRITE_EVENT                    0x17     /**< ATCMD characteristic CHARAC03 write event.*/
#define BLESERVICE_ATCMD_CHARAC03_WRITE_RSP_EVENT                0x18     /**< ATCMD characteristic CHARAC03 write response event.*/
#define BLESERVICE_ATCMD_CHARAC04_WRITE_EVENT                    0x19     /**< ATCMD characteristic CHARAC04 write event.*/
#define BLESERVICE_ATCMD_CHARAC04_WRITE_RSP_EVENT                0x1a     /**< ATCMD characteristic CHARAC04 write response event.*/
#define BLESERVICE_ATCMD_CHARAC02_ERROR_RSP_EVENT                0x1b     /**< ATCMD characteristic CHARAC02 error response event. */
#define BLESERVICE_ATCMD_CHARAC01_RESTORE_BOND_DATA_EVENT        0x1c     /**< ATCMD characteristic CHARAC01 restore bond data event. */
/** @} */


/**
 * @ingroup app_atcmd_def
 * @defgroup app_atcmd_structureDef BLE ATCMD Structure Definitions
 * @{
 * @details Here shows the structure definitions of the ATCMD.
 * @}
*/

/** ATCMD Handles Definition
 * @ingroup app_atcmd_structureDef
*/
typedef struct ble_svcs_atcmd_handles_s
{
    uint16_t hdl_charac01;                        /**< Handle of CHARAC01. */
    uint16_t hdl_charac01_cccd;                   /**< Handle of CHARAC01 client characteristic configuration. */
    uint16_t hdl_charac01_user_description;       /**< Handle of CHARAC01 user description. */
    uint16_t hdl_charac01_presentation_format;    /**< Handle of CHARAC01 presentation format. */
    uint16_t hdl_charac01_report_reference;       /**< Handle of CHARAC01 report reference. */
    uint16_t hdl_charac02;                        /**< Handle of CHARAC02. */
    uint16_t hdl_charac03;                        /**< Handle of CHARAC03. */
    uint16_t hdl_charac04;                        /**< Handle of CHARAC04. */
} ble_svcs_atcmd_handles_t;


/** ATCMD Data Definition
 * @ingroup app_atcmd_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_atcmd_data_s
{
    uint16_t charac01_cccd;    /**< CHARAC01 cccd value */
} ble_svcs_atcmd_data_t;


/** ATCMD Application Data Structure Definition
 * @ingroup app_atcmd_structureDef
*/
typedef struct ble_svcs_atcmd_subinfo_s
{
    ble_svcs_atcmd_handles_t    handles;    /**< ATCMD attribute handles */
    ble_svcs_atcmd_data_t       data;       /**< ATCMD attribute data */
} ble_svcs_atcmd_subinfo_t;

typedef struct ble_svcs_atcmd_info_s
{
    ble_gatt_role_t             role;         /**< BLE GATT role */
    ble_svcs_atcmd_subinfo_t    info;
    // ble_svcs_atcmd_subinfo_t    client_info;
    // ble_svcs_atcmd_subinfo_t    server_info;
} ble_svcs_atcmd_info_t;

/**
 * @ingroup app_atcmd_def
 * @defgroup app_atcmd_App BLE ATCMD Definitions for Application
 * @{
 * @details Here shows the definitions of the ATCMD for application uses.
 * @}
*/

/** ble_svcs_evt_atcmd_handler_t
 * @ingroup app_atcmd_App
 * @note This callback receives the ATCMD events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_atcmd_handler_t)(ble_evt_att_param_t *p_param);

/** ATCMD Initialization
*
* @ingroup app_atcmd_App
*
* @attention There is only one instance of ATCMD shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to ATCMD information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_STATUS_ERR_INVALID_HOSTID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_STATUS_ERR_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_atcmd_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_atcmd_info_t *p_info, ble_svcs_evt_atcmd_handler_t callback);

/** Get ATCMD Handle Numbers
*
* @ingroup app_atcmd_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to ATCMD information.
*
* @retval BLE_STATUS_ERR_INVALID_HOSTID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_atcmd_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_atcmd_info_t *p_info);

#endif //_BLE_SERVICE_ATCMD_H_

