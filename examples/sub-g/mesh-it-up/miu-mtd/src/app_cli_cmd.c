#include <openthread/cli.h>
#include "common/code_utils.hpp"

#if CONFIG_APP_TASK_OTA_ENABLE
#include "app_ota.h"
#endif
#include <stdlib.h>
#include <string.h>
#include "app_cli_cmd.h"
#include "app_led.h"
#include "app_mac_raw.h"
#include "app_uart.h"
#include "app_udp.h"
#include "log.h"
#include "miu_bin_version.h"
#include "miu_ext_mem.h"
#include "miu_port.h"
#include "uart.h"
#include "util_string.h"

static void print_help() {
    printf("app udp send <ipv6> -x <hex data> \r\n");
    printf("app udp send <ipv6> -c <string data> \r\n");
    printf("app udp port \r\n");
    printf("app led <on/off/toggle/flash> \r\n");
    printf("app mem *(check memory info) \r\n");
    printf("app libver \r\n");
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
    } else if (!strcmp(aArgs[0], "radio")) {
        ot_radio_dbg_dump();
    } else if (!strcmp(aArgs[0], "evt")) {
        ot_evt_count_dump();
    } else if (!strcmp(aArgs[0], "libver")) {
        printf("miu_port lib ver: %s\r\n", miu_port_lib_version());
    } else {
        print_help();
    }
    return error;
}

static const otCliCommand sExtensionCommands[] = {
    {"app", app_cmd_process},
};

void app_cli_cmd_init(void) {
    IgnoreError(otCliSetUserCommands(
        sExtensionCommands, OT_ARRAY_LENGTH(sExtensionCommands), NULL));
}
