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
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define APP_HW_TIMER_ID     1


/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * @brief Defines the types of parameters for the application queue.
 * @details This enumeration is used to differentiate between application requests and other events.
 * 
 * @note
 * - Application requests include specific events and their associated parameters.
 * - The application request event identifies the type of request.
 * - The application request parameter holds the details for the request.
 * - Other events encompass BLE events and BLE service events.
 * - This enumeration is used to differentiate between application requests and other events.
 */
typedef enum
{
    QUEUE_TYPE_APP_REQ,   /**< Application queue type: application request.*/
    QUEUE_TYPE_OTHERS,    /**< Application queue type: others including BLE events and BLE service events.*/
} app_queue_param_type_t;

/**
 * @brief Enumeration of application request events.
 * @details This enumeration identifies the types of application requests.
 * 
 * @note
 * - These events trigger specific actions within the application.
 * - The associated parameters provide details for each request.
 */
typedef enum
{
    APP_REQUEST_IDLE,             /**< Application request event: idle.*/
    APP_REQUEST_ADV_START,        /**< Application request event: advertising start.*/
    APP_REQUEST_CONN_UPDATE,      /**< Application request event: connection update.*/
    APP_REQUEST_TEST_PARAM_SET,   /**< Application request event: set test parameters.*/
    APP_REQUEST_TEST_PARAM_GET,   /**< Application request event: get test parameters.*/
    APP_REQUEST_TX_TEST,          /**< Application request event: start TX test.*/
    APP_REQUEST_LATENCY_0_SET,    /**< Application request event: set latency parameter.*/
} app_request_t;

/**
 * @brief Enumeration of BLE states.
 * @details This enumeration identifies the states of the BLE application.
 * 
 * @note
 * - These states represent the different stages of the BLE application.
 */
typedef enum
{
    STATE_STANDBY,        /**< Application state: standby.*/
    STATE_ADVERTISING,    /**< Application state: advertising.*/
    STATE_TEST_STANDBY,   /**< Application state: test standby mode.*/
    STATE_TEST_TXING,     /**< Application state: test transmitting.*/
    STATE_TEST_RXING,     /**< Application state: test receiving.*/
} app_state_t;

/**
 * @brief Structure for application request parameters.
 * @details This structure holds the parameters for application requests.
 * 
 * @note
 * - The host_id identifies the source of the request.
 * - The app_req indicates the type of application request.
 */
typedef struct
{
    uint8_t   host_id;    /**< Application request parameter: host id.*/
    uint16_t  app_req;    /**< Application request parameter: @ref app_request_t "application request event".*/
} app_req_param_t;

/**
 * @brief Structure representing the parameters for the application queue.
 * @details This structure holds the details for events and requests in the application queue.
 * 
 * @note
 * - The `event` field specifies the type of event.
 * - The `from_isr` field indicates if the request comes from an interrupt.
 * - The `param_type` field identifies the type of parameter (application request or other).
 * - The `param` union contains the specific parameters for the event or request.
 */
typedef struct
{
    uint8_t   event;        /**< Application queue parameter: event.*/
    uint8_t   from_isr;     /**< Application queue parameter: Dose the Request come from interruption? */
    uint16_t  param_type;   /**< Application queue parameter: @ref app_queue_param_type_t "application queue type".*/
    union
    {
        app_req_param_t  app_req;   /**< Application queue parameter: application request event. */
        ble_tlv_t        *pt_tlv;   /**< Application queue parameter: parameters (type: @ref ble_event_t, length, and value). */
    } param;
} app_queue_t;


/**
 * @brief Structure representing the application test parameters.
 * @details This structure holds the parameters used for testing BLE connections.
 * 
 * @note
 * - The `phy` field specifies the PHY layer used for the test.
 * - The `packet_data_len` field indicates the length of the data packets.
 * - The `mtu_size` field specifies the Maximum Transmission Unit size.
 * - The `conn_interval` field represents the connection interval.
 * - The `conn_latency` field indicates the connection latency.
 * - The `conn_supervision_timeout` field specifies the supervision timeout for the connection.
 */
typedef struct
{
    uint8_t      phy;                         /**< Application test parameter: PHY. */
    uint8_t      packet_data_len;             /**< Application test parameter: package data length. */
    uint8_t      mtu_size;                    /**< Application test parameter: MTU size. */
    uint16_t     conn_interval;               /**< Application test parameter: connection interval. */
    uint16_t     conn_latency;                /**< Application test parameter: connection latency. */
    uint16_t     conn_supervision_timeout;    /**< Application test parameter: supervision timeout. */
} app_test_param_t;

/**
 * @brief Structure representing the request test parameters.
 * @details This structure holds the parameters used for testing BLE connections.
 * 
 * @note
 * - The `phy` field specifies the PHY layer used for the test.
 * - The `packet_data_len` field indicates the length of the data packets.
 * - The `conn_interval_min` field represents the minimum connection interval.
 * - The `conn_interval_max` field indicates the maximum connection interval.
 */
typedef struct
{
    uint8_t      phy;                         /**< Test parameter from request: PHY.*/
    uint8_t      packet_data_len;             /**< Test parameter from request: package data length.*/
    uint16_t     conn_interval_min;           /**< Test parameter from request: connection minimum interval.*/
    uint16_t     conn_interval_max;           /**< Test parameter from request: connection maximum interval.*/
} request_temp_param_t;

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/


#ifdef __cplusplus
};
#endif

#endif /* __BLE_APP_H__*/
