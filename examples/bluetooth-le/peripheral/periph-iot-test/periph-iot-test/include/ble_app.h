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
    APP_REQUEST_SET_MAC_ADDR,
    APP_REQUEST_ADV_START,
    APP_REQUEST_ADV_STOP,
    APP_REQUEST_HANDLE_STRESS_TEST_TX,
    APP_REQUEST_TEST_START,
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

typedef enum
{
    DEMO_NORMAL_TEST,
    DEMO_STRESS_TEST
} test_mode_t;
/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/



#ifdef __cplusplus
};
#endif

#endif /* __BLE_APP_H__*/
