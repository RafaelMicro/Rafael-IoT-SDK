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
#include <semphr.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "cli.h"
#include "log.h"
#include "main.h"
#include "util_string.h"

static SemaphoreHandle_t appSemHandle = NULL;

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

static void ot_stateChangeCallback(uint32_t flags, void* p_context) {
    otInstance* instance = (otInstance*)p_context;
    uint8_t* p;
    // printOtChangedFlags(flags);
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
            log_info("child removed      : %02x%02x%02x%02x%02x%02x%02x%02x",
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
            log_info("child changned     : %02x%02x%02x%02x%02x%02x%02x%02x",
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
            log_info("couter added       : %02x%02x%02x%02x%02x%02x%02x%02x",
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
            log_info("couter remove      : %02x%02x%02x%02x%02x%02x%02x%02x",
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

static void app_udp_cb(otMessage* otMsg, const otMessageInfo* otInfo) {
    uint8_t* p = NULL;

    uint16_t len = 0;

    len = otMessageGetLength(otMsg) - otMessageGetOffset(otMsg);

    p = pvPortMalloc(len);

    do {
        if (p == NULL)
            break;

        otMessageRead(otMsg, otMessageGetOffset(otMsg), p, len);

        if (!strncmp(p, "app", 3)) {
            //execute cli app command
            char* cmd = (char*)pvPortMalloc(len + 1);
            if (cmd == NULL) {
                log_error("malloc cmd failed");
                break;
            }
            memset(cmd, 0, len + 1);
            memcpy(cmd, p, len);
            cmd[len] = '\0';
            log_info("remove cmd: %s", cmd);
            if (shell_exec_string(cmd) != 0) {
                log_error("app cli execute failed");
            }
        } else {
            log_info_hexdump("UDP", p, len);
        }

    } while (0);

    if (p != NULL)
        vPortFree(p);
}

static void otdatasetInit(otInstance* instance) {
    otOperationalDatasetTlvs app_dataset_tlv;
    otOperationalDataset app_dataset;
    if (otDatasetGetActiveTlvs(instance, &app_dataset_tlv) != OT_ERROR_NONE) {
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
        log_warn("APP Dataset flash is Null, set default value");
        /* Set activetimestamp */
        app_dataset.mActiveTimestamp.mSeconds = 1;
        app_dataset.mComponents.mIsActiveTimestampPresent = true;

        /* Set Channel */
        app_dataset.mChannel = 1;
        app_dataset.mComponents.mIsChannelPresent = true;

        /* Set Pan ID */
        app_dataset.mPanId = (otPanId)netconfig.panId;
        app_dataset.mComponents.mIsPanIdPresent = true;

        /* Set Wake-up Channel */
        // app_dataset.mWakeupChannel = netconfig.channel;
        // app_dataset.mComponents.mIsWakeupChannelPresent = true;

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
    } else {
        otDatasetParseTlvs(&app_dataset_tlv, &app_dataset);
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
#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    otLoggingSetLevel(OT_LOG_LEVEL_NOTE);
#endif
    otdatasetInit(instance);
    otAppCliInit(instance);
    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);
    app_sockInit(instance, app_udp_cb, CONFIG_APP_TASK_UDP_LISTEN_PORT);
    /*led pin init*/
    app_led_pin_init();

    /*auto start networking*/
    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);
}

void app_task(void) {
    appSemHandle = xSemaphoreCreateBinary();

    while (true) {
        if (xSemaphoreTake(appSemHandle, 10000)) {
            /*Customer written code*/
        }
    }
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
        data = pvPortMalloc(data_lens);
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

        data = pvPortMalloc(data_lens);
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

    app_udpSend(dst_addr, data, data_lens);
    vPortFree(data);
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
            log_out("Unknown udp subcommand\r\n");
        }
    } else if (!strncmp(argv[1], "led", 3)) {
        ret = handle_led_command(argc, argv, log_out);
    } else {
        log_out("Unknown command\r\n");
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