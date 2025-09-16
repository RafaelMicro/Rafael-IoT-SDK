#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_wdt.h"
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
#include "hosal_dpd.h"
#endif
#include "app_hooks.h"
#include "uart_stdio.h"


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

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    
    printf("/*****Start WDT Reset TEST******/\r\n");
    

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    printf("reset cause: %8x\r\n", hosal_get_all_reset_cause());
    if (hosal_reset_by_wdt()) {
        puts("reset by watch dog timer \r\n");
        clear_reset_cause();
    }
#endif /* defined(CONFIG_RT584) */
    hosal_wdt_reset_event_get(&reset_cnt);
    printf("number of resets:%d\r\n",reset_cnt);

    if( reset_cnt > 10 ){
        hosal_wdt_reset_event_clear();
        printf("clear wdt number of resets\r\n");

        hosal_wdt_reset_event_get(&reset_cnt);
        printf("After clean, number of resets:%d\r\n",reset_cnt);
    }

    init_wdt();

    printf("\r\n");

    while(1) {}
    
    return 0;
}

