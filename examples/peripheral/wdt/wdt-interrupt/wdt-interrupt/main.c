#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_wdt.h"
#include "app_hooks.h"
#include "uart_stdio.h"


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

    puts("WDT Interrupt\r\n");
    hosal_wdt_kick();
}

void init_wdt(void) {
    hosal_wdt_config_mode_t cfg;
    hosal_wdt_config_tick_t tick;

    cfg.int_enable = 1;
    cfg.lock_enable = 1;
    cfg.prescale = HOSAL_WDT_PRESCALE_32;
    cfg.reset_enable = 0;

    tick.wdt_ticks = 0xFFFFFFFF;
    tick.wdt_min_ticks = 0;
    tick.int_ticks = 0xFFFFFFFF - 1000000;

    hosal_wdt_start(cfg, tick, wdt_cb);
    NVIC_EnableIRQ(Wdt_IRQn);
    puts("WDT start\r\n");
}

int32_t main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/*****Start WDT Interrupt******/\r\n");

    init_wdt();

    while(1) {}

    return 0;
}

