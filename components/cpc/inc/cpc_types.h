/**
 * @file cpc_types.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CPC_TYPE_H
#define CPC_TYPE_H

#include "cpc.h"
#include "cpc_config.h"
#include "cpc_timer.h"
#include "slist.h"

#if defined(COMPONENT_CATALOG_PRESENT)
#include "component_catalog.h"
#endif

#if defined(CATALOG_CPC_SECONDARY_PRESENT)
#define CPC_ON_POLL_PRESENT
#endif

#define SECURITY_ENABLED (0)

#if (!defined(CATALOG_CPC_PRIMARY_PRESENT)                                     \
     && !defined(CATALOG_CPC_SECONDARY_PRESENT))
// This is required for unit testing (assumed that we are unit testing on a secondary)
#define CPC_ON_POLL_PRESENT
#endif

typedef void (*cpc_on_poll_t)(
    uint8_t endpoint_id, void* arg, void* poll_data,
    uint32_t
        poll_data_length, // Rx buffer is freed once this on_poll function return
    void** reply_data, uint32_t* reply_data_lenght,
    void** on_write_complete_arg);

typedef void (*cpc_on_final_t)(uint8_t endpoint_id, void* arg, void* answer,
                               uint32_t answer_lenght);

typedef struct {
    void* on_fnct_arg;
#ifdef CPC_ON_FINAL_PRESENT
    cpc_on_final_t on_final;
#endif
#ifdef CPC_ON_POLL_PRESENT
    cpc_on_poll_t on_poll;
    void* data; // Anwser
    uint32_t data_length;
#endif
} cpc_poll_final_t;

typedef struct {
    slist_node_t node;
    slist_node_t node_closing;
    uint8_t id;
    uint8_t flags;
    uint8_t seq;
    uint8_t ack;
    uint8_t configured_tx_window_size;
    uint8_t current_tx_window_space;
    uint8_t frames_count_re_transmit_queue;
    uint8_t packet_re_transmit_count;
    uint32_t re_transmit_timeout;
    uint64_t last_iframe_sent_timestamp;
    uint64_t smoothed_rtt;
    uint64_t rtt_variation;
    cpc_timer_handle_t* re_transmit_timer;
    cpc_timer_handle_t* close_timer;
    cpc_endpoint_state_t state;
    cpc_poll_final_t poll_final;
    cpc_on_write_completed_t on_iframe_write_completed;
    cpc_on_data_reception_t on_iframe_data_reception;
    void* on_iframe_data_reception_arg;
    cpc_on_write_completed_t on_uframe_write_completed;
    cpc_on_data_reception_t on_uframe_data_reception;
    void* on_uframe_data_reception_arg;
    cpc_on_error_callback_t on_error;
    void* on_error_arg;
    slist_node_t* iframe_receive_queue;
    slist_node_t* uframe_receive_queue;
    slist_node_t* re_transmit_queue;
    slist_node_t* holding_list;
} cpc_endpoint_t;

#endif
