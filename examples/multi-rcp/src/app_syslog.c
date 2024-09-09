/**
 * @file app_syslog.c
 * @brief This file is to define the syslog function for application.
 * @version 0.1
 * @date 2024/08/14
 * @license
 * @description
 * This file is to define the syslog function for application.
 * 
 */
#if defined(CONFIG_BUILD_COMPONENT_SYSLOG)
//=============================================================================
//                Include
//=============================================================================
#include <string.h>
#include "app_syslog.h"
#include "log.h"
#include "syslog.h"

#include "cli.h"
#include "shell.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================
static const char* g_app_type[APP_SYSLOG_MAX] = {[APP_SYSLOG_CPC] = "CPC",
                                                 [APP_SYSLOG_UART] = "URT",
                                                 [APP_SYSLOG_UPGRADE] = "UPG",
                                                 [APP_SYSLOG_ZIGBEE] = "ZBG",
                                                 [APP_SYSLOG_HCI] = "HCI"};

static void app_syslog_print_uart(syslog_t* log) {
    switch (log->sub_type) {
        case APP_SYSLOG_UART_INIT:
            printf("Initial UART-%d, baudrate %d", log->msg1, log->msg2);
            break;
        case APP_SYSLOG_UART_SEND:
            printf("Send Data %p, length %d", log->msg1, log->msg2);
            break;
        case APP_SYSLOG_UART_RECV:
            printf("Recv Data %p, length %d", log->msg1, log->msg2);
            break;
    }
}

static void app_syslog_print(syslog_t* log) {
    printf("[%10u] APP%d-%s:", log->tick, log->module, g_app_type[log->type]);

    switch (log->type) {
        case APP_SYSLOG_UART: app_syslog_print_uart(log); break;
    }

    printf("\r\n");
}

static int _cli_cmd_app_syslog(int argc, char** argv, cb_shell_out_t log_out,
                               void* pExtra) {

    syslog_print(SYSLOG_MODULE_APPLICATION, APP_SYSLOG_UART);
    return 0;
}

//=============================================================================
//                Public Function Definition
//=============================================================================

void app_syslog_init(void) {
    syslog_print_fn_register(SYSLOG_MODULE_APPLICATION, app_syslog_print);
}

const sh_cmd_t g_cli_cmd_app_syslog STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "app_syslog",
    .pDescription = "Show application syslog",
    .cmd_exec = _cli_cmd_app_syslog,
};

#endif
