#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_timer.h"
#include "log.h"
#include "task.h"
#include "hosal_uart.h"
#define RT_TIMER0 0
#define RT_TIMER1 1
#define RT_TIMER2 2

/************************************************************/
uint8_t timer0_done, timer1_done, timer2_done;

/* timer0 callback function */
void timer0_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_timer_current_get(timer_id);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_timer_current_get(timer_id);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* timer2 callback function */
void timer2_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_timer_current_get(timer_id);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    return;
}

void timer_freerun_upcnt(void) {
    hosal_timer_config_t cfg0, cfg1, cfg2;
    hosal_timer_tick_config_t tick_cfg0, tick_cfg1, tick_cfg2;

    /* This setting is 500ms timeout */
    cfg0.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg0.int_enable = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg0.one_shot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = 0;
    tick_cfg0.timeout_ticks = 1000000;

    /* This setting is 1s timeout */
    cfg1.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg1.int_enable = HOSAL_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg1.one_shot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;

    tick_cfg1.timeload_ticks = 0;
    tick_cfg1.timeout_ticks = 1000000;

    /* This setting is 8s timeout */
    cfg2.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg2.int_enable = HOSAL_TIMER_INT_ENABLE;
    cfg2.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg2.one_shot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg2.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg2.user_prescale = 0;

    tick_cfg2.timeload_ticks = 0;
    tick_cfg2.timeout_ticks = 8000000;

    hosal_timer_init(RT_TIMER0, cfg0, timer0_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer0_IRQn));
    timer0_done = 0;
    hosal_timer_init(RT_TIMER1, cfg1, timer1_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer1_IRQn));
    timer1_done = 0;
    hosal_timer_init(RT_TIMER2, cfg2, timer2_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer2_IRQn));
    timer2_done = 0;

    hosal_timer_start(RT_TIMER0, tick_cfg0);
    hosal_timer_start(RT_TIMER1, tick_cfg1);
    hosal_timer_start(RT_TIMER2, tick_cfg2);
}

int main(void) {

    puts("/*****Start Timer Freerun Upcount*****/\r\n");
    puts("\r\n");

    timer_freerun_upcnt();

    while (1) {
        if (timer0_done) {
            hosal_timer_finalize(RT_TIMER0);
            NVIC_DisableIRQ((IRQn_Type)(Timer0_IRQn));
        }

        if (timer1_done) {
            hosal_timer_finalize(RT_TIMER1);
            NVIC_DisableIRQ((IRQn_Type)(Timer1_IRQn));
        }

        if (timer2_done) {
            hosal_timer_finalize(RT_TIMER2);
            NVIC_DisableIRQ((IRQn_Type)(Timer2_IRQn));
        }
    }
    return 0;
}
