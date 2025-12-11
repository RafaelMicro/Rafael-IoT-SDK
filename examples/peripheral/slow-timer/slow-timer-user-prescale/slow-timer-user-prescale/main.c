/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_slow_timer.h"
#include "app_hooks.h"
#include "uart_stdio.h"


#define RT_SLOW_TIMER0     0
#define RT_SLOW_TIMER1     1


/************************************************************/

/* timer0 callback function */
void timer0_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_slow_timer_current_get(timer_id, &value);
    printf("Slow Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_slow_timer_current_get(timer_id, &value);
    printf("Slow Timer%d, value:%d\r\n", timer_id, value);
    return;
}


void slow_timer_freerun_upcnt(void) {
    hosal_slow_timer_config_t cfg0, cfg1;
    hosal_slow_timer_tick_config_t tick_cfg0, tick_cfg1;

    /* This setting is 10s timeout */
    cfg0.counting_mode = HOSAL_SLOW_TIMER_UP_COUNTING;
    cfg0.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_SLOW_TIMER_FREERUN_MODE;
    cfg0.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_DISABLE;
    /* when user_prescale not equal 0, prescale is ignore */
    cfg0.prescale = HOSAL_SLOW_TIMER_PRESCALE_16;
    cfg0.user_prescale = 319;
    cfg0.repeat_delay = 0;

    tick_cfg0.timeload_ticks = 0;
    tick_cfg0.timeout_ticks = 1000;

    /* This setting is 20s timeout */
    cfg1.counting_mode = HOSAL_SLOW_TIMER_UP_COUNTING;
    cfg1.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_SLOW_TIMER_FREERUN_MODE;
    cfg1.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_DISABLE;
    /* when user_prescale not equal 0, prescale is ignore */
    cfg1.prescale = HOSAL_SLOW_TIMER_PRESCALE_32;
    cfg1.user_prescale = 639;
    cfg1.repeat_delay = 0;

    tick_cfg1.timeload_ticks = 0;
    tick_cfg1.timeout_ticks = 1000;

    hosal_slow_timer_init(RT_SLOW_TIMER0, cfg0, timer0_cb);
    hosal_slow_timer_init(RT_SLOW_TIMER1, cfg1, timer1_cb);

    NVIC_EnableIRQ((IRQn_Type)(SlowTimer0_IRQn));
    NVIC_EnableIRQ((IRQn_Type)(SlowTimer1_IRQn));
    

    hosal_slow_timer_start(RT_SLOW_TIMER0, tick_cfg0);
    hosal_slow_timer_start(RT_SLOW_TIMER1, tick_cfg1);
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);
           
    puts("/*****Start Slow Timer User Prescale*****/ \r\n");
    
    slow_timer_freerun_upcnt();

    while (1) {}
    return 0;
}

