/**
 * @file cpc.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CPC_H
#define CPC_H

#if defined(__linux__)
#error Wrong platform - this header file is intended for the secondary application
#endif

#include "cpc_config.h"

#include "status.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "stdint.h"

#include <FreeRTOS.h>
#include "mem_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SENUM(name)                                                            \
    typedef uint8_t name;                                                      \
    enum name##_enum
#define SENUM_GENERIC(name, type)                                              \
    typedef type name;                                                         \
    enum name##_enum

// Data Types

/// @brief Enumeration representing user endpoint.
SENUM(cpc_user_endpoint_id_t){
    CPC_ENDPOINT_USER_ID_0 = 90, ///< User endpoint ID 0
    CPC_ENDPOINT_USER_ID_1 = 91, ///< User endpoint ID 1
    CPC_ENDPOINT_USER_ID_2 = 92, ///< User endpoint ID 2
    CPC_ENDPOINT_USER_ID_3 = 93, ///< User endpoint ID 3
    CPC_ENDPOINT_USER_ID_4 = 94, ///< User endpoint ID 4
    CPC_ENDPOINT_USER_ID_5 = 95, ///< User endpoint ID 5
    CPC_ENDPOINT_USER_ID_6 = 96, ///< User endpoint ID 6
    CPC_ENDPOINT_USER_ID_7 = 97, ///< User endpoint ID 7
    CPC_ENDPOINT_USER_ID_8 = 98, ///< User endpoint ID 8
    CPC_ENDPOINT_USER_ID_9 = 99, ///< User endpoint ID 9
};

/// @brief Enumeration representing the possible endpoint state.
SENUM(cpc_endpoint_state_t){
    CPC_STATE_OPEN = 0,                      ///< State open
    CPC_STATE_CLOSED,                        ///< State close
    CPC_STATE_CLOSING,                       ///< State closing
    CPC_STATE_ERROR_DESTINATION_UNREACHABLE, ///< Error state, destination unreachable
    CPC_STATE_ERROR_SECURITY_INCIDENT,       ///< Error state, security incident
    CPC_STATE_ERROR_FAULT,                   ///< Error state, fault
    CPC_STATE_FREED,                         ///< State freed
};

/// @brief Enumeration representing the possible configurable options for an endpoint.
SENUM(cpc_endpoint_option_t){
    CPC_ENDPOINT_ON_IFRAME_RECEIVE =
        0, ///< Set callback for iframe received notification
    CPC_ENDPOINT_ON_IFRAME_RECEIVE_ARG, ///< Set callback argument for iframe received notification
    CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, ///< Set callback for write complete notification
    CPC_ENDPOINT_ON_ERROR,     ///< Set callback for error notification
    CPC_ENDPOINT_ON_ERROR_ARG, ///< Set callback argument for error notification
    // Private options
    CPC_ENDPOINT_ON_UFRAME_RECEIVE,
    CPC_ENDPOINT_ON_UFRAME_RECEIVE_ARG,
    CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED,
    CPC_ENDPOINT_ON_POLL,
    CPC_ENDPOINT_ON_POLL_ARG,
    CPC_ENDPOINT_ON_FINAL,
    CPC_ENDPOINT_ON_FINAL_ARG,
};

#define CPC_OPEN_ENDPOINT_FLAG_NONE               0
#define CPC_OPEN_ENDPOINT_FLAG_DISABLE_ENCRYPTION (0x01 << 3)

typedef void (*cpc_on_write_completed_t)(cpc_user_endpoint_id_t endpoint_id,
                                         void* buffer, void* arg,
                                         status_t status);

/***************************************************************************/ /**
                                                                                   * Typedef for the user - supplied callback function which is called when
                                                                                   * CPC receive data on an endpoint.
                                                                                   *
                                                                                   * @param endpoint_id   Endpoint ID
                                                                                   *
                                                                                   * @param arg   User-specific argument .
                                                                                   ******************************************************************************/
typedef void (*cpc_on_data_reception_t)(uint8_t endpoint_id, void* arg);

/***************************************************************************/ /**
                                                                                   * Typedef for the user-supplied callback function which is called when
                                                                                   * CPC detects a fatal error on an endpoint.
                                                                                   *
                                                                                   * @param endpoint_id   Endpoint ID
                                                                                   *
                                                                                   * @param arg   User-specific argument .
                                                                                   ******************************************************************************/
typedef void (*cpc_on_error_callback_t)(uint8_t endpoint_id, void* arg);

/// @brief Struct representing an CPC endpoint handle.
typedef struct {
    void* ep;           ///< Endpoint object; Do not touch
    uint8_t id;         ///< Endpoint ID; Do not touch
    uint32_t ref_count; ///< Endpoint reference counter; Do not touch
} cpc_endpoint_handle_t;

/// @brief Struct representing CPC Core debug stats.
typedef struct {
    uint32_t rxd_packet;     ///< Number of packet received
    uint32_t rxd_data_frame; ///< Number of frame with payload (dataframe);
    uint32_t rxd_data_frame_queued;  ///< Number of dataframe with data queued
    uint32_t rxd_data_frame_dropped; ///< Number of dataframe with data dropped

    uint32_t rxd_supervisory_frame; ///< Number of supervisory frame received
    uint32_t
        rxd_supervisory_frame_processed; ///< Number of supervisory frame processed
    uint32_t
        rxd_supervisory_frame_dropped; ///< Number of supervisory frame dropped

    uint32_t rxd_unnumbered_frame; ///< Number of unnumbered frame received
    uint32_t
        rxd_unnumbered_frame_processed; ///< Number of unnumbered frame processed
    uint32_t
        rxd_unnumbered_frame_dropped; ///< Number of unnumbered frame dropped

    uint32_t
        rxd_duplicate_data_frame; ///< Number of duplicate dataframe received
    uint32_t rxd_ack;             ///< Number of ACK supervisory-frame received
    uint32_t
        rxd_reject_destination_unreachable; ///< Number of destination unreachable supervisory-frame received
    uint32_t
        rxd_reject_seq_mismatch; ///< Number of out of order supervisory-frame received
    uint32_t
        rxd_reject_checksum_mismatch; ///< Number of checksum error supervisory-frame received
    uint32_t
        rxd_reject_security_issue; ///< Number of security issue supervisory-frame received
    uint32_t
        rxd_reject_out_of_memory; ///< Number of out of memorry supervisory-frame received
    uint32_t rxd_reject_fault; ///< Number of fault supervisory-frame received

    uint32_t txd_data_frame; ///< Number of dataframe transmitted
    uint32_t txd_ack;        ///< Number of ACK supervisory-frame transmitted
    uint32_t
        txd_reject_destination_unreachable; ///< Number of destination unreachable supervisory-frame transmitted
    uint32_t
        txd_reject_seq_mismatch; ///< Number of out of order supervisory-frame transmitted
    uint32_t
        txd_reject_checksum_mismatch; ///< Number of checksum error supervisory-frame transmitted
    uint32_t
        txd_reject_security_issue; ///< Number of security issue supervisory-frame transmitted
    uint32_t
        txd_reject_out_of_memory; ///< Number of out of memorry supervisory-frame transmitted
    uint32_t
        txd_reject_fault; ///< Number of fault supervisory-frame transmitted

    uint32_t retxd_data_frame; ///< Number of dataframe retransmitted

    uint32_t
        frame_transmit_submitted; ///< Number of frame submitted to the driver
    uint32_t
        frame_transmit_completed; ///< Number of frame confirmed sent by the driver
    uint32_t
        data_frame_transmit_completed; ///< Number of dataframe-frame confirmed sent by the driver
    uint32_t
        supervisory_frame_transmit_completed; ///< Number of supervisory-frame confirmed sent by the driver
} cpc_endpoint_debug_counters_t;

/// @brief Struct representing CPC Core debug counters.
typedef struct {
    uint32_t endpoint_opened;        ///< Number of endpoint opened
    uint32_t endpoint_closed;        ///< Number of endpoint closed
    uint32_t rxd_frame;              ///< Total number of frame received
    uint32_t rxd_valid_iframe;       ///< Total number of i-frame received
    uint32_t rxd_valid_uframe;       ///< Total number of u-frame received
    uint32_t rxd_valid_sframe;       ///< Total number of s-frame received
    uint32_t rxd_data_frame_dropped; ///< Total number of frame dropped
    uint32_t
        txd_reject_destination_unreachable; ///< Total number of unreachable destination transmisted
    uint32_t txd_reject_error_fault; ///< Total number of fault transmisted
    uint32_t
        txd_completed; ///< Total number of frame confirmed sent by the driver
    uint32_t retxd_data_frame; ///< Total number of dataframe retransmission
    uint32_t driver_error;     ///< Total number of error reported by the driver
    uint32_t
        driver_packet_dropped; ///< Total number of frame dropped by the driver
    uint32_t
        invalid_header_checksum; ///< Total number of frame received with invalid header checksum
    uint32_t
        invalid_payload_checksum; ///< Total number of frame received with invalid frame checksum
} cpc_core_debug_counters_t;

/// @brief Struct representing a memory pool handle.
typedef struct cpc_mem_pool_handle_t {
    mem_pool_handle_t* pool_handle; ///< Pool handle; Do not touch
    uint32_t used_block_cnt;        ///< Number of block in use
} cpc_mem_pool_handle_t;

/// @brief Struct representing a memory pool debug.
typedef struct {
    cpc_mem_pool_handle_t*
        buffer_handle; ///< Buffer handle object memory pool usage
    cpc_mem_pool_handle_t* hdlc_header; ///< HDLC object memory pool usage
    cpc_mem_pool_handle_t*
        hdlc_reject;                  ///< HDLC reject object  memory pool usage
    cpc_mem_pool_handle_t* rx_buffer; ///< RX buffer memory pool usage
    cpc_mem_pool_handle_t* endpoint;  ///< Endpoint object memory pool usage
    cpc_mem_pool_handle_t* rx_queue_item; ///< RX queue object memory pool usage
    cpc_mem_pool_handle_t* tx_queue_item; ///< TX queue object memory pool usage
    cpc_mem_pool_handle_t*
        endpoint_closed_arg_item; ///< Endpoint closing object memory pool usage
    cpc_mem_pool_handle_t*
        system_command; ///< System endpoint object memory pool usage
} cpc_debug_memory_t;

#define CPC_USER_ENDPOINT_ID_START ((uint8_t)CPC_ENDPOINT_USER_ID_0)
#define CPC_USER_ENDPOINT_ID_END                                               \
    ((uint8_t)(CPC_ENDPOINT_USER_ID_0 + CPC_USER_ENDPOINT_MAX_COUNT - 1))

#define CPC_FLAG_NO_BLOCK 0x01

#define CPC_APP_DATA_MAX_LENGTH (4087)

#define CPC_TRANSMIT_WINDOW_MIN_SIZE 1
#define CPC_TRANSMIT_WINDOW_MAX_SIZE 64

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
                                                                                   * Initialize CPC module.
                                                                                   *
                                                                                   * @return Status code.
                                                                                   ******************************************************************************/
status_t cpc_init(void);

/***************************************************************************/ /**
                                                                               *  The bare metal process action function.
                                                                               ******************************************************************************/
#if !defined(CATALOG_KERNEL_PRESENT)
void cpc_process_action(void);
#endif

status_t cpc_open_user_endpoint(cpc_endpoint_handle_t* endpoint_handle,
                                cpc_user_endpoint_id_t id, uint8_t flags,
                                uint8_t tx_window_size);

status_t cpc_set_endpoint_option(cpc_endpoint_handle_t* endpoint_handle,
                                 cpc_endpoint_option_t option, void* value);

status_t cpc_close_endpoint(cpc_endpoint_handle_t* endpoint_handle);

status_t cpc_read(cpc_endpoint_handle_t* endpoint_handle, void** data,
                  uint16_t* data_length, uint32_t timeout, uint8_t flags);

status_t cpc_write(cpc_endpoint_handle_t* endpoint_handle, void* data,
                   uint16_t data_length, uint8_t flag,
                   void* on_write_completed_arg);

cpc_endpoint_state_t
cpc_get_endpoint_state(cpc_endpoint_handle_t* endpoint_handle);

bool cpc_get_endpoint_encryption(cpc_endpoint_handle_t* endpoint_handle);

status_t cpc_free_rx_buffer(void* data);

uint32_t __cpc_flash_erase(flash_erase_mode_t mode, uint32_t flash_addr);

void __cpc_flash_write(uint32_t page_addr, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif // CPC_H
