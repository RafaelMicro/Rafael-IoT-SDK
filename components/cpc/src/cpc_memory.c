/**
 * @file cpc_memory.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cpc_api.h"
#include "cpc_config.h"
#include "cpc_drv.h"
#include "cpc_hdlc.h"
#include "cpc_system_common.h"
#include "mem_pool.h"

#if (CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1)
#define MEM_POOL_ALLOC(mempool, block)                                         \
    {                                                                          \
        if (block != NULL) {                                                   \
            mempool->used_block_cnt++;                                         \
        }                                                                      \
    }
#define MEM_POOL_FREE(mempool) mempool->used_block_cnt--;
#else
#define MEM_POOL_ALLOC(mempool, block)
#define MEM_POOL_FREE(mempool)
#endif

static mem_pool_handle_t mempool_buffer_handle;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_buffer_handle, cpc_buffer_handle_t,
                                  CPC_BUFFER_HANDLE_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_buffer_handle = {
    .pool_handle = &mempool_buffer_handle, .used_block_cnt = 0};

static mem_pool_handle_t mempool_hdlc_header;
MEM_POOL_DECLARE_BUFFER(mempool_hdlc_header, CPC_HDLC_HEADER_RAW_SIZE,
                        CPC_HDLC_HEADER_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_hdlc_header = {
    .pool_handle = &mempool_hdlc_header, .used_block_cnt = 0};

static mem_pool_handle_t mempool_hdlc_reject;
MEM_POOL_DECLARE_BUFFER(mempool_hdlc_reject, CPC_HDLC_REJECT_PAYLOAD_SIZE,
                        CPC_HDLC_REJECT_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_hdlc_reject = {
    .pool_handle = &mempool_hdlc_reject, .used_block_cnt = 0};

static mem_pool_handle_t mempool_rx_buffer;
MEM_POOL_DECLARE_BUFFER(mempool_rx_buffer, CPC_RX_DATA_MAX_LENGTH,
                        CPC_RX_BUFFER_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_rx_buffer = {
    .pool_handle = &mempool_rx_buffer, .used_block_cnt = 0};

static mem_pool_handle_t mempool_endpoint;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_endpoint, cpc_endpoint_t,
                                  CPC_ENDPOINT_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_endpoint = {
    .pool_handle = &mempool_endpoint, .used_block_cnt = 0};

static mem_pool_handle_t mempool_rx_queue_item;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_rx_queue_item,
                                  cpc_receive_queue_item_t,
                                  CPC_RX_QUEUE_ITEM_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_rx_queue_item = {
    .pool_handle = &mempool_rx_queue_item, .used_block_cnt = 0};

static mem_pool_handle_t mempool_tx_queue_item;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_tx_queue_item,
                                  cpc_transmit_queue_item_t,
                                  CPC_TX_QUEUE_ITEM_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_tx_queue_item = {
    .pool_handle = &mempool_tx_queue_item, .used_block_cnt = 0};

static mem_pool_handle_t mempool_tx_queue_item_sframe;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_tx_queue_item_sframe,
                                  cpc_transmit_queue_item_t,
                                  CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_tx_queue_item_sframe = {
    .pool_handle = &mempool_tx_queue_item_sframe, .used_block_cnt = 0};

static mem_pool_handle_t mempool_endpoint_closed_arg_item;
MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_endpoint_closed_arg_item,
                                  cpc_endpoint_closed_arg_t,
                                  CPC_TX_QUEUE_ITEM_MAX_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_endpoint_closed_arg_item = {
    .pool_handle = &mempool_endpoint_closed_arg_item, .used_block_cnt = 0};

static mem_pool_handle_t mempool_system_command;
MEM_POOL_DECLARE_BUFFER(mempool_system_command, CPC_SYSTEM_COMMAND_BUFFER_SIZE,
                        CPC_SYSTEM_COMMAND_BUFFER_COUNT);
static cpc_mem_pool_handle_t cpc_mempool_system_command = {
    .pool_handle = &mempool_system_command, .used_block_cnt = 0};

extern cpc_drv_capabilities_t cpc_driver_capabilities;

static void* alloc_object(cpc_mem_pool_handle_t* pool);

static void free_object(cpc_mem_pool_handle_t* pool, void* block);

__weak void cpc_memory_on_rx_buffer_free(void);

void cpc_init_buffers(void) {
    mem_pool_create(cpc_mempool_buffer_handle.pool_handle,
                    sizeof(cpc_buffer_handle_t), CPC_BUFFER_HANDLE_MAX_COUNT,
                    mempool_buffer_handle_buffer,
                    sizeof(mempool_buffer_handle_buffer));

    mem_pool_create(cpc_mempool_hdlc_header.pool_handle,
                    CPC_HDLC_HEADER_RAW_SIZE, CPC_HDLC_HEADER_MAX_COUNT,
                    mempool_hdlc_header_buffer,
                    sizeof(mempool_hdlc_header_buffer));

    mem_pool_create(cpc_mempool_hdlc_reject.pool_handle,
                    CPC_HDLC_REJECT_PAYLOAD_SIZE, CPC_HDLC_REJECT_MAX_COUNT,
                    mempool_hdlc_reject_buffer,
                    sizeof(mempool_hdlc_reject_buffer));

    mem_pool_create(cpc_mempool_rx_buffer.pool_handle, CPC_RX_DATA_MAX_LENGTH,
                    CPC_RX_BUFFER_MAX_COUNT, mempool_rx_buffer_buffer,
                    sizeof(mempool_rx_buffer_buffer));

    mem_pool_create(cpc_mempool_endpoint.pool_handle, sizeof(cpc_endpoint_t),
                    CPC_ENDPOINT_COUNT, mempool_endpoint_buffer,
                    sizeof(mempool_endpoint_buffer));

    mem_pool_create(cpc_mempool_rx_queue_item.pool_handle,
                    sizeof(cpc_receive_queue_item_t),
                    CPC_RX_QUEUE_ITEM_MAX_COUNT, mempool_rx_queue_item_buffer,
                    sizeof(mempool_rx_queue_item_buffer));

    mem_pool_create(cpc_mempool_tx_queue_item.pool_handle,
                    sizeof(cpc_transmit_queue_item_t),
                    CPC_TX_QUEUE_ITEM_MAX_COUNT, mempool_tx_queue_item_buffer,
                    sizeof(mempool_tx_queue_item_buffer));

    mem_pool_create(cpc_mempool_tx_queue_item_sframe.pool_handle,
                    sizeof(cpc_transmit_queue_item_t),
                    CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT,
                    mempool_tx_queue_item_sframe_buffer,
                    sizeof(mempool_tx_queue_item_sframe_buffer));

    mem_pool_create(cpc_mempool_endpoint_closed_arg_item.pool_handle,
                    sizeof(cpc_endpoint_closed_arg_t),
                    CPC_TX_QUEUE_ITEM_MAX_COUNT,
                    mempool_endpoint_closed_arg_item_buffer,
                    sizeof(mempool_endpoint_closed_arg_item_buffer));

    mem_pool_create(cpc_mempool_endpoint_closed_arg_item.pool_handle,
                    sizeof(cpc_endpoint_closed_arg_t),
                    CPC_TX_QUEUE_ITEM_MAX_COUNT,
                    mempool_endpoint_closed_arg_item_buffer,
                    sizeof(mempool_endpoint_closed_arg_item_buffer));

    mem_pool_create(
        cpc_mempool_system_command.pool_handle, CPC_SYSTEM_COMMAND_BUFFER_SIZE,
        CPC_SYSTEM_COMMAND_BUFFER_COUNT, mempool_system_command_buffer,
        sizeof(mempool_system_command_buffer));
}

/***************************************************************************/ /**
                                                                               * Get a CPC handle buffer
                                                                               ******************************************************************************/
status_t cpc_get_buffer_handle(cpc_buffer_handle_t** handle) {
    cpc_buffer_handle_t* buf_handle;
    CPC_CHECK_NULL_POINTER(handle);
    CPC_CHECK_OBJ_LOAD(buf_handle, &cpc_mempool_buffer_handle);

    buf_handle->hdlc_header = NULL;
    buf_handle->data = NULL;
    buf_handle->data_length = 0;
    buf_handle->endpoint = NULL;
    buf_handle->fcs[0] = 0;
    buf_handle->fcs[1] = 0;
    buf_handle->control = 0;
    buf_handle->address = 0;
    buf_handle->buffer_type = CPC_UNKNOWN_BUFFER;
    buf_handle->arg = NULL;
    buf_handle->reason = CPC_REJECT_NO_ERROR;
    buf_handle->ref_count = 0;

    *handle = buf_handle;
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get a CPC header buffer
                                                                               ******************************************************************************/
status_t cpc_get_hdlc_header_buffer(void** header) {
    CPC_CHECK_NULL_POINTER(header);
    CPC_CHECK_OBJ_LOAD(*header, &cpc_mempool_hdlc_header);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get a CPC header buffer for transmitting a reject
                                                                               ******************************************************************************/
status_t cpc_get_reject_buffer(cpc_buffer_handle_t** handle) {
    cpc_buffer_handle_t* buf_handle;

    CPC_CHECK_STATUS_OK(cpc_get_buffer_handle(&buf_handle));

    buf_handle->data = alloc_object(&cpc_mempool_hdlc_reject);
    buf_handle->buffer_type = CPC_HDLC_REJECT_BUFFER;

    if (buf_handle->data == NULL) {
        free_object(&cpc_mempool_buffer_handle, buf_handle);
        return CPC_STATUS_NO_MORE_RESOURCE;
    }

    buf_handle->data_length = 1;
    *handle = buf_handle;

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get a CPC buffer for reception.
                                                                               ******************************************************************************/
status_t cpc_get_buffer_handle_for_rx(cpc_buffer_handle_t** handle) {
    CPC_CHECK_NULL_POINTER(handle);
    cpc_buffer_handle_t* buf_handle;
    status_t status;

    CPC_CHECK_STATUS_OK(cpc_get_buffer_handle(&buf_handle));

    cpc_get_hdlc_header_buffer(&buf_handle->hdlc_header);
    if (buf_handle->hdlc_header == NULL) {
        free_object(&cpc_mempool_buffer_handle, buf_handle);
        return CPC_STATUS_NO_MORE_RESOURCE;
    }

    if (!cpc_driver_capabilities.use_raw_rx_buffer) {
        buf_handle->data = alloc_object(&cpc_mempool_rx_buffer);
        if (buf_handle->data == NULL) {
            cpc_free_hdlc_header(buf_handle->hdlc_header);
            free_object(&cpc_mempool_buffer_handle, buf_handle);
            return CPC_STATUS_NO_MORE_RESOURCE;
        }
    }

    buf_handle->data_length = 0u;
    buf_handle->buffer_type = CPC_RX_BUFFER;
    buf_handle->reason = CPC_REJECT_NO_ERROR;

    *handle = buf_handle;
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get a CPC RAW buffer for reception.
                                                                               ******************************************************************************/
status_t cpc_get_raw_rx_buffer(void** raw_rx_buffer) {
    configASSERT(cpc_driver_capabilities.use_raw_rx_buffer);
    CPC_CHECK_OBJ_LOAD(*raw_rx_buffer, &cpc_mempool_rx_buffer);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get a CPC RAW buffer for reception.
                                                                               ******************************************************************************/
status_t cpc_free_raw_rx_buffer(void* raw_rx_buffer) {
    configASSERT(cpc_driver_capabilities.use_raw_rx_buffer);
    CPC_CHECK_NULL_POINTER(raw_rx_buffer);
    free_object(&cpc_mempool_rx_buffer, raw_rx_buffer);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Free rx handle and all associate buffers
                                                                               ******************************************************************************/
status_t cpc_drop_buffer_handle(cpc_buffer_handle_t* handle) {
    CPC_CHECK_NULL_POINTER(handle);
    bool is_rx_buffer = (handle->buffer_type == CPC_RX_BUFFER);

    if (handle->ref_count > 0)
        return CPC_STATUS_BUSY; // Can't free the buffer_handle, it is being used elsewhere

    if (handle->data != NULL) {
        switch (handle->buffer_type) {
            case CPC_HDLC_REJECT_BUFFER: {
                free_object(&cpc_mempool_hdlc_reject, handle->data);
                break;
            }
            case CPC_RX_BUFFER: {
                free_object(&cpc_mempool_rx_buffer, handle->data);
                break;
            }
            default: break;
        }
    }

    if (handle->hdlc_header != NULL)
        cpc_free_hdlc_header(handle->hdlc_header);
    free_object(&cpc_mempool_buffer_handle, handle);

    if (is_rx_buffer)
        cpc_memory_on_rx_buffer_free(); // Notify that at least one RX buffer is available

    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Convert rx buffer handle to a receive queue item;
                                                                               *  Free hdlc header and handle
                                                                               *  Alloc queue item and set fields
                                                                               ******************************************************************************/
status_t cpc_push_back_rx_data_in_receive_queue(cpc_buffer_handle_t* handle,
                                                slist_node_t** head,
                                                uint16_t data_length) {
    CPC_CHECK_NULL_POINTER(handle);
    CPC_CHECK_NULL_POINTER(head);
    cpc_receive_queue_item_t* item;

    CPC_CHECK_STATUS_OK(cpc_get_receive_queue_item(&item));

    item->node.node = NULL;
    item->data = handle->data;
    item->buffer_type = handle->buffer_type;
    item->data_length = data_length;
    slist_push_back(head, &item->node);
    cpc_free_hdlc_header(handle->hdlc_header);
    free_object(&cpc_mempool_buffer_handle, handle);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Free rx buffer returned by cpc_read().
                                                                               ******************************************************************************/
status_t cpc_free_rx_buffer(void* data) {
    CPC_CHECK_NULL_POINTER(data);
    free_object(&cpc_mempool_rx_buffer, data);
    // Notify that at least one RX buffer is available
    cpc_memory_on_rx_buffer_free();
    return CPC_STATUS_OK;
}

status_t cpc_free_hdlc_header(void* data) {
    CPC_CHECK_NULL_POINTER(data);
    free_object(&cpc_mempool_hdlc_header, data);
    return CPC_STATUS_OK;
}

status_t cpc_get_receive_queue_item(cpc_receive_queue_item_t** item) {
    CPC_CHECK_NULL_POINTER(item);
    CPC_CHECK_OBJ_LOAD(*item, &cpc_mempool_rx_queue_item);
    return CPC_STATUS_OK;
}

status_t cpc_free_receive_queue_item(cpc_receive_queue_item_t* item,
                                     void** data, uint16_t* data_length) {
    CPC_CHECK_NULL_POINTER(item);
    *data = item->data;
    *data_length = item->data_length;
    free_object(&cpc_mempool_rx_queue_item, item);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Free receive queue item and the data buffer
                                                                               ******************************************************************************/
void cpc_drop_receive_queue_item(cpc_receive_queue_item_t* item) {
    if (item == NULL)
        return;

    if (item->data != NULL) {
        if (item->buffer_type == CPC_HDLC_REJECT_BUFFER)
            free_object(&cpc_mempool_hdlc_reject, item->data);
        else if (item->buffer_type == CPC_RX_BUFFER) {
            free_object(&cpc_mempool_rx_buffer, item->data);
            // Notify that at least one RX buffer is available
            cpc_memory_on_rx_buffer_free();
        }
    }

    free_object(&cpc_mempool_rx_queue_item, item);
}

/***************************************************************************/ /**
                                                                               * Get a transmit queue item.
                                                                               ******************************************************************************/
status_t cpc_get_transmit_queue_item(cpc_transmit_queue_item_t** item) {
    CPC_CHECK_NULL_POINTER(item);
    CPC_CHECK_OBJ_LOAD(*item, &cpc_mempool_tx_queue_item);
    return CPC_STATUS_OK;
}

status_t cpc_free_transmit_queue_item(cpc_transmit_queue_item_t* item) {
    CPC_CHECK_NULL_POINTER(item);
    CORE_ATOMIC_SECTION(free_object(&cpc_mempool_tx_queue_item, item););
    return CPC_STATUS_OK;
}

status_t cpc_get_sframe_transmit_queue_item(cpc_transmit_queue_item_t** item) {
    CPC_CHECK_NULL_POINTER(item);
    CPC_CHECK_OBJ_LOAD(*item, &cpc_mempool_tx_queue_item_sframe);
    return CPC_STATUS_OK;
}

status_t cpc_free_sframe_transmit_queue_item(cpc_transmit_queue_item_t* item) {
    CPC_CHECK_NULL_POINTER(item);
    CORE_ATOMIC_SECTION(free_object(&cpc_mempool_tx_queue_item_sframe, item););
    return CPC_STATUS_OK;
}

status_t cpc_get_endpoint(cpc_endpoint_t** endpoint) {
    CPC_CHECK_NULL_POINTER(endpoint);
    CPC_CHECK_OBJ_LOAD(*endpoint, &cpc_mempool_endpoint);
    memset(*endpoint, 0, sizeof(cpc_endpoint_t));
    return CPC_STATUS_OK;
}

void cpc_free_endpoint(cpc_endpoint_t* endpoint) {
    free_object(&cpc_mempool_endpoint, endpoint);
}

/***************************************************************************/ /**
                                                                               * Get endpoint closed argument item
                                                                               ******************************************************************************/
status_t cpc_get_closed_arg(cpc_endpoint_closed_arg_t** arg) {
    CPC_CHECK_NULL_POINTER(arg);
    CPC_CHECK_OBJ_LOAD(*arg, &cpc_mempool_endpoint_closed_arg_item);
    return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Free endpoint closed argument item
                                                                               ******************************************************************************/
void cpc_free_closed_arg(cpc_endpoint_closed_arg_t* arg) {
    free_object(&cpc_mempool_endpoint_closed_arg_item, arg);
}

void cpc_push_back_buffer_handle(slist_node_t** head, slist_node_t* item,
                                 cpc_buffer_handle_t* buf_handle) {
    configASSERT(buf_handle != NULL);
    MCU_ATOMIC_SECTION(buf_handle->ref_count++;);
    slist_push_back(head, item);
}

void cpc_push_buffer_handle(slist_node_t** head, slist_node_t* item,
                            cpc_buffer_handle_t* buf_handle) {
    configASSERT(buf_handle != NULL);
    MCU_ATOMIC_SECTION(buf_handle->ref_count++;);
    slist_push(head, item);
}

status_t cpc_get_system_command_buffer(cpc_system_cmd_t** item) {
    CPC_CHECK_NULL_POINTER(item);
    CPC_CHECK_OBJ_LOAD(*item, &cpc_mempool_system_command);
    return CPC_STATUS_OK;
}

status_t cpc_free_command_buffer(cpc_system_cmd_t* item) {
    CPC_CHECK_NULL_POINTER(item);
    CORE_ATOMIC_SECTION(free_object(&cpc_mempool_system_command, item););
    return CPC_STATUS_OK;
}

status_t cpc_get_security_tag_buffer(void** tag_buffer) {
    (void)tag_buffer;
    return CPC_STATUS_NOT_AVAILABLE;
}

status_t cpc_free_security_tag_buffer(void* tag_buffer) {
    (void)tag_buffer;
    return CPC_STATUS_NOT_AVAILABLE;
}

/* Alloc object from a specified pool; Manage stat as well */
static void* alloc_object(cpc_mem_pool_handle_t* pool) {
    void* block = mem_pool_alloc(pool->pool_handle);
    MEM_POOL_ALLOC(pool, block);
    return block;
}

/* Free object from a specified pool; Manage stat as well */
static void free_object(cpc_mem_pool_handle_t* pool, void* block) {
    mem_pool_free(pool->pool_handle, block);
    MEM_POOL_FREE(pool);
}
