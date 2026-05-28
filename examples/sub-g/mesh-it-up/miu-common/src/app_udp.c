/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */
#include <config/ip6.h>
#include <miu_port.h>
#include <openthread/ip6.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include "FreeRTOS.h"

#include <main.h>
#include "cli.h"
#include "log.h"
#include "queue.h"
#include "string.h"
#include "util_string.h"

typedef enum {
    THREAD_UDP_EVENT_SEND = 0x01,
    THREAD_UDP_EVENT_RECEIVED = 0x02,
} udp_event_id_t;

typedef struct {
    uint8_t event;
    otIp6Address ipv6; /*dst or src*/
    uint8_t buf[OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH + 1];
    uint16_t lens;
} app_udp_msg_t;

static xQueueHandle app_udp_msg_queue;
static otUdpSocket appSock;
static uint16_t appUdpPort = CONFIG_APP_TASK_UDP_LISTEN_PORT;

void app_udpReceived_task(uint8_t* data, uint16_t data_lens,
                          otIp6Address src_ipv6) {
    log_info_hexdump("UDP", data, data_lens);
}

void app_udpSend_task(uint8_t* data, uint16_t data_lens,
                      otIp6Address dst_ipv6) {

    otMessage* pmsg;
    otMessageInfo messageInfo;
    otInstance* instance = otrGetInstance();

    memset(&messageInfo, 0, sizeof(messageInfo));

    memcpy(messageInfo.mPeerAddr.mFields.m8, &dst_ipv6.mFields.m8,
           OT_IP6_ADDRESS_SIZE);
    // messageInfo.mSockAddr = *otThreadGetMeshLocalEid(otrGetInstance());
    messageInfo.mPeerPort = appUdpPort;
    messageInfo.mHopLimit = 255;
    messageInfo.mAllowZeroHopLimit = false;

    pmsg = otIp6NewMessage(instance, NULL);
    if (OT_ERROR_NONE == otMessageAppend(pmsg, data, data_lens)
        && OT_ERROR_NONE == otMessageSetLength(pmsg, data_lens)) {
        if (OT_ERROR_NONE != otUdpSendDatagram(instance, pmsg, &messageInfo)) {
            otMessageFree(pmsg);
        }
    }
}

void app_udp_task(void* arg) {
    app_udp_msg_t app_udp_msg;
    if (xQueueReceive(app_udp_msg_queue, &app_udp_msg, 0) == pdPASS) {

        if (app_udp_msg.event == THREAD_UDP_EVENT_SEND) {
            app_udpSend_task(app_udp_msg.buf, app_udp_msg.lens,
                             app_udp_msg.ipv6);
        } else if (app_udp_msg.event == THREAD_UDP_EVENT_RECEIVED) {
            app_udpReceived_task(app_udp_msg.buf, app_udp_msg.lens,
                                 app_udp_msg.ipv6);
        }
    }
}

static void otUdpReceive_handler(void* aContext, otMessage* aMessage,
                                 const otMessageInfo* aMessageInfo) {
    app_udp_msg_t rx_udp_msg;
    rx_udp_msg.event = THREAD_UDP_EVENT_RECEIVED;
    rx_udp_msg.lens = otMessageGetLength(aMessage)
                      - otMessageGetOffset(aMessage);
    /*copy source ip*/
    memcpy((void*)&rx_udp_msg.ipv6, (void*)&aMessageInfo->mPeerAddr,
           sizeof(otIp6Address));

    if (rx_udp_msg.lens < (OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH + 1)) {
        /*copy msg*/
        otMessageRead(aMessage, otMessageGetOffset(aMessage), rx_udp_msg.buf,
                      rx_udp_msg.lens);
        if (xQueueSend(app_udp_msg_queue, &rx_udp_msg, 0) == pdPASS) {
            ot_app_task_post(app_udp_task, NULL);
        }

    } else {
        log_info("App Recv len %u/%u to big ", rx_udp_msg.lens,
                 (OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH + 1));
    }
}

int app_udpSend(otIp6Address dstaddr, uint8_t* p, uint16_t len, bool fromISR) {
    otError error = OT_ERROR_BUSY;
    app_udp_msg_t tx_udp_msg;
    tx_udp_msg.event = THREAD_UDP_EVENT_SEND;
    tx_udp_msg.lens = len;
    memcpy(&tx_udp_msg.ipv6.mFields.m8, &dstaddr.mFields.m8,
           OT_IP6_ADDRESS_SIZE);
    if (tx_udp_msg.lens < (OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH + 1)) {
        memcpy(tx_udp_msg.buf, p, tx_udp_msg.lens);
        if (fromISR) {
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            if (xQueueSendFromISR(app_udp_msg_queue, &tx_udp_msg,
                                  &pxHigherPriorityTaskWoken)
                == pdPASS) {
                ot_app_task_post(app_udp_task, NULL);
                error = OT_ERROR_NONE;
            }
        } else {
            if (xQueueSend(app_udp_msg_queue, &tx_udp_msg, 0) == pdPASS) {
                ot_app_task_post(app_udp_task, NULL);
                error = OT_ERROR_NONE;
            }
        }

    } else {
        log_info("App Send len %u/%u to big \r\n", len,
                 (OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH + 1));
    }
    return (error != OT_ERROR_NONE);
}

uint8_t app_sockInit(otInstance* instance, uint16_t udp_port) {
    otSockAddr sockAddr;

    uint8_t ret;

    memset(&appSock, 0, sizeof(otUdpSocket));
    memset(&sockAddr, 0, sizeof(otSockAddr));

    ret = otUdpOpen(instance, &appSock, otUdpReceive_handler, instance);

    if (OT_ERROR_NONE == ret) {
        appUdpPort = udp_port;
        sockAddr.mPort = appUdpPort;
        ret = otUdpBind(instance, &appSock, &sockAddr, OT_NETIF_THREAD_HOST);
        if (OT_ERROR_NONE == ret) {
            log_info("UDP PORT           : 0x%x", sockAddr.mPort);
        }
        /* Init rx done queue*/
        app_udp_msg_queue = xQueueCreate(5, sizeof(app_udp_msg_t));
    }

    return ret;
}