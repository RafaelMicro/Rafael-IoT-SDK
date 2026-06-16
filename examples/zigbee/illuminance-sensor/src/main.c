/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zigbee_platform.h"
#include "zigbee_zcl_msg_handler.h"
#include "zigbee_api.h"
#include "device_api.h"

#include "hosal_flash.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "hosal_lpm.h"
#include "hosal_wdt.h"

#include "FreeRTOS.h"
#include "task.h"

#include "log.h"
#include "app_hooks.h"
#include "uart_stdio.h"


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

static uint8_t g_joined_network = 0;
static uint16_t g_panid = 0xFFFF;
static uint16_t g_short_addr = 0xFFFF;
static uint8_t g_joined_channel = 0xFF;

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    hosal_flash_init();
    log_info("%s version : %s", CONFIG_BUILD_PORJECT, CONFIG_PROJECT_VERSION);

    enhanced_flash_dataset_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);
    hosal_lpm_init();
#ifdef ZB_USE_SLEEP
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO);
#endif

    zigbee_app_init();
    zbStart();
    vTaskStartScheduler();
    while(1) {;}
}

/*
 * Watchdog (WDT)
 *
 * The kick strategy adapts at compile time to whether this build is a low-power
 * (sleep) device, decided by ZB_USE_SLEEP (set when CONFIG_HOSAL_SOC_IDLE_SLEEP):
 *   - ZB_USE_SLEEP defined (sleep device): the chip sleeps between events and the
 *     WDT is frozen while asleep, so the app task stays blocked on portMAX_DELAY
 *     and the WDT is fed from the idle hook - a notification-independent heartbeat
 *     that runs whenever the system returns to idle / before it sleeps.
 *   - ZB_USE_SLEEP not defined (mains-powered): the WDT runs continuously, so the
 *     app task waits with a finite timeout and kicks every loop iteration.
 * WDT_TIMEOUT_MS must exceed the longest contiguous *awake* processing burst
 * (network steering scan, flash erase/write, OTA chunk handling), not the sleep
 * period.
 */
#define WDT_MS(n)            ((n) * 1000) /* prescale 32 -> 1MHz -> 1 tick = 1us */
#define WDT_TIMEOUT_MS       8000         /* reset if not kicked within this awake time */
#define WDT_INT_MS           500          /* interrupt warning window before reset */
#define WDT_KICK_INTERVAL_MS 1000         /* (non-sleep) app loop kick period */

#ifdef CONFIG_APP_WDT_ENABLE
/* Set by the WDT ISR, consumed and printed from task context in app_main_loop. */
static volatile uint8_t g_wdt_timeout_warn = 0;

/*
 * WDT interrupt, fired WDT_INT_MS before a reset would occur. Runs in ISR
 * context, so it MUST NOT call FreeRTOS APIs or block - log_error() ends up
 * taking a UART semaphore, which asserts/blocks in an ISR. We only raise a
 * flag here; app_main_loop prints the warning in task context.
 */
static void app_wdt_isr_cb(void) {
    g_wdt_timeout_warn = 1;
}

static void app_wdt_init(void) {
    hosal_wdt_config_mode_t wdt_mode;
    hosal_wdt_config_tick_t wdt_ticks;

    wdt_mode.int_enable   = 1;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable  = 0; /* keep 0: sys_software_reset() hangs if the WDT is locked */
    wdt_mode.prescale     = HOSAL_WDT_PRESCALE_32; /* 32MHz/32 = 1MHz */

    wdt_ticks.wdt_ticks     = WDT_MS(WDT_TIMEOUT_MS);
    wdt_ticks.int_ticks     = WDT_MS(WDT_INT_MS);
    wdt_ticks.wdt_min_ticks = 0;

    /* If the previous boot ended in a WDT reset, report it - reliable even
     * when a hard hang prevented the live warning below from ever printing. */
    uint32_t wdt_rst_cnt = 0;
    hosal_wdt_reset_event_get(&wdt_rst_cnt);
    if (wdt_rst_cnt) {
        log_warn("Previous reset was caused by WDT (count=%u)", (unsigned)wdt_rst_cnt);
    }
    hosal_wdt_reset_event_clear();
    hosal_wdt_start(wdt_mode, wdt_ticks, app_wdt_isr_cb);
    /* Keep the IRQ in the FreeRTOS syscall-safe band (>= configMAX_SYSCALL_
     * INTERRUPT_PRIORITY) as good practice; the ISR itself only sets a flag. */
    NVIC_SetPriority(Wdt_IRQn, 0x04);
    NVIC_EnableIRQ(Wdt_IRQn);
}

#ifdef ZB_USE_SLEEP
/*
 * Sleep device heartbeat. Runs whenever every task is idle, i.e. right before the
 * system enters low power. Reaching idle proves the system is healthy, so this is
 * the correct, notification-independent place to kick the WDT. A genuine hang (a
 * task that spins without yielding) never lets the idle task run, so the WDT still
 * fires. We must NOT rely only on kicking inside app_main_loop: the chip wakes for
 * radio RX / parent poll / stack processing without ever notifying the app task.
 */
void vApplicationIdleHook(void) {
    hosal_wdt_kick();
}
#endif
#endif

void app_main_loop(void* parameters_ptr) {
    zb_app_event_t sevent = ZB_APP_EVENT_NONE;

    ZB_THREAD_SAFE(
        ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_illuminance_sensor_ctx);
        for (int i = 0; i < simple_desc_illuminance_sensor_ctx.ep_count; i++) {
            ZB_AF_SET_ENDPOINT_HANDLER(
                simple_desc_illuminance_sensor_ctx.ep_desc_list[i]->ep_id,
                zigbee_zcl_msg_handler);
        }
    )
    ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_INIT);

#ifdef CONFIG_APP_WDT_ENABLE
    app_wdt_init();
#endif

    for (;;) {
#ifdef CONFIG_APP_WDT_ENABLE
        if (g_wdt_timeout_warn) {
            g_wdt_timeout_warn = 0;
            log_error("WDT timeout warning: system was not kicked in time");
        }
#endif
#if defined(CONFIG_APP_WDT_ENABLE) && !defined(ZB_USE_SLEEP)
        /* Non-sleep device: WDT runs continuously; finite wait + kick every loop. */
        hosal_wdt_kick();
        if (ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(WDT_KICK_INTERVAL_MS)) != 0) {
#else
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0) {
#endif
#if defined(CONFIG_APP_WDT_ENABLE) && defined(ZB_USE_SLEEP)
            hosal_wdt_kick(); /* supplementary; primary heartbeat is the idle hook */
#endif
            ZIGBEE_APP_GET_NOTIFY(sevent);

            switch (sevent) {
                case ZB_APP_EVENT_INIT: {
                    zigbee_app_nwk_start(ZIGBEE_CHANNEL_ALL_MASK(), 32, 0);
                    log_info("ZigBee APP init");
                    start_sensor_timer();
                    set_led_onoff(LED_BLUE,1);
                } break;

                case ZB_APP_EVENT_NOT_JOINED: {

                    ZB_THREAD_SAFE(bdb_start_top_level_commissioning(
                        ZB_BDB_NETWORK_STEERING));
                    log_info("ZigBee APP not joined");
                } break;

                case ZB_APP_EVENT_JOINED: {
                    log_info("ZigBee APP joined");
                    g_joined_network = 1;
                    set_led_onoff(LED_BLUE,0);

                    ZB_THREAD_SAFE(g_panid = zb_get_pan_id();
                                   g_short_addr = zb_get_short_address();
                                   g_joined_channel = zb_get_current_channel();)

                    log_info("PAN ID: %04X, Short Addr: %04X, Channel: %d",
                             g_panid, g_short_addr, g_joined_channel);

                } break;

                case ZB_APP_EVENT_FACTORY_RESET: {
                    zigbee_do_factory_reset();
                } break;
                default: break;
            }
        }
    }
}