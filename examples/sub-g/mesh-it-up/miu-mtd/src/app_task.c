/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "app_led.h"
#include "miu_port.h"
#include "app_mac_raw.h"
#include "app_miu_config.h"
#include "app_task.h"
#include "app_udp.h"
#include "cli.h"
#include "hosal_lpm.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "subg_ctrl.h"
#include "util_string.h"
#if CONFIG_APP_TASK_OTA_ENABLE
#include "app_ota.h"
#endif

#include <openthread/logging.h>
#include <openthread/random_noncrypto.h>

#if defined(CONFIG_RF1301)
#define BIN_TYPE_ARR 'm', 'i', 'u', '-', 'm', 't', 'd', '-', '1', '3', '0', '1'
#elif defined(CONFIG_RT581) || defined(CONFIG_RT582)
#define BIN_TYPE_ARR 'm', 'i', 'u', '-', 'm', 't', 'd', '-', 'r', '5', '8', '1'
#else
#error "Unknown IC type, please define BIN_TYPE_ARR"
#endif
const sys_information_t systeminfo = SYSTEMINFO_INIT(BIN_TYPE_ARR);

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
#define MAC_PIB_MAC_MAX_BE                    3
#define MAC_PIB_MAC_MIN_BE                    2
static uint16_t cca_duration_table[] = {0, 0, 0, 380, 700, 1140, 260, 480};
static uint32_t frame_total_wait_time_table[] = {0,      0,      0,     70000,
                                                 100000, 150080, 55000, 80000};
static const char* const data_rate_str[] = {
    "2M", "1M", "500K", "200K", "100K", "50K", "300K", "150K", "75K"};
static const char* const band_str[] = {"SubG_915M", "2P4G",      "SubG_868M",
                                       "SubG_433M", "SubG_315M", "SubG_470M"};
uint16_t cca_duration = 0;
uint32_t frame_total_wait_time = 0;
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

typedef struct {
    char networkName[OT_NETWORK_NAME_MAX_SIZE + 1];
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE];
    uint8_t networkKey[OT_NETWORK_KEY_SIZE];
    uint8_t meshLocalPrefix[OT_MESH_LOCAL_PREFIX_SIZE];
    uint8_t pskc[OT_PSKC_MAX_SIZE];
    uint8_t channel;
    uint16_t panId;
} __attribute__((packed)) AppNetworkConfig;

static void ot_stateChangeCallback(uint32_t flags, void* p_context) {
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
        }
    }
}

static void otsleepInit(otInstance* instance) {
    /*set device mode type*/
    otLinkModeConfig mode;
    mode.mDeviceType = false;
    mode.mNetworkData = false;
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    mode.mRxOnWhenIdle = false;
    //sed
    otLinkSetPollPeriod(
        instance, CONFIG_APP_TASK_SLEEP_POLL_PERIOD); // 1s sleep poll period
#else
    mode.mRxOnWhenIdle = true;
#endif
    otThreadSetLinkMode(instance, mode);
    /* low power mode init */

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE,
                     LOW_POWER_WAKEUP_SLOW_TIMER);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, LOW_POWER_WAKEUP_UART_RX);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
}

static void otnetworkinfo(otInstance* instance) {
    log_info("Channel            : %d", otLinkGetChannel(instance));
    log_info("Ext PAN ID         : %02x%02x%02x%02x%02x%02x%02x%02x",
             otThreadGetExtendedPanId(instance)->m8[0],
             otThreadGetExtendedPanId(instance)->m8[1],
             otThreadGetExtendedPanId(instance)->m8[2],
             otThreadGetExtendedPanId(instance)->m8[3],
             otThreadGetExtendedPanId(instance)->m8[4],
             otThreadGetExtendedPanId(instance)->m8[5],
             otThreadGetExtendedPanId(instance)->m8[6],
             otThreadGetExtendedPanId(instance)->m8[7]);
    log_info("Mesh Local Prefix  : %02x%02x:%02x%02x:%02x%02x:%02x%02x::/64",
             otThreadGetMeshLocalPrefix(instance)->m8[0],
             otThreadGetMeshLocalPrefix(instance)->m8[1],
             otThreadGetMeshLocalPrefix(instance)->m8[2],
             otThreadGetMeshLocalPrefix(instance)->m8[3],
             otThreadGetMeshLocalPrefix(instance)->m8[4],
             otThreadGetMeshLocalPrefix(instance)->m8[5],
             otThreadGetMeshLocalPrefix(instance)->m8[6],
             otThreadGetMeshLocalPrefix(instance)->m8[7]);
    otNetworkKey netKey;
    otThreadGetNetworkKey(instance, &netKey);
    log_info("Network Key        : "
             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             netKey.m8[0], netKey.m8[1], netKey.m8[2], netKey.m8[3],
             netKey.m8[4], netKey.m8[5], netKey.m8[6], netKey.m8[7],
             netKey.m8[8], netKey.m8[9], netKey.m8[10], netKey.m8[11],
             netKey.m8[12], netKey.m8[13], netKey.m8[14], netKey.m8[15]);

    log_info("Network Name       : %s", otThreadGetNetworkName(instance));
    log_info("Link Mode          : %d, %d, %d",
             otThreadGetLinkMode(instance).mRxOnWhenIdle,
             otThreadGetLinkMode(instance).mDeviceType,
             otThreadGetLinkMode(instance).mNetworkData);
    log_info("PAN ID             : 0x%04x", otLinkGetPanId(instance));
    log_info("Extaddr            : %02x%02x%02x%02x%02x%02x%02x%02x",
             otLinkGetExtendedAddress(instance)->m8[0],
             otLinkGetExtendedAddress(instance)->m8[1],
             otLinkGetExtendedAddress(instance)->m8[2],
             otLinkGetExtendedAddress(instance)->m8[3],
             otLinkGetExtendedAddress(instance)->m8[4],
             otLinkGetExtendedAddress(instance)->m8[5],
             otLinkGetExtendedAddress(instance)->m8[6],
             otLinkGetExtendedAddress(instance)->m8[7]);
}

static void otdatasetInit(otInstance* instance) {
    otOperationalDatasetTlvs app_dataset_tlv;
    otOperationalDataset app_dataset;
    bool load_default_config = false;
    const char* const desired_network_name = "Rafael Miu";
#if CONFIG_TEST_TOOL_USE
    /* Fixed test-only dataset — must NOT match production default. */
    AppNetworkConfig netconfig = {
        .networkName = "RafaelMiuTest",
        .extPanId = {0x54, 0x45, 0x53, 0x54, 0x00, 0x00, 0x00, 0x01},
        .networkKey = {0xFA, 0xCE, 0xB0, 0x0C, 0xDE, 0xAD, 0xBE, 0xEF, 0x01,
                       0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
        .meshLocalPrefix = {0xfd, 0x00, 0x54, 0x45, 0x53, 0x54, 0x00, 0x00},
        .pskc = {0x54, 0x45, 0x53, 0x54, 0x74, 0x6f, 0x6f, 0x6c, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        .channel = 2,
        .panId = 0x5A5A};
#else
    AppNetworkConfig netconfig = {
        .networkName = "Rafael Miu",
        .extPanId = {0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00},
        .networkKey = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
        .meshLocalPrefix = {0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00},
        .pskc = {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70, 0x61, 0x6b,
                 0x65, 0x74, 0x65, 0x73, 0x74, 0x00},
        .channel = 2,
        .panId = 0xabcd};
#endif

#if CONFIG_TEST_TOOL_USE
    /* Test tool mode: always use the fixed dataset above, ignore flash.
     * Guarantees all FTD/MTD share the same key/channel/panid on every boot. */
    load_default_config = true;
    (void)desired_network_name;
#else
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
#endif

    /* set extaddr to equal eui64*/
    otExtAddress extAddress;
    otLinkGetFactoryAssignedIeeeEui64(instance, &extAddress);
    otLinkSetExtendedAddress(instance, &extAddress);

    /* set mle eid to equal eui64*/
    otIp6InterfaceIdentifier iid;
    memcpy(iid.mFields.m8, extAddress.m8, OT_EXT_ADDRESS_SIZE);
    otIp6SetMeshLocalIid(instance, &iid);

    if (load_default_config) {
        memset(&app_dataset, 0, sizeof(app_dataset));
        memset(&app_dataset_tlv, 0, sizeof(app_dataset_tlv));
        /* Set activetimestamp */
        app_dataset.mActiveTimestamp.mSeconds = 0;
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

        if (otDatasetSetActiveTlvs(instance, &app_dataset_tlv)
            != OT_ERROR_NONE) {
            log_info("Failed to set Active Dataset TLVs");
        }
    }

    log_info("Active Timestamp   : %lld",
             (unsigned long long)app_dataset.mActiveTimestamp.mSeconds);
    otnetworkinfo(instance);
}

#if CONFIG_APP_TASK_OTA_ENABLE
void ota_state_change_cb(uint8_t state) {
    switch (state) {
        case OTA_IDLE: log_info("change to ota idle state "); break;
        case OTA_DATA_SENDING: log_info("change to ota sending state "); break;
        case OTA_DATA_RECEIVING:
            log_info("change to ota receiving state");
            break;
        case OTA_UNICAST_RECEIVING:
            log_info("change to ota unicase receiving state");
            break;
        case OTA_REQUEST_SENDING:
            log_info("change to ota request sending state");
            break;
        case OTA_DONE: log_info("change to ota done state"); break;
        case OTA_REBOOT: log_info("change to ota reboot state"); break;
        default: break;
    }
}
#endif

void otrInitUser(otInstance* instance) {

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    otLoggingSetLevel(OT_LOG_LEVEL_WARN);
#endif
    otAppCliInit(instance);

    otsleepInit(instance);

    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
    app_sockInit(instance, CONFIG_APP_TASK_UDP_LISTEN_PORT);
    app_macRawInit(instance);

    /*led pin init*/
    app_led_pin_init();

#if CONFIG_APP_TASK_OTA_ENABLE
    ota_bootloader_info_check();
    ota_init(instance, ota_state_change_cb);
#endif

    otdatasetInit(instance);

    otIp6SetEnabled(instance, true);

    otThreadSetEnabled(instance, true);
}

void app_task(void) {
    while (true) {
        otInstance* instance = NULL;
        /* Always wrap OpenThread API calls in OT_THREAD_SAFE to prevent race conditions with the radio task. */
        OT_THREAD_SAFE(instance = otrGetInstance();)
        if (instance) {
            /*start ...*/
        }
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
}

void app_common_init() {

    /*phy init*/
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Mesh It Up MTD");
    log_info("Band               : %s", band_str[sPhyFrequencyBand]);
    log_info("Data Rate          : %s", data_rate_str[sPhyDataRate]);
    log_info(
        "bin version        : %s "
        "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        systeminfo.prefix, systeminfo.sysinfo[0], systeminfo.sysinfo[1],
        systeminfo.sysinfo[2], systeminfo.sysinfo[3], systeminfo.sysinfo[4],
        systeminfo.sysinfo[5], systeminfo.sysinfo[6], systeminfo.sysinfo[7],
        systeminfo.sysinfo[8], systeminfo.sysinfo[9], systeminfo.sysinfo[10],
        systeminfo.sysinfo[11], systeminfo.sysinfo[12], systeminfo.sysinfo[13],
        systeminfo.sysinfo[14], systeminfo.sysinfo[15]);
    log_info("miu_port lib ver   : %s", miu_port_lib_version());

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
    log_info("Channel Frequency : %d ", OPENTHREAD_CONFIG_CHANNEL_FREQUENCY);
    log_info("Channel Spacing   : %d ", OPENTHREAD_CONFIG_CHANNEL_SPACING);
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