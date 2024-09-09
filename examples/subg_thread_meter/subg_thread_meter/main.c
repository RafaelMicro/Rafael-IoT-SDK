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
#include "subg_ctrl.h"
#include "task.h"

#define RAIDO_MAC_ADDR_FLASH_ID_MODE  0
#define RAIDO_MAC_ADDR_MP_SECTOR_MODE 1

extern void rafael_radio_subg_datarate_set(uint8_t datarate);
extern void rafael_radio_subg_band_set(uint8_t ch_min, uint8_t ch_max,
                                       uint16_t band);

void wdt_isr(void) { wdt_kick(); }

void wdt_init(void) {
    wdt_config_mode_t wdt_mode;
    wdt_config_tick_t wdt_cfg_ticks;

    wdt_mode.int_enable = 1;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 0;
    wdt_mode.prescale = WDT_PRESCALE_32;

    wdt_cfg_ticks.wdt_ticks = 5000 * 2000;
    wdt_cfg_ticks.int_ticks = 20 * 2000;
    wdt_cfg_ticks.wdt_min_ticks = 0;

    Wdt_Start(wdt_mode, wdt_cfg_ticks, wdt_isr);
}

int main(void) {
    wdt_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    rafael_radio_subg_datarate_set(SUBG_CTRL_DATA_RATE_300K);
    rafael_radio_subg_band_set(
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN,
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX,
        OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE);

    log_info("Thread SubG meter\r\n");

    if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 2) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_915M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 3) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_868M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 4) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_433M);
    } else if (OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE == 5) {
        lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_433M);
    }

#if (CONFIG_PLATOFRM_ENABLE_SLEEP == 0)
    cli_init();
#endif
    otrStart();

    app_task();
    return 0;
}
