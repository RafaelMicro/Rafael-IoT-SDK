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
#include "string.h"
#include "log.h"

static otUdpSocket          appSock;
static void (*app_udpHandler) (otMessage *, const otMessageInfo *);
static uint16_t appUdpPort = THREAD_UDP_PORT;

static void otUdpReceive_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo) 
{
    log_info("UPD Packet received, port: %d, len: %d", aMessageInfo->mPeerPort, otMessageGetLength(aMessage));

    if(app_udpHandler)
        app_udpHandler(aMessage, aMessageInfo);

    otMessageFree(aMessage);

}

void app_udpSend(otInstance *instance, uint8_t *p, uint16_t len) 
{
    otNeighborInfo          neighborInfo;
    otNeighborInfoIterator  nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otMessage               *pmsg;
    otMeshLocalPrefix       *pprefix = NULL;
    otMessageInfo           messageInfo;

    pprefix = (otMeshLocalPrefix *)otThreadGetMeshLocalPrefix(instance);

    nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    while (otThreadGetNextNeighborInfo(instance, &nbrIter, &neighborInfo) == OT_ERROR_NONE)
    {

        memset(&messageInfo, 0, sizeof(messageInfo));

        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
        messageInfo.mPeerAddr.mFields.m8[14] = neighborInfo.mRloc16 >> 8;
        messageInfo.mPeerAddr.mFields.m8[15] = neighborInfo.mRloc16 & 0xff;
        messageInfo.mPeerPort = appUdpPort;
        messageInfo.mHopLimit          = 0;
        messageInfo.mAllowZeroHopLimit = false;

        pmsg = otIp6NewMessage(instance, NULL);
        if (OT_ERROR_NONE == otMessageAppend(pmsg, p, len) && OT_ERROR_NONE == otMessageSetLength(pmsg, len)) 
        {
            if (OT_ERROR_NONE == otUdpSendDatagram(instance, pmsg, &messageInfo)) 
            {
                log_info ("UDP Packet sent, Rloc16: %x", neighborInfo.mRloc16);
            }
            else 
            {
                otMessageFree(pmsg);
            }
        }
    }
}

uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port)
{
    otSockAddr sockAddr;

    uint8_t ret;

    memset(&appSock, 0 ,sizeof(otUdpSocket));
    memset(&sockAddr, 0, sizeof(otSockAddr));

    ret = otUdpOpen(instance, &appSock, otUdpReceive_handler, instance);

    if (OT_ERROR_NONE == ret) 
    {
        app_udpHandler = handler;
        sockAddr.mPort = udp_port;
        ret = otUdpBind(instance, &appSock, &sockAddr, OT_NETIF_THREAD);
        if (OT_ERROR_NONE == ret) 
        {
            log_info("UDP PORT           : 0x%x", udp_port);
        }
    }

    return ret;
}
