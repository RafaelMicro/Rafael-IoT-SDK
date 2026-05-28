/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file app_task.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * 
 */

#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "log.h"
#include "main.h"
#include "util_string.h"

static SemaphoreHandle_t appSemHandle = NULL;

static void ot_stateChangeCallback(uint32_t flags, void* p_context) {
    char states[5][10] = {"disabled", "detached", "child", "router", "leader"};
    otInstance* instance = (otInstance*)p_context;
    uint8_t* p;

    if (flags & OT_CHANGED_THREAD_ROLE) {

        uint32_t role = otThreadGetDeviceRole(p_context);
        switch (role) {
            case OT_DEVICE_ROLE_CHILD: break;
            case OT_DEVICE_ROLE_ROUTER: break;
            case OT_DEVICE_ROLE_LEADER: break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default: break;
        }

        if (role) {
            log_info("Current role       : %s",
                     states[otThreadGetDeviceRole(p_context)]);

            p = (uint8_t*)(otLinkGetExtendedAddress(instance)->m8);
            log_info("Extend Address     : %02x%02x-%02x%02x-%02x%02x-%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t*)(otThreadGetMeshLocalPrefix(instance)->m8);
            log_info("Local Prefx        : %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t*)(otThreadGetLinkLocalIp6Address(instance)
                               ->mFields.m8);
            log_info("IPv6 Address       : "
                     "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"
                     "02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9],
                     p[10], p[11], p[12], p[13], p[14], p[15]);

            log_info("Rloc16             : %x", otThreadGetRloc16(instance));

            p = (uint8_t*)(otThreadGetRloc(instance)->mFields.m8);
            log_info("Rloc               : "
                     "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"
                     "02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9],
                     p[10], p[11], p[12], p[13], p[14], p[15]);
        }
    }
}

static void
ot_neighborChangeCallback(otNeighborTableEvent aEvent,
                          const otNeighborTableEntryInfo* aEntryInfo) {
    switch (aEvent) {
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
            log_info("child added %llx",
                     *(uint64_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
            log_info("child removed %llx",
                     *(uint64_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED:
            log_info("child mode changned %llx",
                     *(uint64_t*)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
            log_info("router added %llx",
                     *(uint64_t*)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
            log_info("router removed %llx",
                     *(uint64_t*)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
    }

    xSemaphoreGive(appSemHandle);
}

static void set_network_config(otInstance* instance) {
    otError error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp.mSeconds = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    aDataset.mChannel = CONFIG_APP_THREAD_NETWORK_CHANNEL;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId = (otPanId)CONFIG_APP_THREAD_NETWORK_PANID;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC0, 0xDE, 0x1A, 0xB5,
                                            0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mExtendedPanId.m8, extPanId,
           sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    uint8_t key[OT_NETWORK_KEY_SIZE];

    memset(key, 0x0, OT_NETWORK_KEY_SIZE);
    for (int i = 0; i < 16; i++) {
        key[i] = (uint8_t)utility_strtox(
            (uint8_t*)&CONFIG_APP_THRREAD_NETWORK_MASTER_KEY[i * 2], 0, 2);
    }

    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    /* Set Network Name */
    size_t length = strlen(CONFIG_APP_THREAD_NETWORK_NAME);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, CONFIG_APP_THREAD_NETWORK_NAME, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    /* Set the Active Operational Dataset to this dataset */
    error = otDatasetSetActive(instance, &aDataset);
    if (error != OT_ERROR_NONE) {
        log_error("otDatasetSetActive failed with: %d, %s\r\n", error,
                  otThreadErrorToString(error));
        return;
    }
}

static void app_udp_cb(otMessage* otMsg, const otMessageInfo* otInfo) {
    uint8_t* p = NULL;

    uint16_t len;

    otInstance* instance = otrGetInstance();

    len = otMessageGetLength(otMsg) - otMessageGetOffset(otMsg);

    p = pvPortMalloc(len);

    do {
        if (p == NULL)
            break;

        otMessageRead(otMsg, otMessageGetOffset(otMsg), p, len);

        log_info("UDP Packet received, port: %d, len: %d", otInfo->mPeerPort,
                 len);
        log_info("UDP Packet data: %s", p);

    } while (0);

    if (p != NULL)
        vPortFree(p);
}

void otrInitUser(otInstance* instance) {

    otLinkModeConfig mode;

    otAppCliInit((otInstance*)instance);

    log_info("Thread version     : %s", otGetVersionString());

    memset(&mode, 0, sizeof(mode));

    mode.mDeviceType = 1;
    mode.mRxOnWhenIdle = 1;
    mode.mNetworkData = 1;

    otThreadSetLinkMode(instance, mode);

    set_network_config(instance);

    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);

    log_info("Link Mode           %d, %d, %d",
             otThreadGetLinkMode(instance).mRxOnWhenIdle,
             otThreadGetLinkMode(instance).mDeviceType,
             otThreadGetLinkMode(instance).mNetworkData);
    log_info("Network name        : %s", otThreadGetNetworkName(instance));
    log_info("PAN ID              : %x", otLinkGetPanId(instance));

    log_info("channel             : %d", otLinkGetChannel(instance));

    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);

    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);
    app_sockInit(instance, app_udp_cb, CONFIG_APP_THREAD_UDP_LISTEN_PORT);
}

void app_task(void) {
    otMeshLocalPrefix* pprefix = NULL;
    uint8_t *p, i;
    otMessageInfo messageInfo;
    otNeighborInfo neighborInfo;
    otNeighborInfoIterator nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;

    otChildInfo childInfo;

    appSemHandle = xSemaphoreCreateBinary();

    while (true) {
        if (xSemaphoreTake(appSemHandle, 10000)) {
            OT_THREAD_SAFE(
                otInstance* instance = otrGetInstance(); if (instance) {
                    pprefix = (otMeshLocalPrefix*)otThreadGetMeshLocalPrefix(
                        instance);

                    for (i = 0; i < otThreadGetMaxAllowedChildren(instance);
                         i++) {
                        if (OT_ERROR_NONE
                            != otThreadGetChildInfoByIndex(instance, i,
                                                           &childInfo)) {
                            continue;
                        }

                        log_info("Child(%02d) Rloc16           : %x", i,
                                 childInfo.mRloc16);
                        p = childInfo.mExtAddress.m8;
                        log_info("Child(%02d) Ext Addr         : "
                                 "%02x%02x-%02x%02x-%02x%02x-%02x%02x",
                                 i, p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                 p[7]);

                        memset(&messageInfo, 0, sizeof(messageInfo));

                        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8,
                               sizeof(pprefix->m8));
                        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                        messageInfo.mPeerAddr.mFields.m8[14] = childInfo.mRloc16
                                                               >> 8;
                        messageInfo.mPeerAddr.mFields.m8[15] = childInfo.mRloc16
                                                               & 0xff;

                        p = messageInfo.mPeerAddr.mFields.m8;
                        log_info("Child(%02d) Rloc             : "
                                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                                 "%02x%02x:%02x%02x:%02x%02x",
                                 i, p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                 p[7], p[8], p[9], p[10], p[11], p[12], p[13],
                                 p[14], p[15]);
                    }

                    i = 0;
                    nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
                    while (otThreadGetNextNeighborInfo(instance, &nbrIter,
                                                       &neighborInfo)
                           == OT_ERROR_NONE) {
                        log_info("Neighbor(%02d) Rloc16        : %x", i,
                                 neighborInfo.mRloc16);
                        p = neighborInfo.mExtAddress.m8;
                        log_info("Neighbor(%02d) Ext Addr      : "
                                 "%02x%02x-%02x%02x-%02x%02x-%02x%02x",
                                 i, p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                 p[7]);

                        memset(&messageInfo, 0, sizeof(messageInfo));

                        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8,
                               sizeof(pprefix->m8));
                        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                        messageInfo.mPeerAddr.mFields.m8[14] =
                            neighborInfo.mRloc16 >> 8;
                        messageInfo.mPeerAddr.mFields.m8[15] =
                            neighborInfo.mRloc16 & 0xff;

                        p = messageInfo.mPeerAddr.mFields.m8;
                        log_info("Neighbor(%02d) Rloc          : "
                                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                                 "%02x%02x:%02x%02x:%02x%02x",
                                 i, p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                 p[7], p[8], p[9], p[10], p[11], p[12], p[13],
                                 p[14], p[15]);
                        i++;
                    }
                });
        }
    }
}