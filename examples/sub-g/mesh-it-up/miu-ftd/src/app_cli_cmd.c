#include <openthread/cli.h>
#include "common/code_utils.hpp"

#if CONFIG_APP_TASK_OTA_ENABLE
#include "app_ota.h"
#endif
#include <string.h>
#include "app_cli_cmd.h"
#include "app_led.h"
#include "app_mac_raw.h"
#include "app_uart.h"
#include "app_udp.h"
#include "log.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "uart.h"
#include "util_string.h"

static void print_help() {
    printf("app udp send <ipv6> -x <hex data> \r\n");
    printf("app udp send <ipv6> -c <string data> \r\n");
    printf("app udp port \r\n");
    printf("app led <on/off/toggle/flash> \r\n");
    printf("app mem *(check memory info) \r\n");
}

static otError app_cmd_process(void* aContext, uint8_t aArgsLength,
                               char* aArgs[]) {
    otError error = OT_ERROR_NONE;
    if (!strcmp(aArgs[0], "udp")) {
        if (aArgsLength < 3) {
            printf("Too few parameters \r\n");
            return OT_ERROR_INVALID_ARGS;
        }

        if (!strncmp(aArgs[1], "send", 4)) {
            if (aArgsLength < 5) {
                printf("Too few parameters \r\n");
                return OT_ERROR_INVALID_ARGS;
            }

            otIp6Address dst_addr;
            if (otIp6AddressFromString(aArgs[2], &dst_addr) != OT_ERROR_NONE) {
                printf("Invalid IPv6 address \r\n");
                return OT_ERROR_FAILED;
            }

            uint8_t* data = NULL;
            uint16_t data_lens = 0;

            if (!strncmp(aArgs[3], "-x", 2)) {
                // Send hex data
                data_lens = (strlen(aArgs[4]) + 1) / 2;
                data = xMalloc(data_lens);
                if (!data)
                    return OT_ERROR_FAILED;

                for (uint16_t i = 0; i < data_lens; i++) {
                    data[i] = (utility_strtox(aArgs[5] + i * 2, 0, 2) & 0xFF);
                }
            } else if (!strncmp(aArgs[3], "-c", 2)) {
                // Send string data
                for (uint8_t i = 4; i < aArgsLength; i++) {
                    data_lens += strlen(aArgs[i]) + 1;
                }

                data = xMalloc(data_lens);
                if (!data) {
                    printf("handle_udp_send malloc fail");
                    return OT_ERROR_FAILED;
                }

                uint16_t offset = 0;
                for (uint8_t i = 4; i < aArgsLength; i++) {
                    size_t len = strlen(aArgs[i]);
                    memcpy(&data[offset], aArgs[i], len);
                    offset += len;
                    data[offset++] = 0x20;
                }

                if (offset > 0)
                    offset--; // remove last space
                data_lens = offset;
            } else {
                printf("Unknown send format. Use -x or -c. \r\n");
                return OT_ERROR_FAILED;
            }

            app_udpSend(dst_addr, data, data_lens, false);
            if (data)
                xFree(data);
        } else if (!strncmp(aArgs[1], "port", 4)) {
            printf("app udp port: %d \r\n", CONFIG_APP_TASK_UDP_LISTEN_PORT);
        } else {
            printf("Unknown udp subcommand \r\n");
        }
    } else if (!strcmp(aArgs[0], "led")) {
        if (aArgsLength < 2) {
            printf("Too few parameters \r\n");
            return -1;
        }
        if (!strncmp(aArgs[1], "on", 2)) {
            app_set_led0_on();
        } else if (!strncmp(aArgs[1], "off", 3)) {
            app_set_led0_off();
        } else if (!strncmp(aArgs[1], "toggle", 6)) {
            app_set_led0_toggle();
        } else if (!strncmp(aArgs[1], "flash", 5)) {
            app_set_led0_flash();
        } else {
            printf("Unknown LED command \r\n");
            return OT_ERROR_FAILED;
        }
    } else if (!strcmp(aArgs[0], "mem")) {
        extMemory();
    } else {
        print_help();
    }
    return error;
}

#if CONFIG_APP_TASK_OTA_ENABLE

static void ota_print_help() {
    printf("ota start <segments> <interval>                             *(multicast OTA to all)\r\n");
    printf("ota send <ipv6>                                             *(unicast OTA to one device)\r\n");
    printf("ota stop                                                    \r\n");
    printf("ota self \r\n");
    printf("ota debug <level> \r\n");
    printf("ota erase \r\n");
    printf("ota rxmode <on/off> \r\n");
#if CONFIG_MIU_DEVICE_TYPE_FTD
    printf("ota status <ipv6> <ftd/mtd/all> <report timeout(ms)>    *(query OTA state, log: OTA_STATUS|...)\r\n");
#endif
    printf("ota execute reboot <ipv6> <time(ms)>                    *(reboot matching bin_type devices)\r\n");
    printf("ota download *(Change to hex mode)\r\n");
}

otError app_cmd_ota_process(void* aContext, uint8_t aArgsLength,
                            char* aArgs[]) {
    int ret = 0;

    if (!strncmp(aArgs[0], "help", 4)) {
        ota_print_help();
    } else if (!strncmp(aArgs[0], "start", 5)) {
        if (aArgsLength < 3) {
            printf("Too few parameters \r\n");
            return OT_ERROR_INVALID_ARGS;
        }
        uint16_t segments = 255;
        uint16_t interval = 1500;
        segments = utility_strtol(aArgs[1], 0);
        interval = utility_strtol(aArgs[2], 0);
        printf("segments_size %u ,interval %u \r\n", segments, interval);
        ota_start(segments, interval);
    } else if (!strncmp(aArgs[0], "send", 4)) {
        if (aArgsLength < 2) {
            printf("Too few parameters \r\n");
            return OT_ERROR_INVALID_ARGS;
        }
        ota_send(aArgs[1]);
    } else if (!strncmp(aArgs[0], "self", 4)) {
        ota_update_self();
    } else if (!strncmp(aArgs[0], "stop", 4)) {
        ota_stop();
        otIp6Address dst_addr;
        char dst_addr_str[] = "ff03::1";
        otIp6AddressFromString(dst_addr_str, &dst_addr);
        ota_send_execute(dst_addr, OTA_EXECUTE_STOP, 0);
    } else if (!strncmp(aArgs[0], "debug", 5)) {
        if (aArgsLength > 1) {
            unsigned int level = 0;
            level = utility_strtol(aArgs[1], 0);
            ota_debug_level(level);
        } else {
            printf("Ota debug level %d \r\n", ota_debug_level_get());
        }
    } else if (!strncmp(aArgs[0], "erase", 5)) {
        ota_reset();
    } else if (!strncmp(aArgs[0], "rxmode", 6)) {
        if (aArgsLength < 2) {
            printf("Too few parameters \r\n");
            return OT_ERROR_INVALID_ARGS;
        }
        if (!strncmp(aArgs[1], "on", 2)) {
            ota_send_rxmode(true);
        } else {
            ota_send_rxmode(false);
        }
    } else if (!strncmp(aArgs[0], "status", 6)) {
#if CONFIG_MIU_DEVICE_TYPE_FTD
        if (aArgsLength < 3) {
            printf("Too few parameters \r\n");
            return OT_ERROR_INVALID_ARGS;
        }
        otIp6Address dst_addr;
        otIp6AddressFromString(aArgs[1], &dst_addr);
        uint16_t report_timeout = utility_strtol(aArgs[3], 0);
        printf("report timeout %d \r\n", report_timeout);
        if (!strncmp(aArgs[2], "ftd", 3)) {
            ota_send_status_get(dst_addr, OTA_STATUS_TYPE_FTD, report_timeout);
        } else if (!strncmp(aArgs[2], "mtd", 3)) {
            ota_send_status_get(dst_addr, OTA_STATUS_TYPE_MTD, report_timeout);
        } else {
            ota_send_status_get(dst_addr, OTA_STATUS_TYPE_ALL, report_timeout);
        }
#endif
    } else if (!strncmp(aArgs[0], "execute", 7)) {
        if (!strncmp(aArgs[1], "reboot", 6)) {
            if (aArgsLength < 3) {
                printf("Too few parameters \r\n");
                return OT_ERROR_INVALID_ARGS;
            }
            otIp6Address dst_addr;
            otIp6AddressFromString(aArgs[2], &dst_addr);
            uint16_t time = utility_strtol(aArgs[3], 0);
            printf("reboot time %d \r\n", time);
            ota_send_execute(dst_addr, OTA_EXECUTE_REBOOT, time);
        }
    }
#if CONFIG_MIU_DEVICE_TYPE_FTD
    else if (!strncmp(aArgs[0], "download", 6)) {
        otPlatUartDisable();
        app_uart0_enable();
        log_set_level(LOG_LEVEL_NEVER);
    }
#endif
    else {
        uint8_t img_bin_type[12] = {0};
        ota_flash_read_bin_type(img_bin_type);
        printf("ota state : %s \r\n", OtaStateToString(ota_get_state()));
        printf("ota image version : 0x%08x \r\n", ota_get_image_version());
        printf("ota image bin type : %.12s \r\n", img_bin_type);
        printf("ota image size : 0x%08x \r\n", ota_get_image_size());
        printf("ota image crc : 0x%08x \r\n", ota_get_image_crc());
        printf("current bin version : 0x%08x \r\n",
               GET_BIN_VERSION(systeminfo.sysinfo));
    }

    return ret;
}
#endif

static const otCliCommand sExtensionCommands[] = {
    {"app", app_cmd_process},
#if CONFIG_APP_TASK_OTA_ENABLE
    {"ota", app_cmd_ota_process},
#endif
};

void app_cli_cmd_init(void) {
    IgnoreError(otCliSetUserCommands(
        sExtensionCommands, OT_ARRAY_LENGTH(sExtensionCommands), NULL));
}
