#ifndef _BLE_SERVICE_HIDS_H_
#define _BLE_SERVICE_HIDS_H_

/**************************************************************************//**
 * @file  ble_service_hids.h
 * @brief Provide the Definition of HIDS.
*****************************************************************************/
#include "ble_service_common.h"
#include "ble_att_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************
 * HIDS Definitions
 **************************************************************************/
/** @defgroup service_hids_def BLE HIDS Definitions
 * @{
 * @ingroup service_def
 * @details Here shows the definitions of the HIDS.
 * @}
**************************************************************************/

/**
 * @ingroup service_hids_def
 * @defgroup service_hids_UUIDDef BLE HIDS UUID Definitions
 * @{
 * @details Here shows the definitions of the BLE HIDS UUID Definitions.
*/
extern const uint16_t attr_uuid_hids_primary_service[];                      /**< HIDS service UUID. */
extern const uint16_t attr_uuid_hids_charc_information[];                    /**< HIDS characteristic INFORMATION UUID. */
extern const uint16_t attr_uuid_hids_charc_control_point[];                  /**< HIDS characteristic CONTROL_POINT UUID. */
extern const uint16_t attr_uuid_hids_charc_protocol_mode[];                  /**< HIDS characteristic PROTOCOL_MODE UUID. */
extern const uint16_t attr_uuid_hids_charc_report_map[];                     /**< HIDS characteristic REPORT_MAP UUID. */
extern const uint16_t attr_uuid_hids_charc_boot_keyboard_input_report[];     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_boot_keyboard_output_report[];    /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_keyboard_input_report[];          /**< HIDS characteristic KEYBOARD_INPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_keyboard_output_report[];         /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_boot_mouse_input_report[];        /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_mouse_input_report[];             /**< HIDS characteristic MOUSE_INPUT_REPORT UUID. */
extern const uint16_t attr_uuid_hids_charc_consumer_input_report[];          /**< HIDS characteristic CONSUMER_INPUT_REPORT UUID. */
/** @} */

/**
 * @defgroup service_hids_ServiceChardef BLE HIDS Service and Characteristic Definitions
 * @{
 * @ingroup service_hids_def
 * @details Here shows the definitions of the HIDS service and characteristic.
 * @}
*/

/**
 * @ingroup service_hids_ServiceChardef
 * @{
*/
extern const ble_att_param_t att_hids_primary_service;                                          /**< HIDS primary service. */
extern const ble_att_param_t att_hids_characteristic_information;                               /**< HIDS characteristic INFORMATION. */
extern const ble_att_param_t att_hids_information;                                              /**< HIDS INFORMATION value. */
extern const ble_att_param_t att_hids_characteristic_control_point;                             /**< HIDS characteristic CONTROL_POINT. */
extern const ble_att_param_t att_hids_control_point;                                            /**< HIDS CONTROL_POINT value. */
extern const ble_att_param_t att_hids_characteristic_protocol_mode;                             /**< HIDS characteristic PROTOCOL_MODE. */
extern const ble_att_param_t att_hids_protocol_mode;                                            /**< HIDS PROTOCOL_MODE value. */
extern const ble_att_param_t att_hids_characteristic_report_map;                                /**< HIDS characteristic REPORT_MAP. */
extern const ble_att_param_t att_hids_report_map;                                               /**< HIDS REPORT_MAP value. */
extern const ble_att_param_t att_hids_characteristic_boot_keyboard_input_report;                /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT. */
extern const ble_att_param_t att_hids_boot_keyboard_input_report;                               /**< HIDS BOOT_KEYBOARD_INPUT_REPORT value. */
extern const ble_att_param_t att_hids_boot_keyboard_input_report_client_charc_configuration;    /**< HIDS BOOT_KEYBOARD_INPUT_REPORT client characteristic configuration. */
extern const ble_att_param_t att_hids_characteristic_boot_keyboard_output_report;               /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT. */
extern const ble_att_param_t att_hids_boot_keyboard_output_report;                              /**< HIDS BOOT_KEYBOARD_OUTPUT_REPORT value. */
extern const ble_att_param_t att_hids_characteristic_keyboard_input_report;                     /**< HIDS characteristic KEYBOARD_INPUT_REPORT. */
extern const ble_att_param_t att_hids_keyboard_input_report;                                    /**< HIDS KEYBOARD_INPUT_REPORT value. */
extern const ble_att_param_t att_hids_keyboard_input_report_client_charc_configuration;         /**< HIDS KEYBOARD_INPUT_REPORT client characteristic configuration. */
extern const ble_att_param_t att_hids_keyboard_input_report_report_reference;                   /**< HIDS KEYBOARD_INPUT_REPORT report reference. */
extern const ble_att_param_t att_hids_characteristic_keyboard_output_report;                    /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT. */
extern const ble_att_param_t att_hids_keyboard_output_report;                                   /**< HIDS KEYBOARD_OUTPUT_REPORT value. */
extern const ble_att_param_t att_hids_keyboard_output_report_report_reference;                  /**< HIDS KEYBOARD_OUTPUT_REPORT report reference. */
extern const ble_att_param_t att_hids_characteristic_boot_mouse_input_report;                   /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT. */
extern const ble_att_param_t att_hids_boot_mouse_input_report;                                  /**< HIDS BOOT_MOUSE_INPUT_REPORT value. */
extern const ble_att_param_t att_hids_boot_mouse_input_report_client_charc_configuration;       /**< HIDS BOOT_MOUSE_INPUT_REPORT client characteristic configuration. */
extern const ble_att_param_t att_hids_characteristic_mouse_input_report;                        /**< HIDS characteristic MOUSE_INPUT_REPORT. */
extern const ble_att_param_t att_hids_mouse_input_report;                                       /**< HIDS MOUSE_INPUT_REPORT value. */
extern const ble_att_param_t att_hids_mouse_input_report_client_charc_configuration;            /**< HIDS MOUSE_INPUT_REPORT client characteristic configuration. */
extern const ble_att_param_t att_hids_mouse_input_report_report_reference;                      /**< HIDS MOUSE_INPUT_REPORT report reference. */
extern const ble_att_param_t att_hids_characteristic_consumer_input_report;                     /**< HIDS characteristic CONSUMER_INPUT_REPORT. */
extern const ble_att_param_t att_hids_consumer_input_report;                                    /**< HIDS CONSUMER_INPUT_REPORT value. */
extern const ble_att_param_t att_hids_consumer_input_report_client_charc_configuration;         /**< HIDS CONSUMER_INPUT_REPORT client characteristic configuration. */
extern const ble_att_param_t att_hids_consumer_input_report_report_reference;                   /**< HIDS CONSUMER_INPUT_REPORT report reference. */
/** @} */


/** HIDS Definition
 * @ingroup service_hids_ServiceChardef
*/
#define ATT_HIDS_SERVICE                                                      \
    &att_hids_primary_service,                                                \
    &att_hids_characteristic_information,                                     \
    &att_hids_information,                                                    \
    &att_hids_characteristic_control_point,                                   \
    &att_hids_control_point,                                                  \
    &att_hids_characteristic_protocol_mode,                                   \
    &att_hids_protocol_mode,                                                  \
    &att_hids_characteristic_report_map,                                      \
    &att_hids_report_map,                                                     \
    &att_hids_characteristic_boot_keyboard_input_report,                      \
    &att_hids_boot_keyboard_input_report,                                     \
    &att_hids_boot_keyboard_input_report_client_charc_configuration,          \
    &att_hids_characteristic_boot_keyboard_output_report,                     \
    &att_hids_boot_keyboard_output_report,                                    \
    &att_hids_characteristic_keyboard_input_report,                           \
    &att_hids_keyboard_input_report,                                          \
    &att_hids_keyboard_input_report_client_charc_configuration,               \
    &att_hids_keyboard_input_report_report_reference,                         \
    &att_hids_characteristic_keyboard_output_report,                          \
    &att_hids_keyboard_output_report,                                         \
    &att_hids_keyboard_output_report_report_reference,                        \
    &att_hids_characteristic_boot_mouse_input_report,                         \
    &att_hids_boot_mouse_input_report,                                        \
    &att_hids_boot_mouse_input_report_client_charc_configuration,             \
    &att_hids_characteristic_mouse_input_report,                              \
    &att_hids_mouse_input_report,                                             \
    &att_hids_mouse_input_report_client_charc_configuration,                  \
    &att_hids_mouse_input_report_report_reference,                            \
    &att_hids_characteristic_consumer_input_report,                           \
    &att_hids_consumer_input_report,                                          \
    &att_hids_consumer_input_report_client_charc_configuration,               \
    &att_hids_consumer_input_report_report_reference                          \


/**************************************************************************
 * HIDS Application Definitions
 **************************************************************************/
/** @defgroup app_hids_def BLE HIDS Application Definitions
 * @{
 * @ingroup service_hids_def
 * @details Here shows the definitions of the HIDS for application.
 * @}
**************************************************************************/

/**
 * @ingroup app_hids_def
 * @defgroup app_hids_eventDef BLE HIDS Service and Characteristic Definitions
 * @{
 * @details Here shows the event definitions of the HIDS.
*/
#define BLESERVICE_HIDS_INFORMATION_READ_EVENT                                  0x01     /**< HIDS characteristic INFORMATION read event.*/
#define BLESERVICE_HIDS_INFORMATION_READ_RSP_EVENT                              0x02     /**< HIDS characteristic INFORMATION read response event.*/
#define BLESERVICE_HIDS_CONTROL_POINT_WRITE_WITHOUT_RSP_EVENT                   0x03     /**< HIDS characteristic CONTROL_POINT write without response event.*/
#define BLESERVICE_HIDS_PROTOCOL_MODE_READ_EVENT                                0x04     /**< HIDS characteristic PROTOCOL_MODE read event.*/
#define BLESERVICE_HIDS_PROTOCOL_MODE_READ_RSP_EVENT                            0x05     /**< HIDS characteristic PROTOCOL_MODE read response event.*/
#define BLESERVICE_HIDS_PROTOCOL_MODE_WRITE_WITHOUT_RSP_EVENT                   0x06     /**< HIDS characteristic PROTOCOL_MODE write without response event.*/
#define BLESERVICE_HIDS_REPORT_MAP_READ_EVENT                                   0x07     /**< HIDS characteristic REPORT_MAP read event.*/
#define BLESERVICE_HIDS_REPORT_MAP_READ_RSP_EVENT                               0x08     /**< HIDS characteristic REPORT_MAP read response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_READ_EVENT                   0x09     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT read event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_READ_RSP_EVENT               0x0a     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_WRITE_EVENT                  0x0b     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT write event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_WRITE_RSP_EVENT              0x0c     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_NOTIFY_EVENT                 0x0d     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT notify event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_READ_EVENT              0x0e     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT cccd read event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_READ_RSP_EVENT          0x0f     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT cccd read response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT             0x10     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT cccd write event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_WRITE_RSP_EVENT         0x11     /**< HIDS characteristic BOOT_KEYBOARD_INPUT_REPORT cccd write response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_READ_EVENT                  0x12     /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT read event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_READ_RSP_EVENT              0x13     /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_WRITE_EVENT                 0x14     /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT write event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_WRITE_RSP_EVENT             0x15     /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_BOOT_KEYBOARD_OUTPUT_REPORT_WRITE_WITHOUT_RSP_EVENT     0x16     /**< HIDS characteristic BOOT_KEYBOARD_OUTPUT_REPORT write without response event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_READ_EVENT                        0x17     /**< HIDS characteristic KEYBOARD_INPUT_REPORT read event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_READ_RSP_EVENT                    0x18     /**< HIDS characteristic KEYBOARD_INPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_WRITE_EVENT                       0x19     /**< HIDS characteristic KEYBOARD_INPUT_REPORT write event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_WRITE_RSP_EVENT                   0x1a     /**< HIDS characteristic KEYBOARD_INPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_NOTIFY_EVENT                      0x1b     /**< HIDS characteristic KEYBOARD_INPUT_REPORT notify event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_REPORT_READ_EVENT                 0x1c     /**< HIDS characteristic KEYBOARD_INPUT_REPORT report reference read event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_REPORT_READ_RSP_EVENT             0x1d     /**< HIDS characteristic KEYBOARD_INPUT_REPORT report reference read response event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_READ_EVENT                   0x1e     /**< HIDS characteristic KEYBOARD_INPUT_REPORT cccd read event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_READ_RSP_EVENT               0x1f     /**< HIDS characteristic KEYBOARD_INPUT_REPORT cccd read response event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT                  0x20     /**< HIDS characteristic KEYBOARD_INPUT_REPORT cccd write event.*/
#define BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_WRITE_RSP_EVENT              0x21     /**< HIDS characteristic KEYBOARD_INPUT_REPORT cccd write response event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_READ_EVENT                       0x22     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT read event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_READ_RSP_EVENT                   0x23     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_WRITE_EVENT                      0x24     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT write event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_WRITE_RSP_EVENT                  0x25     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_WRITE_WITHOUT_RSP_EVENT          0x26     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT write without response event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_REPORT_READ_EVENT                0x27     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT report reference read event.*/
#define BLESERVICE_HIDS_KEYBOARD_OUTPUT_REPORT_REPORT_READ_RSP_EVENT            0x28     /**< HIDS characteristic KEYBOARD_OUTPUT_REPORT report reference read response event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_READ_EVENT                      0x29     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT read event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_READ_RSP_EVENT                  0x2a     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_WRITE_EVENT                     0x2b     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT write event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_WRITE_RSP_EVENT                 0x2c     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_NOTIFY_EVENT                    0x2d     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT notify event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_READ_EVENT                 0x2e     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT cccd read event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_READ_RSP_EVENT             0x2f     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT cccd read response event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT                0x30     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT cccd write event.*/
#define BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_WRITE_RSP_EVENT            0x31     /**< HIDS characteristic BOOT_MOUSE_INPUT_REPORT cccd write response event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_READ_EVENT                           0x32     /**< HIDS characteristic MOUSE_INPUT_REPORT read event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_READ_RSP_EVENT                       0x33     /**< HIDS characteristic MOUSE_INPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_WRITE_EVENT                          0x34     /**< HIDS characteristic MOUSE_INPUT_REPORT write event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_WRITE_RSP_EVENT                      0x35     /**< HIDS characteristic MOUSE_INPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_NOTIFY_EVENT                         0x36     /**< HIDS characteristic MOUSE_INPUT_REPORT notify event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_REPORT_READ_EVENT                    0x37     /**< HIDS characteristic MOUSE_INPUT_REPORT report reference read event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_REPORT_READ_RSP_EVENT                0x38     /**< HIDS characteristic MOUSE_INPUT_REPORT report reference read response event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_READ_EVENT                      0x39     /**< HIDS characteristic MOUSE_INPUT_REPORT cccd read event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_READ_RSP_EVENT                  0x3a     /**< HIDS characteristic MOUSE_INPUT_REPORT cccd read response event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT                     0x3b     /**< HIDS characteristic MOUSE_INPUT_REPORT cccd write event.*/
#define BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_WRITE_RSP_EVENT                 0x3c     /**< HIDS characteristic MOUSE_INPUT_REPORT cccd write response event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_READ_EVENT                        0x3d     /**< HIDS characteristic CONSUMER_INPUT_REPORT read event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_READ_RSP_EVENT                    0x3e     /**< HIDS characteristic CONSUMER_INPUT_REPORT read response event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_WRITE_EVENT                       0x3f     /**< HIDS characteristic CONSUMER_INPUT_REPORT write event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_WRITE_RSP_EVENT                   0x40     /**< HIDS characteristic CONSUMER_INPUT_REPORT write response event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_NOTIFY_EVENT                      0x41     /**< HIDS characteristic CONSUMER_INPUT_REPORT notify event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_READ_EVENT                   0x42     /**< HIDS characteristic CONSUMER_INPUT_REPORT cccd read event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_READ_RSP_EVENT               0x43     /**< HIDS characteristic CONSUMER_INPUT_REPORT cccd read response event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_WRITE_EVENT                  0x44     /**< HIDS characteristic CONSUMER_INPUT_REPORT cccd write event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_WRITE_RSP_EVENT              0x45     /**< HIDS characteristic CONSUMER_INPUT_REPORT cccd write response event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_REPORT_READ_EVENT                 0x46     /**< HIDS characteristic CONSUMER_INPUT_REPORT report reference read event.*/
#define BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_REPORT_READ_RSP_EVENT             0x47     /**< HIDS characteristic CONSUMER_INPUT_REPORT report reference read response event.*/
/** @} */


/**
 * @ingroup app_hids_def
 * @defgroup app_hids_structureDef BLE HIDS Structure Definitions
 * @{
 * @details Here shows the structure definitions of the HIDS.
 * @}
*/

/** HIDS Handles Definition
 * @ingroup app_hids_structureDef
*/
typedef struct ble_svcs_hids_handles_s
{
    uint16_t hdl_information;                                /**< Handle of INFORMATION. */
    uint16_t hdl_control_point;                              /**< Handle of CONTROL_POINT. */
    uint16_t hdl_protocol_mode;                              /**< Handle of PROTOCOL_MODE. */
    uint16_t hdl_report_map;                                 /**< Handle of REPORT_MAP. */
    uint16_t hdl_boot_keyboard_input_report;                 /**< Handle of BOOT_KEYBOARD_INPUT_REPORT. */
    uint16_t hdl_boot_keyboard_input_report_cccd;            /**< Handle of BOOT_KEYBOARD_INPUT_REPORT client characteristic configuration. */
    uint16_t hdl_boot_keyboard_output_report;                /**< Handle of BOOT_KEYBOARD_OUTPUT_REPORT. */
    uint16_t hdl_keyboard_input_report;                      /**< Handle of KEYBOARD_INPUT_REPORT. */
    uint16_t hdl_keyboard_input_report_cccd;                 /**< Handle of KEYBOARD_INPUT_REPORT client characteristic configuration. */
    uint16_t hdl_keyboard_input_report_report_reference;     /**< Handle of KEYBOARD_INPUT_REPORT report reference. */
    uint16_t hdl_keyboard_output_report;                     /**< Handle of KEYBOARD_OUTPUT_REPORT. */
    uint16_t hdl_keyboard_output_report_report_reference;    /**< Handle of KEYBOARD_OUTPUT_REPORT report reference. */
    uint16_t hdl_boot_mouse_input_report;                    /**< Handle of BOOT_MOUSE_INPUT_REPORT. */
    uint16_t hdl_boot_mouse_input_report_cccd;               /**< Handle of BOOT_MOUSE_INPUT_REPORT client characteristic configuration. */
    uint16_t hdl_mouse_input_report;                         /**< Handle of MOUSE_INPUT_REPORT. */
    uint16_t hdl_mouse_input_report_cccd;                    /**< Handle of MOUSE_INPUT_REPORT client characteristic configuration. */
    uint16_t hdl_mouse_input_report_report_reference;        /**< Handle of MOUSE_INPUT_REPORT report reference. */
    uint16_t hdl_consumer_input_report;                      /**< Handle of CONSUMER_INPUT_REPORT. */
    uint16_t hdl_consumer_input_report_cccd;                 /**< Handle of CONSUMER_INPUT_REPORT client characteristic configuration. */
    uint16_t hdl_consumer_input_report_report_reference;     /**< Handle of CONSUMER_INPUT_REPORT report reference. */
} ble_svcs_hids_handles_t;


/** HIDS Data Definition
 * @ingroup app_hids_structureDef
 * @note User defined.
*/
typedef struct ble_svcs_hids_data_s
{
    uint8_t  boot_keyboard_intput_report[6];     /**< BOOT_KEYBOARD_INPUT_REPORT data value */
    uint16_t boot_keyboard_input_report_cccd;    /**< BOOT_KEYBOARD_INPUT_REPORT cccd value */
    uint8_t  boot_keyboard_output_report[6];     /**< BOOT_KEYBOARD_OUTPUT_REPORT data value */
    uint8_t  keyboard_intput_report[8];          /**< KEYBOARD_INPUT_REPORT data value */
    uint16_t keyboard_input_report_cccd;         /**< KEYBOARD_INPUT_REPORT cccd value */
    uint8_t  keyboard_output_report[8];          /**< KEYBOARD_OUTPUT_REPORT data value */
    uint8_t  boot_mouse_input_report[6];         /**< BOOT_MOUSE_INPUT_REPORT data value */
    uint16_t boot_mouse_input_report_cccd;       /**< BOOT_MOUSE_INPUT_REPORT cccd value */
    uint8_t  mouse_input_report[8];              /**< MOUSE_INPUT_REPORT data value */
    uint16_t mouse_input_report_cccd;            /**< MOUSE_INPUT_REPORT cccd value */
    uint8_t  consumer_input_report[2];           /**< CONSUMER_INPUT_REPORT data value */
    uint16_t consumer_input_report_cccd;         /**< CONSUMER_INPUT_REPORT cccd value */
} ble_svcs_hids_data_t;


/** HIDS Application Data Structure Definition
 * @ingroup app_hids_structureDef
*/
typedef struct ble_svcs_hids_subinfo_s
{
    ble_svcs_hids_handles_t handles;    /**< HIDS attribute handles */
    ble_svcs_hids_data_t    data;       /**< HIDS attribute data */
} ble_svcs_hids_subinfo_t;


typedef struct ble_svcs_hids_info_s
{
    ble_gatt_role_t           role;         /**< BLE GATT role */
    ble_svcs_hids_subinfo_t   client_info;
    ble_svcs_hids_subinfo_t   server_info;
} ble_svcs_hids_info_t;


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/

/**
 * @ingroup app_hids_def
 * @defgroup app_hids_App BLE HIDS Definitions for Application
 * @{
 * @details Here shows the definitions of the HIDS for application uses.
 * @}
*/

/** ble_svcs_evt_hids_handler_t
 * @ingroup app_hids_App
 * @note This callback receives the HIDS events. Each of these events can be associated with parameters.
*/
typedef void (*ble_svcs_evt_hids_handler_t)(ble_evt_att_param_t *p_param);

/** HIDS Initialization
*
* @ingroup app_hids_App
*
* @attention There is only one instance of HIDS shall be exposed on a device (if role is @ref BLE_GATT_ROLE_SERVER). \n
*            Callback shall be ignored if role is @ref BLE_GATT_ROLE_SERVER).
*
* @param[in] host_id : the link's host id.
* @param[in] role : @ref ble_gatt_role_t "BLE GATT role".
* @param[in] p_info : a pointer to HIDS information.
* @param[in] callback : a pointer to a callback function that receive the service events.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_CMD_NOT_SUPPORTED : Registered services buffer full.
* @retval BLE_ERR_OK  : Setting success.
*/
ble_err_t ble_svcs_hids_init(uint8_t host_id, ble_gatt_role_t role, ble_svcs_hids_info_t *p_info, ble_svcs_evt_hids_handler_t callback);

/** Get HIDS Handle Numbers
*
* @ingroup app_hids_App
*
* @attention - role = @ref BLE_GATT_ROLE_CLIENT \n
*              MUST call this API to get service information after received @ref BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED  \n
*            - role = @ref BLE_GATT_ROLE_SERVER \n
*              MUST call this API to get service information before connection established. \n
*
* @param[in] host_id : the link's host id.
* @param[out] p_info : a pointer to HIDS information.
*
* @retval BLE_ERR_INVALID_HOST_ID : Error host id.
* @retval BLE_ERR_INVALID_PARAMETER : Invalid parameter.
* @retval BLE_ERR_OK : Setting success.
*/
ble_err_t ble_svcs_hids_handles_get(uint8_t host_id, ble_gatt_role_t role, ble_svcs_hids_info_t *p_info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_SERVICE_HIDS_H_ */
