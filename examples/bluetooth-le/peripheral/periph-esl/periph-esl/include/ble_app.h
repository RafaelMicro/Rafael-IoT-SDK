/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

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
/**
 * @brief Defines the types of parameters for the application queue.
 * @details This enumeration is used to differentiate between application requests, display
 *          requests, and other events carried in the application message queue.
 *
 * @note
 * - Application requests include specific events and their associated parameters.
 * - Display requests carry a TLV payload describing an EPD refresh/change operation.
 * - Other events encompass BLE events and BLE service events.
 */
typedef enum
{
    QUEUE_TYPE_APP_REQ,      /**< Application queue type: application request.*/
    QUEUE_TYPE_DISPLAY_REQ,  /**< Application queue type: display request.*/
    QUEUE_TYPE_OTHERS,       /**< Application queue type: others including BLE events and BLE service events.*/
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
    APP_REQUEST_IDLE,              /**< Application request event: idle.*/
    APP_REQUEST_ADV_START,         /**< Application request event: advertising start.*/
    APP_REQUEST_FOTA_TIMER_EXPIRY, /**< Application request event: FOTA timer expired.*/
    APP_REQUEST_SYNC_TERMINATE     /**< Application request event: Sync terminate.*/
} app_request_t;

/**
 * @brief Enumeration of display request types.
 * @details This enumeration identifies the reason an EPD display update was queued from the
 *          ESL control point command handler.
 *
 * @note
 * - DISPLAY_REQ_DISPLAY_REFRESH re-displays the currently selected image on a display (no new
 *   image is selected).
 * - DISPLAY_REQ_DISPLAY_CHANGE selects a new image index for a display and then displays it.
 */
typedef enum
{
    DISPLAY_REQ_DISPLAY_REFRESH,            /**< Display request type: refresh display only.*/
    DISPLAY_REQ_DISPLAY_CHANGE,            /**< Display request type: display with specific image.*/
} app_display_req_type_t;

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
    STATE_CONNECTED,      /**< Application state: connected.*/
} ble_state_t;

/**
 * @brief Structure for a display refresh request.
 * @details This structure carries the parameters for a @ref DISPLAY_REQ_DISPLAY_REFRESH
 *          display request, used to re-display the currently selected image.
 */
typedef struct
{
    uint8_t   display_index;   /**< Display request parameter: display index.*/
    uint8_t   image_index;     /**< Display request parameter: image index to redisplay.*/
} display_req_display_refresh_param_t;

/**
 * @brief Structure for a display change request.
 * @details This structure carries the parameters for a @ref DISPLAY_REQ_DISPLAY_CHANGE
 *          display request, used to select and display a new image.
 */
typedef struct
{
    uint8_t   display_index;   /**< Display request parameter: display index.*/
    uint8_t   image_index;     /**< Display request parameter: image index to display.*/
} display_req_display_change_param_t;

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
 * - The `param_type` field identifies the type of parameter (application request, display
 *   request, or other).
 * - The `param` union contains the specific parameters for the event or request.
 */
typedef struct
{
    uint8_t   event;        /**< Application queue parameter: event.*/
    uint8_t   from_isr;     /**< Application queue parameter: Does the Request come from interruption? */
    uint16_t  param_type;   /**< Application queue parameter: @ref app_queue_param_type_t "application queue type".*/
    union
    {
        app_req_param_t  app_req;   /**< Application queue parameter: application request event. */
        ble_tlv_t        *pt_tlv;   /**< Application queue parameter: TLV payload (length and value). Its `type` field
                                          is a @ref ble_event_t when param_type is @ref QUEUE_TYPE_OTHERS, or an
                                          @ref app_display_req_type_t when param_type is @ref QUEUE_TYPE_DISPLAY_REQ. */
    } param;
} app_queue_t;

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/



#ifdef __cplusplus
};
#endif

#endif /* __BLE_APP_H__*/
