/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */
#include <miu_port.h>
#include "FreeRTOS.h"
#include "openthread-core-config.h"

#include <main.h>
#include "cli.h"
#include "log.h"
#include "queue.h"
#include "string.h"
#include "util_string.h"

#define THREAD_MAC_RAW_EVENT_SEND     0x1
#define THREAD_MAC_RAW_EVENT_RECEIVED 0x2

typedef struct {
    uint8_t event;
    uint8_t mac_addr[8]; /*dst or src*/
    int8_t rssi;         /*src rssi*/
    uint8_t channel;     /*dst*/
    uint16_t panid;      /*dst*/
    uint8_t buf[OT_RADIO_FRAME_MAX_SIZE + 1];
    uint16_t lens;
} app_mac_raw_msg_t;

static xQueueHandle app_mac_raw_msg_queue;

void app_macRawReceived_task(uint8_t* data, uint16_t data_lens,
                             uint8_t* src_mac, int8_t src_rssi) {
    log_info("lens(%d), src(%02x%02x%02x%02x%02x%02x%02x%02x), rssi(%d) \r\n",
             data_lens, src_mac[0], src_mac[1], src_mac[2], src_mac[3],
             src_mac[4], src_mac[5], src_mac[6], src_mac[7], src_rssi);
    log_info_hexdump("mac_rx", data, data_lens);
}

int app_macRawSend_task(uint8_t* data, uint16_t data_lens, uint8_t* dst_mac,
                        uint8_t dst_channel, uint16_t dst_panid) {

    otInstance* instance = otrGetInstance();
    return radio_mac_send(instance, dst_mac, data, data_lens, dst_channel,
                          dst_panid);
}

void app_macRaw_task(void* arg) {

    app_mac_raw_msg_t app_mac_raw_msg;
    memset((void*)&app_mac_raw_msg, 0, sizeof(app_mac_raw_msg_t));
    if (xQueueReceive(app_mac_raw_msg_queue, &app_mac_raw_msg, 0) == pdPASS) {

        if (app_mac_raw_msg.event == THREAD_MAC_RAW_EVENT_SEND) {
            int ret = app_macRawSend_task(
                app_mac_raw_msg.buf, app_mac_raw_msg.lens,
                app_mac_raw_msg.mac_addr, app_mac_raw_msg.channel,
                app_mac_raw_msg.panid);
            if (ret != 0) {
                log_info("app_macRawSend_task fail %d", ret);
            }
        } else if (app_mac_raw_msg.event == THREAD_MAC_RAW_EVENT_RECEIVED) {
            app_macRawReceived_task(app_mac_raw_msg.buf, app_mac_raw_msg.lens,
                                    app_mac_raw_msg.mac_addr,
                                    app_mac_raw_msg.rssi);
        }
    }
}

static void app_macRawReceived_cb(uint8_t* data, uint16_t lens, int8_t rssi,
                                  uint8_t* src_addr) {
    app_mac_raw_msg_t app_mac_rx_raw_msg;
    memset((void*)&app_mac_rx_raw_msg, 0, sizeof(app_mac_raw_msg_t));
    app_mac_rx_raw_msg.event = THREAD_MAC_RAW_EVENT_RECEIVED;
    app_mac_rx_raw_msg.lens = lens;
    memcpy(app_mac_rx_raw_msg.mac_addr, src_addr, 8);
    app_mac_rx_raw_msg.rssi = rssi;

    if (lens > 0 && app_mac_rx_raw_msg.lens < (OT_RADIO_FRAME_MAX_SIZE + 1)) {
        memcpy(app_mac_rx_raw_msg.buf, data, lens);
        if (xQueueSend(app_mac_raw_msg_queue, &app_mac_rx_raw_msg, 0)
            == pdPASS) {
            ot_app_task_post(app_macRaw_task, NULL);
        }
    } else {
        log_error(
            "Mac Raw Received len %u is not a valid AES block multiple\r\n",
            lens);
    }
}

int app_macRawSend(uint8_t* dstaddr, uint8_t* data, uint16_t len,
                   uint16_t panid, uint8_t channel, bool fromISR) {
    otError error = OT_ERROR_BUSY;
    app_mac_raw_msg_t app_mac_tx_raw_msg;

    memset((void*)&app_mac_tx_raw_msg, 0, sizeof(app_mac_raw_msg_t));
    app_mac_tx_raw_msg.event = THREAD_MAC_RAW_EVENT_SEND;
    app_mac_tx_raw_msg.lens = len;
    memcpy(app_mac_tx_raw_msg.mac_addr, dstaddr, 8);
    app_mac_tx_raw_msg.channel = channel;
    app_mac_tx_raw_msg.panid = panid;

    if (app_mac_tx_raw_msg.lens < (OT_RADIO_FRAME_MAX_SIZE + 1)) {
        memcpy(app_mac_tx_raw_msg.buf, data, len);
        if (fromISR) {
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            if (xQueueSendFromISR(app_mac_raw_msg_queue, &app_mac_tx_raw_msg,
                                  &pxHigherPriorityTaskWoken)
                == pdPASS) {
                ot_app_task_post(app_macRaw_task, NULL);
                error = OT_ERROR_NONE;
            }
        } else {
            if (xQueueSend(app_mac_raw_msg_queue, &app_mac_tx_raw_msg, 0)
                == pdPASS) {
                ot_app_task_post(app_macRaw_task, NULL);
                error = OT_ERROR_NONE;
            }
        }

    } else {
        log_info("App Mac Raw Send len %u/%u to big \r\n", len,
                 (OT_RADIO_FRAME_MAX_SIZE + 1));
    }
    return (error != OT_ERROR_NONE);
}

uint8_t app_macRawInit(otInstance* instance) {
    otSockAddr sockAddr;

    uint8_t ret = 0;

    otPlatRadioMacReceivedCallback(app_macRawReceived_cb);

    app_mac_raw_msg_queue = xQueueCreate(5, sizeof(app_mac_raw_msg_t));
    return ret;
}