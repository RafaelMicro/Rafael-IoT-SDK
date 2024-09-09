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

#include <openthread/udp.h>
#include <openthread/thread.h>
#include <openthread_port.h>

#include "FreeRTOS.h"

#include <main.h>
#include "log.h"

static otUdpSocket          appSock;
static void (*app_udpHandler) (otMessage *, const otMessageInfo *);

static void otUdpReceive_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo) 
{
    log_info("UPD Packet received, port: %d, len: %d", aMessageInfo->mPeerPort, otMessageGetLength(aMessage));
    if(app_udpHandler)
    {
        app_udpHandler(aMessage, aMessageInfo);
    }   
}

uint8_t app_udpSend(otInstance *instance, otIp6Address *addr, uint8_t *p, uint16_t len, uint16_t port) 
{
    uint8_t ret;
    otMessage *pmsg;
    otMessageInfo messageInfo;

    do
    {
        memset(&messageInfo, 0, sizeof(messageInfo));
        memcpy(messageInfo.mPeerAddr.mFields.m8, addr->mFields.m8, OT_IP6_ADDRESS_SIZE);
        messageInfo.mPeerPort = port;
        messageInfo.mHopLimit = 0;
        messageInfo.mAllowZeroHopLimit = false;

        pmsg = otIp6NewMessage(instance, NULL);
        
        if(pmsg == NULL)
        {
            ret = OT_ERROR_NO_BUFS;
            break;
        }
        ret = otMessageAppend(pmsg, p, len);

        if(ret != OT_ERROR_NONE)
            break;
        ret = otMessageSetLength(pmsg, len);

        if(ret != OT_ERROR_NONE)
            break;

        ret = otUdpSendDatagram(instance, pmsg, &messageInfo);


    } while (0);
    
    if(ret != OT_ERROR_NONE)
    {
        if(pmsg)
            otMessageFree(pmsg);
    }

    return ret;
}

uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port)
{
    otSockAddr sockAddr;

    uint8_t ret;

    do
    {
        if(otUdpIsOpen(instance, &appSock))
        {
            ret = OT_ERROR_ALREADY;
            break;
        }

        memset(&appSock, 0 ,sizeof(otUdpSocket));
        memset(&sockAddr, 0, sizeof(otSockAddr));

        ret = otUdpOpen(instance, &appSock, otUdpReceive_handler, instance);

        if (OT_ERROR_NONE == ret) 
        {
            app_udpHandler = handler;
            log_info("udp port bind %u\r\n",udp_port);
            sockAddr.mPort = udp_port;
            ret = otUdpBind(instance, &appSock, &sockAddr, OT_NETIF_THREAD);
        }
    } while (0);
    
    return ret;
}
