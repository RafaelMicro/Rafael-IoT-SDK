#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"
#include "hosal_wdt.h"

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
    uint32_t save_case;
    
    puts("/*****Start WDT Interrupt******/\r\n");

    init_wdt();

    while (1) {}
}

