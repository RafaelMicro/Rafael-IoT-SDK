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
#include "EnhancedFlashDataset.h"
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

/*
 * WDT settings for a mains-powered (non-sleep) device.
 *
 * The light is a router and never sleeps, so the WDT counter runs continuously.
 * The app task must therefore kick it on a fixed period: we wait on the task
 * notification with a finite timeout (WDT_KICK_INTERVAL_MS) and kick every loop
 * iteration, whether or not an event arrived. WDT_TIMEOUT_MS must be larger than
 * WDT_KICK_INTERVAL_MS plus the longest single event-processing burst.
 */
#define WDT_MS(n)            ((n) * 1000) /* prescale 32 -> 1MHz -> 1 tick = 1us */
#define WDT_TIMEOUT_MS       5000         /* reset if not kicked within this time */
#define WDT_INT_MS           500          /* interrupt warning window before reset */
#define WDT_KICK_INTERVAL_MS 1000         /* app loop wake-up / kick period */

static uint8_t g_joined_network = 0;
static uint16_t g_panid = 0xFFFF;
static uint16_t g_short_addr = 0xFFFF;
static uint8_t g_joined_channel = 0xFF;

uint8_t reset_to_default;
void check_reboot_count(void) {
    uint8_t reboot_count;
    size_t actual_len;
    efd_get_env_blob("reboot", (void *) &reboot_count, sizeof(uint8_t), &actual_len);
    reboot_count +=1;
    efd_set_env_blob("reboot", (void *) &reboot_count, sizeof(uint8_t));
    delay_ms(500);
    if(reboot_count >= 5) {
        reset_to_default = 1;
    }
    reboot_count = 0;
    efd_set_env_blob("reboot", (void *) &reboot_count, sizeof(uint8_t));
}
int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    hosal_flash_init();
    log_info("%s version : %s", CONFIG_BUILD_PORJECT, CONFIG_PROJECT_VERSION);

    enhanced_flash_dataset_init();
    check_reboot_count();
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
#ifdef CONFIG_APP_WDT_ENABLE
/* Set by the WDT ISR, consumed and printed from task context in app_main_loop. */
static volatile uint8_t g_wdt_timeout_warn = 0;

/*
 * Watchdog interrupt, fired WDT_INT_MS before a reset would occur. Runs in ISR
 * context, so it MUST NOT call FreeRTOS APIs or block - log_error() ends up
 * taking a UART semaphore, which asserts/blocks in an ISR. We only raise a flag
 * here; app_main_loop prints the warning in task context.
 */
static void app_wdt_isr_cb(void) {
    g_wdt_timeout_warn = 1;
}

/**
 * @brief Configure and start the watchdog.
 */
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
#endif

void app_main_loop(void* parameters_ptr) {
    zb_app_event_t sevent = ZB_APP_EVENT_NONE;

    ZB_THREAD_SAFE(
        ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_light_ctx);
        for (int i = 0; i < simple_desc_light_ctx.ep_count; i++) {
            ZB_AF_SET_ENDPOINT_HANDLER(
                simple_desc_light_ctx.ep_desc_list[i]->ep_id,
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
        /*
         * Non-sleep device: the WDT runs continuously, so wait with a finite
         * timeout and kick every iteration regardless of whether an event was
         * received. The lowest-priority app task acts as the heartbeat.
         */
        hosal_wdt_kick();
        if (ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(WDT_KICK_INTERVAL_MS)) != 0) {
#else
        if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) != 0) {
#endif
            ZIGBEE_APP_GET_NOTIFY(sevent);

            switch (sevent) {
                case ZB_APP_EVENT_INIT: {
                    zigbee_app_nwk_start(ZIGBEE_CHANNEL_ALL_MASK(), 32, 0);
                    log_info("ZigBee APP init");
                    if (reset_to_default) {
                        zigbee_do_factory_reset();
                    }
                } break;

                case ZB_APP_EVENT_NOT_JOINED: {

                    ZB_THREAD_SAFE(bdb_start_top_level_commissioning(
                        ZB_BDB_NETWORK_STEERING));
                    log_info("ZigBee APP not joined");
                } break;

                case ZB_APP_EVENT_JOINED: {
                    log_info("ZigBee APP joined");
                    g_joined_network = 1;
                    scene_db_check();
                    startup_db_check();
                    set_startup_status();
                                    
                    uint8_t mCurrentProtocol = 1;
                    efd_set_env_blob("prot", &mCurrentProtocol, sizeof(uint8_t));
                    size_t actual_len;
                    efd_get_env_blob("prot", (void *) &mCurrentProtocol, sizeof(uint8_t), &actual_len);
                    if(actual_len == 0) {
                        log_info("Protocol not found,");
                    }
                    else
                    {
                        log_info("Protocol: %s", mCurrentProtocol ==0 ? "None" : 
                        mCurrentProtocol == 1 ? "Zigbee" : "Matter");
                    }
                    ZB_THREAD_SAFE(g_panid = zb_get_pan_id();
                                   g_short_addr = zb_get_short_address();
                                   g_joined_channel = zb_get_current_channel();)

                    log_info("PAN ID: %04X, Short Addr: %04X, Channel: %d",
                             g_panid, g_short_addr, g_joined_channel);

                } break;
                default: break;
            }
        }
    }
}