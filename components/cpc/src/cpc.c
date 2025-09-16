/**
 * @file cpc.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "FreeRTOSConfig.h"

#include <stdbool.h>
#include <stddef.h>
#include "cpc.h"
#include "cpc_api.h"
#include "cpc_timer.h"

#include "cpc_config.h"
#include "cpc_crc.h"
#include "cpc_drv.h"
#include "cpc_hdlc.h"
#include "cpc_system_common.h"
#include "cpc_user_interface.h"

#define LOCK_ENDPOINTS_LIST()
#define RELEASE_ENDPOINTS_LIST()
#define LOCK_ENDPOINT(ep)
#define RELEASE_ENDPOINT(ep)

#if !defined(CPC_SECURITY_NONCE_FRAME_COUNTER_RESET_VALUE)
#define CPC_SECURITY_NONCE_FRAME_COUNTER_RESET_VALUE 0
#endif

#if (CPC_HDLC_SEQ_8 == 1)
#define CPC_HDLC_SEQ       7u
#define CPC_HDLC_SEQ_RANGE 8u
#else
#define CPC_HDLC_SEQ       3u
#define CPC_HDLC_SEQ_RANGE 4u
#endif

#define ABS(a) ((a) < 0 ? -(a) : (a))

static slist_node_t* endpoints;
static slist_node_t* closed_endpoint_list;
static slist_node_t* transmit_queue;
static cpc_dispatcher_handle_t dispatcher_handle;
cpc_drv_capabilities_t cpc_driver_capabilities;

static uint32_t rx_process_flag = 0, tx_process_flag = 0;

#if ((CPC_DEBUG_CORE_EVENT_COUNTERS == 1)                                      \
     || (CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1))
cpc_core_debug_t cpc_core_debug;
#endif

__weak void cpc_system_process(void);

static status_t init(void);

static status_t open_endpoint(cpc_endpoint_handle_t* endpoint, uint8_t id,
                              uint8_t flags, uint8_t tx_window_size);

static cpc_endpoint_t* find_endpoint(uint8_t id);

static bool sort_endpoints(slist_node_t* item_l, slist_node_t* item_r);

static status_t write(cpc_endpoint_t* ep, void* data, uint16_t data_length,
                      uint8_t flags, void* on_write_completed_arg);

static void decode_packet(void);
static int receive_ack(cpc_endpoint_t* endpoint, uint8_t ack);

static void receive_iframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t address,
                           uint8_t control, uint8_t seq);
static void receive_sframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t control,
                           uint16_t data_length);
static void receive_uframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t control,
                           uint16_t data_length);

static status_t transmit_ack(cpc_endpoint_t* endpoint);

static status_t re_transmit_frame(cpc_endpoint_t* endpoint);

static void transmit_reject(cpc_endpoint_t* endpoint, uint8_t address,
                            uint8_t ack, cpc_reject_reason_t reason);

static void queue_for_transmission(cpc_endpoint_t* ep,
                                   cpc_transmit_queue_item_t* item,
                                   cpc_buffer_handle_t* handle);

static status_t process_tx_queue(void);
static void process_close(void);
static void defer_endpoint_free(cpc_endpoint_t* ep);
static void process_deferred_on_write_completed(void* data);
static bool free_closed_endpoint_if_empty(cpc_endpoint_t* ep);
static void clean_tx_queues(cpc_endpoint_t* endpoint);
static void re_transmit_timeout_callback(void* data);
static void endpoint_close_timeout_callback(void* data);
static void notify_error(cpc_endpoint_t* endpoint);
static bool is_seq_valid(uint8_t seq, uint8_t ack);
static bool cpc_enter_api(cpc_endpoint_handle_t* endpoint_handle);
static void cpc_exit_api(cpc_endpoint_handle_t* endpoint_handle);

status_t cpc_init(void) {
    slist_init(&endpoints);
    slist_init(&closed_endpoint_list);
    slist_init(&transmit_queue);
    cpc_init_buffers();
    rx_process_flag = 0;
    tx_process_flag = 0;
    return init();
}

status_t cpc_open_user_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                cpc_user_endpoint_id_t id, uint8_t flags,
                                uint8_t tx_window_size) {
    configASSERT((uint8_t)id >= CPC_USER_ENDPOINT_ID_START);
    configASSERT((uint8_t)id <= CPC_USER_ENDPOINT_ID_END);
    return open_endpoint(endpoint_handle, (uint8_t)id, flags, tx_window_size);
}

status_t cpc_open_service_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                   cpc_service_endpoint_id_t id, uint8_t flags,
                                   uint8_t tx_window_size) {
    configASSERT((uint8_t)id <= CPC_SERVICE_ENDPOINT_ID_END);
    return open_endpoint(endpoint_handle, (uint8_t)id, flags, tx_window_size);
}

status_t cpc_open_temporary_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                     uint8_t* id, uint8_t flags,
                                     uint8_t tx_window_size) {
    uint8_t id_free = 0;
    for (uint8_t i = CPC_TEMPORARY_ENDPOINT_ID_START;
         i <= CPC_TEMPORARY_ENDPOINT_ID_END; i++) {
        cpc_endpoint_t* endpoint = find_endpoint(i);
        if (endpoint == NULL) {
            id_free = i;
            break;
        }
    }

    if (id_free == 0)
        return CPC_STATUS_NO_MORE_RESOURCE;

    configASSERT(id_free >= CPC_TEMPORARY_ENDPOINT_ID_START);
    configASSERT(id_free <= CPC_TEMPORARY_ENDPOINT_ID_END);
    *id = id_free;
    return open_endpoint(endpoint_handle, id_free, flags, tx_window_size);
}

status_t cpc_set_endpoint_option(cpc_endpoint_handle_t* endpoint_handle,
                                 cpc_endpoint_option_t option, void* value) {
    cpc_endpoint_t* ep;
    status_t status = CPC_STATUS_OK;

    CPC_CHECK_NULL_POINTER(endpoint_handle);
    if (!cpc_enter_api(endpoint_handle))
        return CPC_STATUS_INVALID_STATE;

    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);

    if (ep == NULL) {
        // Close has already been called, we are in the process of closing the endpoint
        ep = find_endpoint(endpoint_handle->id);
        if (ep == NULL)
            status = CPC_STATUS_INVALID_STATE;
        else if (ep->state == CPC_STATE_CLOSING)
            status = CPC_STATUS_INVALID_STATE;
        else
            status = CPC_STATUS_OWNERSHIP;
        cpc_exit_api(endpoint_handle);
        return status;
    }

    LOCK_ENDPOINT(ep);
    switch (option) {
        case CPC_ENDPOINT_ON_IFRAME_RECEIVE: {
            ep->on_iframe_data_reception = (cpc_on_data_reception_t)value;
            break;
        }
        case CPC_ENDPOINT_ON_IFRAME_RECEIVE_ARG: {
            ep->on_iframe_data_reception_arg = value;
            break;
        }
        case CPC_ENDPOINT_ON_UFRAME_RECEIVE: {
            ep->on_uframe_data_reception = (cpc_on_data_reception_t)value;
            break;
        }
        case CPC_ENDPOINT_ON_UFRAME_RECEIVE_ARG: {
            ep->on_uframe_data_reception_arg = value;
            break;
        }
        case CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED: {
            ep->on_iframe_write_completed = (cpc_on_write_completed_t)value;
            break;
        }
        case CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED: {
            ep->on_uframe_write_completed = (cpc_on_write_completed_t)value;
            break;
        }
        case CPC_ENDPOINT_ON_FINAL: {
#if defined(CPC_ON_FINAL_PRESENT)
            ep->poll_final.on_final = (cpc_on_final_t)value;
#else
            status = CPC_STATUS_INVALID_PARAMETER;
#endif
            break;
        }
        case CPC_ENDPOINT_ON_POLL: {
#if defined(CPC_ON_POLL_PRESENT)
            ep->poll_final.on_poll = (cpc_on_poll_t)value;
#else
            status = CPC_STATUS_INVALID_PARAMETER;
#endif
            break;
        }
        case CPC_ENDPOINT_ON_POLL_ARG:
        case CPC_ENDPOINT_ON_FINAL_ARG: {
            ep->poll_final.on_fnct_arg = value;
            break;
        }
        case CPC_ENDPOINT_ON_ERROR: {
            ep->on_error = (cpc_on_error_callback_t)value;
            break;
        }
        case CPC_ENDPOINT_ON_ERROR_ARG: {
            ep->on_error_arg = value;
            break;
        }
        default: {
            status = CPC_STATUS_INVALID_PARAMETER;
            break;
        }
    }

    RELEASE_ENDPOINT(ep);

    cpc_exit_api(endpoint_handle);
    return status;
}

void cpc_set_state(cpc_endpoint_handle_t* endpoint_handle,
                   cpc_endpoint_state_t state) {
    cpc_endpoint_t* ep;

    MCU_ENTER_CRITICAL();
    if (endpoint_handle->ref_count > 1) {
        MCU_EXIT_CRITICAL();
        return;
    }
    endpoint_handle->ref_count = 0u;

    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);

    if (ep) {
        ep->state = state;
        ep->seq = 0;
        ep->ack = 0;
        ep->frames_count_re_transmit_queue = 0;
    }
    MCU_EXIT_CRITICAL();
}

/***************************************************************************/ /**
                                                                               * Close an endpoint
                                                                               ******************************************************************************/
status_t cpc_close_endpoint(cpc_endpoint_handle_t* endpoint_handle) {
    status_t status;
    cpc_endpoint_t* ep;

    MCU_ENTER_CRITICAL();
    if (endpoint_handle->ref_count > 1) {
        MCU_EXIT_CRITICAL();
        return CPC_STATUS_BUSY;
    }
    endpoint_handle->ref_count = 0u;

    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);

    if (ep == NULL) {
        // Close has already been called, we are in the process of closing the endpoint
        ep = find_endpoint(endpoint_handle->id);
        if (ep == NULL)
            status = CPC_STATUS_OK;
        else if (ep->state == CPC_STATE_CLOSING
                 || ep->state == CPC_STATE_CLOSED)
            status = CPC_STATUS_OK;
        else
            status = CPC_STATUS_OWNERSHIP;

        MCU_EXIT_CRITICAL();
        return status;
    }

    MCU_EXIT_CRITICAL();
    LOCK_ENDPOINT(ep);

    // Notify the host that we want to close an endpoint
    if (endpoint_handle->id != CPC_ENDPOINT_SYSTEM) {
        status = cpc_send_disconnection_notification(ep->id);
        if (status != CPC_STATUS_OK) {
            RELEASE_ENDPOINT(ep);
            MCU_EXIT_CRITICAL();
            return CPC_STATUS_BUSY;
        }
    }

    while (ep->iframe_receive_queue != NULL) {
        // Drop the data from the receive Queue;
        //   Data reception is not allowed when the endpoint is in closing state
        //   Not possible to read anymore data once the endpoint is closed (or in closing state)
        slist_node_t* node;
        cpc_receive_queue_item_t* item;
        node = slist_pop(&ep->iframe_receive_queue);
        item = SLIST_ENTRY(node, cpc_receive_queue_item_t, node);

        cpc_drop_receive_queue_item(item);
    }

    while (ep->uframe_receive_queue != NULL) {
        // Drop the data from the receive Queue;
        //   Data reception is not allowed when the endpoint is in closing state
        //   Not possible to read anymore data once the endpoint is closed (or in closing state)
        slist_node_t* node;
        cpc_receive_queue_item_t* item;
        node = slist_pop(&ep->uframe_receive_queue);
        item = SLIST_ENTRY(node, cpc_receive_queue_item_t, node);

        cpc_drop_receive_queue_item(item);
    }

    switch (ep->state) {
        case CPC_STATE_OPEN: break;
        case CPC_STATE_ERROR_DESTINATION_UNREACHABLE:
        case CPC_STATE_ERROR_SECURITY_INCIDENT:
        case CPC_STATE_ERROR_FAULT: {
            clean_tx_queues(ep);
            break;
        } // Fatal error; must clean everything and free endpoint
        case CPC_STATE_CLOSING:
        case CPC_STATE_CLOSED:
        default: {
            configASSERT(false);
            return CPC_STATUS_FAIL;
        } // Should not reach this case
    }

    if (endpoint_handle->id == CPC_ENDPOINT_SYSTEM) {
        ep->state = CPC_STATE_CLOSED;
        defer_endpoint_free(ep);
    } else {
        ep->state = CPC_STATE_CLOSING;

        // We expect the host to close the endpoint in a reasonable time, start a timer
        status = cpc_timer_restart_timer(
            &ep->close_timer,
            cpc_timer_ms_to_tick(CPC_DISCONNECTION_NOTIFICATION_TIMEOUT_MS),
            endpoint_close_timeout_callback, ep);
        configASSERT(status == CPC_STATUS_OK);
    }

    RELEASE_ENDPOINT(ep);

    // Set endpoint to null, so we cannot read and send data anymore or
    // closing the endpoint again
    endpoint_handle->ep = NULL;

    // ep->state = CPC_STATE_OPEN;
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Abort read from an endpoint
                                                                               ******************************************************************************/

/***************************************************************************/ /**
                                                                               * Read data from an endpoint
                                                                               ******************************************************************************/
status_t cpc_read(cpc_endpoint_handle_t* endpoint_handle, void** data,
                  uint16_t* data_length, uint32_t timeout, uint8_t flags) {
    slist_node_t** receive_queue = NULL;
    cpc_endpoint_t* ep = NULL;
    cpc_receive_queue_item_t* item = NULL;
    slist_node_t* node = NULL;
    status_t status = CPC_STATUS_EMPTY;

    CPC_CHECK_NULL_POINTER(endpoint_handle);
    if (!cpc_enter_api(endpoint_handle))
        return CPC_STATUS_INVALID_STATE;

    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);
    if (ep == NULL) {
        ep = find_endpoint(endpoint_handle->id);
        status = (ep != NULL) ? CPC_STATUS_OWNERSHIP : CPC_STATUS_INVALID_STATE;
        cpc_exit_api(endpoint_handle);
        return status;
    }

    LOCK_ENDPOINT(ep);
    receive_queue = (flags & CPC_FLAG_UNNUMBERED_INFORMATION)
                        ? &ep->uframe_receive_queue
                        : &ep->iframe_receive_queue;

    if (*receive_queue == NULL) {
        if (ep->state != CPC_STATE_OPEN) {
            // Return error only when the receive queue is empty
            RELEASE_ENDPOINT(ep);
            cpc_exit_api(endpoint_handle);
            return CPC_STATUS_INVALID_STATE;
        }

        (void)timeout;
        (void)flags;
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return status;
    }

    // Allow read even if the state is "error".
    // Error will be returned only when the queue is empty.
    node = slist_pop(receive_queue);
    if (node == NULL) {
        *data = NULL;
        *data_length = 0;
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return CPC_STATUS_EMPTY;
    }

    item = SLIST_ENTRY(node, cpc_receive_queue_item_t, node);
    cpc_free_receive_queue_item(item, data, data_length);
    RELEASE_ENDPOINT(ep);
    cpc_exit_api(endpoint_handle);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Write data from an endpoint
                                                                               ******************************************************************************/
status_t cpc_write(cpc_endpoint_handle_t* endpoint_handle, void* data,
                   uint16_t data_length, uint8_t flags,
                   void* on_write_completed_arg) {
    cpc_endpoint_t* ep;
    status_t status;

    CPC_CHECK_NULL_POINTER(endpoint_handle);
    CPC_CHECK_NULL_POINTER(data);
    if (data_length == 0)
        return CPC_STATUS_INVALID_PARAMETER;

    // Payload must be 4087 or less
    configASSERT(data_length <= CPC_APP_DATA_MAX_LENGTH);

    if (!cpc_enter_api(endpoint_handle))
        return CPC_STATUS_INVALID_STATE;
    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);

    if (ep == NULL) {
        ep = find_endpoint(endpoint_handle->id);
        status = (ep != NULL) ? CPC_STATUS_OWNERSHIP : CPC_STATUS_INVALID_STATE;
        cpc_exit_api(endpoint_handle);
        return status;
    }

#if defined(CATALOG_CPC_SECONDARY_PRESENT)
    // Secondary cannot send poll message
    // Can answer only using the on_poll callback
    configASSERT((flags & CPC_FLAG_UNNUMBERED_POLL) == 0);
#endif

    status = write(ep, data, data_length, flags, on_write_completed_arg);
    cpc_exit_api(endpoint_handle);

    return status;
}

/***************************************************************************/ /**
                                                                               * Get endpoint status
                                                                               ******************************************************************************/
cpc_endpoint_state_t
cpc_get_endpoint_state(cpc_endpoint_handle_t* endpoint_handle) {
    cpc_endpoint_t* ep;
    cpc_endpoint_state_t state;

    MCU_ATOMIC_LOAD(ep, (cpc_endpoint_t*)endpoint_handle->ep);
    if (ep != NULL) {
        MCU_ATOMIC_LOAD(state, ep->state);
        return state;
    }

    ep = find_endpoint(endpoint_handle->id);
    if (ep != NULL) {
        MCU_ATOMIC_LOAD(state, ep->state);
        return state;
    }

    return CPC_STATE_FREED;
}

/***************************************************************************/ /**
                                                                               * Get endpoint encryption
                                                                               ******************************************************************************/
bool cpc_get_endpoint_encryption(cpc_endpoint_handle_t* endpoint_handle) {
    (void)endpoint_handle;
    return false;
}

/***************************************************************************/ /**
                                                                               * Notify remote is disconnected
                                                                               ******************************************************************************/
void cpc_remote_disconnected(uint8_t endpoint_id) {
    cpc_endpoint_t* ep = find_endpoint(endpoint_id);
    if (ep == NULL)
        return; // Endpoint is not being used

    if (ep->state == CPC_STATE_CLOSING) {
        // Stop the close timer
        cpc_timer_stop_timer(ep->close_timer);
        ep->state = CPC_STATE_CLOSED;
        defer_endpoint_free(ep);
        return;
    }

    // Check if endpoint is in error
    if (ep->state == CPC_STATE_OPEN) {
        ep->state = CPC_STATE_ERROR_DESTINATION_UNREACHABLE;

        // Stop re-transmit timeout
        clean_tx_queues(ep);
        notify_error(ep);
    }
}

/***************************************************************************/ /**
                                                                               * Calculate the re transmit timeout
                                                                               * Implemented using Karn’s algorithm
                                                                               * Based off of RFC 2988 Computing TCP's Retransmission Timer
                                                                               ******************************************************************************/
static void compute_re_transmit_timeout(cpc_endpoint_t* endpoint) {
    // Implemented using Karn’s algorithm
    // Based off of RFC 2988 Computing TCP's Retransmission Timer
    static bool first_rtt_measurement = true;

    uint64_t round_trip_time = 0;
    uint64_t rto = 0;
    int64_t delta = 0;

    const long k = 4; // This value is recommended by the Karn’s algorithm
    configASSERT(endpoint != NULL);
    round_trip_time = cpc_timer_get_tick_count64()
                      - endpoint->last_iframe_sent_timestamp;

    if (first_rtt_measurement) {
        endpoint->smoothed_rtt = round_trip_time;
        endpoint->rtt_variation = round_trip_time / 2;
        first_rtt_measurement = false;
    } else {
        // RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'| where beta is 0.25
        delta = ABS((int64_t)endpoint->smoothed_rtt - (int64_t)round_trip_time);
        endpoint->rtt_variation = 3 * (endpoint->rtt_variation / 4) + delta / 4;

        // SRTT <- (1 - alpha) * SRTT + alpha * R' where alpha is 0.125
        endpoint->smoothed_rtt = 7 * (endpoint->smoothed_rtt / 8)
                                 + round_trip_time / 8;
    }

    // Impose a lowerbound on the variation, we don't want the RTO to converge too close to the RTT
    if (endpoint->rtt_variation < cpc_timer_ms_to_tick(
            CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS))
        endpoint->rtt_variation = cpc_timer_ms_to_tick(
            CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS);

    rto = endpoint->smoothed_rtt + k * endpoint->rtt_variation;

    if (rto > cpc_timer_ms_to_tick(CPC_MAX_RE_TRANSMIT_TIMEOUT_MS))
        rto = cpc_timer_ms_to_tick(CPC_MAX_RE_TRANSMIT_TIMEOUT_MS);
    else if (rto < cpc_timer_ms_to_tick(CPC_MIN_RE_TRANSMIT_TIMEOUT_MS))
        rto = cpc_timer_ms_to_tick(CPC_MIN_RE_TRANSMIT_TIMEOUT_MS);

    endpoint->re_transmit_timeout = rto;
}

// extern void cpc_sys_signal(void);

void cpc_signal_event(cpc_signal_type_t signal_type) {
    (void)signal_type;

    MCU_ATOMIC_SECTION(
        if (signal_type == CPC_SIGNAL_RX) {
            if (rx_process_flag == 0) {
                rx_process_flag++;
            }
        } else if (signal_type == CPC_SIGNAL_TX) {
            if (tx_process_flag == 0) {
                tx_process_flag++;
            }
        });

    cpc_sys_signal();
}

uint32_t cpc_get_event_signal(void) {

    uint32_t flag;
    MCU_ATOMIC_SECTION(flag = (rx_process_flag << 1) | tx_process_flag;);

    return flag;
}

void cpc_drv_notify_tx_complete(cpc_buffer_handle_t* buffer_handle) {
    uint8_t control_byte;
    uint8_t frame_type;
    status_t status = CPC_STATUS_OK;
    cpc_endpoint_t* endpoint;
    bool can_be_freed = false;

    // Recover what type is the frame
    control_byte = cpc_hdlc_get_control(buffer_handle->hdlc_header);
    frame_type = cpc_hdlc_get_frame_type(control_byte);

    endpoint = buffer_handle->endpoint;
    if ((endpoint == NULL) // Endpoint already closed
        || ((frame_type != CPC_HDLC_FRAME_TYPE_DATA)
            && (frame_type != CPC_HDLC_FRAME_TYPE_UNNUMBERED))) {
        if ((frame_type == CPC_HDLC_FRAME_TYPE_DATA)
            || (frame_type == CPC_HDLC_FRAME_TYPE_UNNUMBERED)) {
            // Data associated with a closed endpoint,
            // but buffer was still referenced in the driver
            cpc_endpoint_closed_arg_t* arg = buffer_handle->arg;

            configASSERT(arg != NULL);
            if ((frame_type == CPC_HDLC_FRAME_TYPE_DATA)
                && (arg->on_iframe_write_completed != NULL)) {
                // Notify caller that it can free the tx buffer now
                // log_info("cpc_drv_notify_tx_complete");
                arg->on_iframe_write_completed(arg->id, buffer_handle->data,
                                               arg->arg,
                                               CPC_STATUS_TRANSMIT_INCOMPLETE);
            } else if ((frame_type == CPC_HDLC_FRAME_TYPE_UNNUMBERED)
                       && (arg->on_uframe_write_completed != NULL)) {
                arg->on_uframe_write_completed(arg->id, buffer_handle->data,
                                               arg->arg,
                                               CPC_STATUS_TRANSMIT_INCOMPLETE);
            }
            buffer_handle->data = NULL;
            cpc_free_closed_arg(arg);
        }
        // Free handle and data buffer
        cpc_drop_buffer_handle(buffer_handle);
    } else {
        if (frame_type == CPC_HDLC_FRAME_TYPE_DATA) {
            if (buffer_handle->on_write_complete_pending) {
                // Push to the dispatcher queue in order to call on_write_completed outside of IRQ context
                status = cpc_dispatcher_push(
                    &dispatcher_handle, process_deferred_on_write_completed,
                    buffer_handle);
                configASSERT(status == CPC_STATUS_OK);
                buffer_handle->on_write_complete_pending = false;
            } else {
                // Drop the buffer restart re_transmit_timer if it was not acknowledged (still referenced)
                if (cpc_drop_buffer_handle(buffer_handle) == CPC_STATUS_BUSY) {}
            }
        } else if (frame_type == CPC_HDLC_FRAME_TYPE_UNNUMBERED) {
            if (endpoint->on_uframe_write_completed != NULL) {
                // Notify caller that it can free the tx buffer now
                endpoint->on_uframe_write_completed(
                    endpoint->id, buffer_handle->data, buffer_handle->arg,
                    CPC_STATUS_OK);
            }

            // Free handle
            buffer_handle->data = NULL;
            cpc_drop_buffer_handle(buffer_handle);

            // Increase tx window
            endpoint->current_tx_window_space++;

            if (endpoint->state == CPC_STATE_CLOSED)
                can_be_freed = true;
            else if (endpoint->holding_list != NULL) {
                // Put data frames hold in the endpoint in the tx queue if space in transmit window
                while (endpoint->holding_list != NULL
                       && endpoint->current_tx_window_space > 0) {
                    cpc_transmit_queue_item_t* qitem;
                    uint8_t type;
                    slist_node_t* item_node = CPC_POP_BUFFER_HANDLE_LIST(
                        &endpoint->holding_list, cpc_transmit_queue_item_t);
                    qitem = SLIST_ENTRY(item_node, cpc_transmit_queue_item_t,
                                        node);
                    type = cpc_hdlc_get_frame_type(qitem->handle->control);
                    if ((type == CPC_HDLC_FRAME_TYPE_UNNUMBERED)
                        || (endpoint->frames_count_re_transmit_queue == 0)) {
                        // If next frame in holding list is an unnumbered or if no frame to retx
                        // Transmit uframe or iframe
                        cpc_push_back_buffer_handle(&transmit_queue, item_node,
                                                    qitem->handle);
                        endpoint->current_tx_window_space--;
                    } else {
                        // if next is iframe; wait ACK before transmitting
                        cpc_push_buffer_handle(&endpoint->holding_list,
                                               item_node, qitem->handle);
                        break;
                    }
                }
            }
        }
    }

    // Notify task if transmit frames are still queued in endpoints
    if (transmit_queue != NULL) {
        configASSERT(can_be_freed == false);
        cpc_signal_event(CPC_SIGNAL_TX);
    }

    if (can_be_freed)
        defer_endpoint_free(endpoint);
}

void cpc_drv_notify_rx_data(void) { cpc_signal_event(CPC_SIGNAL_RX); }

/***************************************************************************/ /**
                                                                               * Determines if CPC is ok to enter sleep mode.
                                                                               ******************************************************************************/
#if !defined(CATALOG_KERNEL_PRESENT) && defined(CATALOG_POWER_MANAGER_PRESENT)
bool cpc_is_ok_to_sleep(void) {
    return (rx_process_flag == 0
            && (transmit_queue == NULL || !cpc_drv_is_transmit_ready()));
}
#endif

/***************************************************************************/ /**
                                                                               * Determines if CPC is ok to return to sleep mode on ISR exit.
                                                                               ******************************************************************************/
#if !defined(CATALOG_KERNEL_PRESENT) && defined(CATALOG_POWER_MANAGER_PRESENT)
power_manager_on_isr_exit_t cpc_sleep_on_isr_exit(void) {
    return (cpc_is_ok_to_sleep() ? POWER_MANAGER_IGNORE : POWER_MANAGER_WAKEUP);
}
#endif

/***************************************************************************/ /**
                                                                               * Initialize core objects and driver
                                                                               ******************************************************************************/
static status_t init(void) {
    status_t status;

    cpc_drv_get_capabilities(&cpc_driver_capabilities);
    status = cpc_drv_init();
    if (status != CPC_STATUS_OK) {
        configASSERT(false);
        return status;
    }

    status = cpc_system_init();
    if (status != CPC_STATUS_OK) {
        configASSERT(false);
        return status;
    }

    cpc_dispatcher_init_handle(&dispatcher_handle);
    return CPC_STATUS_OK;
}

void cpc_process_action(void) {

    do {
        decode_packet();
        cpc_system_process();
        process_tx_queue();

    } while (rx_process_flag || tx_process_flag);
    process_close();
    cpc_dispatcher_process();
}

static status_t open_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                              uint8_t id, uint8_t flags,
                              uint8_t tx_window_size) {
    cpc_endpoint_t* ep;
    status_t status;

    configASSERT(tx_window_size >= CPC_TRANSMIT_WINDOW_MIN_SIZE);
    configASSERT(tx_window_size <= CPC_TRANSMIT_WINDOW_MAX_SIZE);

    ep = find_endpoint(id);
    if (ep != NULL)
        return (ep->state == CPC_STATE_OPEN) ? CPC_STATUS_ALREADY_EXISTS
                                             : CPC_STATUS_BUSY;

    status = cpc_get_endpoint(&ep);
    if (status != CPC_STATUS_OK)
        return status;

    ep->flags = flags;
    ep->id = id;
    ep->seq = 0;
    ep->ack = 0;
    ep->configured_tx_window_size = tx_window_size;
    ep->current_tx_window_space = ep->configured_tx_window_size;
    ep->frames_count_re_transmit_queue = 0;
    ep->state = CPC_STATE_OPEN;
    ep->node.node = NULL;
    ep->re_transmit_timeout = cpc_timer_ms_to_tick(
        CPC_INIT_RE_TRANSMIT_TIMEOUT_MS);

    slist_init(&ep->iframe_receive_queue);
    slist_init(&ep->uframe_receive_queue);
    slist_init(&ep->re_transmit_queue);
    slist_init(&ep->holding_list);

    LOCK_ENDPOINTS_LIST();
    slist_push(&endpoints, &ep->node);
    slist_sort(&endpoints, sort_endpoints);
    RELEASE_ENDPOINTS_LIST();

    endpoint_handle->id = id;
    MCU_ATOMIC_LOAD(endpoint_handle->ref_count, 1);
    endpoint_handle->ep = ep;

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Check if endpoint ID is already in use
                                                                               ******************************************************************************/
static cpc_endpoint_t* find_endpoint(uint8_t id) {
    cpc_endpoint_t* endpoint;

    //MCU_ENTER_CRITICAL();
    SLIST_FOR_EACH_ENTRY(endpoints, endpoint, cpc_endpoint_t, node) {
        if (endpoint->id == id) {
            // Should not be in the endpoint list if freed
            configASSERT(endpoint->state != CPC_STATE_FREED);
            //MCU_EXIT_CRITICAL();
            return endpoint;
        }
    }

    //MCU_EXIT_CRITICAL();
    return NULL;
}

/***************************************************************************/ /**
                                                                               * Endpoint list sorting function
                                                                               ******************************************************************************/
static bool sort_endpoints(slist_node_t* item_l, slist_node_t* item_r) {
    cpc_endpoint_t* ep_left = SLIST_ENTRY(item_l, cpc_endpoint_t, node);
    cpc_endpoint_t* ep_right = SLIST_ENTRY(item_r, cpc_endpoint_t, node);
    return (ep_left->id < ep_right->id);
}

/***************************************************************************/ /**
                                                                               * Queue a buffer_handle for transmission. This function will either put the
                                                                               * buffer in the transmit queue if there is enough room in the endpoint's TX
                                                                               * window, or in the endpoint holding list otherwise.
                                                                               * This function doesn't lock the endpoint, it's caller's responsibility.
                                                                               ******************************************************************************/
static void queue_for_transmission(cpc_endpoint_t* ep,
                                   cpc_transmit_queue_item_t* item,
                                   cpc_buffer_handle_t* handle) {
    // Check if there is still place in the transmit window
    if (ep->current_tx_window_space > 0) {
        ep->current_tx_window_space--;

        // Put frame in Tx Q so that it can be transmitted by CPC Core later
        MCU_ATOMIC_SECTION(
            cpc_push_back_buffer_handle(&transmit_queue, &item->node, handle););

        // Signal task/process_action that frame is in Tx Queue
        // log_info("tx seq %d ack %d", ep->seq, ep->ack);
        // log_info("control %d", handle->control);
        //cpc_signal_event(CPC_SIGNAL_TX);
    } else {
        //log_warn("ep %d tx queue full", ep->id);
        // Put frame in endpoint holding list to wait for more space in the transmit window
        MCU_ATOMIC_SECTION(cpc_push_back_buffer_handle(&ep->holding_list,
                                                       &item->node, handle););
        //log_warn("tx seq %d ack %d", ep->seq, ep->ack);
    }
    cpc_signal_event(CPC_SIGNAL_TX);
}

/***************************************************************************/ /**
                                                                               * Write data from an endpoint
                                                                               ******************************************************************************/
static status_t write(cpc_endpoint_t* ep, void* data, uint16_t data_length,
                      uint8_t flags, void* on_write_completed_arg) {
    cpc_buffer_handle_t* frame_handle;
    cpc_transmit_queue_item_t* item;
    status_t status;
    bool iframe = true;
    uint16_t fcs;
    uint8_t type = 0;

    LOCK_ENDPOINT(ep);

    if ((flags & CPC_FLAG_UNNUMBERED_INFORMATION)
        || (flags & CPC_FLAG_UNNUMBERED_ACKNOWLEDGE)
        || (flags & CPC_FLAG_UNNUMBERED_POLL)) {
        if (!(ep->flags & CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE)) {
            RELEASE_ENDPOINT(ep);
            return CPC_STATUS_INVALID_PARAMETER;
        }

        iframe = false;

        if (flags & CPC_FLAG_UNNUMBERED_INFORMATION)
            type = CPC_HDLC_CONTROL_UNNUMBERED_TYPE_INFORMATION;
        else if ((flags & CPC_FLAG_UNNUMBERED_POLL))
            type = CPC_HDLC_CONTROL_UNNUMBERED_TYPE_POLL_FINAL;
        else if ((flags & CPC_FLAG_UNNUMBERED_ACKNOWLEDGE))
            type = CPC_HDLC_CONTROL_UNNUMBERED_TYPE_ACKNOWLEDGE;

    } else if (ep->flags & CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE) {
        RELEASE_ENDPOINT(ep);
        return CPC_STATUS_INVALID_PARAMETER;
    }

    if (ep->state != CPC_STATE_OPEN) {
        RELEASE_ENDPOINT(ep);
        return CPC_STATUS_INVALID_STATE;
    }

    // Get item to queue frame
    status = cpc_get_transmit_queue_item(&item);
    if (status != CPC_STATUS_OK) {
        log_error("no tx queue");
        RELEASE_ENDPOINT(ep);
        return status;
    }

    // Get new frame handler
    status = cpc_get_buffer_handle(&frame_handle);
    if (status != CPC_STATUS_OK) {
        log_error("no buffer %X", status);
        configASSERT(cpc_free_transmit_queue_item(item) == CPC_STATUS_OK);
        RELEASE_ENDPOINT(ep);
        return status;
    }

    // Link the data buffer inside the frame buffer
    frame_handle->data = data;
    frame_handle->data_length = data_length;
    frame_handle->endpoint = ep;
    frame_handle->address = ep->id;
    frame_handle->arg = on_write_completed_arg;

    // Add buffer to tx_queue_item
    item->handle = frame_handle;

    if (!iframe)
        frame_handle->control = cpc_hdlc_create_control_unumbered(type);
    else {
        // Set the SEQ number and ACK number in the control byte
        //log_info("tx seq %d ack %d p%d", ep->seq, ep->ack, is_final);
        frame_handle->control = cpc_hdlc_create_control_data(
            ep->seq, ep->ack, flags & CPC_FLAG_INFORMATION_FINAL);
        // Update endpoint sequence number
        ep->seq++;
        ep->seq %= CPC_HDLC_SEQ_RANGE;
    }

    // Compute payload CRC
    fcs = cpc_get_crc_sw(data, data_length);
    frame_handle->fcs[0] = (uint8_t)fcs;
    frame_handle->fcs[1] = (uint8_t)(fcs >> 8);

    queue_for_transmission(ep, item, frame_handle);
    RELEASE_ENDPOINT(ep);

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * De-multiplex receive frame and put it in right endpoint queue.
                                                                               ******************************************************************************/
static void decode_packet(void) {
    uint8_t address;
    uint8_t type;
    uint8_t control;
    uint8_t seq;
    uint8_t ack;
    uint16_t hcs;
    uint16_t data_length;
    cpc_buffer_handle_t* rx_handle;
    cpc_endpoint_t* endpoint;
    uint16_t rx_buffer_payload_len;
    if (cpc_drv_read_data(&rx_handle, &rx_buffer_payload_len)
        != CPC_STATUS_OK) {
        return;
    }
    MCU_ATOMIC_SECTION(if (rx_process_flag) rx_process_flag--;);
    if (!cpc_driver_capabilities.preprocess_hdlc_header) {
        // Validate header checksum. In case it is invalid, drop the packet.
        hcs = cpc_hdlc_get_hcs(rx_handle->hdlc_header);
        if (!cpc_validate_crc_sw(rx_handle->hdlc_header, CPC_HDLC_HEADER_SIZE,
                                 hcs)
            || ((uint8_t*)rx_handle->hdlc_header)[0] != CPC_HDLC_FLAG_VAL) {
            // If HCS is invalid, we drop the packet as we cannot NAK it
            cpc_drop_buffer_handle(rx_handle);
            log_error("INVALID_HEADER_CHECKSUM");
            return;
        }
    }

    address = cpc_hdlc_get_address(rx_handle->hdlc_header);
    control = cpc_hdlc_get_control(rx_handle->hdlc_header);
    data_length = cpc_hdlc_get_length(rx_handle->hdlc_header);
    seq = cpc_hdlc_get_seq(control);
    ack = cpc_hdlc_get_ack(control);
    type = cpc_hdlc_get_frame_type(control);

    // If driver return worst case; set true data size
    rx_handle->data_length = (data_length < rx_buffer_payload_len)
                                 ? data_length
                                 : rx_buffer_payload_len;

    endpoint = find_endpoint(address);
    if (endpoint == NULL) {
        if (type != CPC_HDLC_FRAME_TYPE_SUPERVISORY)
            transmit_reject(NULL, address, 0, CPC_REJECT_UNREACHABLE_ENDPOINT);
        // log_error("Drop");
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    LOCK_ENDPOINT(endpoint);

    if ((data_length == 0)
        && (type == CPC_HDLC_FRAME_TYPE_DATA
            || type == CPC_HDLC_FRAME_TYPE_SUPERVISORY)
        && rx_handle->reason == CPC_REJECT_NO_ERROR) {
        // Clean Tx queue
        if (receive_ack(endpoint, ack) != 0)
            log_error_hexdump("", rx_handle->hdlc_header, 7);
    }

    // We need to keep at least one buffer for receiving acks
    if (type == CPC_HDLC_FRAME_TYPE_DATA && cpc_drv_is_out_of_rx_buffers()) {
        transmit_reject(endpoint, address, endpoint->ack,
                        CPC_REJECT_OUT_OF_MEMORY);
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    if (rx_handle->reason != CPC_REJECT_NO_ERROR) {
        transmit_reject(endpoint, address, endpoint->ack, rx_handle->reason);
        rx_handle->reason = CPC_REJECT_NO_ERROR;
        cpc_drop_buffer_handle(rx_handle);
    } else if (type == CPC_HDLC_FRAME_TYPE_DATA)
        receive_iframe(endpoint, rx_handle, address, control, seq);
    else if (type == CPC_HDLC_FRAME_TYPE_SUPERVISORY)
        receive_sframe(endpoint, rx_handle, control, data_length);
    else if (type == CPC_HDLC_FRAME_TYPE_UNNUMBERED)
        receive_uframe(endpoint, rx_handle, control, data_length);
    else {
        // transmit_reject(endpoint, address, endpoint->ack, CPC_REJECT_ERROR);
        cpc_drop_buffer_handle(rx_handle);
    }

    if (endpoint->state != CPC_STATE_CLOSED) {
        RELEASE_ENDPOINT(endpoint);
        return;
    }

    if (!free_closed_endpoint_if_empty(endpoint)) {
        RELEASE_ENDPOINT(endpoint);
        return;
    }
    return;
}

/***************************************************************************/ /**
                                                                               * Process received ACK frame
                                                                               ******************************************************************************/
static int receive_ack(cpc_endpoint_t* endpoint, uint8_t ack) {
    cpc_transmit_queue_item_t* item;
    slist_node_t* item_node;
    cpc_buffer_handle_t* frame;
    uint8_t control_byte;
    uint8_t seq_number;
    uint8_t ack_range_min;
    uint8_t ack_range_max;
    uint8_t frames_count_ack = 0;
    // Protect the re_transmit_queue from being popped in a re_transmit timeout

    MCU_ENTER_CRITICAL();
    // vTaskSuspend(uart_taskHandle);

    // Return if no frame to acknowledge
    if (endpoint->re_transmit_queue == NULL) {
        // vTaskResume(uart_taskHandle);
        MCU_EXIT_CRITICAL();
        return -1;
    }

    // Get the sequence number of the first frame in the re-transmission queue
    item = SLIST_ENTRY(endpoint->re_transmit_queue, cpc_transmit_queue_item_t,
                       node);
    frame = item->handle;

    control_byte = cpc_hdlc_get_control(frame->hdlc_header);
    seq_number = cpc_hdlc_get_seq(control_byte);

    // Calculate the acceptable ACK number range
    ack_range_min = seq_number + 1;
    ack_range_min %= CPC_HDLC_SEQ_RANGE;

    ack_range_max = seq_number + endpoint->frames_count_re_transmit_queue;
    ack_range_max %= CPC_HDLC_SEQ_RANGE;

    // Check that received ACK number is in range
    if (ack_range_max >= ack_range_min) {
        if (ack < ack_range_min || ack > ack_range_max) {
            log_error("seq_number %d, ack %d", seq_number, ack);
            // Invalid ack number
            // vTaskResume(uart_taskHandle);
            MCU_EXIT_CRITICAL();
            return -1;
        }
    } else if (ack > ack_range_max && ack < ack_range_min) {
        log_error("!");
        // Invalid ack number
        // vTaskResume(uart_taskHandle);
        MCU_EXIT_CRITICAL();
        return -1;
    }

    // Find number of frames acknowledged with ACK number
    if (ack > seq_number)
        frames_count_ack = ack - seq_number;
    else {
        frames_count_ack = CPC_HDLC_SEQ_RANGE - seq_number;
        frames_count_ack += ack;
    }

    // Reset re-transmit counter
    endpoint->packet_re_transmit_count = 0u;

    // Calculate re_transmit_timeout
    compute_re_transmit_timeout(endpoint);

    // Stop incomming re-transmit timeout
    (void)cpc_timer_stop_timer(endpoint->re_transmit_timer);
#if 1
    // Remove all acknowledged frames in re-transmit queue
    for (uint8_t i = 0; i < frames_count_ack; i++) {
        // log_info("count_ack %d i %d", frames_count_ack, i);
        item_node = CPC_POP_BUFFER_HANDLE_LIST(&endpoint->re_transmit_queue,
                                               cpc_transmit_queue_item_t);

        // if (item_node == NULL)
        //     continue;
        configASSERT(item_node != NULL);

        item = SLIST_ENTRY(item_node, cpc_transmit_queue_item_t, node);
        frame = item->handle;

        // if (frame->ref_count == 0) {

        // Only iframe can be acked
        if (endpoint->on_iframe_write_completed != NULL) {
            endpoint->on_iframe_write_completed(endpoint->id, frame->data,
                                                frame->arg, CPC_STATUS_OK);
        }

        // Free the header buffer, the buffer handle and queue item
        frame->data = NULL;
        cpc_drop_buffer_handle(frame);
        frame = NULL;
        // } else {
        // log_error("@");
        // frame->on_write_complete_pending = true;
        // }
        cpc_free_transmit_queue_item(item);

        // Update transmit window
        endpoint->current_tx_window_space++;

        // Update number of frames in re-transmit queue
        endpoint->frames_count_re_transmit_queue--;
    }

    MCU_EXIT_CRITICAL();
    // Put data frames hold in the endpoint in the tx queue if space in transmit window
    while (endpoint->holding_list != NULL
           && endpoint->current_tx_window_space > 0) {
        MCU_ENTER_CRITICAL();
        item_node = CPC_POP_BUFFER_HANDLE_LIST(&endpoint->holding_list,
                                               cpc_transmit_queue_item_t);
        item = SLIST_ENTRY(item_node, cpc_transmit_queue_item_t, node);
        frame = item->handle;
        cpc_push_back_buffer_handle(&transmit_queue, item_node, frame);
        endpoint->current_tx_window_space--;
        MCU_EXIT_CRITICAL();
    }
#endif
    return 0;
}

/***************************************************************************/ /**
                                                                               * Process received iframe
                                                                               ******************************************************************************/
static void receive_iframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t address,
                           uint8_t control, uint8_t seq) {
    status_t status;
    uint16_t rx_buffer_payload_len;
    uint16_t fcs;
    uint32_t reply_data_length = 0;
    void* reply_data = NULL;
    void* on_write_completed_arg = NULL;
    if (endpoint->state == CPC_STATE_CLOSING) {
        // Close endpoint has been called. The Receive side is closed (rx queue cleaned)
        // Endpoint is not yet removed from the list because we need to complete the transmission(s)
        transmit_reject(endpoint, address, 0, CPC_REJECT_UNREACHABLE_ENDPOINT);
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    rx_buffer_payload_len = cpc_hdlc_get_length(rx_handle->hdlc_header) - 2;

    configASSERT(rx_buffer_payload_len <= CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH);

    // Validate payload checksum. In case it is invalid, NAK the packet.
    if (rx_buffer_payload_len > 0) {
        fcs = cpc_hdlc_get_fcs(rx_handle->data, rx_buffer_payload_len);
        if (!cpc_validate_crc_sw(rx_handle->data, rx_buffer_payload_len, fcs)) {
            // transmit_reject(endpoint, address, endpoint->ack,
            //                 CPC_REJECT_CHECKSUM_MISMATCH);
            cpc_drop_buffer_handle(rx_handle);
            return;
        }
    }

    if (endpoint->flags & CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE) {
        // iframe disable; drop packet and send reject
        transmit_reject(endpoint, address, endpoint->ack, CPC_REJECT_ERROR);
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    // data received, Push in Rx Queue and send Ack
    if (seq == endpoint->ack) {
#if defined(CPC_ON_POLL_PRESENT)
        if (cpc_hdlc_is_poll_final(control)) {
#if (!defined(CPC_DEVICE_UNDER_TEST))
            // Only system endpoint can use poll/final
            if (endpoint->id != 0) {
                transmit_reject(endpoint, address, endpoint->ack,
                                CPC_REJECT_ERROR);
                cpc_drop_buffer_handle(rx_handle);
                return;
            }
#endif
            if (endpoint->poll_final.on_poll != NULL) {
                endpoint->poll_final.on_poll(
                    endpoint->id, (void*)CPC_HDLC_FRAME_TYPE_DATA,
                    rx_handle->data, rx_buffer_payload_len, &reply_data,
                    &reply_data_length, &on_write_completed_arg);
            }
            cpc_free_rx_buffer(rx_handle->data);
            rx_handle->data = NULL;
            cpc_drop_buffer_handle(rx_handle);
        }
#endif
        else {
            status_t ret;
            ret = cpc_push_back_rx_data_in_receive_queue(
                rx_handle, &endpoint->iframe_receive_queue,
                rx_buffer_payload_len);

            if (ret != CPC_STATUS_OK) {
                transmit_reject(endpoint, address, endpoint->ack,
                                CPC_REJECT_OUT_OF_MEMORY);
                cpc_drop_buffer_handle(rx_handle);
                return;
            }
        }

        // Notify the user if a callback is registered
        // We expect the users to not call cpc_read from there
        if (endpoint->on_iframe_data_reception != NULL) {
            endpoint->on_iframe_data_reception(
                endpoint->id, endpoint->on_iframe_data_reception_arg);
        }

        // Update endpoint acknowledge number
        endpoint->ack++;
        endpoint->ack %= CPC_HDLC_SEQ_RANGE;
        // Send ack
        transmit_ack(endpoint);
#if defined(CPC_ON_POLL_PRESENT)
        // Send poll reply (final) if required
        if (reply_data != NULL && reply_data_length > 0) {
            status = write(endpoint, reply_data, reply_data_length,
                           CPC_FLAG_INFORMATION_FINAL, on_write_completed_arg);
            if (status != CPC_STATUS_OK)
                log_error("w status 0x%04X", status);
            configASSERT(status == CPC_STATUS_OK);
        } else {
            cpc_free_command_buffer(reply_data);
        }
#endif
    } else if (is_seq_valid(seq, endpoint->ack)) {
        // The packet was already received. We must re-send a ACK because the other side missed it the first time
        transmit_ack(endpoint);
        cpc_drop_buffer_handle(rx_handle);
    } else {
        transmit_reject(endpoint, address, endpoint->ack,
                        CPC_REJECT_SEQUENCE_MISMATCH);
        cpc_drop_buffer_handle(rx_handle);
        return;
    }
}

#if defined(CPC_ENABLE_TEST_FEATURES)
/***************************************************************************/ /**
                                                                               * Received an S-Frame rejecting a frame
                                                                               ******************************************************************************/
__weak void cpc_on_frame_rejected(cpc_endpoint_t* endpoint,
                                  cpc_reject_reason_t reason) {
    (void)endpoint;
    (void)reason;
}
#endif

/***************************************************************************/ /**
                                                                               * Process received sframe
                                                                               ******************************************************************************/
static void receive_sframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t control,
                           uint16_t data_length) {
    // Supervisory packet received
    bool fatal_error = false;
    bool notify = false;
    cpc_endpoint_state_t new_state = endpoint->state;
    uint8_t supervisory_function = cpc_hdlc_get_supervisory_function(control);

    switch (supervisory_function) {
        case CPC_HDLC_ACK_SUPERVISORY_FUNCTION:
            // ACK; already processed previously by receive_ack(), so nothing to do
            break;

        case CPC_HDLC_REJECT_SUPERVISORY_FUNCTION:
            configASSERT((data_length - 2) == CPC_HDLC_REJECT_PAYLOAD_SIZE);
            switch (*((cpc_reject_reason_t*)rx_handle->data)) {
                case CPC_REJECT_SEQUENCE_MISMATCH: {
                    // This is not a fatal error when the tx window is > 1
                    fatal_error = true;
                    new_state = CPC_STATE_ERROR_FAULT;
                    break;
                }
                case CPC_REJECT_CHECKSUM_MISMATCH: {
                    if (endpoint->re_transmit_queue != NULL) {
                        re_transmit_frame(endpoint);
                    }
                    break;
                }
                case CPC_REJECT_OUT_OF_MEMORY: {
                    // Re-transmit mechanism based on the timer will kick in at a later
                    // time and attempt to retransmit the packet. Here we need just to
                    // keep a trace of it and no additional operations are needed
                    break;
                }
                case CPC_REJECT_SECURITY_ISSUE: {
                    fatal_error = true;
                    new_state = CPC_STATE_ERROR_SECURITY_INCIDENT;
                    notify = true;
                    break;
                }
                case CPC_REJECT_UNREACHABLE_ENDPOINT: {
                    fatal_error = true;
                    new_state = CPC_STATE_ERROR_DESTINATION_UNREACHABLE;
                    notify = true;
                    break;
                }
                case CPC_REJECT_ERROR:
                default: {
                    fatal_error = true;
                    new_state = CPC_STATE_ERROR_FAULT;
                    notify = true;
                    break;
                }
            }
#if defined(CPC_ENABLE_TEST_FEATURES)
            cpc_on_frame_rejected(endpoint,
                                  *((cpc_reject_reason_t*)rx_handle->data));
#endif
            break;
        default:
            // Should not reach this case
            configASSERT(false);
            break; // Drop packet by executing the rest of the function
    }

    // Free buffers
    cpc_drop_buffer_handle(rx_handle);

    if ((fatal_error) && (endpoint->state != CPC_STATE_CLOSING)) {
        // Stop incomming re-transmit timeout
        cpc_timer_stop_timer(endpoint->re_transmit_timer);
        endpoint->state = new_state;
        if (notify)
            notify_error(endpoint);
    } else if ((fatal_error) && (endpoint->state == CPC_STATE_CLOSING)) {
        // Force shutdown
        clean_tx_queues(endpoint);
        // Free is completed in free_closed_endpoint_if_empty
    }
}

/***************************************************************************/ /**
                                                                               * Process received uframe
                                                                               ******************************************************************************/
static void receive_uframe(cpc_endpoint_t* endpoint,
                           cpc_buffer_handle_t* rx_handle, uint8_t control,
                           uint16_t data_length) {
    uint16_t payload_len;
    uint8_t type;
    uint16_t fcs;

    payload_len = (data_length > CPC_HDLC_FCS_SIZE)
                      ? data_length - CPC_HDLC_FCS_SIZE
                      : 0;
    if (payload_len > 0) {
        configASSERT(rx_handle->data != NULL);
        fcs = cpc_hdlc_get_fcs(rx_handle->data, payload_len);
    }

    type = cpc_hdlc_get_unumbered_type(control);
    if (payload_len > 0
        && !cpc_validate_crc_sw(rx_handle->data, payload_len, fcs)) {
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    if (!(endpoint->flags & CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE)) {
        cpc_drop_buffer_handle(rx_handle);
        return;
    }

    if ((type == CPC_HDLC_CONTROL_UNNUMBERED_TYPE_INFORMATION)
        && !(endpoint->flags
             & CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE)) {
        if (cpc_push_back_rx_data_in_receive_queue(
                rx_handle, &endpoint->uframe_receive_queue, payload_len)
            != CPC_STATUS_OK) {
            cpc_drop_buffer_handle(rx_handle);
            return;
        }

        // Notify the user if a callback is registered
        // We expect the users to not call cpc_read from there
        if (endpoint->on_uframe_data_reception != NULL) {
            endpoint->on_uframe_data_reception(
                endpoint->id, endpoint->on_uframe_data_reception_arg);
        }
    } else if (type == CPC_HDLC_CONTROL_UNNUMBERED_TYPE_POLL_FINAL) {
        if (endpoint->poll_final.on_poll != NULL) {
            void* reply_data;
            uint32_t reply_data_length = 0;

            void* on_write_completed_arg;
            endpoint->poll_final.on_poll(
                endpoint->id, (void*)CPC_HDLC_FRAME_TYPE_UNNUMBERED,
                rx_handle->data, payload_len, &reply_data, &reply_data_length,
                &on_write_completed_arg);
            if (reply_data != NULL && reply_data_length > 0) {
                status_t status = write(endpoint, reply_data, reply_data_length,
                                        CPC_FLAG_UNNUMBERED_POLL,
                                        on_write_completed_arg);
                if ((status != CPC_STATUS_OK)
                    && (endpoint->on_uframe_write_completed != NULL)) {
                    endpoint->on_uframe_write_completed(
                        endpoint->id, reply_data, on_write_completed_arg,
                        status);
                }
            } else
                cpc_free_command_buffer(reply_data);
        }
        cpc_free_rx_buffer(rx_handle->data);
        rx_handle->data = NULL;
        cpc_drop_buffer_handle(rx_handle);
    } else if (type == CPC_HDLC_CONTROL_UNNUMBERED_TYPE_RESET_SEQ) {
        cpc_drop_buffer_handle(rx_handle);
        if (endpoint->id != 0) {
            // Can only reset sequence numbers on the system endpoint, drop the packet
            return;
        } else {
            // Reset sequence numbers on the system endpoint
            endpoint->seq = 0;
            endpoint->ack = 0;
            // Send an unnumbered acknowledgement
            write(endpoint, NULL, 0, CPC_FLAG_UNNUMBERED_ACKNOWLEDGE, NULL);
        }
    } else {
        cpc_drop_buffer_handle(rx_handle);
        return;
    }
}

/***************************************************************************/ /**
                                                                               * Transmit ACK frame
                                                                               ******************************************************************************/
static status_t transmit_ack(cpc_endpoint_t* endpoint) {
    status_t status;
    cpc_buffer_handle_t* frame_handle;
    cpc_transmit_queue_item_t* item;

    // Get tx queue item
    status = cpc_get_sframe_transmit_queue_item(&item);
    if (status != CPC_STATUS_OK)
        return status;

    // Get new frame handler
    status = cpc_get_buffer_handle(&frame_handle);
    if (status != CPC_STATUS_OK) {
        configASSERT(cpc_free_sframe_transmit_queue_item(item)
                     == CPC_STATUS_OK);
        return status;
    }

    frame_handle->endpoint = endpoint;
    frame_handle->address = endpoint->id;

    // Set ACK number in the supervisory control byte
    frame_handle->control = cpc_hdlc_create_control_supervisory(endpoint->ack,
                                                                0);

    // Put frame in Tx Q so that it can be transmitted by CPC Core later
    item->handle = frame_handle;
    MCU_ATOMIC_SECTION(cpc_push_back_buffer_handle(&transmit_queue, &item->node,
                                                   item->handle););

    return CPC_STATUS_OK;
}

#if defined(CPC_ENABLE_TEST_FEATURES)
/***************************************************************************/ /**
                                                                               * Called on re-transmition of frame
                                                                               ******************************************************************************/
__weak void cpc_on_frame_retransmit(cpc_transmit_queue_item_t* item) {
    (void)item;
}
#endif

/***************************************************************************/ /**
                                                                               * Re-transmit frame
                                                                               ******************************************************************************/
static status_t re_transmit_frame(cpc_endpoint_t* endpoint) {
    cpc_transmit_queue_item_t* item;
    slist_node_t* item_node;
    bool free_hdlc_header = true;

    configASSERT(endpoint != NULL);

    MCU_ATOMIC_SECTION(
        item_node = CPC_POP_BUFFER_HANDLE_LIST(&endpoint->re_transmit_queue,
                                               cpc_transmit_queue_item_t);

        if (item_node == NULL) return CPC_STATUS_NOT_AVAILABLE;

        item = SLIST_ENTRY(item_node, cpc_transmit_queue_item_t, node);
#if defined(CPC_ENABLE_TEST_FEATURES)
        cpc_on_frame_retransmit(item);
#endif

        // Free the header buffer
        if (free_hdlc_header) {
            cpc_free_hdlc_header(item->handle->hdlc_header);
            //item->handle->hdlc_header = NULL;
        }

        // Put frame in Tx Q so that it can be transmitted by CPC Core later
        MCU_ATOMIC_SECTION(cpc_push_buffer_handle(&transmit_queue, &item->node,
                                                  item->handle););

        endpoint->packet_re_transmit_count++;
        endpoint->frames_count_re_transmit_queue--;)
    // Signal task/process_action that frame is in Tx Queue
    cpc_signal_event(CPC_SIGNAL_TX);

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Transmit REJECT frame
                                                                               ******************************************************************************/
static void transmit_reject(cpc_endpoint_t* endpoint, uint8_t address,
                            uint8_t ack, cpc_reject_reason_t reason) {
    uint16_t fcs;
    cpc_buffer_handle_t* handle;
    status_t status;
    cpc_transmit_queue_item_t* item;

    // Get tx queue item
    status = cpc_get_sframe_transmit_queue_item(&item);
    if (status != CPC_STATUS_OK)
        return; // Try again when the primary will re-transmit

    status = cpc_get_reject_buffer(&handle);
    if (status != CPC_STATUS_OK) {
        configASSERT(cpc_free_sframe_transmit_queue_item(item)
                     == CPC_STATUS_OK);
        return; // Try again when the primary will re-transmit
    }

    handle->endpoint = endpoint;
    handle->address = address;

    // Set the SEQ number and ACK number in the control byte
    handle->control = cpc_hdlc_create_control_supervisory(
        ack, CPC_HDLC_REJECT_SUPERVISORY_FUNCTION);

    // Set in reason
    *((uint8_t*)handle->data) = (uint8_t)reason;

    // Compute payload CRC
    MCU_ATOMIC_SECTION({
        fcs = cpc_get_crc_sw(handle->data, 1);
        handle->fcs[0] = (uint8_t)fcs;
        handle->fcs[1] = (uint8_t)(fcs >> 8);
    });

    // Put frame in Tx Q so that it can be transmitted by CPC Core later
    item->handle = handle;
    MCU_ATOMIC_SECTION(
        cpc_push_back_buffer_handle(&transmit_queue, &item->node, handle););
    (void)endpoint;
}

/***************************************************************************/ /**
                                                                               * Transmit the next data frame queued in a endpoint's transmit queue.
                                                                               ******************************************************************************/
static status_t process_tx_queue(void) {
    status_t status;
    slist_node_t* node = NULL;
    cpc_transmit_queue_item_t* item;
    cpc_buffer_handle_t* frame;
    uint8_t frame_type;
    uint16_t data_length;
    slist_node_t* tx_queue;
    bool free_hdlc_header = true;

    if (cpc_drv_is_transmit_ready() == false)
        return CPC_STATUS_TRANSMIT_BUSY;

    MCU_ATOMIC_SECTION(if (tx_process_flag) tx_process_flag--;);
    // This condition is always true when security is not used.
    // It's just to keep the code a bit more clean.
    if (node == NULL) {
        // Return if nothing to transmit
        MCU_ATOMIC_LOAD(tx_queue, transmit_queue);
        if (tx_queue == NULL)
            return CPC_STATUS_EMPTY;

        // Get first queued frame for transmission
        MCU_ATOMIC_SECTION(node = CPC_POP_BUFFER_HANDLE_LIST(
                               &transmit_queue, cpc_transmit_queue_item_t););
    }

    item = SLIST_ENTRY(node, cpc_transmit_queue_item_t, node);

    frame = item->handle;

    frame_type = cpc_hdlc_get_frame_type(frame->control);

    status = cpc_get_hdlc_header_buffer(&frame->hdlc_header);
    if (status != CPC_STATUS_OK) {
        // Retry later on
        MCU_ATOMIC_SECTION(
            cpc_push_buffer_handle(&transmit_queue, &item->node, frame););
        return CPC_STATUS_NO_MORE_RESOURCE;
    }

    data_length = (frame->data_length != 0) ? frame->data_length + 2 : 0;

    if (frame_type == CPC_HDLC_FRAME_TYPE_DATA) {
        // Update ACK cnt with latest
        LOCK_ENDPOINT(frame->endpoint);
        cpc_hdlc_set_control_ack(&frame->control, frame->endpoint->ack);
        RELEASE_ENDPOINT(frame->endpoint);
    }

    /*
     * header must be created after it is known if the frame should be encrypted
     * or not, as it has an impact on the total length of the message.
     */
    //MCU_ENTER_CRITICAL();
    cpc_hdlc_create_header(frame->hdlc_header, frame->address, data_length,
                           frame->control, true);
    //MCU_EXIT_CRITICAL();

    // Pass frame to driver for transmission
    MCU_ENTER_CRITICAL();
    status = cpc_drv_transmit_data(frame, frame->data_length);
    if (status != CPC_STATUS_OK) {
        // Retry later on
        cpc_push_buffer_handle(&transmit_queue, &item->node, frame);

        // In case the driver returns an error we will wait for driver
        // notification before resuming transmission. If the security
        // is used, the HDLC header must not be freed as it's part of
        // the security tag (it's authenticated data), meaning that if
        // the header changes on a subsequent retransmit, the security
        // tag will be invalid and the other end will fail to decrypt.
        if (free_hdlc_header) {
            cpc_free_hdlc_header(frame->hdlc_header);
            //frame->hdlc_header = NULL;
        }
        MCU_EXIT_CRITICAL();
        return status;
    }

    // Put frame in re-transmission queue if it's a I-frame type (with data)
    if (frame_type == CPC_HDLC_FRAME_TYPE_DATA) {
        cpc_push_back_buffer_handle(&frame->endpoint->re_transmit_queue,
                                    &item->node, frame);
        frame->endpoint->frames_count_re_transmit_queue++;
        // Remember when we sent this i-frame in order to calculate round trip time
        // Only do so if this is not a re_transmit
        if (frame->endpoint->packet_re_transmit_count == 0u)
            frame->endpoint->last_iframe_sent_timestamp =
                cpc_timer_get_tick_count64();

        status = cpc_timer_restart_timer(&frame->endpoint->re_transmit_timer,
                                         frame->endpoint->re_transmit_timeout,
                                         re_transmit_timeout_callback, frame);
        configASSERT(status == CPC_STATUS_OK);
        // MCU_EXIT_CRITICAL();
    } else {
        // MCU_EXIT_CRITICAL();
        (frame_type == CPC_HDLC_FRAME_TYPE_SUPERVISORY)
            ? cpc_free_sframe_transmit_queue_item(item)
            : cpc_free_transmit_queue_item(item);
    }
    // MCU_ENTER_CRITICAL();
    // cpc_drv_notify_tx_complete(frame);
    MCU_EXIT_CRITICAL();

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Process endpoint that need to be closed
                                                                               ******************************************************************************/
static void process_close(void) {
    if (closed_endpoint_list == NULL)
        return;

    bool freed;
    slist_node_t* node;
    MCU_ATOMIC_SECTION(node = slist_pop(&closed_endpoint_list););
    do {
        cpc_endpoint_t* endpoint = SLIST_ENTRY(node, cpc_endpoint_t,
                                               node_closing);
        LOCK_ENDPOINT(endpoint);
        freed = free_closed_endpoint_if_empty(endpoint);
        MCU_ATOMIC_SECTION(node = slist_pop(&closed_endpoint_list););
        if (!freed) {
            // Something is preventing us from freeing the endpoint
            // process_close will be called again when freeing is possible
            RELEASE_ENDPOINT(endpoint);
        }
    } while (node != NULL);
}

/***************************************************************************/ /**
                                                                               * Add endpoint to closing list
                                                                               ******************************************************************************/
static void defer_endpoint_free(cpc_endpoint_t* ep) {
    MCU_ATOMIC_SECTION(slist_push(&closed_endpoint_list, &ep->node_closing););
    cpc_signal_event(CPC_SIGNAL_CLOSED);
}

/***************************************************************************/ /**
                                                                               * Called by the dispatcher when the driver completes TX on an i-frame that was acked
                                                                               ******************************************************************************/
static void process_deferred_on_write_completed(void* data) {
    cpc_endpoint_t* endpoint;
    cpc_buffer_handle_t* buffer_handle;

    configASSERT(data != NULL);
    buffer_handle = (cpc_buffer_handle_t*)data;

    endpoint = find_endpoint(buffer_handle->address);

    if (endpoint->on_iframe_write_completed != NULL) {
        // log_info("process_deferred_on_write_completed");
        endpoint->on_iframe_write_completed(endpoint->id, buffer_handle->data,
                                            buffer_handle->arg, CPC_STATUS_OK);
    }

    cpc_drop_buffer_handle(buffer_handle);
}

/***************************************************************************/ /**
                                                                               * Try to free endpoint in closed state (Must be called with the endpoint locked)
                                                                               ******************************************************************************/
static bool free_closed_endpoint_if_empty(cpc_endpoint_t* ep) {
    bool wait = false;
    bool freed = false;

    // This function must not be called if the endpoint is not in the closed state
    configASSERT(ep->state == CPC_STATE_CLOSED);

    if (ep->current_tx_window_space != ep->configured_tx_window_size)
        wait = true;

    // Don't need to check `holding_list` because `receive_ack()` fills the
    // `tx_queue` and reduce `current_tx_window_space`. So it is impossible
    // to have something in the holding list and have `current_tx_window_space`
    // equal to `configured_tx_window_size`

    if (!wait) {
        // Stop incoming re-transmit timeout
        (void)cpc_timer_stop_timer(ep->re_transmit_timer);
        ep->state = CPC_STATE_FREED;
        LOCK_ENDPOINTS_LIST();
        slist_remove(&endpoints, &ep->node);
        RELEASE_ENDPOINTS_LIST();

        cpc_free_endpoint(ep);
        freed = true;
    }

    return freed;
}

/***************************************************************************/ /**
                                                                               * Clean queue item
                                                                               ******************************************************************************/
static void clean_single_queue_item(cpc_endpoint_t* endpoint,
                                    cpc_transmit_queue_item_t* queue_item,
                                    slist_node_t** queue) {
    uint8_t type = cpc_hdlc_get_frame_type(queue_item->handle->control);

    if ((type == CPC_HDLC_FRAME_TYPE_DATA)
        && (endpoint->on_iframe_write_completed != NULL)) {
        log_info("clean_single_queue_item");
        endpoint->on_iframe_write_completed(
            endpoint->id, queue_item->handle->data, queue_item->handle->arg,
            CPC_STATUS_TRANSMIT_INCOMPLETE);
    } else if ((type == CPC_HDLC_FRAME_TYPE_UNNUMBERED)
               && (endpoint->on_uframe_write_completed != NULL)) {
        endpoint->on_uframe_write_completed(
            endpoint->id, queue_item->handle->data, queue_item->handle->arg,
            CPC_STATUS_TRANSMIT_INCOMPLETE);
    }

    CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(queue, &queue_item,
                                       cpc_transmit_queue_item_t);
    queue_item->handle->data = NULL;
    cpc_drop_buffer_handle(queue_item->handle);
    (type == CPC_HDLC_FRAME_TYPE_SUPERVISORY)
        ? cpc_free_sframe_transmit_queue_item(queue_item)
        : cpc_free_transmit_queue_item(queue_item);
}

/***************************************************************************/ /**
                                                                               * Function for freeing items in tx queues
                                                                               ******************************************************************************/
static void clean_tx_queues(cpc_endpoint_t* endpoint) {
    slist_node_t* node;

    LOCK_ENDPOINT(endpoint);

    // Enter atomic region for the following reasons:
    // - Re-transmit timer callback is an ISR and will access the re_transmit_queue.
    // - Transmit completed callback is an ISR and will access the transmit_queue
    // - Transmit completed callback is an ISR and will access the holding_list
    MCU_ENTER_CRITICAL();
    // vTaskSuspend(cpc_sys_taskHandle);
    node = transmit_queue;
    while (node != NULL) {
        cpc_transmit_queue_item_t* queue_item = SLIST_ENTRY(
            node, cpc_transmit_queue_item_t, node);

        node = node->node;
        if (queue_item->handle->endpoint == endpoint) {
            clean_single_queue_item(endpoint, queue_item, &transmit_queue);
            endpoint->current_tx_window_space++;
        }
    }

    node = endpoint->re_transmit_queue;
    while (node != NULL) {
        cpc_transmit_queue_item_t* queue_item = SLIST_ENTRY(
            node, cpc_transmit_queue_item_t, node);

        node = node->node;
        CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(&endpoint->re_transmit_queue,
                                           &queue_item,
                                           cpc_transmit_queue_item_t);
        if (queue_item->handle->ref_count == 0) {
            // Can only be iframe in re_transmit_queue
            if (endpoint->on_iframe_write_completed != NULL) {
                // log_info("clean_tx_queues");
                endpoint->on_iframe_write_completed(
                    endpoint->id, queue_item->handle->data,
                    queue_item->handle->arg, CPC_STATUS_TRANSMIT_INCOMPLETE);
            }
            queue_item->handle->data = NULL;
            cpc_drop_buffer_handle(queue_item->handle);
        } else {
            // Will be freed when driver will notify it has completed the transmit
            cpc_endpoint_closed_arg_t* arg;

            configASSERT(cpc_get_closed_arg(&arg) == CPC_STATUS_OK);
            arg->id = endpoint->id;
            arg->on_iframe_write_completed =
                endpoint->on_iframe_write_completed;
            arg->on_uframe_write_completed =
                endpoint->on_uframe_write_completed;
            arg->arg = queue_item->handle->arg;
            queue_item->handle->endpoint = NULL;
            queue_item->handle->arg = arg;
        }

        cpc_free_transmit_queue_item(queue_item);
        endpoint->frames_count_re_transmit_queue--;
    }

    // Stop incoming re-transmit timeout
    (void)cpc_timer_stop_timer(endpoint->re_transmit_timer);

    endpoint->packet_re_transmit_count = 0u;
    endpoint->current_tx_window_space = endpoint->configured_tx_window_size;

    node = endpoint->holding_list;
    while (node != NULL) {
        cpc_transmit_queue_item_t* queue_item = SLIST_ENTRY(
            node, cpc_transmit_queue_item_t, node);
        node = node->node;
        clean_single_queue_item(endpoint, queue_item, &endpoint->holding_list);
    }
    // vTaskResume(cpc_sys_taskHandle);
    MCU_EXIT_CRITICAL();
    RELEASE_ENDPOINT(endpoint);
}

/***************************************************************************/ /**
                                                                               * Callback for endpoint close timeout
                                                                               ******************************************************************************/
static void endpoint_close_timeout_callback(void* data) {
    cpc_endpoint_t* ep = (cpc_endpoint_t*)data;
    MCU_ENTER_CRITICAL();
    // If true, the host took too long to reply, close the endpoint anyway
    if (ep->state == CPC_STATE_CLOSING) {
        ep->state = CPC_STATE_CLOSED;
        defer_endpoint_free(ep);
    }
    MCU_EXIT_CRITICAL();
}

/***************************************************************************/ /**
                                                                               * Callback for re-transmit frame
                                                                               ******************************************************************************/
static void re_transmit_timeout_callback(void* data) {
    cpc_buffer_handle_t* buffer_handle = (cpc_buffer_handle_t*)data;
    MCU_ENTER_CRITICAL();
    // vTaskSuspend(uart_taskHandle);
    // vTaskSuspend(cpc_sys_taskHandle);
    if (buffer_handle->endpoint->packet_re_transmit_count >= CPC_RE_TRANSMIT) {
        if (buffer_handle->endpoint->id != CPC_ENDPOINT_SYSTEM) {
            // buffer_handle->endpoint->state = CPC_STATE_ERROR_DESTINATION_UNREACHABLE;
            cpc_timer_stop_timer(buffer_handle->endpoint->re_transmit_timer);
            // notify_error(buffer_handle->endpoint);
        }
    } else {
        buffer_handle->endpoint->re_transmit_timeout *= 2;
        // RTO(new) = RTO(before retransmission) *2 )
        // this is explained in Karn’s Algorithm
        if (buffer_handle->endpoint->re_transmit_timeout
            > cpc_timer_ms_to_tick(CPC_MAX_RE_TRANSMIT_TIMEOUT_MS)) {
            buffer_handle->endpoint->re_transmit_timeout = cpc_timer_ms_to_tick(
                CPC_MAX_RE_TRANSMIT_TIMEOUT_MS);
        }
        re_transmit_frame(buffer_handle->endpoint);
    }
    // vTaskResume(uart_taskHandle);
    // vTaskResume(cpc_sys_taskHandle);
    MCU_EXIT_CRITICAL();
}

static void notify_error(cpc_endpoint_t* endpoint) {
    if (endpoint->on_error != NULL)
        endpoint->on_error(endpoint->id, endpoint->on_error_arg);
}

static bool is_seq_valid(uint8_t seq, uint8_t ack) {
    return (seq == (ack - 1u)) || (ack == 0u && seq == CPC_HDLC_SEQ);
}

static bool cpc_enter_api(cpc_endpoint_handle_t* endpoint_handle) {
    if (endpoint_handle == NULL)
        return false;
    MCU_ENTER_CRITICAL();
    if (endpoint_handle->ref_count == 0) {
        MCU_EXIT_CRITICAL();
        return false;
    }
    endpoint_handle->ref_count++;
    MCU_EXIT_CRITICAL();
    return true;
}

/***************************************************************************/ /**
                                                                               * Decrement the endpoint reference counter
                                                                               ******************************************************************************/
static void cpc_exit_api(cpc_endpoint_handle_t* endpoint_handle) {
    MCU_ATOMIC_SECTION(endpoint_handle->ref_count--;);
}

uint32_t __cpc_flash_erase(flash_erase_mode_t mode, uint32_t flash_addr) {
    uint32_t ret;
    MCU_ATOMIC_SECTION({
        while (flash_check_busy()) {};
        ret = flash_erase(mode, flash_addr);
        while (flash_check_busy()) {};
    });
    return ret;
}

void __cpc_flash_write(uint32_t page_addr, uint32_t addr) {
    MCU_ATOMIC_SECTION({
        flash_write_page(addr, page_addr);
        while (flash_check_busy()) {}
    });
}