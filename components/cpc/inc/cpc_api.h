
#ifndef CPC_API_H
#define CPC_API_H

#include <stdarg.h>
#include <stddef.h>

#include "status.h"

#include "cpc.h"
#include "cpc_api.h"
#include "cpc_config.h"
#include "cpc_system_common.h"
#include "cpc_timer.h"
#include "cpc_types.h"
#include "log.h"

#ifndef __weak
#define __weak __attribute__((weak))
#endif

/** Convenience macro for implementing an ATOMIC section. */
#define CORE_ATOMIC_SECTION(yourcode)                                          \
    {                                                                          \
        enter_critical_section();                                              \
        {yourcode} leave_critical_section();                                   \
    }
#define atomic_load(dest, source)                                              \
    {                                                                          \
        enter_critical_section();                                              \
        ((dest) = (source));                                                   \
        leave_critical_section();                                              \
    }
#define atomic_store(dest, source)                                             \
    {                                                                          \
        enter_critical_section();                                              \
        ((source) = (dest));                                                   \
        leave_critical_section();                                              \
    }

//#define MCU_DECLARE_IRQ_STATE   CORE_DECLARE_IRQ_STATE
#define MCU_ENTER_CRITICAL enter_critical_section
#define MCU_EXIT_CRITICAL  leave_critical_section
#define MCU_ATOMIC_SECTION CORE_ATOMIC_SECTION
#define MCU_ENTER_ATOMIC   enter_critical_section
#define MCU_EXIT_ATOMIC    leave_critical_section
#define MCU_ATOMIC_LOAD    atomic_load
#define MCU_ATOMIC_STORE   atomic_store

#define CPC_PROTOCOL_VERSION (3)

#define CPC_ENDPOINT_SYSTEM_M (1)

#ifdef CPC_ENDPOINT_ZIGBEE_ENABLED
#define CPC_ENDPOINT_ZIGBEE_COUNT (1)
#else
#define CPC_ENDPOINT_ZIGBEE_COUNT (0)
#endif

#ifdef CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED
#define CPC_ENDPOINT_BLUETOOTH_RCP_COUNT (1)
#else
#define CPC_ENDPOINT_BLUETOOTH_RCP_COUNT (0)
#endif

// Frame Flags
#define CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE                                  \
    (0x01                                                                      \
     << 0) // I-frame is enabled by default; This flag MUST be set to disable the i-frame support by the endpoint
#define CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE                                   \
    (0x01                                                                      \
     << 1) // U-frame is disabled by default; This flag MUST be set to enable u-frame support by the endpoint
#define CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE (0x01 << 2)

// CPC_FLAG_NO_BLOCK = 0x01 << 0
#define CPC_FLAG_UNNUMBERED_INFORMATION (0x01 << 1)
#define CPC_FLAG_UNNUMBERED_POLL        (0x01 << 2)
#define CPC_FLAG_UNNUMBERED_ACKNOWLEDGE (0x01 << 3)
#define CPC_FLAG_INFORMATION_FINAL      (0x01 << 4)

/// @brief Enumeration representing the possible id
SENUM(cpc_service_endpoint_id_t){
    CPC_ENDPOINT_SYSTEM = 0, ///< System control
#ifdef CPC_ENDPOINT_ZIGBEE_ENABLED
    CPC_ENDPOINT_ZIGBEE = 5, ///< ZigBee EZSP endpoint
#endif                       // CPC_ENDPOINT_ZIGBEE_ENABLED
#ifdef CPC_ENDPOINT_15_4_ENABLED
    CPC_ENDPOINT_15_4 = 12, ///< 802.15.4 endpoint
#endif                      // CPC_ENDPOINT_15_4_ENABLED
#ifdef CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED
    CPC_ENDPOINT_BLUETOOTH_RCP = 14, ///< Bluetooth RCP endpoint
#endif                               // CPC_ENDPOINT_BLUETOOTH_RCP_ENABLED
    CPC_ENDPOINT_LAST_ID_MARKER,     // DO NOT USE THIS ENDPOINT ID
};

#if defined(CATALOG_CPC_PRIMARY_PRESENT)
#define CPC_ON_FINAL_PRESENT
#endif

#define CPC_SERVICE_ENDPOINT_ID_START ((uint8_t)CPC_ENDPOINT_SYSTEM)
#define CPC_SERVICE_ENDPOINT_ID_END   ((uint8_t)CPC_ENDPOINT_LAST_ID_MARKER - 1)
#define CPC_SERVICE_ENDPOINT_MAX_COUNT                                         \
    (CPC_SERVICE_ENDPOINT_ID_END - CPC_SERVICE_ENDPOINT_ID_START + 1)

/***************************************************************************/ /**
 * The maximum size of a system endpoint command buffer.
 *
 * @note
 *   For the moment, this value must be manually set.
 *
 * @note : it is set to the size of the payload of a
 * CMD_PROPERTY_GET::PROP_ENDPOINT_STATES.
 *
 ******************************************************************************/
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define CPC_ENDPOINT_MAX_COUNT 256

#define CPC_SYSTEM_COMMAND_BUFFER_SIZE                                         \
    (sizeof(cpc_system_cmd_t) + sizeof(cpc_system_property_cmd_t)              \
     + CPC_ENDPOINT_MAX_COUNT * sizeof(cpc_endpoint_state_t) / 2)

#ifndef CPC_SYSTEM_COMMAND_BUFFER_COUNT
#define CPC_SYSTEM_COMMAND_BUFFER_COUNT 5
#endif

#define CPC_RX_DATA_MAX_LENGTH (CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH + 2)

#define CPC_HDLC_REJECT_MAX_COUNT          ((CPC_RX_BUFFER_MAX_COUNT / 2) + 1)
#define CPC_RX_QUEUE_ITEM_MAX_COUNT        (1 + ((CPC_RX_BUFFER_MAX_COUNT / 4) * 3))
#define CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT (CPC_RX_QUEUE_ITEM_MAX_COUNT)
#define CPC_BUFFER_HANDLE_MAX_COUNT                                            \
    (CPC_TX_QUEUE_ITEM_MAX_COUNT + CPC_RX_BUFFER_MAX_COUNT                     \
     + CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT)
#define CPC_RE_TRANSMIT                                  (4)
#define CPC_MAX_RE_TRANSMIT_TIMEOUT_MS                   (50)
#define CPC_INIT_RE_TRANSMIT_TIMEOUT_MS                  (20)
#define CPC_MIN_RE_TRANSMIT_TIMEOUT_MS                   (20)
#define CPC_DISCONNECTION_NOTIFICATION_TIMEOUT_MS        (1000)
#define CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS (3)

#if (CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH > 4087)
#error Invalid CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH; Must be less or equal to 4087
#endif

#if !defined(CPC_ENDPOINT_TEMPORARY_MAX_COUNT)
#if defined(CATALOG_CPC_PERF_PRESENT)
#define CPC_ENDPOINT_TEMPORARY_MAX_COUNT (10)
#else
#define CPC_ENDPOINT_TEMPORARY_MAX_COUNT (0) // Not yet available
#endif
#endif

#define CPC_TEMPORARY_ENDPOINT_ID_START 100
#define CPC_TEMPORARY_ENDPOINT_ID_END                                          \
    CPC_TEMPORARY_ENDPOINT_ID_START + CPC_ENDPOINT_TEMPORARY_MAX_COUNT - 1
#if (CPC_TEMPORARY_ENDPOINT_ID_END > 254)
#error Invalid TEMPORARY ENDPOINT COUNT
#endif

#define CPC_ENDPOINT_MIN_COUNT (CPC_ENDPOINT_SYSTEM_M)

#define CPC_ENDPOINT_INTERNAL_COUNT                                            \
    (CPC_ENDPOINT_SYSTEM_M + CPC_ENDPOINT_ZIGBEE_COUNT                         \
     + CPC_ENDPOINT_BLUETOOTH_RCP_COUNT)

#if !defined(CPC_ENDPOINT_COUNT)
#define CPC_ENDPOINT_COUNT                                                     \
    (CPC_ENDPOINT_INTERNAL_COUNT + CPC_USER_ENDPOINT_MAX_COUNT                 \
     + CPC_ENDPOINT_TEMPORARY_MAX_COUNT)
#endif

#if (CPC_ENDPOINT_COUNT > CPC_ENDPOINT_MAX_COUNT)
#error Invalid CPC_ENDPOINT_COUNT; Must be less than CPC_ENDPOINT_MAX_COUNT
#elif (CPC_ENDPOINT_COUNT < CPC_ENDPOINT_MIN_COUNT)
#error Invalid CPC_ENDPOINT_COUNT; Must be greater than CPC_ENDPOINT_MIN_COUNT
#endif

#define CPC_HDLC_HEADER_MAX_COUNT                                              \
    (CPC_RX_BUFFER_MAX_COUNT                                                   \
     + (MIN(CPC_TRANSMIT_WINDOW_MAX_SIZE * CPC_ENDPOINT_COUNT,                 \
            CPC_BUFFER_HANDLE_MAX_COUNT)))

#define CPC_POP_BUFFER_HANDLE_LIST(head_ptr, item_type)                        \
    ({                                                                         \
        slist_node_t* item_node = slist_pop(head_ptr);                         \
        item_type* item = SLIST_ENTRY(item_node, item_type, node);             \
        if (item != NULL) {                                                    \
            configASSERT(item->handle != NULL);                                \
            if (item->handle->ref_count == 0)                                  \
                log_error("%p", item->handle);                                 \
            configASSERT(item->handle->ref_count > 0);                         \
            MCU_ATOMIC_SECTION(item->handle->ref_count--;)                     \
        }                                                                      \
        item_node;                                                             \
    })

#define CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(head_ptr, item_ptr, item_type)      \
    ({                                                                         \
        item_type* item = *item_ptr;                                           \
        configASSERT(item != NULL);                                            \
        configASSERT(item->handle != NULL);                                    \
        configASSERT(item->handle->ref_count > 0);                             \
        MCU_ATOMIC_SECTION(item->handle->ref_count--;)                         \
        slist_remove(head_ptr, &item->node);                                   \
    })

#if CPC_USER_ENDPOINT_MAX_COUNT > 10
#error "CPC_USER_ENDPOINT_MAX_COUNT must be less than 10"
#endif

// Signal count max = closing signal per endpoint + tx queue items + rx queue items
#define EVENT_SIGNAL_MAX_COUNT                                                 \
    CPC_ENDPOINT_COUNT + CPC_TX_QUEUE_ITEM_MAX_COUNT                           \
        + CPC_RX_QUEUE_ITEM_MAX_COUNT

SENUM(cpc_signal_type_t){
    CPC_SIGNAL_RX,
    CPC_SIGNAL_TX,
    CPC_SIGNAL_CLOSED,
    CPC_SIGNAL_SYSTEM,
};

SENUM(cpc_reject_reason_t){CPC_REJECT_NO_ERROR = 0,
                           CPC_REJECT_CHECKSUM_MISMATCH,
                           CPC_REJECT_SEQUENCE_MISMATCH,
                           CPC_REJECT_OUT_OF_MEMORY,
                           CPC_REJECT_SECURITY_ISSUE,
                           CPC_REJECT_UNREACHABLE_ENDPOINT,
                           CPC_REJECT_ERROR};

SENUM(cpc_buffer_type_t){CPC_UNKNOWN_BUFFER, CPC_RX_BUFFER,
                         CPC_HDLC_REJECT_BUFFER};

typedef struct {
    void* hdlc_header;
    void* data;
    uint16_t data_length;
    uint8_t fcs[2];
    uint8_t control;
    uint8_t address;
    uint8_t ref_count;
    cpc_buffer_type_t buffer_type;
    cpc_endpoint_t* endpoint;
    cpc_reject_reason_t reason;
    void* arg;
    bool on_write_complete_pending;
} cpc_buffer_handle_t;

typedef struct {
    uint8_t id;
    cpc_on_write_completed_t on_iframe_write_completed;
    cpc_on_write_completed_t on_uframe_write_completed;
    void* arg;
} cpc_endpoint_closed_arg_t;

typedef struct {
    slist_node_t node;
    cpc_buffer_handle_t* handle;
} cpc_transmit_queue_item_t;

typedef struct {
    slist_node_t node;
    void* data;
    uint16_t data_length;
    cpc_buffer_type_t buffer_type;
} cpc_receive_queue_item_t;

typedef void (*cpc_dispatcher_fnct_t)(void* data);

typedef struct {
    slist_node_t node;
    cpc_dispatcher_fnct_t fnct;
    void* data;
    bool submitted;
} cpc_dispatcher_handle_t;

#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 *
 * @param[in] endpoint_handle  Endpoint Handle.
 *
 * @param[in] id  Endpoint ID.
 *
 * @param[in] flags  Endpoint flags.
 *                      CPC_OPEN_ENDPOINT_FLAG_NONE                Default behaviors
 *                      CPC_OPEN_ENDPOINT_FLAG_DISABLE_ENCRYPTION  Disable encryption on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE      Discard I - frame received on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE       Enable reception of U-frame on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE  Only allow U-frame of type P/F
 *
 * @param[in] tx_window_size  Endpoint TX Window size.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_open_service_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                   cpc_service_endpoint_id_t id, uint8_t flags,
                                   uint8_t tx_window_size);

/***************************************************************************/ /**
 * Open temporary endpoint.
 *
 * @param[in] endpoint_handle  Endpoint Handle.
 *
 * @param[out] id  Endpoint ID.
 *
 * @param[in] flags  Endpoint flags:
 *                      CPC_OPEN_ENDPOINT_FLAG_NONE                Default behaviors
 *                      CPC_OPEN_ENDPOINT_FLAG_DISABLE_ENCRYPTION  Disable encryption on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE      Discard I-frame received on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE       Enable reception of u-frame on the endpoint
 *                      CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE  Only allow U-frame of type P/F
 *
 * @param[in] tx_window_size  Endpoint TX Window size.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_open_temporary_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                     uint8_t* id, uint8_t flags,
                                     uint8_t tx_window_size);

/***************************************************************************/ /**
 * Initialize CPC buffers' handling module.
 ******************************************************************************/
void cpc_init_buffers(void);

/***************************************************************************/ /**
 * Get a CPC buffer handle.
 *
 * @param[out] handle  Address of the variable that will receive the handle
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_buffer_handle(cpc_buffer_handle_t** handle);

/***************************************************************************/ /**
 * Get a CPC header buffer.
 *
 * @param[out] buffer  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_hdlc_header_buffer(void** buffer);

/***************************************************************************/ /**
 * Get a CPC header and buffer for transmitting a reject packet.
 *
 * @param[out] handle  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_reject_buffer(cpc_buffer_handle_t** handle);

/***************************************************************************/ /**
 * Get a CPC RAW buffer for reception.
 *
 * @param[out] handle  Address of the variable that will receive the RAW buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_raw_rx_buffer(void** raw_rx_buffer);

/***************************************************************************/ /**
 * Free a CPC RAW rx buffer.
 *
 * @param[out] handle  Address of the variable that will receive the RAW buffer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_raw_rx_buffer(void* raw_rx_buffer);

/***************************************************************************/ /**
 * Get a CPC buffer for reception. This also allocates a buffer for the HDLC
 * header and a RX buffer if necessary.
 *
 * @param[out] handle  Address of the variable that will receive the buffer
 *                     pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_buffer_handle_for_rx(cpc_buffer_handle_t** handle);

/***************************************************************************/ /**
 * Free header, buffer and handle.
 *
 * @param[in] handle  Handle to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_drop_buffer_handle(cpc_buffer_handle_t* handle);

/***************************************************************************/ /**
 * Allocate queue item and push data buffer in receive queue then free
 * header and buffer handle.
 *
 * @param[in] handle  Handle to free.
 * @param[in] head    Queue head pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_push_back_rx_data_in_receive_queue(cpc_buffer_handle_t* handle,
                                                slist_node_t** head,
                                                uint16_t data_length);

/***************************************************************************/ /**
 * Free CPC buffer.
 *
 * @param[in] buffer  Pointer to hdlc header to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_hdlc_header(void* data);

/***************************************************************************/ /**
 * Free CPC system command buffer.
 *
 * @param[in] item Pointer to system command buffer to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_command_buffer(cpc_system_cmd_t* item);

/***************************************************************************/ /**
 * Get a a system command buffer.
 *
 * @param[out] item Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_system_command_buffer(cpc_system_cmd_t** item);

/***************************************************************************/ /**
 * Get a receive queue item.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_receive_queue_item(cpc_receive_queue_item_t** item);

/***************************************************************************/ /**
 * Free receive queue item.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @param[out] data  Pointer to variable that will receive the rx buffer pointer.
 *
 * @param[out] data_length  Pointer to variable that will receive the data length.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_receive_queue_item(cpc_receive_queue_item_t* item,
                                     void** data, uint16_t* data_length);

/***************************************************************************/ /**
 * Free receive queue item and data buffer.
 *
 * @param[in] item  Pointer to item to free.
 ******************************************************************************/
void cpc_drop_receive_queue_item(cpc_receive_queue_item_t* item);

/***************************************************************************/ /**
 * Get a transmit queue item.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_transmit_queue_item(cpc_transmit_queue_item_t** item);

/***************************************************************************/ /**
 * Free transmit queue item.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_transmit_queue_item(cpc_transmit_queue_item_t* item);

/***************************************************************************/ /**
 * Get a transmit queue item for S-Frame.
 *
 * @param[out] item  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_sframe_transmit_queue_item(cpc_transmit_queue_item_t** item);

/***************************************************************************/ /**
 * Free transmit queue item from S-Frame pool.
 *
 * @param[in] item  Pointer to item to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_sframe_transmit_queue_item(cpc_transmit_queue_item_t* item);

/***************************************************************************/ /**
 * Get an endoint.
 *
 * @param[out] endpoint  Address of the variable that will receive the item pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_endpoint(cpc_endpoint_t** endpoint);

/***************************************************************************/ /**
 * Free endpoint.
 *
 * @param[in] endpoint  Pointer to endpoint to free.
 ******************************************************************************/
void cpc_free_endpoint(cpc_endpoint_t* endpoint);

/***************************************************************************/ /**
 * Get endpoint closed argument item.
 *
 *@param[out] endpoint  Address of the variable that will receive the argument
 *                      item pointer.
 *
 *@return Status code.
 ******************************************************************************/
status_t cpc_get_closed_arg(cpc_endpoint_closed_arg_t** arg);

/***************************************************************************/ /**
 * Free endpoint closed argument.
 *
 * @param[in] endpoint  Pointer to endpoint to free.
 ******************************************************************************/
void cpc_free_closed_arg(cpc_endpoint_closed_arg_t* arg);

/***************************************************************************/ /**
 * Signal process needed.
 ******************************************************************************/
void cpc_signal_event(cpc_signal_type_t signal_type);

/***************************************************************************/ /**
 * Push back a list item containing an allocated buffer handle.
 * This list must then be popped using the macro CPC_POP_BUFFER_HANDLE_LIST.
 ******************************************************************************/
void cpc_push_back_buffer_handle(slist_node_t** head, slist_node_t* item,
                                 cpc_buffer_handle_t* buf_handle);

/***************************************************************************/ /**
 * Push a list item containing an allocated buffer handle.
 * This list must then be popped using the macro CPC_POP_BUFFER_HANDLE_LIST.
 ******************************************************************************/
void cpc_push_buffer_handle(slist_node_t** head, slist_node_t* item,
                            cpc_buffer_handle_t* buf_handle);

/***************************************************************************/ /**
 * Get a buffer to store a security tag.
 *
 * @param[out] tag_buffer  Address of the variable that will receive the argument
 *                         tag_buffer pointer.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_get_security_tag_buffer(void** tag_buffer);

/***************************************************************************/ /**
 * Free a security tag buffer.
 *
 * @param[in] tag_buffer  Pointer to the buffer to free.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_free_security_tag_buffer(void* tag_buffer);

/***************************************************************************/ /**
 * Initialize CPC System Endpoint.
 *
 * @brief
 *   This function initializes the system endpoint module by opening the system
 *   endpoint. This function must be called after CPC init.
 ******************************************************************************/
status_t cpc_system_init(void);

/***************************************************************************/ /**
 * Process the system endpoint.
 *
 * @brief
 *   This function reads incoming commands on the system endpoint and processes
 *   them accordingly.
 ******************************************************************************/
void cpc_system_process(void);

/***************************************************************************/ /**
 * Initialize the dispatcher handle.
 *
 * @param[in] handle  Dispatch queue node.
 ******************************************************************************/
void cpc_dispatcher_init_handle(cpc_dispatcher_handle_t* handle);

/***************************************************************************/ /**
 * Push function in dispatch queue along with the data to be passed when
 * dispatched.
 *
 * @param[in] handle  Dispatch queue node.
 * @param[in] fnct    Function to be dispatched.
 * @param[in] data    Data to pass to the function.
 *
 * @return Status code.
 ******************************************************************************/
status_t cpc_dispatcher_push(cpc_dispatcher_handle_t* handle,
                             cpc_dispatcher_fnct_t fnct, void* data);

/***************************************************************************/ /**
 * Remove function from dispatch queue along with the data to be passed when
 * dispatched.
 *
 * @param[in] handle  Dispatch queue node.
 ******************************************************************************/
void cpc_dispatcher_cancel(cpc_dispatcher_handle_t* handle);

/***************************************************************************/ /**
 * Process the dispatch queue.
 *
 * @brief
 *   This function empty the dispatch queue by calling all the functions
 *   registered.
 ******************************************************************************/
void cpc_dispatcher_process(void);

/***************************************************************************/ /**
 * Notify the user that an endpoint on the host has closed.
 ******************************************************************************/
void cpc_remote_disconnected(uint8_t endpoint_id);

/***************************************************************************/ /**
 * Endpoint was closed, notify the host.
 ******************************************************************************/
status_t cpc_send_disconnection_notification(uint8_t endpoint_id);

/***************************************************************************/ /**
 * Called on re-transmition of frame.
 ******************************************************************************/
void cpc_on_frame_retransmit(cpc_transmit_queue_item_t* item);
void cpc_set_state(cpc_endpoint_handle_t* endpoint_handle,
                   cpc_endpoint_state_t state);
/** @} (end addtogroup cpc) */

#ifdef __cplusplus
}
#endif

#endif // CPC_H
