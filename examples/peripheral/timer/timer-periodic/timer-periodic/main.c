#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_timer.h"
#include "app_hooks.h"
#include "uart_stdio.h"


#define RT_TIMER0     0
#define RT_TIMER1     1
#define RT_TIMER2     2

/************************************************************/

/* timer0 callback function */
void timer0_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_timer_current_get(timer_id, &value);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_timer_current_get(timer_id, &value);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* timer2 callback function */
void timer2_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_timer_current_get(timer_id, &value);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

void timer_periodic_mode(void) {
    hosal_timer_config_t cfg0, cfg1, cfg2;
    hosal_timer_tick_config_t tick_cfg0, tick_cfg1, tick_cfg2;

    /* This setting is 500ms timeout */
    cfg0.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg0.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg0.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = 1000000;
    tick_cfg0.timeout_ticks = 0;

    /* This setting is 1s timeout */
    cfg1.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg1.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg1.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;
    
    tick_cfg1.timeload_ticks = 1000000;
    tick_cfg1.timeout_ticks = 0;

    /* This setting is 8s timeout */
    cfg2.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg2.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg2.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg2.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg2.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg2.user_prescale = 0;

    tick_cfg2.timeload_ticks = 4000000;
    tick_cfg2.timeout_ticks = 0;

    hosal_timer_init(RT_TIMER0, cfg0, timer0_cb);
    hosal_timer_init(RT_TIMER1, cfg1, timer1_cb);
    hosal_timer_init(RT_TIMER2, cfg2, timer2_cb);

    NVIC_EnableIRQ((IRQn_Type)(Timer0_IRQn));
    NVIC_EnableIRQ((IRQn_Type)(Timer1_IRQn));
    NVIC_EnableIRQ((IRQn_Type)(Timer2_IRQn));

    hosal_timer_start(RT_TIMER0, tick_cfg0);
    hosal_timer_start(RT_TIMER1, tick_cfg1);
    hosal_timer_start(RT_TIMER2, tick_cfg2);
}


int32_t main(void) {
    
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/*****Start Timer Periodic*****/");
    printf("\r\n");

    timer_periodic_mode();

    while(1) {}

    return 0;
}
