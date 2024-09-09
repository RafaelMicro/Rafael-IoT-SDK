#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_slow_timer.h"

#if defined(CONFIG_RT584)
#define RT_SLOW_TIMER0     0
#define RT_SLOW_TIMER1     1
#else
#define RT_SLOW_TIMER0     3
#define RT_SLOW_TIMER1     4
#endif

/************************************************************/

/* slow timer0 callback function */
void timer0_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_slow_timer_current_get(timer_id);
    printf("Slow Timer%d, value:%d", timer_id, value);
    puts("\r\n");
    return;
}

/* slow timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_slow_timer_current_get(timer_id);
    printf("Slow Timer%d, value:%d", timer_id, value);
    puts("\r\n");
    return;
}


void slow_timer_periodic(void) {
    hosal_slow_timer_config_t cfg0, cfg1;
    hosal_slow_timer_tick_config_t tick_cfg0, tick_cfg1;

    /* This setting is 500ms timeout */
    cfg0.counting_mode = HOSAL_SLOW_TIMER_DOWN_COUNTING;
    cfg0.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_SLOW_TIMER_PERIODIC_MODE;
    cfg0.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_ENABLE;
    cfg0.prescale = HOSAL_SLOW_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;
    cfg0.repeat_delay = 0;

    
    tick_cfg0.timeload_ticks = 1000;
    tick_cfg0.timeout_ticks = 0;

    /* This setting is 1s timeout */
    cfg1.counting_mode = HOSAL_SLOW_TIMER_DOWN_COUNTING;
    cfg1.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_SLOW_TIMER_PERIODIC_MODE;
    cfg1.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_SLOW_TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;
    cfg1.repeat_delay = 0;

    tick_cfg1.timeload_ticks = 1000;
    tick_cfg1.timeout_ticks = 0;
    

    hosal_slow_timer_init(RT_SLOW_TIMER0, cfg0, timer0_cb);
    hosal_slow_timer_init(RT_SLOW_TIMER1, cfg1, timer1_cb);


    hosal_slow_timer_start(RT_SLOW_TIMER0, tick_cfg0);
    hosal_slow_timer_start(RT_SLOW_TIMER1, tick_cfg1);
}

int main(void) {
    printf("/*****Start Slow Timer Periodic*****/");
    puts("\r\n");

    slow_timer_periodic();

    while (1) {}
    return 0;
}

