/**
 * @file cpc_hci.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <string.h>
#include <task.h>
#include <timers.h>

#include <log.h>

#include <cpc.h>
#include <cpc_api.h>
#include <cpc_user_interface.h>

#include <hci_bridge.h>
#include <hosal_rf.h>

#include "log.h"

#define CPC_HCI_NOTIFY_HANDEL hci_mgr.event
#define CPC_HCI_NOTIFY_SIGNAL __cpc_hci_signal()
#define CPC_HCI_NOTIFY_ISR(ebit)                                               \
    (CPC_HCI_NOTIFY_HANDEL |= ebit);                                           \
    CPC_HCI_NOTIFY_SIGNAL;
#define CPC_HCI_NOTIFY(ebit)                                                   \
    MCU_ENTER_CRITICAL();                                                      \
    CPC_HCI_NOTIFY_HANDEL |= ebit;                                             \
    MCU_EXIT_CRITICAL();                                                       \
    CPC_HCI_NOTIFY_SIGNAL;
#define CPC_HCI_GET_NOTIFY(ebit)                                               \
    MCU_ATOMIC_SECTION({                                                       \
        ebit = CPC_HCI_NOTIFY_HANDEL;                                          \
        CPC_HCI_NOTIFY_HANDEL = CPC_HCI_EVENT_NONE;                            \
    });

typedef enum {
    CPC_HCI_EVENT_NONE = 0,

    CPC_HCI_EVENT_CPC_READ = 0x00000002,
    CPC_HCI_EVENT_HCI_IN = 0x00000004,
    CPC_HCI_EVENT_CPC_WRITE_DONE = 0x00000008,
    CPC_HCI_EVENT_CPC_ERROR = 0x00000010,

    CPC_HCI_EVENT_ALL = 0xffffffff,
} cpc_hci_event_t;

typedef struct {
    uint8_t addr_type;      /**<Address type.*/
    uint8_t Reserved;       /**<Reserved byte.*/
    uint8_t ble_addr[6];    /**<BLE unique mac address code.*/
    uint8_t zigbee_addr[8]; /**<ZigBee unique mac address code.*/
    uint8_t ble_irk[16];    /**<BLE Identity Resolving Key code.*/
} ble_unique_code_format_t;

typedef struct {
    cpc_hci_event_t event;
    TaskHandle_t task;
    StreamBufferHandle_t xStreamBuffer;
    cpc_endpoint_handle_t ep;

    uint32_t host_connect      : 1;
    uint32_t hci_tx_done       : 1;
    uint32_t hci_event_trigger : 1;
    uint32_t reserved          : 21;
    uint32_t cpc_event_cnt     : 8;

    uint16_t le_scan_interval;
    uint16_t le_scan_window;
} g_cpc_hci_t;

static g_cpc_hci_t hci_mgr = {
    .host_connect = 0,
    .le_scan_interval = 0x20,
    .le_scan_window = 0x10,
    .hci_tx_done = 1,
    .hci_event_trigger = 0,
    .cpc_event_cnt = 0,
};

static void __cpc_hci_signal(void) {
    if (!xPortIsInsideInterrupt()) {
        xTaskNotifyGive(hci_mgr.task);
        return;
    }
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
    vTaskNotifyGiveFromISR(hci_mgr.task, &pxHigherPriorityTaskWoken);
}

void __cpc_hci_write_done_evt(cpc_user_endpoint_id_t endpoint_id, void* buffer,
                              void* arg, status_t status) {
    (void)endpoint_id;
    (void)buffer;
    if (arg)
        vPortFree(arg);
    MCU_ATOMIC_SECTION(hci_mgr.hci_tx_done++;);
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_WRITE_DONE);
}

void __cpc_hci_read_evt(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    if (hci_mgr.host_connect == 0)
        MCU_ATOMIC_LOAD(hci_mgr.host_connect, 1);

    CORE_ATOMIC_SECTION(hci_mgr.cpc_event_cnt++;);
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_READ);
}

void __cpc_hci_error_evt(uint8_t endpoint_id, void* arg) {
    (void)endpoint_id;
    (void)arg;
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_CPC_ERROR);
}

static void __cpc_hci_ep_init(void) {
    if (!cpc_open_service_endpoint(&hci_mgr.ep, CPC_ENDPOINT_BLUETOOTH_RCP, 0,
                                   10))
        log_info("Opened bluetooth controller EP");
    cpc_set_endpoint_option(&hci_mgr.ep, CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                            (void*)__cpc_hci_write_done_evt);
    cpc_set_endpoint_option(&hci_mgr.ep, CPC_ENDPOINT_ON_IFRAME_RECEIVE,
                            (void*)__cpc_hci_read_evt);
    cpc_set_endpoint_option(&hci_mgr.ep, CPC_ENDPOINT_ON_ERROR,
                            (void*)__cpc_hci_error_evt);
}

static inline void cpc_hci_buffer_send(uint8_t* p_data, uint32_t data_len) {
    size_t xBytesSent, xBytesAvailable;
    xBytesSent = 0;

    do {
        xBytesAvailable = xStreamBufferSpacesAvailable(hci_mgr.xStreamBuffer);

        while (xBytesAvailable < data_len) {
            /* Wait */
            vTaskDelay(10);
        }

        xBytesSent = xStreamBufferSend(hci_mgr.xStreamBuffer,
                                       p_data + xBytesSent, data_len, 10);
        data_len -= xBytesSent;
    } while (data_len > 0);
}

static inline void handle_pta_cmd(uint8_t* p_cpc_buffer) {
    hosal_rf_pta_ctrl_t pta_ctrl;
    pta_ctrl.enable = p_cpc_buffer[4];
    pta_ctrl.inverse = p_cpc_buffer[5];

    hosal_rf_ioctl(HOSAL_RF_IOCTL_PTA_CTRL_SET, (void*)&pta_ctrl);

    uint8_t pdata[] = {0x04, 0x0E, 0x04, 0x01, 0x02, 0xFC, 0x00};

    cpc_hci_buffer_send(pdata, sizeof(pdata));

    CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
}

static inline void handle_scan_interval_window_cmd(uint8_t* p_cpc_buffer) {

    hci_mgr.le_scan_interval = p_cpc_buffer[4] | (p_cpc_buffer[5] << 8);
    hci_mgr.le_scan_window = p_cpc_buffer[6] | (p_cpc_buffer[7] << 8);

    if (hci_mgr.le_scan_window > hci_mgr.le_scan_interval) {
        hci_mgr.le_scan_window = 0x10;
        hci_mgr.le_scan_interval = 0x20;
    }

    uint8_t pdata[] = {0x04, 0x0E, 0x04, 0x01, 0x03, 0xFC, 0x00};
    cpc_hci_buffer_send(pdata, sizeof(pdata));

    CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
}

static inline void handle_set_mac_address_cmd(uint8_t* p_cpc_buffer) {
    uint8_t* p_tmp_buf = NULL;
    uint8_t i;

    log_info("MAC Address : %02X:%02X:%02X:%02X:%02X:%02X", p_cpc_buffer[4],
             p_cpc_buffer[5], p_cpc_buffer[6], p_cpc_buffer[7], p_cpc_buffer[8],
             p_cpc_buffer[9]);

    p_tmp_buf = pvPortMalloc(0x100);

    if (p_tmp_buf) {
        flash_read_sec_register((uint32_t)p_tmp_buf, 0x1100);
        __cpc_flash_erase(FLASH_ERASE_SECURE, 0x1100);

        for (i = 0; i < 6; i++)
            memcpy((p_tmp_buf + 2 + i), &p_cpc_buffer[9 - i], 6);
        for (i = 0; i < 3; i++) {
            memcpy((p_tmp_buf + 8 + i), &p_cpc_buffer[9 - i], 3);
            memcpy((p_tmp_buf + 13 + i), &p_cpc_buffer[6 - i], 3);
        }
        p_tmp_buf[11] = 0xFF;
        p_tmp_buf[12] = 0xFF;

        flash_write_sec_register((uint32_t)p_tmp_buf, 0x1100);
        vPortFree(p_tmp_buf);
    }

    if (hci_mgr.le_scan_window > hci_mgr.le_scan_interval) {
        hci_mgr.le_scan_window = 0x10;
        hci_mgr.le_scan_interval = 0x20;
    }

    uint8_t pdata[] = {0x04, 0x0E, 0x04, 0x01, 0x04, 0xFC, 0x00};
    cpc_hci_buffer_send(pdata, sizeof(pdata));
    CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
}

static inline void __cpc_hci_vsc_cmd(uint8_t* p_cpc_buffer) {
    switch (p_cpc_buffer[1]) {
        case 0x02: handle_pta_cmd(p_cpc_buffer); break;
        case 0x03: handle_scan_interval_window_cmd(p_cpc_buffer); break;
        case 0x04: handle_set_mac_address_cmd(p_cpc_buffer); break;
        default: break;
    }
}

static void __cpc_hci_ep_proc(cpc_hci_event_t evt) {
    uint32_t rval = 0;
    uint16_t data_len;
    uint8_t* p_cpc_buffer;
    uint8_t ep_state;

    uint8_t* ptr = NULL;

    if (CPC_HCI_EVENT_CPC_ERROR & evt) {
        ep_state = cpc_get_endpoint_state(&hci_mgr.ep);

        log_error("cpc hci ep error %d", ep_state);
        if (ep_state == CPC_STATE_ERROR_DESTINATION_UNREACHABLE)
            cpc_system_reset(0);

        cpc_close_endpoint(&hci_mgr.ep);
        cpc_set_state(&hci_mgr.ep, CPC_STATE_OPEN);
        MCU_ATOMIC_LOAD(hci_mgr.host_connect, 0);

        MCU_ATOMIC_LOAD(hci_mgr.ep.ref_count, 1);
    }

    if ((CPC_HCI_EVENT_CPC_READ & evt)) {

        do {
            if (hci_mgr.cpc_event_cnt == 0)
                break;
            rval = cpc_read(&hci_mgr.ep, (void**)&p_cpc_buffer, &data_len, 0,
                            1);

            if (rval) {
                log_error("cpc read error %04lX", rval);
            } else {
                CORE_ATOMIC_SECTION(hci_mgr.cpc_event_cnt--;);
                if (p_cpc_buffer[0] == 0x01 && p_cpc_buffer[1] == 0x0b
                    && p_cpc_buffer[2] == 0x20 && p_cpc_buffer[3] == 0x07) {

                    p_cpc_buffer[5] = (hci_mgr.le_scan_interval & 0xFF);
                    p_cpc_buffer[6] = ((hci_mgr.le_scan_interval >> 8) & 0xFF);

                    p_cpc_buffer[7] = (hci_mgr.le_scan_window & 0xFF);
                    p_cpc_buffer[8] = ((hci_mgr.le_scan_window >> 8) & 0xFF);

                    log_info("LE Set Scan Parameters %04lX %04lX",
                             hci_mgr.le_scan_window, hci_mgr.le_scan_interval);
                }

                if (p_cpc_buffer[0] == 0x01 && p_cpc_buffer[2] == 0xFC) {
                    __cpc_hci_vsc_cmd(p_cpc_buffer);
                } else {
                    hci_bridge_message_write((ble_hci_message_t*)p_cpc_buffer);
                }
                cpc_free_rx_buffer(p_cpc_buffer);
            }
        } while (hci_mgr.cpc_event_cnt > 0);
    }
    // if (evt & CPC_HCI_EVENT_CPC_WRITE_DONE) {
    //     MCU_ATOMIC_LOAD(hci_mgr.hci_tx_done, 1);
    // }
    // if (evt & CPC_HCI_EVENT_HCI_IN) {

    // }

    if (hci_mgr.hci_tx_done > 0) {
        size_t xReceivedBytes = 0;
        size_t xBytes2send = xStreamBufferBytesAvailable(hci_mgr.xStreamBuffer);

        if (xBytes2send == 0) {
            MCU_ATOMIC_SECTION(hci_mgr.hci_event_trigger--;);
            return;
        }

        ptr = pvPortMalloc(xBytes2send);
        if (ptr == NULL)
            return;
        xReceivedBytes = xStreamBufferReceive(hci_mgr.xStreamBuffer, ptr,
                                              xBytes2send, 100);

        rval = cpc_write(&hci_mgr.ep, ptr, xBytes2send, 0, ptr);
        if (rval != 0) {
            log_error("cpc hci write error %04lX", rval);
            vPortFree(ptr);
            return;
        }
        MCU_ATOMIC_SECTION(hci_mgr.hci_tx_done--;);
        MCU_ATOMIC_SECTION(hci_mgr.hci_event_trigger--;);
    }
}

static int __hci_evt_cb(uint8_t* p_data, uint16_t data_len) {
    size_t xBytesSent, xBytesAvailable;
    if (hci_mgr.host_connect == 0)
        return 0;
    if (data_len <= 0)
        return 0;

    cpc_hci_buffer_send(p_data, data_len);

    CPC_HCI_NOTIFY(CPC_HCI_EVENT_HCI_IN);
    MCU_ATOMIC_SECTION(hci_mgr.hci_event_trigger++;);
    return 0;
}

static void __cpc_hci_set_mac_address(void) {
    uint8_t* p_read_param;
    uint8_t* p_cmp;
    uint8_t _waddr = 1;
    uint8_t hci_set_addr[] = {0x01, 0x01, 0xFC, 0x09, 0x00, 0xF7, 0x00,
                              0x00, 0x4C, 0xC1, 0x0B, 0x64, 0x08};

    p_read_param = pvPortMalloc(256);
    if (p_read_param == NULL)
        return;

    p_cmp = pvPortMalloc(sizeof(ble_unique_code_format_t));
    if (p_cmp == NULL) {
        vPortFree(p_read_param);
        return;
    }

    flash_read_sec_register((uint32_t)p_read_param, 0x1100);
    memset(p_cmp, 0, sizeof(ble_unique_code_format_t));
    if (!memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)))
        _waddr = 0;
    memset(p_cmp, 0xFF, sizeof(ble_unique_code_format_t));
    if (!memcmp(p_read_param, p_cmp, sizeof(ble_unique_code_format_t)))
        _waddr = 0;
    vPortFree(p_cmp);

    if (_waddr) {
        for (int i = 0; i < 6; i++)
            hci_set_addr[4 + i] = p_read_param[2 + i];
        hci_bridge_message_write((ble_hci_message_t*)&hci_set_addr);
    }
    vPortFree(p_read_param);
}

static void __cpc_hci_task(void* parameters_ptr) {
    cpc_hci_event_t sevent = CPC_HCI_EVENT_NONE;

    __cpc_hci_set_mac_address();
    while (1) {
        CPC_HCI_GET_NOTIFY(sevent);
        __cpc_hci_ep_proc(sevent);
        if (hci_mgr.hci_event_trigger > 0) {
            ulTaskNotifyTake(pdTRUE, 20);
        } else
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

// static void scan_100msTimerCallback( TimerHandle_t xTimer )
// {
//     log_warn("500 ms no adv report !");
// }

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void cpc_hci_init(void) {
    const uint32_t hci_str_buffer_size = CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH;
    hci_bridge_init();

    hci_bridge_callback_set(HIC_INTERFACE_CALLBACK_TYPE_EVENT, __hci_evt_cb);
    hci_bridge_callback_set(HIC_INTERFACE_CALLBACK_TYPE_DATA, __hci_evt_cb);

    __cpc_hci_ep_init();
    hci_mgr.xStreamBuffer = xStreamBufferCreate(hci_str_buffer_size, 1);
    if (hci_mgr.xStreamBuffer == NULL) {
        log_error("HCI Message buffer create fail!");
        configASSERT(0);
    }
    CPC_TASK_CREATE(__cpc_hci_task, "HCI_CPC", 512, NULL,
                    E_TASK_PRIORITY_NORMAL, &hci_mgr.task);
}