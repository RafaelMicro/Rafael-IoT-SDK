/**
 * @file cpc_drv_secondary_uart_usart.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mcu.h"

#include <string.h>
#include "slist.h"

#include "cpc.h"
#include "cpc_api.h"
#include "cpc_config.h"
#include "cpc_crc.h"
#include "cpc_drv.h"
#include "cpc_hdlc.h"
#include "mem_pool.h"

#include "cpc_internal.h"

/*******************************************************************************
 ********************************   DATATYPE   *********************************
 ******************************************************************************/
typedef struct {
    slist_node_t node;
    cpc_buffer_handle_t* handle;
    uint16_t payload_len;
} buf_entry_t;

#define CPC_DRV_UART_FLOW_CONTROL_TYPE 0
#define usartHwFlowControlNone_D       0
#define usartHwFlowControlCtsAndRts_D  1

#ifndef CONFIG_CPC_UART_CAPABILITIES_HWFC
#define CONFIG_CPC_UART_CAPABILITIES_HWFC 0
#endif
/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

#if ((CPC_RX_DATA_MAX_LENGTH > 2048)                                           \
     && (CPC_DRV_UART_FLOW_CONTROL_TYPE == usartHwFlowControlNone_D))
#error Buffer larger than 2048 bytes without Hardware Flow Control is not supported
#endif

#if (CPC_DRV_UART_RX_QUEUE_SIZE > CPC_RX_BUFFER_MAX_COUNT)
#error Invalid configuration CPC_RX_BUFFER_MAX_COUNT must be greater than CPC_DRV_UART_RX_QUEUE_SIZE
#endif

#define CPC_CONCAT_PASTER(first, second, third) first##second##third
#define CPC_PERIPH_CLOCK(first, second)         first##second

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static slist_node_t* rx_free_list_head;
static slist_node_t* rx_free_no_buf_list_head;
static slist_node_t* rx_pending_list_head;

static slist_node_t* tx_free_list_head;
static slist_node_t* tx_submitted_list_head;

static buf_entry_t rx_buf_entries_tbl[10];
static buf_entry_t tx_buf_entries_tbl[10];

static buf_entry_t* current_rx_entry;

static bool tx_ready = true;
static volatile bool rx_need_rx_entry = false;

static volatile uint16_t already_recvd_cnt_worst = 0;

static volatile bool resync_completed = false;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

status_t cpc_drv_init(void) {
    uint32_t buf_cnt;

    slist_init(&rx_free_list_head);
    slist_init(&rx_free_no_buf_list_head);
    slist_init(&rx_pending_list_head);
    slist_init(&tx_free_list_head);
    slist_init(&tx_submitted_list_head);

    current_rx_entry = NULL;

    for (buf_cnt = 0; buf_cnt < 10; buf_cnt++) {
        buf_entry_t* entry = &rx_buf_entries_tbl[buf_cnt];
        if (cpc_get_buffer_handle_for_rx(&entry->handle) != CPC_STATUS_OK) {
            configASSERT(false);
            return CPC_STATUS_ALLOCATION_FAILED;
        }
        cpc_push_buffer_handle(&rx_free_list_head, &entry->node, entry->handle);
    }

    for (buf_cnt = 0; buf_cnt < 10; buf_cnt++) {
        buf_entry_t* entry = &tx_buf_entries_tbl[buf_cnt];
        slist_push(&tx_free_list_head, &entry->node);
    }
    return CPC_STATUS_OK;
}

bool cpc_drv_is_out_of_rx_buffers(void) {
    MCU_ENTER_CRITICAL();

    if (rx_free_list_head && !rx_free_list_head->node) {
        MCU_EXIT_CRITICAL();
        return true;
    }
    MCU_EXIT_CRITICAL();
    return false;
}

void cpc_drv_get_capabilities(cpc_drv_capabilities_t* capabilities) {
    if (capabilities == NULL)
        return;

    *capabilities = (cpc_drv_capabilities_t){0};
    capabilities->preprocess_hdlc_header = true;
    capabilities->use_raw_rx_buffer = true;
    capabilities->uart_flowcontrol = CONFIG_CPC_UART_CAPABILITIES_HWFC;
}

/***************************************************************************/ /**
                                                                               * Read bytes from UART.
                                                                               ******************************************************************************/
status_t cpc_drv_read_data(cpc_buffer_handle_t** buffer_handle,
                           uint16_t* payload_rx_len) {
    MCU_ENTER_CRITICAL();
    buf_entry_t* entry = (buf_entry_t*)CPC_POP_BUFFER_HANDLE_LIST(
        &rx_pending_list_head, buf_entry_t);

    if (entry == NULL) {
        MCU_EXIT_CRITICAL();
        return CPC_STATUS_EMPTY;
    }
    MCU_EXIT_CRITICAL();

    *buffer_handle = entry->handle;
    *payload_rx_len = CPC_RX_DATA_MAX_LENGTH;

    MCU_ENTER_CRITICAL();
    if (cpc_get_buffer_handle_for_rx(&entry->handle) == CPC_STATUS_OK)
        cpc_push_buffer_handle(&rx_free_list_head, &entry->node, entry->handle);
    else
        slist_push(&rx_free_no_buf_list_head, &entry->node);
    MCU_EXIT_CRITICAL();
    return CPC_STATUS_OK;
}

void cpc_drv_trnsmit_complete(void) {
    enter_critical_section();
    buf_entry_t* entry = (buf_entry_t*)CPC_POP_BUFFER_HANDLE_LIST(
        &tx_submitted_list_head, buf_entry_t);
    // log_info("enrtyptr = %p", entry);
    tx_ready = true;
    leave_critical_section();
    cpc_drv_notify_tx_complete(entry->handle);
    slist_push(&tx_free_list_head, &entry->node);
}

/***************************************************************************/ /**
                                                                               * Write bytes to UART.
                                                                               ******************************************************************************/
extern int cpc_uart_data_send(cpc_buffer_handle_t* buffer_handle,
                              uint16_t payload_tx_len);

status_t cpc_drv_transmit_data(cpc_buffer_handle_t* buffer_handle,
                               uint16_t payload_tx_len) {
    buf_entry_t* entry;
    uint16_t tx_len = 0;
    uint8_t* p_tx_data = NULL;

    entry = (buf_entry_t*)slist_pop(&tx_free_list_head);
    if (entry == NULL)
        return CPC_STATUS_NOT_READY;

    entry->handle = buffer_handle;
    entry->payload_len = payload_tx_len;

    cpc_push_back_buffer_handle(&tx_submitted_list_head, &entry->node,
                                entry->handle);
    cpc_uart_data_send(buffer_handle, payload_tx_len);
    return CPC_STATUS_OK;

    // if (payload_tx_len > 0) {
    //     tx_len = CPC_HDLC_HEADER_RAW_SIZE + payload_tx_len + 2;
    //     p_tx_data = pvPortMalloc(tx_len);
    //     if(p_tx_data) {
    //         memcpy(p_tx_data, entry->handle->hdlc_header, CPC_HDLC_HEADER_RAW_SIZE);
    //         memcpy(&p_tx_data[7], entry->handle->data, payload_tx_len);
    //         memcpy(&p_tx_data[7 + payload_tx_len], entry->handle->fcs, 2);
    //     }
    // }
    // else {
    //     tx_len = CPC_HDLC_HEADER_RAW_SIZE;
    //     p_tx_data = pvPortMalloc(tx_len);
    //     if(p_tx_data) memcpy(p_tx_data, entry->handle->hdlc_header, CPC_HDLC_HEADER_RAW_SIZE);
    // }

    // if(p_tx_data) {
    //     tx_ready = false;
    //     cpc_uart_data_send(p_tx_data, tx_len);
    // }

    // return CPC_STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Checks if driver is ready to transmit.
                                                                               ******************************************************************************/
bool cpc_drv_is_transmit_ready(void) {
    slist_node_t* head;
    bool flag;
    MCU_ATOMIC_LOAD(flag, tx_ready);
    MCU_ATOMIC_LOAD(head, tx_free_list_head);
    return (head != NULL && flag);
}

/***************************************************************************/ /**
                                                                               * Get currently configured bus speed
                                                                               ******************************************************************************/

/***************************************************************************/ /**
                                                                               * Notification when RX buffer becomes free.
                                                                               ******************************************************************************/
void cpc_memory_on_rx_buffer_free(void) {
    status_t status;
    if (rx_free_no_buf_list_head == NULL)
        return;

    MCU_ENTER_CRITICAL();
    do {
        if (rx_free_no_buf_list_head == NULL)
            break;

        buf_entry_t* entry = (buf_entry_t*)rx_free_no_buf_list_head;

        status = cpc_get_buffer_handle_for_rx(&entry->handle);
        if (status == CPC_STATUS_OK) {
            (void)slist_pop(&rx_free_no_buf_list_head);
            cpc_push_buffer_handle(&rx_free_list_head, &entry->node,
                                   entry->handle);
        }
    } while (status == CPC_STATUS_OK && rx_free_no_buf_list_head != NULL);

    MCU_EXIT_CRITICAL();
}

void cpc_drv_uart_push_data(uint8_t* p_data, uint16_t length) {
    void* buffer_ptr = NULL;
    if (cpc_get_raw_rx_buffer(&buffer_ptr) != CPC_STATUS_OK)
        return;

    memcpy((uint8_t*)buffer_ptr, p_data, length);

    current_rx_entry->handle->data = buffer_ptr;
    current_rx_entry->handle->data_length = length;
    MCU_ENTER_CRITICAL();
    cpc_push_back_buffer_handle(&rx_pending_list_head, &current_rx_entry->node,
                                current_rx_entry->handle);
    MCU_EXIT_CRITICAL();
    cpc_drv_notify_rx_data();

    //log_info_hexdump("CPC RxH", (uint8_t *)current_rx_entry->handle->hdlc_header, CPC_HDLC_HEADER_RAW_SIZE);
    //log_info_hexdump("CPC RxD", (uint8_t *)current_rx_entry->handle->data, length);
}

uint32_t cpc_drv_uart_push_header(uint8_t* pHeader) {
    uint32_t ret_len = 0;

    current_rx_entry = (buf_entry_t*)CPC_POP_BUFFER_HANDLE_LIST(
        &rx_free_list_head, buf_entry_t);

    // log_info("current_rx_entry = %p", current_rx_entry);
    if (current_rx_entry == NULL)
        return ret_len;

    ret_len = cpc_hdlc_get_length(pHeader);

    // Copy useful fields of header. Unfortunately with this method the header must always be copied
    memcpy(current_rx_entry->handle->hdlc_header, pHeader,
           CPC_HDLC_HEADER_RAW_SIZE);
    current_rx_entry->handle->data_length = CPC_HDLC_HEADER_RAW_SIZE + ret_len;

    if (ret_len == 0) {
        cpc_push_back_buffer_handle(&rx_pending_list_head,
                                    &current_rx_entry->node,
                                    current_rx_entry->handle);
        cpc_drv_notify_rx_data();
    }
    return ret_len;
}