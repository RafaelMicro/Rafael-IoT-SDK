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
#include "app_control_cmd.h"
#include "app_led.h"
#include "app_mac_raw.h"
#include "app_miu_config.h"
#include "app_net_mgm.h"
#include "app_task.h"
#include "app_udp.h"
#include "cli.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "subg_ctrl.h"
#include "util_string.h"

#include <openthread/logging.h>
#include <openthread/random_noncrypto.h>

#define BIN_TYPE_ARR 'm', 'i', 'u', '-', 'm', 't', 'd', 't', 'y', 'p', 'e', '-'
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

#define CONFIG_APP_JOINER_BACKOFF_MAX      30000
#define CONFIG_APP_JOINER_BACKOFF_INTERVAL 5000

static uint8_t g_app_joiner_attempt = 0;

static SemaphoreHandle_t appSemHandle = NULL;
static TimerHandle_t sJoinRetryTimer = NULL;

static QueueHandle_t appEventQueue;

typedef struct {
    char networkName[OT_NETWORK_NAME_MAX_SIZE + 1];
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE];
    uint8_t networkKey[OT_NETWORK_KEY_SIZE];
    uint8_t meshLocalPrefix[OT_MESH_LOCAL_PREFIX_SIZE];
    uint8_t pskc[OT_PSKC_MAX_SIZE];
    uint8_t channel;
    uint16_t panId;
} __attribute__((packed)) AppNetworkConfig;

void app_evt_single(void* data, app_event_id_t id) {
    app_event_t evt;
    evt.data = data;
    evt.id = id;
    xQueueSendFromISR(appEventQueue, &evt, NULL);
}

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
            app_evt_single(NULL, APP_EVENT_CHANGE_ROLE);
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
}

uint8_t join_start_channel = 0;
static uint8_t next_channel = 0;

void app_join_send(otInstance* instance) {
    uint8_t channel_min = 0;
    uint8_t channel_max = 0;
    uint8_t dstAddr[8] = {0xff, 0xff, 0xff, 0xff,
                          0xff, 0xff, 0xff, 0xff}; // broadcast
    otRadioChRange_t range;
    otPlatRadioGetChannelRange(&range);
    otLinkSetChannel(instance, next_channel);
    app_ctrl_send_cmd(dstAddr, CMD_ID_NETWORK_JOIN_REQUEST, FLAG_MAC, NULL, 0,
                      0xffff, otLinkGetChannel(instance));
    log_info("[Network] >> Join Request (channel: %d)",
             otLinkGetChannel(instance));
    next_channel++;
    if (next_channel > range.maxChannel) {
        next_channel -= range.maxChannel;
    }
    if (next_channel == join_start_channel) {
        otLinkSetChannel(instance, join_start_channel);
        app_evt_single(NULL, APP_EVENT_JOIN_FAILED);
    } else {
        xTimerStart(sJoinRetryTimer, 0);
    }
}

void appJoinRetryTimerCallback(TimerHandle_t xTimer) {
    app_evt_single(NULL, APP_EVENT_RETRY_JOIN_NOW);
}

void app_start_join(otInstance* instance) {
    join_start_channel = otLinkGetChannel(instance);
    next_channel = join_start_channel;
    if (sJoinRetryTimer == NULL) {
        sJoinRetryTimer = xTimerCreate(
            "sJoinRetryTimer", pdMS_TO_TICKS(1500), pdFALSE, NULL,
            (TimerCallbackFunction_t)appJoinRetryTimerCallback);
    }
    xTimerStart(sJoinRetryTimer, 0);
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

    /* set extaddr to equal eui64*/
    otExtAddress extAddress;
    otLinkGetFactoryAssignedIeeeEui64(instance, &extAddress);
    otLinkSetExtendedAddress(instance, &extAddress);

    /* set mle eid to equal eui64*/
    otIp6InterfaceIdentifier iid;
    memcpy(iid.mFields.m8, extAddress.m8, OT_EXT_ADDRESS_SIZE);
    otIp6SetMeshLocalIid(instance, &iid);

    if (load_default_config) {
        app_start_join(instance);
    } else {
        app_dataset.mActiveTimestamp.mSeconds = 0;
        app_dataset.mActiveTimestamp.mTicks = 0;
        app_dataset.mActiveTimestamp.mAuthoritative = false;
        app_dataset.mComponents.mIsActiveTimestampPresent = true;

        otDatasetUpdateTlvs(&app_dataset, &app_dataset_tlv);

        otDatasetSetActiveTlvs(instance, &app_dataset_tlv);

        otThreadSetEnabled(instance, true);

        log_info("Active Timestamp   : %lld",
                 (unsigned long long)app_dataset.mActiveTimestamp.mSeconds);
        otnetworkinfo(instance);
    }
}

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

    otIp6SetEnabled(instance, true);

    otdatasetInit(instance);
#if CONFIG_APP_TASK_CENTRAL_ENABLE
    net_mgm_init(instance);
#endif
}

void app_join_response_handler(uint16_t panid, uint8_t* net_key) {
    otInstance* instance = otrGetInstance();
    uint32_t role = otThreadGetDeviceRole(instance);
    if (role != OT_DEVICE_ROLE_DISABLED) {
        return;
    }
    otNetworkKey key;

    memcpy(&key.m8, net_key, sizeof(key));
    log_info("join panid: %04x", panid);
    log_info_hexdump("join key", key.m8, sizeof(key.m8));

    /* Set Pan ID */
    otLinkSetPanId(instance, (otPanId)panid);

    /* Set network key */
    otThreadSetNetworkKey(instance, &key);

    log_info("channel             : %d ", otLinkGetChannel(instance));
    log_info("PAN ID              : %x ", otLinkGetPanId(instance));
    otNetworkKey networkKey;
    otThreadGetNetworkKey(instance, &networkKey);
    log_info_hexdump("networkkey          : ", networkKey.m8,
                     OT_NETWORK_KEY_SIZE);

    app_evt_single(NULL, APP_EVENT_JOIN_SUCCESS);
}

void app_task(void) {
    app_event_t evt;
    appEventQueue = xQueueCreate(10, sizeof(app_event_t));

    while (true) {
        if (xQueueReceive(appEventQueue, &evt, portMAX_DELAY)) {
            OT_THREAD_SAFE(
                otInstance* instance = otrGetInstance(); if (instance) {
                    uint32_t role = otThreadGetDeviceRole(instance);
                    switch (evt.id) {
                        case APP_EVENT_JOIN_SUCCESS:
                            log_info("Proceeding to attach...");
                            otnetworkinfo(instance);
                            otThreadSetEnabled(instance, true);
                            if (sJoinRetryTimer) {
                                xTimerDelete(sJoinRetryTimer, 0);
                                sJoinRetryTimer = NULL;
                            }
                            break;
                        case APP_EVENT_RETRY_JOIN_NOW:
                        case APP_EVENT_JOIN_FAILED:
                            if (role != OT_DEVICE_ROLE_DISABLED) {
                                log_info("not join state ");
                                break;
                            }
                            if (evt.id == APP_EVENT_RETRY_JOIN_NOW) {
                                app_join_send(instance);
                                break;
                            }
                            g_app_joiner_attempt++;
                            const uint32_t kStepCount =
                                CONFIG_APP_JOINER_BACKOFF_MAX
                                / CONFIG_APP_JOINER_BACKOFF_INTERVAL;
                            const uint32_t kPeriodCount = kStepCount * 2 - 1;
                            const uint32_t kStepSize =
                                CONFIG_APP_JOINER_BACKOFF_INTERVAL;
                            uint16_t backoff_time_max = 0;
                            uint32_t phase = (g_app_joiner_attempt - 1)
                                             % kPeriodCount;

                            if (phase < kStepCount) {
                                backoff_time_max = (phase + 1) * kStepSize;
                            } else {
                                backoff_time_max = (kPeriodCount - phase)
                                                   * kStepSize;
                            }

                            int retry_delay = otRandomNonCryptoGetUint16InRange(
                                200, backoff_time_max);

                            log_info("Retrying join in %d milliseconds...",
                                     retry_delay);
                            vTaskDelay(pdMS_TO_TICKS(retry_delay));
                            // otInstanceErasePersistentInfo(otrGetInstance());
                            app_start_join(instance);
                            break;
                        case APP_EVENT_CHANGE_ROLE:
#if CONFIG_APP_TASK_CENTRAL_ENABLE
                            if (role == OT_DEVICE_ROLE_CHILD) {
                                enroll_send_time = 0;
                                enroll_send_try = 0;
                                enroll_done = false;
                            }
#endif
                            break;
#if CONFIG_APP_TASK_CENTRAL_ENABLE
                        case APP_EVENT_NET_MGM_ENROLL_REQ_SEND:
                            net_mgm_enroll_req_send(instance);
                            break;
                        case APP_EVENT_NET_MGM_ENROLL_REQ_SEND_TRYOUT:
                            otThreadSetEnabled(instance, false);
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
    log_out("app udp send <ipv6> -x <hex data> \r\n");
    log_out("app udp send <ipv6> -c <string data> \r\n");
    log_out("app udp port \r\n");
    log_out("app led <on/off/toggle/flash> \r\n");
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
        if (!data)
            return -1;

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
        log_out("Unknown send format. Use -x or -c.\r\n");
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
        log_out("Unknown LED command\r\n");
        return -1;
    }

    return 0;
}

static int _cli_cmd_miu_app(int argc, char** argv, cb_shell_out_t log_out,
                            void* pExtra) {
    int ret = -1;

    if (argc < 2) {
        log_out("Too few parameters \r\n");
        return ret;
    }

    if (!strncmp(argv[1], "help", 4)) {
        print_help(log_out);
        ret = 0;
    } else if (!strncmp(argv[1], "udp", 3)) {
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
            log_out("Unknown udp subcommand\r\n");
        }
    } else if (!strncmp(argv[1], "led", 3)) {
        ret = handle_led_command(argc, argv, log_out);
    } else if (!strncmp(argv[1], "mem", 3)) {
        extMemory();
        ret = 0;
    } else {
        print_help(log_out);
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