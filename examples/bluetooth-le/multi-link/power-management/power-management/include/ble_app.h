#ifndef __BLE_APP_H__
#define __BLE_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include "ble_api.h"
#include "ble_advertising.h"
#include "ble_scan.h"
#include "ble_att_gatt.h"
#include "ble_gap.h"

// LINK please refer to "ble_profile_def.c" --> att_db_mapping
#define APP_POWER_MEASUREMENT_C_HOST_ID  0         // POWER_MEASUREMENT: Central
#define APP_POWER_MEASUREMENT_P_HOST_ID  1         // POWER_MEASUREMENT: Peripheral

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef enum
{
    QUEUE_TYPE_APP_REQ,             /**< Application queue type: application request.*/
    QUEUE_TYPE_OTHERS,              /**< Application queue type: others including BLE events and BLE service events.*/
} app_queue_param_type;

typedef enum
{
    APP_REQUEST_IDLE,               /**< Application request event: idle.*/
    APP_REQUEST_CREATE_CONN,        /**< Application request event: create connection.*/
    APP_REQUEST_CREATE_CONN_CANCEL,
    APP_REQUEST_TRSPC_MULTI_CMD,    /**< Application request event: Exchange information.*/
    APP_REQUEST_PROCESS_UART_CMD,
    APP_REQUEST_DISCONNECT,
    APP_REQUEST_ADV_START,
    APP_REQUEST_ADV_STOP,
    APP_REQUEST_SCAN_START,
    APP_REQUEST_SCAN_STOP,
    APP_REQUEST_CONN_UPDATE_PARAM,
    APP_REQUEST_PHY_UPDATE,
} app_request_t;

typedef enum
{
    STATE_STANDBY,                  /**< Application state: standby.*/
    STATE_ADVERTISING,              /**< Application state: advertising.*/
    STATE_SCANNING,                 /**< Application state: scanning.*/
    STATE_INITIATING,               /**< Application state: initialing.*/
    STATE_CONNECTED,                /**< Application state: connected.*/
} ble_state_t;


typedef struct
{
    uint8_t   host_id;              /**< Application request parameter: host id.*/
    uint16_t  app_req;              /**< Application request parameter: @ref app_request_t "application request event".*/
} app_req_param_t;


typedef struct
{
    uint8_t   event;                /**< Application queue parameter: event.*/
    uint8_t   from_isr;             /**< Application queue parameter: Dose the Request come from interruption? */
    uint16_t  param_type;           /**< Application queue parameter: @ref app_queue_param_type "application queue type".*/
    union
    {
        app_req_param_t  app_req;   /**< Application queue parameter: application request event. */
        ble_tlv_t        *pt_tlv;   /**< Application queue parameter: parameters (type: @ref ble_event_t, length, and value). */
    } param;
} app_queue_t;

typedef enum App_Active_Role
{
    APP_ACTINE_NONE,
    APP_ACTIVE_PERIPHERAL,
    APP_ACTIVE_CENTRAL
} App_Active_Role;

typedef struct AppParam_P
{
    ble_adv_param_t       adv_param;
    uint8_t               trsp_data[DEFAULT_MTU];
    uint8_t               trsp_data_length;
} AppParam_P;

typedef struct AppParam_C
{
    ble_scan_param_t      scan_param;
    uint8_t               trsp_data[DEFAULT_MTU];
    uint8_t               trsp_data_length;
} AppParam_C;

typedef struct AppParam
{
    App_Active_Role   active_role;
    ble_gap_conn_param_update_param_t    conn_param;
    ble_phy_t         rf_phy;
    ble_coded_phy_option_t phy_option;
    AppParam_P        app_param_p;
    AppParam_C        app_param_c;
} AppParam;

extern AppParam        appParam;
extern ble_gap_addr_t g_target_addr;
/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);


#ifdef __cplusplus
};
#endif

#endif /* __BLE_APP_H__*/
