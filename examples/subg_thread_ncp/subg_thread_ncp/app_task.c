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

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "main.h"
#include "log.h"

static SemaphoreHandle_t    appSemHandle          = NULL;
extern otOperationalDataset at_dataset;
extern void __udp_cb(otMessage *otMsg, const otMessageInfo *otInfo);
extern uint16_t at_udp_port;

void otrInitUser(otInstance * instance)
{
    otLinkModeConfig mode;
    otAppCliInit((otInstance * )instance);
    memset(&mode, 0, sizeof(mode));

#if OPENTHREAD_FTD==1
    mode.mDeviceType         = 1;
    mode.mRxOnWhenIdle       = 1;
    mode.mNetworkData        = 1;
#endif    

#if OPENTHREAD_MTD==1
    mode.mDeviceType         = 0;
    mode.mNetworkData        = 0;
    mode.mRxOnWhenIdle       = 0;
    otLinkSetPollPeriod(instance, 500);
#endif

    otThreadSetLinkMode(instance, mode);

    /*set meshLocalPrefix*/
    uint8_t meshLocalPrefix[OT_MESH_LOCAL_PREFIX_SIZE] = {0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00};

    /* check the Active Dataset */
    memset(&at_dataset,0x0,sizeof(otOperationalDataset));
    otDatasetGetActive(instance, &at_dataset);    
    if(!at_dataset.mActiveTimestamp.mSeconds)
    {
        at_dataset.mActiveTimestamp.mSeconds = 1;
        at_dataset.mComponents.mIsActiveTimestampPresent = true;
    }
    
    memcpy(at_dataset.mMeshLocalPrefix.m8, meshLocalPrefix, OT_MESH_LOCAL_PREFIX_SIZE);
    at_dataset.mComponents.mIsMeshLocalPrefixPresent = true;

    /* Set the Active Operational Dataset to this dataset */
    otDatasetSetActive(instance, &at_dataset);

    log_info("ActiveTimestamp    : %u",(uint32_t)at_dataset.mActiveTimestamp.mSeconds);
    log_info("ExtendedPanId      : %02X%02X%02X%02X%02X%02X%02X%02X", 
            at_dataset.mExtendedPanId.m8[0],at_dataset.mExtendedPanId.m8[1],
            at_dataset.mExtendedPanId.m8[2],at_dataset.mExtendedPanId.m8[3],
            at_dataset.mExtendedPanId.m8[4],at_dataset.mExtendedPanId.m8[5],
            at_dataset.mExtendedPanId.m8[6],at_dataset.mExtendedPanId.m8[7]);
    log_info("MeshLocalPrefix    : %02X%02X%02X%02X%02X%02X%02X%02X", 
            at_dataset.mMeshLocalPrefix.m8[0],at_dataset.mMeshLocalPrefix.m8[1],
            at_dataset.mMeshLocalPrefix.m8[2],at_dataset.mMeshLocalPrefix.m8[3],
            at_dataset.mMeshLocalPrefix.m8[4],at_dataset.mMeshLocalPrefix.m8[5],
            at_dataset.mMeshLocalPrefix.m8[6],at_dataset.mMeshLocalPrefix.m8[7]);

    log_info("Link Mode           %d, %d, %d", 
        otThreadGetLinkMode(instance).mRxOnWhenIdle, 
        otThreadGetLinkMode(instance).mDeviceType, 
        otThreadGetLinkMode(instance).mNetworkData);
    log_info("Network name        : %s", otThreadGetNetworkName(instance));
    log_info("PAN ID              : %x", otLinkGetPanId(instance));

    log_info("channel             : %d", otLinkGetChannel(instance));
    otNetworkKey networkKey;
    otThreadGetNetworkKey(instance, &networkKey);

    log_info("networkkey          : %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
        networkKey.m8[0],networkKey.m8[1],networkKey.m8[2],networkKey.m8[3],
        networkKey.m8[4],networkKey.m8[5],networkKey.m8[6],networkKey.m8[7],
        networkKey.m8[8],networkKey.m8[9],networkKey.m8[10],networkKey.m8[11],
        networkKey.m8[12],networkKey.m8[13],networkKey.m8[14],networkKey.m8[15]);

    const otExtAddress *extAddress= otLinkGetExtendedAddress(instance);

    log_info("extaddr            : %02X%02X%02X%02X%02X%02X%02X%02X", 
        extAddress->m8[0],extAddress->m8[1],extAddress->m8[2],extAddress->m8[3],
        extAddress->m8[4],extAddress->m8[5],extAddress->m8[6],extAddress->m8[7]);

    log_info("Thread version     : %s", otGetVersionString());

    /*auto start and become to leader*/
    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);
    otThreadBecomeLeader(instance);

    log_info("State %s \r\n",otThreadDeviceRoleToString(otThreadGetDeviceRole(instance)));
    app_sockInit(instance, __udp_cb, at_udp_port);
}


void app_task (void) 
{
    otMeshLocalPrefix       *pprefix = NULL;
    uint8_t                 *p, i;
    otMessageInfo           messageInfo;
    otMessage               *pmsg;
    otNeighborInfo          neighborInfo;
    otNeighborInfoIterator  nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
#if OPENTHREAD_FTD
    otChildInfo             childInfo;
#else
    otRouterInfo            parentInfo;
#endif

    appSemHandle = xSemaphoreCreateBinary();

    while (true) 
    {
        if (xSemaphoreTake(appSemHandle, 10000))
        {
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if (instance) {
                    pprefix = (otMeshLocalPrefix *)otThreadGetMeshLocalPrefix(instance);

    #if OPENTHREAD_FTD
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
    #else

                    if (OT_ERROR_NONE == otThreadGetParentInfo(instance, &parentInfo)) {

                        memset(&messageInfo, 0, sizeof(messageInfo));

                        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                        messageInfo.mPeerAddr.mFields.m8[14] = parentInfo.mRloc16 >> 8;
                        messageInfo.mPeerAddr.mFields.m8[15] = parentInfo.mRloc16 & 0xff;
                        
                        p = messageInfo.mPeerAddr.mFields.m8;

                        log_info("Parent Rloc16                : %x", parentInfo.mRloc16);
                        p = parentInfo.mExtAddress.m8;
                        log_info("Parent Ext Addr              : %02x%02x-%02x%02x-%02x%02x-%02x%02x",
                                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
                        log_info("Parent Rloc                  : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                    }
    #endif

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

                        pmsg = otIp6NewMessage(instance, NULL);
                        if (pmsg) {
                            messageInfo.mHopLimit          = 0;
                            messageInfo.mAllowZeroHopLimit = false;

                            if (OT_ERROR_NONE != otIcmp6SendEchoRequest(instance, pmsg, &messageInfo, 0)) {
                                otMessageFree(pmsg);
                            }
                        }
                    }
                }            
            );
        }
    }

}