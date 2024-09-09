/**
 * @file app_task.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include "main.h"
#include "log.h"

static SemaphoreHandle_t    appSemHandle          = NULL;

static void ot_stateChangeCallback(uint32_t flags, void * p_context) 
{
    char states[5][10] = {"disabled", "detached", "child", "router", "leader"};
    otInstance *instance = (otInstance *)p_context;
    uint8_t *p;

    if (flags & OT_CHANGED_THREAD_ROLE)
    {

        uint32_t role = otThreadGetDeviceRole(p_context);
        switch(role)
        {
            case OT_DEVICE_ROLE_CHILD:
                break;
            case OT_DEVICE_ROLE_ROUTER:
                break;
            case OT_DEVICE_ROLE_LEADER:
                break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default:
                break;
        }

        if (role) {
            log_info("Current role       : %s", states[otThreadGetDeviceRole(p_context)]);

            p = (uint8_t *)(otLinkGetExtendedAddress(instance)->m8);
            log_info("Extend Address     : %02x%02x-%02x%02x-%02x%02x-%02x%02x", 
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t *)(otThreadGetMeshLocalPrefix(instance)->m8);
            log_info("Local Prefx        : %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t *)(otThreadGetLinkLocalIp6Address(instance)->mFields.m8);
            log_info("IPv6 Address       : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

            log_info("Rloc16             : %x", otThreadGetRloc16(instance));

            p = (uint8_t *)(otThreadGetRloc(instance)->mFields.m8);
            log_info("Rloc               : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        }
    }
}

#if OPENTHREAD_FTD
static void ot_neighborChangeCallback (otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntryInfo) 
{
    switch (aEvent) {
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
            log_info("child added %llx", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
            log_info("child removed %llx", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED:
            log_info("child mode changned %llx", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
            log_info("router added %llx", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
            log_info("router removed %llx", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
    }

    xSemaphoreGive(appSemHandle);
}
#endif


void otrInitUser(otInstance * instance)
{
    otAppCliInit((otInstance * )instance);
    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);
}


void app_task (void) 
{
    otMeshLocalPrefix       *pprefix = NULL;
    uint8_t                 *p, i;
    otMessageInfo           messageInfo;
    otNeighborInfo          neighborInfo;
    otNeighborInfoIterator  nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;

    otChildInfo             childInfo;


    appSemHandle = xSemaphoreCreateBinary();

    while (true) 
    {
        if (xSemaphoreTake(appSemHandle, 10000))
        {
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if (instance) {

                    pprefix = (otMeshLocalPrefix *)otThreadGetMeshLocalPrefix(instance);

                    for (i = 0; i < otThreadGetMaxAllowedChildren(instance); i ++) {
                        if (OT_ERROR_NONE != otThreadGetChildInfoByIndex(instance, i, &childInfo)) {
                            continue;
                        }

                        log_info("Child(%02d) Rloc16           : %x", i, childInfo.mRloc16);
                        p = childInfo.mExtAddress.m8;
                        log_info("Child(%02d) Ext Addr         : %02x%02x-%02x%02x-%02x%02x-%02x%02x",
                                i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

                        memset(&messageInfo, 0, sizeof(messageInfo));

                        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                        messageInfo.mPeerAddr.mFields.m8[14] = childInfo.mRloc16 >> 8;
                        messageInfo.mPeerAddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                        
                        p = messageInfo.mPeerAddr.mFields.m8;
                        log_info("Child(%02d) Rloc             : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                            i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                    }


                    i = 0;
                    nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
                    while (otThreadGetNextNeighborInfo(instance, &nbrIter, &neighborInfo) == OT_ERROR_NONE){
                        log_info("Neighbor(%02d) Rloc16        : %x", i, neighborInfo.mRloc16);
                        p = neighborInfo.mExtAddress.m8;
                        log_info("Neighbor(%02d) Ext Addr      : %02x%02x-%02x%02x-%02x%02x-%02x%02x",
                                i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

                        memset(&messageInfo, 0, sizeof(messageInfo));

                        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                        messageInfo.mPeerAddr.mFields.m8[14] = neighborInfo.mRloc16 >> 8;
                        messageInfo.mPeerAddr.mFields.m8[15] = neighborInfo.mRloc16 & 0xff;

                        p = messageInfo.mPeerAddr.mFields.m8;
                        log_info("Neighbor(%02d) Rloc          : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                            i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                        i ++;
                    }
                }            
            );
        }
    }

}