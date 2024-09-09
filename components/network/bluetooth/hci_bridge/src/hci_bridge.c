/**
 * @file hci_bridge.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stddef.h>
#include <string.h>

#include "FreeRTOS.h"
#include "hci_bridge.h"
#include "hosal_rf.h"
#include "log.h"
#include "queue.h"
#include "task.h"
/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define HCI_BRIDGE_FRAME_BUFFER_NUM 8
#define HCI_BRIDGE_FRAME_MAX_SIZE   (sizeof(ble_hci_message_t))

typedef struct {
    utils_dlist_t dlist;
    uint16_t len;
    uint8_t* pdata;
} hci_rx_msg_t;

#define HCI_BRIDGE_ALIGHNED_FRAME_SIZE ((sizeof(hci_rx_msg_t) + 3) & 0xfffffffc)
#define HCI_BRIDGE_TOTAL_FRAME_SIZE                                            \
    (HCI_BRIDGE_ALIGHNED_FRAME_SIZE + HCI_BRIDGE_FRAME_MAX_SIZE)

typedef struct {
    utils_dlist_t rxFrameList;
    utils_dlist_t rxEventList;
    utils_dlist_t frameList;
    uint32_t dbgRxFrameNum;
    uint8_t buffPool[HCI_BRIDGE_TOTAL_FRAME_SIZE * HCI_BRIDGE_FRAME_BUFFER_NUM];
} hci_bridge_frame_t;

typedef enum {
    HIC_INTERFACE_EVENT_STATE_NONE = 0,

    HIC_INTERFACE_EVENT_STATE_HCI_EVT = 0x00000002,
    HIC_INTERFACE_EVENT_STATE_HCI_DATA = 0x00000004,

    HIC_INTERFACE_EVENT_STATE_ALL = 0xffffffff,
} hci_bridge_event_t;

#define HCI_BRIDGE_NOTIFY_ISR(ebit)                                            \
    (g_hci_evt_var |= ebit);                                                   \
    __hci_signal()
#define HCI_BRIDGE_NOTIFY(ebit)                                                \
    enter_critical_section();                                                  \
    g_hci_evt_var |= ebit;                                                     \
    leave_critical_section();                                                  \
    __hci_signal()
#define HCI_BRIDGE_GET_NOTIFY(ebit)                                            \
    enter_critical_section();                                                  \
    ebit = g_hci_evt_var;                                                      \
    g_hci_evt_var = HIC_INTERFACE_EVENT_STATE_NONE;                            \
    leave_critical_section()
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
static TaskHandle_t g_hci_taskHandle;
static uint8_t g_tx_sn;
static hci_bridge_frame_t g_hci_frame;
static hci_bridge_event_t g_hci_evt_var;
static hci_bridge_callback_t g_hci_evt_cb;
static hci_bridge_callback_t g_hci_data_cb;
static struct ble_hci_acl_data_sn_struct ghci_message_tx_data;

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static void __hci_data(hci_bridge_event_t evt) {
    hci_rx_msg_t* pframe;

    pframe = NULL;

    do {
        if (!(HIC_INTERFACE_EVENT_STATE_HCI_DATA & evt)) {
            break;
        }
        pframe = NULL;
        enter_critical_section();
        if (!utils_dlist_empty(&g_hci_frame.rxFrameList)) {
            pframe = (hci_rx_msg_t*)g_hci_frame.rxFrameList.next;
            utils_dlist_del(&pframe->dlist);
        }
        leave_critical_section();

        if (pframe) {
            if (g_hci_data_cb)
                g_hci_data_cb(pframe->pdata, pframe->len);

            enter_critical_section();
            utils_dlist_add_tail(&pframe->dlist, &g_hci_frame.frameList);
            leave_critical_section();
        }
    } while (pframe);
}

static void __hci_evt(hci_bridge_event_t evt) {
    hci_rx_msg_t* pframe;

    pframe = NULL;

    do {
        if (!(HIC_INTERFACE_EVENT_STATE_HCI_EVT & evt)) {
            break;
        }
        pframe = NULL;
        enter_critical_section();
        if (!utils_dlist_empty(&g_hci_frame.rxEventList)) {
            pframe = (hci_rx_msg_t*)g_hci_frame.rxEventList.next;
            g_hci_frame.dbgRxFrameNum--;

            utils_dlist_del(&pframe->dlist);
        }
        leave_critical_section();

        if (pframe) {
            if (g_hci_evt_cb)
                g_hci_evt_cb(pframe->pdata, pframe->len);

            enter_critical_section();
            utils_dlist_add_tail(&pframe->dlist, &g_hci_frame.frameList);
            leave_critical_section();
        }
    } while (pframe);
}

static void __hci_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(g_hci_taskHandle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(g_hci_taskHandle);
    }
}

static void __hci_proc(void* pvParameters) {
    hci_bridge_event_t sevent = HIC_INTERFACE_EVENT_STATE_NONE;
    for (;;) {
        HCI_BRIDGE_GET_NOTIFY(sevent);
        __hci_evt(sevent);
        __hci_data(sevent);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

static int __evt_cb(void* p_arg) {
    hci_rx_msg_t* p = NULL;
    uint16_t data_len;
    ble_hci_message_t* pmesg;

    do {
        enter_critical_section();
        if (!utils_dlist_empty(&g_hci_frame.frameList)) {
            p = (hci_rx_msg_t*)g_hci_frame.frameList.next;
            utils_dlist_del(&p->dlist);
        }
        leave_critical_section();

        if (p) {
            pmesg = (ble_hci_message_t*)(p_arg);
            data_len = pmesg->hci_message.hci_event.length + 3;

            p->len = data_len;
            memcpy(p->pdata, pmesg, data_len);

            enter_critical_section();
            utils_dlist_add_tail(&p->dlist, &g_hci_frame.rxEventList);

            g_hci_frame.dbgRxFrameNum++;

            leave_critical_section();

            HCI_BRIDGE_NOTIFY(HIC_INTERFACE_EVENT_STATE_HCI_EVT);
        }
    } while (0);

    return 0;
}

static int __data_cb(void* p_arg) {
    hci_rx_msg_t* p = NULL;
    uint16_t data_len;
    ble_hci_message_t* pmesg;
    do {
        enter_critical_section();
        if (!utils_dlist_empty(&g_hci_frame.frameList)) {
            p = (hci_rx_msg_t*)g_hci_frame.frameList.next;
            utils_dlist_del(&p->dlist);
        }
        leave_critical_section();

        if (p) {
            pmesg = (ble_hci_message_t*)(p_arg);
            data_len = pmesg->hci_message.hci_acl_data.length + 5;

            p->len = data_len;
            memcpy(p->pdata, pmesg, data_len);

            enter_critical_section();
            utils_dlist_add_tail(&p->dlist, &g_hci_frame.rxFrameList);
            leave_critical_section();

            HCI_BRIDGE_NOTIFY(HIC_INTERFACE_EVENT_STATE_HCI_DATA);
        }
    } while (0);
    return 0;
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void hci_bridge_init(void) {
    hci_rx_msg_t* pframe = NULL;
    memset(&g_hci_frame, 0, offsetof(hci_bridge_frame_t, buffPool));

    enter_critical_section();
    utils_dlist_init(&g_hci_frame.frameList);
    utils_dlist_init(&g_hci_frame.rxFrameList);
    utils_dlist_init(&g_hci_frame.rxEventList);
    for (int i = 0; i < HCI_BRIDGE_FRAME_BUFFER_NUM; i++) {
        pframe = (hci_rx_msg_t*)(g_hci_frame.buffPool
                                 + (HCI_BRIDGE_TOTAL_FRAME_SIZE * i));
        pframe->pdata = ((uint8_t*)pframe) + HCI_BRIDGE_ALIGHNED_FRAME_SIZE;
        utils_dlist_add_tail(&pframe->dlist, &g_hci_frame.frameList);
    }
    leave_critical_section();

    hosal_rf_callback_set(HOSAL_RF_BLE_EVENT_CALLBACK, __evt_cb, NULL);
    hosal_rf_callback_set(HOSAL_RF_BLE_RX_CALLBACK, __data_cb, NULL);

    xTaskCreate(__hci_proc, "hci_if", 512, NULL, E_TASK_PRIORITY_OPENTHREAD,
                &g_hci_taskHandle);

    g_tx_sn = 0;
}

void hci_bridge_callback_set(hci_bridge_callback_type_t type,
                             hci_bridge_callback_t pfn_callback) {
    if (type == HIC_INTERFACE_CALLBACK_TYPE_EVENT) {
        g_hci_evt_cb = pfn_callback;
    } else if (type == HIC_INTERFACE_CALLBACK_TYPE_DATA) {
        g_hci_data_cb = pfn_callback;
    }
}

int hci_bridge_message_write(ble_hci_message_t* pmesg) {
    int rval = 0;
    uint8_t transport_id, hci_command_length;
    uint16_t hci_acl_data_length;

    transport_id = pmesg->hci_message.ble_hci_array[0];

    do {
        switch (transport_id) {
            /* HCI Commnad */
            case 0x01:
                hci_command_length =
                    pmesg->hci_message.hci_command.length
                    + 1 /*transport*/ + 2 /*opcode*/ + 1 /*length*/;
                do {
                    rval = hosal_rf_wrire_command((uint8_t*)pmesg,
                                                  hci_command_length);
                } while (rval != HOSAL_RF_STATUS_SUCCESS);
                break;

            /*HCI ACL Data*/
            case 0x02:
                ghci_message_tx_data.transport_id = 0x02;
                ghci_message_tx_data.sequence = g_tx_sn;
                ghci_message_tx_data.handle =
                    pmesg->hci_message.hci_acl_data.handle;
                ghci_message_tx_data.pb_flag =
                    pmesg->hci_message.hci_acl_data.pb_flag;
                ghci_message_tx_data.bc_flag =
                    pmesg->hci_message.hci_acl_data.bc_flag;
                ghci_message_tx_data.length =
                    pmesg->hci_message.hci_acl_data.length;
                memcpy(ghci_message_tx_data.data,
                       pmesg->hci_message.hci_acl_data.data,
                       ghci_message_tx_data.length);

                hci_acl_data_length = pmesg->hci_message.hci_acl_data.length
                                      + 1 /*transport*/ + 2 /* sequence */
                                      + 2 /*handle*/ + 2 /*length*/;
                do {
                    rval = hosal_rf_write_tx_data(
                        (uint8_t*)&ghci_message_tx_data, hci_acl_data_length);
                } while (rval != HOSAL_RF_STATUS_SUCCESS);
                g_tx_sn++;
                break;

            default: break;
        }
        /* code */
    } while (0);

    return rval;
}