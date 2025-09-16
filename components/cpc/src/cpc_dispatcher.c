/**
 * @file cpc_dispatcher.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cpc_api.h"

static slist_node_t* process_queue;
static uint8_t event_counter = 0;

void cpc_dispatcher_init_handle(cpc_dispatcher_handle_t* handle) {
    configASSERT(handle != NULL);

    handle->submitted = false;
    handle->fnct = NULL;
    handle->data = NULL;
}

status_t cpc_dispatcher_push(cpc_dispatcher_handle_t* handle,
                             cpc_dispatcher_fnct_t fnct, void* data) {

    if (handle->submitted)
        return CPC_STATUS_BUSY; // Already dispatched

    handle->fnct = fnct;
    handle->data = data;
    slist_push_back(&process_queue, &handle->node);

    configASSERT(event_counter < 255);
    ++event_counter;

    handle->submitted = true;

    cpc_signal_event(CPC_SIGNAL_SYSTEM);
    return CPC_STATUS_OK;
}

void cpc_dispatcher_cancel(cpc_dispatcher_handle_t* handle) {
    if (!handle->submitted)
        return;
    MCU_ATOMIC_SECTION({
        slist_remove(&process_queue, &handle->node);
        configASSERT(event_counter > 0);
        --event_counter;
        handle->submitted = false;
    });
}

void cpc_dispatcher_process(void) {
    slist_node_t* node;
    cpc_dispatcher_handle_t* handle;
    uint8_t complete_event_cnt;
    uint8_t running_event_cnt;

    MCU_ATOMIC_LOAD(complete_event_cnt, event_counter);
    running_event_cnt = complete_event_cnt;

    do {
        MCU_ATOMIC_SECTION(node = slist_pop(&process_queue););
        if (node == NULL)
            continue;
        handle = SLIST_ENTRY(node, cpc_dispatcher_handle_t, node);
        handle->submitted = false;
        handle->fnct(handle->data);
    } while (node != NULL && --running_event_cnt);

    // Update the global event_counter
    MCU_ATOMIC_SECTION(event_counter = (event_counter >= complete_event_cnt)
                                           ? event_counter - complete_event_cnt
                                           : 0;);
}
