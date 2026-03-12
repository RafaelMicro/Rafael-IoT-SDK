/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config/platform.h>
#include "EnhancedFlashDataset.h"
#include "FreeRTOS.h"
#include "app_hooks.h"
#include "app_task.h"
#include "cli.h"
#include "hosal_dma.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_wdt.h"
#include "main.h"
#include "mcu.h"
#include "miu_ext_mem.h"
#include "task.h"
#include "uart_stdio.h"
#if defined(CONFIG_RF1301) || defined(CONFIG_RT584H) ||  defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L)
#include "hosal_dpd.h"
#endif

#ifndef CONFIG_MIU_DEVICE_TYPE_RCP
#include "dump_boot_info.h"
#endif

#ifndef CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE
#define CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE 8192
#endif

static void pin_mux_init(void) {
    int i;
    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        if ((i != 16) && (i != 17)) {
            hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
            hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
        }
    }
    return;
}

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
static void _init_sleep() {
    //Initialize low power wakeup source, like uart.
    // lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_UART0_RX);
    printf("_init_sleep:%d \r\n", CONFIG_HOSAL_SOC_SLEEP_TIMER_ID);
}
#endif

void vApplicationMallocFailedHook(void) {
    extern void extMemory(void);
    printf("Memory Allocate Failed! Left heap: %u bytes\r\n",
           xPortGetFreeHeapSize());
    extMemory();

    taskDISABLE_INTERRUPTS();
    while (1) {}
}

void wdt_cb(void) {
    /* show when lock enable, can not change wdt setting*/
    hosal_wdt_config_mode_t cfg;
    hosal_wdt_config_tick_t tick;

    cfg.int_enable = 1;
    cfg.lock_enable = 0;
    cfg.prescale = HOSAL_WDT_PRESCALE_32;
    cfg.reset_enable = 0;

    tick.wdt_ticks = 0xFFFFFFFF;
    tick.wdt_min_ticks = 0;
    tick.int_ticks = 0xFFFFFFFF - 4000000;

    hosal_wdt_start(cfg, tick, wdt_cb);

    hosal_wdt_kick();
}

void init_wdt_init(void) {
    hosal_wdt_config_mode_t cfg;
    hosal_wdt_config_tick_t tick;

    cfg.int_enable = 1;
    cfg.lock_enable = 0;
    cfg.prescale = HOSAL_WDT_PRESCALE_32;
    cfg.reset_enable = 0;

    tick.wdt_ticks = 0xFFFFFFFF;
    tick.wdt_min_ticks = 0;
    tick.int_ticks = 0xFFFFFFFF - 1000000;

    hosal_wdt_start(cfg, tick, wdt_cb);
    NVIC_EnableIRQ(Wdt_IRQn);
}

static void app_task_entry(void* pvParameters) {
    /*watch dog init*/
    init_wdt_init(); // debug should be close

    /*falsh Protection Mechanism*/
    enhanced_flash_dataset_init();

    /*sdk cli init*/
    cli_init();

    // common init (ex. RF, PIB)
    app_common_init();

    // start (app_task)
    app_task();

    vTaskDelete(NULL);
}

int main(void) {
    /*gpio init*/
    pin_mux_init();

    /*freertos heap init*/
    vHeapRegionsInt();

    /*debug uart init*/
    uart_stdio_init();

    /*heap lock init*/
    heapLockInit();

    /*dma init; RT58x need call*/
    hosal_dma_init();

#ifndef CONFIG_MIU_DEVICE_TYPE_RCP
    /*boot information dump*/
    _dump_boot_info();
#if 0 //defined(CONFIG_RF1301) || defined(CONFIG_RT584H) ||  defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L)
    printf("reset cause: %8x\r\n", hosal_get_all_reset_cause());
    if (hosal_reset_by_wdt()) {
        puts("reset by watch dog timer \r\n");
        clear_reset_cause();
    }
#endif /* defined(CONFIG_RT584) */
#endif

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    _init_sleep();
#endif

    /*application task start*/
    if (xTaskCreate(app_task_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }

    vTaskStartScheduler();
    while (1) {}

    return 0;
}
