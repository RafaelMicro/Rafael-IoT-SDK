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
uint8_t timer0_done, timer1_done;

/* timer0 callback function */
void timer0_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_timer_current_get(timer_id);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    timer0_done = 1;
    return;
}

/* timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    value = hosal_timer_current_get(timer_id);
    printf("Timer%d, value:%d\r\n", timer_id, value);
    timer1_done = 1;
    return;
}


void timer_oneshot(void) {
    hosal_timer_config_t cfg0, cfg1;
    hosal_timer_tick_config_t tick_cfg0, tick_cfg1;

    /* This setting is 500ms timeout */
    cfg0.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg0.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg0.oneshot_mode = HOSAL_TIMER_ONE_SHOT_ENABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = 1000000;
    tick_cfg0.timeout_ticks = 0;

    /* This setting is 1s timeout */
    cfg1.counting_mode = HOSAL_TIMER_DOWN_COUNTING;
    cfg1.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_TIMER_FREERUN_MODE;
    cfg1.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;

    tick_cfg1.timeload_ticks = 1000000;
    tick_cfg1.timeout_ticks = 0;

    hosal_timer_init(RT_TIMER0, cfg0, timer0_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer0_IRQn));
    timer0_done = 0;
    hosal_timer_init(RT_TIMER1, cfg1, timer1_cb);
    NVIC_EnableIRQ((IRQn_Type)(Timer1_IRQn));
    timer1_done = 0;

    hosal_timer_start(RT_TIMER0, tick_cfg0);
    hosal_timer_start(RT_TIMER1, tick_cfg1);
}

int main(void) {

    puts("/*****Start Timer Freerun Downcount*****/\r\n");
    puts("Timer 0 one shot enable, timer 1 is disable\r\n");
    puts("You can see when timer 0 timeout trigger\r\n");
    puts("you get the current value is timeout ticks, \r\n");
    puts("but timer1 can see timeout trigger, the current value\r\n");
    puts("keep count.\r\n");

    timer_oneshot();

    while (1) {
        if (timer0_done) {
            timer0_done = 0;
            hosal_timer_finalize(RT_TIMER0);
            NVIC_DisableIRQ((IRQn_Type)(Timer0_IRQn));
            puts("timer0 finalize\r\n");
        }

        if (timer1_done) {
            timer1_done = 0;
            hosal_timer_finalize(RT_TIMER1);
            NVIC_DisableIRQ((IRQn_Type)(Timer1_IRQn));
            puts("timer1 finalize\r\n");
        }
    }
    return 0;
}
