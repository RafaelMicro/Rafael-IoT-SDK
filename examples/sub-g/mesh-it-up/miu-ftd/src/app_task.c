/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <FreeRTOS.h>
#include <miu_port.h>
#include <semphr.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "EnhancedFlashDataset.h"
#include "app_control_cmd.h"
#include "app_led.h"
#include "app_mac_raw.h"
#include "app_miu_config.h"
#include "app_net_mgm.h"
#include "app_task.h"
#include "app_udp.h"
#include "cli.h"
#include "hosal_gpio.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "miu_ext_mem.h"
#include "subg_ctrl.h"
#include "util_string.h"

#include <openthread/cli.h>
#include <openthread/dataset_ftd.h>
#include <openthread/ip6.h>
#include <openthread/random_noncrypto.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

#define PHY_PIB_TURNAROUND_TIMER  1000
#define PHY_PIB_CCA_DETECTED_TIME 640 // 8 symbols for 50 kbps-data rate
#define PHY_PIB_CCA_DETECT_MODE   0
#define PHY_PIB_CCA_THRESHOLD     65
#define MAC_PIB_UNIT_BACKOFF_PERIOD                                            \
    1640 // PHY_PIB_TURNAROUND_TIMER + PHY_PIB_CCA_DETECTED_TIME
#define MAC_PIB_MAC_ACK_WAIT_DURATION         2000  // oqpsk neee more then 2000
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 82080 // for 50 kbps-data rate
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       2
#define MAC_PIB_MAC_MAX_BE                    5
#define MAC_PIB_MAC_MIN_BE                    4
static uint16_t cca_duration_table[] = {0, 0, 0, 380, 700, 1140, 260, 480};
static uint32_t frame_total_wait_time_table[] = {0,      0,      0,     70000,
                                                 100000, 150080, 55000, 80000};
static const char* const data_rate_str[] = {
    "2M", "1M", "500K", "200K", "100K", "50K", "300K", "150K", "75K"};
static const char* const band_str[] = {"SubG_915M", "2P4G",      "SubG_868M",
                                       "SubG_433M", "SubG_315M", "SubG_470M"};
uint16_t cca_duration = 0;
uint16_t frame_total_wait_time = 0;
uint32_t backof_period = 0;
/*set subg frequencydatarate*/
#if CONFIG_SUBG_FREQUENCY_BAND_915
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
#elif CONFIG_SUBG_FREQUENCY_BAND_868
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_868M;
#elif CONFIG_SUBG_FREQUENCY_BAND_470
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_470M;
#elif CONFIG_SUBG_FREQUENCY_BAND_433
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_433M;
#else
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
#endif
// static uint8_t sPhyPowerIndex = 30;
/*set subg datarate*/
#if CONFIG_SUBG_DATA_RATE_FSK_300K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_300K;
#elif CONFIG_SUBG_DATA_RATE_FSK_200K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_200K;
#elif CONFIG_SUBG_DATA_RATE_FSK_100K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_100K;
#elif CONFIG_SUBG_DATA_RATE_FSK_50K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_50K;
#elif CONFIG_SUBG_DATA_RATE_OQPSK_25K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_25K;
#else
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_300K;
#endif

static SemaphoreHandle_t appSemHandle = NULL;

static QueueHandle_t appEventQueue;
static TimerHandle_t sProvisionTime = NULL;
static TimerHandle_t sJoinResponseTime = NULL;

static int _cli_cmd_miu_app(int argc, char** argv, cb_shell_out_t log_out,
                            void* pExtra);

typedef struct {
    char networkName[OT_NETWORK_NAME_MAX_SIZE + 1];
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE];
    uint8_t networkKey[OT_NETWORK_KEY_SIZE];
    uint8_t meshLocalPrefix[OT_MESH_LOCAL_PREFIX_SIZE];
    uint8_t pskc[OT_PSKC_MAX_SIZE];
    uint8_t channel;
    uint16_t panId;
} __attribute__((packed)) AppNetworkConfig;

#if 0 //debug used
void printOtChangedFlags(uint32_t flags) {
    if (flags & (1U << 0))
        log_info("OT_CHANGED_IP6_ADDRESS_ADDED");
    if (flags & (1U << 1))
        log_info("OT_CHANGED_IP6_ADDRESS_REMOVED");
    if (flags & (1U << 2))
        log_info("OT_CHANGED_THREAD_ROLE");
    if (flags & (1U << 3))
        log_info("OT_CHANGED_THREAD_LL_ADDR");
    if (flags & (1U << 4))
        log_info("OT_CHANGED_THREAD_ML_ADDR");
    if (flags & (1U << 5))
        log_info("OT_CHANGED_THREAD_RLOC_ADDED");
    if (flags & (1U << 6))
        log_info("OT_CHANGED_THREAD_RLOC_REMOVED");
    if (flags & (1U << 7))
        log_info("OT_CHANGED_THREAD_PARTITION_ID");
    if (flags & (1U << 8))
        log_info("OT_CHANGED_THREAD_KEY_SEQUENCE_COUNTER");
    if (flags & (1U << 9))
        log_info("OT_CHANGED_THREAD_NETDATA");
    if (flags & (1U << 10))
        log_info("OT_CHANGED_THREAD_CHILD_ADDED");
    if (flags & (1U << 11))
        log_info("OT_CHANGED_THREAD_CHILD_REMOVED");
    if (flags & (1U << 12))
        log_info("OT_CHANGED_IP6_MULTICAST_SUBSCRIBED");
    if (flags & (1U << 13))
        log_info("OT_CHANGED_IP6_MULTICAST_UNSUBSCRIBED");
    if (flags & (1U << 14))
        log_info("OT_CHANGED_THREAD_CHANNEL");
    if (flags & (1U << 15))
        log_info("OT_CHANGED_THREAD_PANID");
    if (flags & (1U << 16))
        log_info("OT_CHANGED_THREAD_NETWORK_NAME");
    if (flags & (1U << 17))
        log_info("OT_CHANGED_THREAD_EXT_PANID");
    if (flags & (1U << 18))
        log_info("OT_CHANGED_NETWORK_KEY");
    if (flags & (1U << 19))
        log_info("OT_CHANGED_PSKC");
    if (flags & (1U << 20))
        log_info("OT_CHANGED_SECURITY_POLICY");
    if (flags & (1U << 21))
        log_info("OT_CHANGED_CHANNEL_MANAGER_NEW_CHANNEL");
    if (flags & (1U << 22))
        log_info("OT_CHANGED_SUPPORTED_CHANNEL_MASK");
    if (flags & (1U << 23))
        log_info("OT_CHANGED_COMMISSIONER_STATE");
    if (flags & (1U << 24))
        log_info("OT_CHANGED_THREAD_NETIF_STATE");
    if (flags & (1U << 25))
        log_info("OT_CHANGED_THREAD_BACKBONE_ROUTER_STATE");
    if (flags & (1U << 26))
        log_info("OT_CHANGED_THREAD_BACKBONE_ROUTER_LOCAL");
    if (flags & (1U << 27))
        log_info("OT_CHANGED_JOINER_STATE");
    if (flags & (1U << 28))
        log_info("OT_CHANGED_ACTIVE_DATASET");
    if (flags & (1U << 29))
        log_info("OT_CHANGED_PENDING_DATASET");
    if (flags & (1U << 30))
        log_info("OT_CHANGED_NAT64_TRANSLATOR_STATE");
    if (flags & (1U << 31))
        log_info("OT_CHANGED_PARENT_LINK_QUALITY");
}
#endif

void app_evt_single(void* data, app_event_id_t id) {
    app_event_t evt;
    evt.data = data;
    evt.id = id;
    xQueueSendFromISR(appEventQueue, &evt, NULL);
}

static void ot_stateChangeCallback(uint32_t flags, void* p_context) {
    otInstance* instance = (otInstance*)p_context;
    uint8_t* p;
    // printOtChangedFlags(flags);
    if (flags & OT_CHANGED_THREAD_ROLE) {

        uint32_t role = otThreadGetDeviceRole(p_context);

        log_info("Current role       : %s",
                 otThreadDeviceRoleToString(otThreadGetDeviceRole(p_context)));
        if (role > OT_DEVICE_ROLE_DETACHED) {
            char string[OT_IP6_ADDRESS_STRING_SIZE];

            log_info("Rloc16             : %x", otThreadGetRloc16(instance));

            p = (uint8_t*)(otLinkGetExtendedAddress(instance)->m8);
            log_info("Extend Address     : %02x%02x%02x%02x%02x%02x%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            otIp6AddressToString(otThreadGetRloc(instance), string,
                                 sizeof(string));
            log_info("RLOC IPv6 Address  : %s", string);

            otIp6AddressToString(otThreadGetMeshLocalEid(instance), string,
                                 sizeof(string));
            log_info("Mesh IPv6 Address  : %s", string);

            otIp6AddressToString(otThreadGetLinkLocalIp6Address(instance),
                                 string, sizeof(string));
            log_info("local IPv6 Address : %s", string);

            app_evt_single(NULL, APP_EVENT_CHANGE_ROLE);
        }
    }
}

static void
ot_neighborChangeCallback(otNeighborTableEvent aEvent,
                          const otNeighborTableEntryInfo* aEntryInfo) {
    switch (aEvent) {
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
            log_info("child added        : %02x%02x%02x%02x%02x%02x%02x%02x",
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[0],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[1],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[2],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[3],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[4],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[5],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[6],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[7]);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
            log_info("Child removed      : %02x%02x%02x%02x%02x%02x%02x%02x",
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[0],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[1],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[2],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[3],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[4],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[5],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[6],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[7]);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED:
            log_info("Child changned     : %02x%02x%02x%02x%02x%02x%02x%02x",
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[0],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[1],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[2],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[3],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[4],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[5],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[6],
                     aEntryInfo->mInfo.mChild.mExtAddress.m8[7]);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
            log_info("Router added       : %02x%02x%02x%02x%02x%02x%02x%02x",
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[0],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[1],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[2],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[3],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[4],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[5],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[6],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[7]);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
            log_info("Router remove      : %02x%02x%02x%02x%02x%02x%02x%02x",
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[0],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[1],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[2],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[3],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[4],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[5],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[6],
                     aEntryInfo->mInfo.mRouter.mExtAddress.m8[7]);
            break;
    }

    // xSemaphoreGive(appSemHandle);
}

static void otdatasetInit(otInstance* instance) {
    otOperationalDatasetTlvs app_dataset_tlv;
    otOperationalDataset app_dataset;
    bool load_default_config = false;
    const char* const desired_network_name = "Rafael Miu";
    AppNetworkConfig netconfig = {
        .networkName = "Rafael Miu",
        .extPanId = {0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00},
        .networkKey = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                       0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
        .meshLocalPrefix = {0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00},
        .pskc = {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70, 0x61, 0x6b,
                 0x65, 0x74, 0x65, 0x73, 0x74, 0x00},
        .channel = 1,
        .panId = 0xabcd};

    if (otDatasetGetActiveTlvs(instance, &app_dataset_tlv) != OT_ERROR_NONE) {
        log_warn("APP Dataset flash is Null, setting default value.");
        load_default_config = true;
    } else {
        if (otDatasetParseTlvs(&app_dataset_tlv, &app_dataset)
            != OT_ERROR_NONE) {
            log_warn(
                "Failed to parse active Dataset TLVs, setting default value.");
            load_default_config = true;
        } else if (!app_dataset.mComponents.mIsNetworkNamePresent) {
            log_warn(
                "Active Dataset has no Network Name, setting default value.");
            load_default_config = true;
        } else if (strncmp((const char*)app_dataset.mNetworkName.m8,
                           desired_network_name, strlen(desired_network_name))
                   != 0) {
            log_warn("Active Dataset network name ('%s') does not match "
                     "desired ('%s'), setting default value.",
                     app_dataset.mNetworkName.m8, desired_network_name);
            load_default_config = true;
        }
    }

    if (load_default_config) {
        /* Create new dataset */
        otDatasetCreateNewNetwork(instance, &app_dataset);
        /* Set activetimestamp */
        app_dataset.mActiveTimestamp.mSeconds = 1;
        app_dataset.mActiveTimestamp.mTicks = 0;
        app_dataset.mActiveTimestamp.mAuthoritative = false;
        app_dataset.mComponents.mIsActiveTimestampPresent = true;

        /* Set Channel */
        app_dataset.mChannel = netconfig.channel;
        app_dataset.mComponents.mIsChannelPresent = true;

        /* Set Pan ID */
        app_dataset.mPanId = (otPanId)netconfig.panId;
        app_dataset.mComponents.mIsPanIdPresent = true;

        /* Set Wake-up Channel */
        app_dataset.mWakeupChannel = netconfig.channel;
        app_dataset.mComponents.mIsWakeupChannelPresent = true;

        /* Set Extended Pan ID */
        memcpy(app_dataset.mExtendedPanId.m8, netconfig.extPanId,
               OT_EXT_PAN_ID_SIZE);
        app_dataset.mComponents.mIsExtendedPanIdPresent = true;

        /* Set network key */
        memcpy(app_dataset.mNetworkKey.m8, netconfig.networkKey,
               OT_NETWORK_KEY_SIZE);
        app_dataset.mComponents.mIsNetworkKeyPresent = true;

        /* Set pskc */
        memcpy(app_dataset.mPskc.m8, netconfig.pskc, OT_PSKC_MAX_SIZE);
        app_dataset.mComponents.mIsPskcPresent = true;

        /* Set Network Name */
        size_t length = strlen(netconfig.networkName);
        memcpy(app_dataset.mNetworkName.m8, netconfig.networkName, length);
        app_dataset.mComponents.mIsNetworkNamePresent = true;

        memcpy(app_dataset.mMeshLocalPrefix.m8, netconfig.meshLocalPrefix,
               OT_MESH_LOCAL_PREFIX_SIZE);
        app_dataset.mComponents.mIsMeshLocalPrefixPresent = true;

        otDatasetUpdateTlvs(&app_dataset, &app_dataset_tlv);

        otDatasetSetActiveTlvs(instance, &app_dataset_tlv);
    }

    /* set extaddr to equal eui64*/
    otExtAddress extAddress;
    otLinkGetFactoryAssignedIeeeEui64(instance, &extAddress);
    otLinkSetExtendedAddress(instance, &extAddress);

    /* set mle eid to equal eui64*/
    otIp6InterfaceIdentifier iid;
    memcpy(iid.mFields.m8, extAddress.m8, OT_EXT_ADDRESS_SIZE);
    otIp6SetMeshLocalIid(instance, &iid);

    /*set device mode type*/
    otLinkModeConfig mode;
    mode.mDeviceType = 1;
    mode.mRxOnWhenIdle = 1;
    mode.mNetworkData = 1;
    otThreadSetLinkMode(instance, mode);

    log_info("Active Timestamp   : %lld",
             (unsigned long long)app_dataset.mActiveTimestamp.mSeconds);
    log_info("Channel            : %d", app_dataset.mChannel);
    // log_info("Wake-up Channel    : %d", app_dataset.mWakeupChannel);
    log_info("Ext PAN ID         : %02x%02x%02x%02x%02x%02x%02x%02x",
             app_dataset.mExtendedPanId.m8[0], app_dataset.mExtendedPanId.m8[1],
             app_dataset.mExtendedPanId.m8[2], app_dataset.mExtendedPanId.m8[3],
             app_dataset.mExtendedPanId.m8[4], app_dataset.mExtendedPanId.m8[5],
             app_dataset.mExtendedPanId.m8[6],
             app_dataset.mExtendedPanId.m8[7]);
    log_info(
        "Mesh Local Prefix  : %02x%02x:%02x%02x:%02x%02x:%02x%02x::/64",
        app_dataset.mMeshLocalPrefix.m8[0], app_dataset.mMeshLocalPrefix.m8[1],
        app_dataset.mMeshLocalPrefix.m8[2], app_dataset.mMeshLocalPrefix.m8[3],
        app_dataset.mMeshLocalPrefix.m8[4], app_dataset.mMeshLocalPrefix.m8[5],
        app_dataset.mMeshLocalPrefix.m8[6], app_dataset.mMeshLocalPrefix.m8[7]);
    log_info("Network Key        : "
             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             app_dataset.mNetworkKey.m8[0], app_dataset.mNetworkKey.m8[1],
             app_dataset.mNetworkKey.m8[2], app_dataset.mNetworkKey.m8[3],
             app_dataset.mNetworkKey.m8[4], app_dataset.mNetworkKey.m8[5],
             app_dataset.mNetworkKey.m8[6], app_dataset.mNetworkKey.m8[7],
             app_dataset.mNetworkKey.m8[8], app_dataset.mNetworkKey.m8[9],
             app_dataset.mNetworkKey.m8[10], app_dataset.mNetworkKey.m8[11],
             app_dataset.mNetworkKey.m8[12], app_dataset.mNetworkKey.m8[13],
             app_dataset.mNetworkKey.m8[14], app_dataset.mNetworkKey.m8[15]);

    log_info("Network Name       : %s", app_dataset.mNetworkName.m8);
    log_info("Link Mode          : %d, %d, %d",
             otThreadGetLinkMode(instance).mRxOnWhenIdle,
             otThreadGetLinkMode(instance).mDeviceType,
             otThreadGetLinkMode(instance).mNetworkData);
    log_info("PAN ID             : 0x%04x", app_dataset.mPanId);
    log_info("Extaddr            : %02x%02x%02x%02x%02x%02x%02x%02x",
             extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
             extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
             extAddress.m8[6], extAddress.m8[7]);
}

void otrInitUser(otInstance* instance) {
    otdatasetInit(instance);
    otAppCliInit(instance);
    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);
    app_sockInit(instance, CONFIG_APP_TASK_UDP_LISTEN_PORT);
    app_macRawInit(instance);
    /*led pin init*/
    app_led_pin_init();
#if CONFIG_APP_TASK_CONTROL_CMD_ENABLE
    app_control_cmd_init();
#endif
    /*auto start networking*/
    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);
#if CONFIG_APP_TASK_CENTRAL_ENABLE
    net_mgm_init(instance);
#endif
}

uint16_t app_provision_get_remain_time() {
    uint16_t ret_time = 0;
    TickType_t xRemainingTime;
    if (sProvisionTime && xTimerIsTimerActive(sProvisionTime) == pdTRUE) {
        xRemainingTime = xTimerGetExpiryTime(sProvisionTime)
                         - xTaskGetTickCount();
        ret_time = (uint16_t)(xRemainingTime / 1000);
    }
    return ret_time;
}

void app_provision_timeout_callback(TimerHandle_t xTimer) {
    log_info("provisioning timeout ");
    xTimerDelete(sProvisionTime, 0);
    sProvisionTime = NULL;
}

void app_set_provisioning_mode(bool enable, uint32_t provision_time) {
    otInstance* instance = otrGetInstance();
    otError error = OT_ERROR_NONE;
    if (!instance)
        return; // Instance not ready
    uint32_t role = otThreadGetDeviceRole(instance);
    if (role != OT_DEVICE_ROLE_ROUTER && role != OT_DEVICE_ROLE_LEADER) {
        log_info("Not in a state %d to start/stop provisioning", role);
        return;
    }
    log_info("provisioning mode %u (%u s) \r\n", enable, provision_time);
    if (enable) {
        if (sProvisionTime == NULL) {
            sProvisionTime = xTimerCreate(
                "sProvisionTime", pdMS_TO_TICKS((provision_time * 1000)),
                pdFALSE, NULL,
                (TimerCallbackFunction_t)app_provision_timeout_callback);
        }
        if (xTimerIsTimerActive(sProvisionTime) == pdTRUE) {
            xTimerChangePeriod(sProvisionTime,
                               pdMS_TO_TICKS((provision_time * 1000)), 0);
        } else {
            xTimerStart(sProvisionTime, 0);
        }
    } else {
        if (sProvisionTime) {
            xTimerDelete(sProvisionTime, 0);
            sProvisionTime = NULL;
        }
    }
}

void join_response_send() {
    net_join_response_t join_response;
    uint8_t dstAddr[8] = {0xff, 0xff, 0xff, 0xff,
                          0xff, 0xff, 0xff, 0xff}; // broadcast
    otInstance* instance = otrGetInstance();
    otNetworkKey networkKey;
    otPanId panid = otLinkGetPanId(instance);
    otThreadGetNetworkKey(instance, &networkKey);
    join_response.panid = panid;
    memcpy(&join_response.netkey, networkKey.m8, 16);

    app_ctrl_send_cmd(dstAddr, CMD_ID_NETWORK_JOIN_RESPONSE, FLAG_MAC,
                      (uint8_t*)&join_response, sizeof(join_response), 0xffff,
                      otLinkGetChannel(instance));
    log_info("[Network] >> Join Response");
}

void app_join_response_timeout_callback(TimerHandle_t xTimer) {
    app_evt_single(NULL, APP_EVENT_MAC_JOIN_RESPONSE_SEND);
    xTimerDelete(sJoinResponseTime, 0);
    sJoinResponseTime = NULL;
}

void app_join_request_handler() {
    if (sProvisionTime != NULL
        && xTimerIsTimerActive(sProvisionTime) == pdTRUE) {
        int random_delay = otRandomNonCryptoGetUint16InRange(50, 1000);
        if (sJoinResponseTime == NULL) {
            log_info("Join response starts at %u ms after", random_delay);
            sJoinResponseTime = xTimerCreate(
                "sJoinResponseTime", pdMS_TO_TICKS(random_delay), pdTRUE, NULL,
                (TimerCallbackFunction_t)app_join_response_timeout_callback);
            xTimerStart(sJoinResponseTime, 0);
        } else {
            log_info("Join response sending");
        }
    }
}

void app_task(void) {
    app_event_t evt;
    appEventQueue = xQueueCreate(10, sizeof(app_event_t));

    while (true) {
        if (xQueueReceive(appEventQueue, &evt, portMAX_DELAY)) {
            OT_THREAD_SAFE(
                otInstance* instance = otrGetInstance(); if (instance) {
                    switch (evt.id) {
                        case APP_EVENT_CHANGE_ROLE:
#if CONFIG_APP_TASK_CENTRAL_ENABLE
                            uint32_t role = otThreadGetDeviceRole(instance);
                            if (net_mgm_check_leader_pin_state() == true) {
                                if (role == OT_DEVICE_ROLE_ROUTER
                                    || role == OT_DEVICE_ROLE_CHILD) {
                                    otInstanceReset(instance);
                                }
                            } else {
                                if (role == OT_DEVICE_ROLE_LEADER) {
                                    log_info("Can't become leader, resetting");
                                    otThreadSetEnabled(instance, false);
                                    otInstanceErasePersistentInfo(instance);
                                    otThreadSetEnabled(instance, true);
                                } else {
                                    net_mgm_enroll_req_send(instance);
                                    if (role == OT_DEVICE_ROLE_ROUTER) {
                                        enroll_update_timeout = 30;
                                    } else {
                                        enroll_update_timeout = 0;
                                    }
                                    enroll_send_time = 0;
                                    enroll_send_try = 0;
                                    enroll_done = false;
                                }
                            }
#endif
                            break;
                        case APP_EVENT_MAC_JOIN_RESPONSE_SEND:
                            join_response_send();
                            break;
#if CONFIG_APP_TASK_CENTRAL_ENABLE
                        case APP_EVENT_NET_MGM_ENROLL_REQ_SEND:
                            net_mgm_enroll_req_send(instance);
                            break;
                        case APP_EVENT_NET_MGM_ENROLL_UPDATE_SEND:
                            net_mgm_enroll_update_send(instance);
                            break;
                        case APP_EVENT_NET_MGM_ENROLL_REQ_SEND_TRYOUT:
                            otThreadSetEnabled(instance, false);
                            otInstanceErasePersistentInfo(instance);
                            otThreadSetEnabled(instance, true);
                            break;
#endif
                        default: break;
                    }
                })
        }
    }
}

void app_common_init() {

    /*phy init*/
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Mesh It Up FTD");
    log_info("Band               : %s", band_str[sPhyFrequencyBand]);
    log_info("Data Rate          : %s", data_rate_str[sPhyDataRate]);

    /*subg phy parameter setting*/
    cca_duration = cca_duration_table[sPhyDataRate];
    frame_total_wait_time = frame_total_wait_time_table[sPhyDataRate];
    backof_period = PHY_PIB_TURNAROUND_TIMER + cca_duration;

#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    lmac15p4_init(LMAC15P4_SUBG_OQPSK, sPhyFrequencyBand);
#else
    lmac15p4_init(LMAC15P4_SUBG_FSK, sPhyFrequencyBand);
#endif

    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, cca_duration);

    lmac15p4_mac_pib_set(backof_period, MAC_PIB_MAC_ACK_WAIT_DURATION,
                         MAC_PIB_MAC_MAX_BE, MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         frame_total_wait_time, MAC_PIB_MAC_MAX_FRAME_RETRIES,
                         MAC_PIB_MAC_MIN_BE);

    subg_ctrl_sleep_set(false);
    subg_ctrl_idle_set();
#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    subg_ctrl_modem_config_set(LMAC15P4_SUBG_OQPSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(LMAC15P4_SUBG_OQPSK, 8);
    subg_ctrl_sfd_set(LMAC15P4_SUBG_OQPSK, 0x00007209);
    subg_ctrl_filter_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#else
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);
    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);
    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#endif
    /*subg frequency range setting*/
    log_info("Channel Range     : %d ~ %d",
             OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN,
             OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX);
    log_info("Channel Frequency : %d MHz", OPENTHREAD_CONFIG_CHANNEL_FREQUENCY);
    log_info("Channel Spacing   : %d MHz", OPENTHREAD_CONFIG_CHANNEL_SPACING);
    otRadioChRange_t radiochrange;
    radiochrange.minChannel =
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN;
    radiochrange.maxChannel =
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX;
    radiochrange.frequencyBase = OPENTHREAD_CONFIG_CHANNEL_FREQUENCY;
    radiochrange.frequencySpacing = OPENTHREAD_CONFIG_CHANNEL_SPACING;
    otPlatRadioSetChannelRange(radiochrange);

    /*mesh it up task start*/
    miuStart();
}

static void print_help(cb_shell_out_t log_out) {
    log_out("app udp send <ipv6> -x <hex data> ");
    log_out("app udp send <ipv6> -c <string data> ");
    log_out("app udp port ");
    log_out("app led <on/off/toggle/flash> ");
}

static int handle_udp_send(int argc, char** argv, cb_shell_out_t log_out) {
    if (argc < 6) {
        log_out("Too few parameters \r\n");
        return -1;
    }

    otIp6Address dst_addr;
    if (otIp6AddressFromString(argv[3], &dst_addr) != OT_ERROR_NONE) {
        log_out("Invalid IPv6 address \r\n");
        return -1;
    }

    uint8_t* data = NULL;
    uint16_t data_lens = 0;

    if (!strncmp(argv[4], "-x", 2)) {
        // Send hex data
        data_lens = (strlen(argv[5]) + 1) / 2;
        data = xMalloc(data_lens);
        if (!data)
            return -1;

        for (uint16_t i = 0; i < data_lens; i++) {
            data[i] = (utility_strtox(argv[5] + i * 2, 0, 2) & 0xFF);
        }
    } else if (!strncmp(argv[4], "-c", 2)) {
        // Send string data
        for (uint8_t i = 5; i < argc; i++) {
            data_lens += strlen(argv[i]) + 1;
        }

        data = xMalloc(data_lens);
        if (!data) {
            log_info("handle_udp_send malloc fail");
            return -1;
        }

        uint16_t offset = 0;
        for (uint8_t i = 5; i < argc; i++) {
            size_t len = strlen(argv[i]);
            memcpy(&data[offset], argv[i], len);
            offset += len;
            data[offset++] = 0x20;
        }

        if (offset > 0)
            offset--; // remove last space
        data_lens = offset;
    } else {
        log_out("Unknown send format. Use -x or -c. \r\n");
        return -1;
    }

    app_udpSend(dst_addr, data, data_lens, false);
    if (data)
        xFree(data);
    return 0;
}

static int handle_led_command(int argc, char** argv, cb_shell_out_t log_out) {
    if (argc < 3) {
        log_out("Too few parameters \r\n");
        return -1;
    }

    if (!strncmp(argv[2], "on", 2)) {
        app_set_led0_on();
    } else if (!strncmp(argv[2], "off", 3)) {
        app_set_led0_off();
    } else if (!strncmp(argv[2], "toggle", 6)) {
        app_set_led0_toggle();
    } else if (!strncmp(argv[2], "flash", 5)) {
        app_set_led0_flash();
    } else {
        log_out("Unknown LED command \r\n");
        return -1;
    }

    return 0;
}

#if CONFIG_APP_TASK_CONTROL_CMD_ENABLE
static int handle_ctrl_command(int argc, char** argv, cb_shell_out_t log_out) {
    if (argc < 4) {
        log_out("Too few parameters \r\n");
        return -1;
    }

    otIp6Address dst_addr;
    if (otIp6AddressFromString(argv[2], &dst_addr) != OT_ERROR_NONE) {
        log_out("Invalid IPv6 address \r\n");
        return -1;
    }

    uint8_t cmd = (utility_strtox(argv[3], 0, 2) & 0xFF);
    uint8_t* data = NULL;
    uint16_t data_lens = 0;
    data_lens = (strlen(argv[4]) + 1) / 2;
    data = xMalloc(data_lens);
    if (!data)
        return -1;

    for (uint16_t i = 0; i < data_lens; i++) {
        data[i] = (utility_strtox(argv[4] + i * 2, 0, 2) & 0xFF);
    }

    if (app_ctrl_send_cmd((uint8_t*)&dst_addr, cmd, FLAG_UDP, data, data_lens,
                          0, 0)) {
        log_info("app_ctrl_send_cmd fail");
    }

    if (data)
        xFree(data);

    return 0;
}
#endif

static int _cli_cmd_miu_app(int argc, char** argv, cb_shell_out_t log_out,
                            void* pExtra) {
    int ret = -1;

    if (argc < 2) {
        log_out("Too few parameters \r\n");
        return ret;
    }

    if (!strncmp(argv[1], "help", 4)) {
        print_help(log_out);
        return 0;
    }

    if (!strncmp(argv[1], "udp", 3)) {
        if (argc < 3) {
            log_out("Too few parameters \r\n");
            return -1;
        }

        if (!strncmp(argv[2], "send", 4)) {
            ret = handle_udp_send(argc, argv, log_out);
        } else if (!strncmp(argv[2], "port", 4)) {
            log_out("app udp port: %d \r\n", CONFIG_APP_TASK_UDP_LISTEN_PORT);
            ret = 0;
        } else {
            log_out("Unknown udp subcommand \r\n");
        }
    } else if (!strncmp(argv[1], "led", 3)) {
        ret = handle_led_command(argc, argv, log_out);
    } else if (!strncmp(argv[1], "provisioner", 11)) {
        if (argc < 3) {
            log_out("Too few parameters \r\n");
            return -1;
        }
        if (!strncmp(argv[2], "start", 5)) {
            uint32_t provision_time = 120; // default 120 seconds
            if (argc >= 4) {
                provision_time = utility_strtol(argv[3], 0);
            }
            /*start self provisioning*/
            app_set_provisioning_mode(true, provision_time);
            /*broadcast provisioning set*/
            otIp6Address multicast_addr;
            char multicast_ip[] = "ff03::2";
            otIp6AddressFromString((uint8_t*)&multicast_ip, &multicast_addr);
            if (app_ctrl_send_cmd((uint8_t*)&multicast_addr,
                                  CMD_ID_NETWORK_PROVISIONING_SET, FLAG_UDP,
                                  (uint8_t*)&provision_time, sizeof(uint32_t),
                                  0, 0)) {
                log_info("app_ctrl_send_cmd fail");
            }
        } else if (!strncmp(argv[2], "stop", 4)) {
            uint32_t provision_time = 0;
            /*stop self provisioning*/
            app_set_provisioning_mode(false, provision_time);
            /*broadcast provisioning set*/
            otIp6Address multicast_addr;
            char multicast_ip[] = "ff03::2";
            otIp6AddressFromString((uint8_t*)&multicast_ip, &multicast_addr);
            if (app_ctrl_send_cmd((uint8_t*)&multicast_addr,
                                  CMD_ID_NETWORK_PROVISIONING_SET, FLAG_UDP,
                                  (uint8_t*)&provision_time, sizeof(uint32_t),
                                  0, 0)) {
                log_info("app_ctrl_send_cmd fail");
            }
        } else {
            log_info("provision remain timer %u ",
                     app_provision_get_remain_time());
        }
        ret = 0;
    }
#if CONFIG_APP_TASK_CONTROL_CMD_ENABLE
    else if (!strncmp(argv[1], "ctrl", 4)) {
        ret = handle_ctrl_command(argc, argv, log_out);
    }
#endif
#if CONFIG_APP_TASK_CENTRAL_ENABLE
    else if (!strncmp(argv[1], "node", 4)) {
        if (!strncmp(argv[2], "list", 4)) {
            net_mgm_node_table_display();
        } else if (!strncmp(argv[2], "num", 3)) {
            net_mgm_node_table_num();
        } else {
            log_out("unknown node subcommand \r\n");
            return -1;
        }
        ret = 0;
    }
#endif
    else if (!strncmp(argv[1], "mem", 3)) {
        extMemory();
        ret = 0;
    } else {
        log_out("Unknown command  \r\n");
    }

    if (ret == 0) {
        log_out("+Ok \r\n");
    }

    return ret;
}

const sh_cmd_t g_cli_cmd_miu_app STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "app",
    .pDescription = "Miu APP Command : see app help",
    .cmd_exec = _cli_cmd_miu_app,
};