#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cli.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "mcu.h"
#include "openthread_port.h"
#include "subg_ctrl.h"
#include "task.h"

extern void rafael_radio_subg_datarate_set(uint8_t datarate);
extern void rafael_radio_subg_band_set(uint8_t ch_min, uint8_t ch_max,
                                       uint16_t band);

static uint32_t s_reboot_flag = 0;

void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode) {
    log_warn("Set watchdog reset!");
    s_reboot_flag = 1;
}

__STATIC_INLINE void wdt_isr(void) {
    if (s_reboot_flag == 0)
        wdt_kick();
}

static void init_wdt_init(void) {
    wdt_config_mode_t wdt_mode;
    wdt_config_tick_t wdt_cfg_ticks;

    wdt_mode.int_enable = 1;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 0;
    wdt_mode.prescale = WDT_PRESCALE_32;

    wdt_cfg_ticks.wdt_ticks = 1200 * 1000;
    wdt_cfg_ticks.int_ticks = 200 * 1000;
    wdt_cfg_ticks.wdt_min_ticks = 0;

    Wdt_Start(wdt_mode, wdt_cfg_ticks, wdt_isr);

    NVIC_SetPriority(Wdt_IRQn, 0x1);
}

int main(void) {
    init_wdt_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);
#ifdef CONFIG_TARGET_CUSTOMER
    log_info("Thread SubG - %s\r\n", CONFIG_TARGET_CUSTOMER);
#else
    log_info("Thread SubG\r\n");
#endif
    log_info("SDK Version %s\r\n", RAFAEL_SDK_VER);
#ifdef CONFIG_OPERATION_UART_PORT
    log_info("CPC operation port : UART_%d", CONFIG_OPERATION_UART_PORT);
#endif

    rafael_radio_subg_datarate_set(SUBG_CTRL_DATA_RATE_300K);
    rafael_radio_subg_band_set(
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN,
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX,
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE);

    log_info("Thread SubG ncp\r\n");

    if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 2) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_915M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 3) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_868M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 4) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_433M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 5) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_433M);
    }

    cpc_uart_init();
    cpc_upgrade_init();

    cli_init();

    cpc_subg_cmd_init();
    otrStart();

    app_task();
    return 0;
}
