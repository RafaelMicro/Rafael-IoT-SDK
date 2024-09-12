#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"
#include "hosal_wdt.h"
#if defined(CONFIG_RT584)
#include "hosal_dpd.h"
#endif

void init_wdt(void) {
    hosal_wdt_config_mode_t cfg;
    hosal_wdt_config_tick_t tick;

    cfg.int_enable = 0;
    cfg.lock_enable = 1;
    cfg.prescale = HOSAL_WDT_PRESCALE_32;
    cfg.reset_enable = 1;

    tick.wdt_ticks = 4000000;
    tick.wdt_min_ticks = 1000000;
    tick.int_ticks = 0xFFFFFFFF;

    hosal_wdt_start(cfg, tick, NULL);
    NVIC_DisableIRQ(Wdt_IRQn);
    puts("WDT start\r\n");
}

int32_t main(void) {
    uint32_t reset_cnt;
    
    puts("/*****Start WDT Reset******/\r\n");
    

#if defined (CONFIG_RT584)
    printf("reset cause: %8x\r\n", hosal_get_all_reset_cause());
    if (hosal_reset_by_wdt()) {
        puts("reset by watch dog timer \r\n");
        clear_reset_cause();
    }
    reset_cnt = hosal_wdt_reset_event_get();
    printf("number of resets:%d\r\n", reset_cnt);

    if (reset_cnt > 10) {
        hosal_wdt_reset_event_clear();
        puts("clear wdt number of resets\r\n");

        reset_cnt = hosal_wdt_reset_event_get();
        printf("After clean, number of resets:%d\r\n", reset_cnt);
    }
#else /* defined(CONFIG_RT584) */
        reset_cnt = hosal_wdt_reset_event_get();
    printf("number of resets:%d",reset_cnt);
    puts("\r\n");

    if( reset_cnt > 10 ){
        hosal_wdt_reset_event_clear();
        puts("clear wdt number of resets\r\n");

        reset_cnt = hosal_wdt_reset_event_get();
        printf("After clean, number of resets:%d",reset_cnt);
        puts("\r\n");
    } 
#endif /* !defined(CONFIG_RT584) */

    init_wdt();

    while (1) {}
}

