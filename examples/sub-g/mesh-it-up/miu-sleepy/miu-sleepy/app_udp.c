/**
 * @file app_udp.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <miu_port.h>
#include <openthread/thread.h>
#include <openthread/udp.h>

#include "FreeRTOS.h"

#include <main.h>
#include "cli.h"
#include "log.h"
#include "string.h"
#include "util_string.h"

static otUdpSocket appSock;
static void (*app_udpHandler)(otMessage*, const otMessageInfo*);
static uint16_t appUdpPort = CONFIG_APP_TASK_UDP_LISTEN_PORT;

static void otUdpReceive_handler(void* aContext, otMessage* aMessage,
                                 const otMessageInfo* aMessageInfo) {
    log_info("UPD Packet received, port: %d, len: %d", aMessageInfo->mPeerPort,
             otMessageGetLength(aMessage));

    if (app_udpHandler) {
        app_udpHandler(aMessage, aMessageInfo);
    }
}

void app_udpSend(otIp6Address dstaddr, uint8_t* p, uint16_t len) {
    otMessage* pmsg;
    otMessageInfo messageInfo;
    otInstance* instance = otrGetInstance();

    memset(&messageInfo, 0, sizeof(messageInfo));

    memcpy(messageInfo.mPeerAddr.mFields.m8, &dstaddr, OT_IP6_ADDRESS_SIZE);
    messageInfo.mPeerPort = appUdpPort;
    messageInfo.mHopLimit = 255;
    messageInfo.mAllowZeroHopLimit = false;

    pmsg = otIp6NewMessage(instance, NULL);
    if (OT_ERROR_NONE == otMessageAppend(pmsg, p, len)
        && OT_ERROR_NONE == otMessageSetLength(pmsg, len)) {
        if (OT_ERROR_NONE != otUdpSendDatagram(instance, pmsg, &messageInfo)) {
            otMessageFree(pmsg);
        }
    }
}

uint8_t app_sockInit(otInstance* instance,
                     void (*handler)(otMessage*, const otMessageInfo*),
                     uint16_t udp_port) {
    otSockAddr sockAddr;

    uint8_t ret;

    memset(&appSock, 0, sizeof(otUdpSocket));
    memset(&sockAddr, 0, sizeof(otSockAddr));

    ret = otUdpOpen(instance, &appSock, otUdpReceive_handler, instance);

    if (OT_ERROR_NONE == ret) {
        app_udpHandler = handler;
        appUdpPort = udp_port;
        sockAddr.mPort = appUdpPort;
        ret = otUdpBind(instance, &appSock, &sockAddr, OT_NETIF_THREAD_HOST);
        if (OT_ERROR_NONE == ret) {
            log_info("UDP PORT           : 0x%x", sockAddr.mPort);
        }
    }

    return ret;
}