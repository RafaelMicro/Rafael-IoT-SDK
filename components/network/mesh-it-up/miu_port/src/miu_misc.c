/**
 * @file ot_misc.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <openthread/platform/misc.h>
#include <string.h>
#include "cli.h"
#include "log.h"
#include "mcu.h"
#include "stdio.h"

#define ms_sec(N) (N * 1000)

void otPlatReset(otInstance* aInstance) {
    OT_UNUSED_VARIABLE(aInstance);

    log_info("OT Reset");
    wdt_config_mode_t wdt_mode;
    wdt_config_tick_t wdt_cfg_ticks;

    wdt_mode.int_enable = 0;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 1;
    wdt_mode.prescale = WDT_PRESCALE_32;

    sys_set_retention_reg(6, 7);
    sys_set_retention_reg(7, 0);

    wdt_cfg_ticks.wdt_ticks = ms_sec(1200);
    wdt_cfg_ticks.int_ticks = ms_sec(0);
    wdt_cfg_ticks.wdt_min_ticks = ms_sec(1);

    wdt_start(wdt_mode, wdt_cfg_ticks, NULL);
    while (1)
        ;
    // NVIC_SystemReset();
}

otPlatResetReason otPlatGetResetReason(otInstance* aInstance) {
    OT_UNUSED_VARIABLE(aInstance);
    log_info("otPlatGetResetReason");
    otPlatResetReason reason = OT_PLAT_RESET_REASON_POWER_ON;

    return reason;
}

void otPlatWakeHost(void) {
    // TODO: implement an operation to wake the host from sleep state.
}

void otPlatAssertFail(const char* aFilename, int aLineNumber) {
    log_error("Assert, %s @ %d\r\n", aFilename, aLineNumber);
}

static int _cli_cmd_reset(int argc, char** argv, cb_shell_out_t log_out,
                          void* pExtra) {
    otPlatReset(NULL);
    return 0;
}

const sh_cmd_t g_cli_cmd_reset STATIC_CLI_CMD_ATTRIBUTE = {
    .pCmd_name = "reset",
    .pDescription = "reset",
    .cmd_exec = _cli_cmd_reset,
};