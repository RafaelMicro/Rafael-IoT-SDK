#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openthread_port.h>

#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
#include <zigbee_platform.h>
#endif

#include "hosal_rf.h"
#include "hosal_uart.h"
#include "lmac15p4.h"
#include "mcu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "cli.h"
#include "log.h"

#if defined(CONFIG_BUILD_COMPONENT_SYSLOG)
#include "app_syslog.h"
#include "syslog.h"
#endif

#include "sw_timer.h"

#include <openthread/cli.h>
#include <openthread/coap.h>
#include <openthread/icmp6.h>
#include <openthread/ncp.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread_port.h>

#include <cpc_system_common.h>
#include <cpc_user_interface.h>

#include "EnhancedFlashDataset.h"

#define PHY_PIB_TURNAROUND_TIMER    192
#define PHY_PIB_CCA_DETECTED_TIME   128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE     0
#define PHY_PIB_CCA_THRESHOLD       75
#define MAC_PIB_UNIT_BACKOFF_PERIOD 320
#define MAC_PIB_MAC_ACK_WAIT_DURATION                                          \
    544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE                    5
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       5
#define MAC_PIB_MAC_MIN_BE                    2

static volatile uint32_t s_reboot_flag = 0;
extern hosal_uart_dev_t cpc_uart_dev;
static wdt_config_mode_t wdt_mode;
static wdt_config_tick_t wdt_cfg_ticks;

void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode) {
    log_warn("Reset system!");

    if (reboot_mode) {
        s_reboot_flag = 1;
        return;
    }

    wdt_stop();
    // hosal_uart_finalize(&cpc_uart_dev);
    //s_reboot_flag = 1;

    wdt_mode.int_enable = 0;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 0;
    wdt_mode.prescale = WDT_PRESCALE_32;

    wdt_cfg_ticks.wdt_ticks = 600 * 1000;
    wdt_cfg_ticks.int_ticks = 200 * 1000;
    wdt_cfg_ticks.wdt_min_ticks = 2;

    wdt_start(wdt_mode, wdt_cfg_ticks, NULL);
}

__STATIC_INLINE void wdt_isr(void) {
    if (s_reboot_flag == 0)
        wdt_kick();
}

static void init_wdt_init(void) {
    wdt_mode.int_enable = 0;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 0;
    wdt_mode.prescale = WDT_PRESCALE_32;

    wdt_cfg_ticks.wdt_ticks = 3200 * 1000;
    wdt_cfg_ticks.int_ticks = 200 * 1000;
    wdt_cfg_ticks.wdt_min_ticks = 0;

    wdt_start(wdt_mode, wdt_cfg_ticks, wdt_isr);

    NVIC_SetPriority(Wdt_IRQn, 0x1);
}
#ifdef CONFIG_APP_ENABLE_3W_PTA
void set_pta_grant_pin(uint8_t gpio) {
    uint32_t pta_in_reg = (uint32_t)gpio | (1UL << 7);

    pin_set_mode(gpio, MODE_GPIO);

    outp32(0x40800058, ((inp32(0x40800058) & 0xFFFFFF00) | pta_in_reg));
    outp32(0x40800010, ((inp32(0x40800010) & 0xFFFFFFFF) | 0x77000000));
    outp32(0x4080003C, ((inp32(0x4080003C) | 0x07000000)));
}
#endif
int main(void) {
    s_reboot_flag = 0;

#if defined(CONFIG_BUILD_COMPONENT_SYSLOG)
    syslog_init();
    app_syslog_init();
#endif
    log_info("%s version : %s", CONFIG_BUILD_PORJECT, CONFIG_PROJECT_VERSION);
    sw_timer_init(0);
    init_wdt_init(); // debug should be comment out

    hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL);
    lmac15p4_init(LMAC15P4_2P4G_OQPSK, 0);

    cpc_uart_init();

    cli_init();

    cpc_hci_init();
    cpc_upgrade_init();

#ifdef CONFIG_APP_ENABLE_3W_PTA
    hosal_rf_pta_ctrl_t pta_ctrl;
    pta_ctrl.enable = 0;
    pta_ctrl.inverse = 1;

    set_pta_grant_pin(8);

    hosal_rf_ioctl(HOSAL_RF_IOCTL_PTA_CTRL_SET, (void*)&pta_ctrl);
#endif

    /* PHY PIB */
    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, PHY_PIB_CCA_DETECTED_TIME);
    /* MAC PIB */
    lmac15p4_mac_pib_set(MAC_PIB_UNIT_BACKOFF_PERIOD,
                         MAC_PIB_MAC_ACK_WAIT_DURATION, MAC_PIB_MAC_MAX_BE,
                         MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         MAC_PIB_MAC_MAX_FRAME_RETRIES, MAC_PIB_MAC_MIN_BE);

    otrStart();
#ifdef CONFIG_APP_MULTI_RCP_ZB_GW
    zbStart();
#endif

    while (1) {
        if (!s_reboot_flag)
            wdt_kick();
        vTaskDelay(1000);
    }
    return 0;
}

void otrInitUser(otInstance* instance) {
    otAppCliInit((otInstance*)instance);
    otAppNcpInit((otInstance*)instance);
}
