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
#include "hosal_gpio.h"
#include "hosal_timer.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

#define RT_TIMER0   0
#define RT_TIMER1   1
#define RT_TIMER2   2


/*TIMER0 interrupt */
void timer_handler0(uint32_t timer_id) {
    hosal_gpio_pin_toggle(GPIO22);
    return;
}

/*TIMER1 interrupt*/
void timer_cap_handler1(uint32_t timer_id) {
    uint32_t ch0_value, ch1_value, int_status;
    printf("timer_cap_handler1\r\n");
    hosal_timer_ch0_capture_int_status(timer_id, &int_status);
    if ( int_status ) {
        hosal_timer_ch0_capture_value_get(timer_id, &ch0_value);
        printf("Timer%ld, ch0_value:%ld\r\n", timer_id, ch0_value);
    }

    hosal_timer_ch0_capture_int_status(timer_id, &int_status);
    if ( int_status ) {
        hosal_timer_ch1_capture_value_get(timer_id, &ch1_value);
        printf("Timer%ld, ch1_value:%ld\r\n", timer_id, ch1_value);
    }
    return;
}

/*TIMER2 interrupt*/
void timer_cap_handler2(uint32_t timer_id) {
    uint32_t ch0_value, ch1_value, int_status;
    printf("timer_cap_handler2\r\n");
    hosal_timer_ch0_capture_int_status(timer_id, &int_status);
    if ( int_status ) {
        hosal_timer_ch0_capture_value_get(timer_id, &ch0_value);
        printf("Timer%ld, ch0_value:%ld\r\n", timer_id, ch0_value);
    }

    hosal_timer_ch1_capture_int_status(timer_id, &int_status);
    if ( int_status ) {
        hosal_timer_ch1_capture_value_get(timer_id, &ch1_value);
        printf("Timer%ld, ch1_value:%ld\r\n", timer_id, ch1_value);
    }

    return;
}

void timer_capture(void) {
    hosal_timer_config_t cfg0;
    hosal_timer_tick_config_t tick_cfg0;
    hosal_timer_capture_config_mode_t cfg1, cfg2;

    //Timer0
    cfg0.counting_mode = TIMER_UP_COUNTING;
    cfg0.int_en = TIMER_INT_ENABLE;
    cfg0.mode = TIMER_PERIODIC_MODE;
    cfg0.oneshot_mode = TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = TIMER_PRESCALE_32;
    cfg0.user_prescale = 0;

    tick_cfg0.timeload_ticks = 0;
    tick_cfg0.timeout_ticks = 1000000;

    hosal_timer_init(RT_TIMER0, cfg0, timer_handler0);
    hosal_gpio_cfg_output(GPIO22);

    //Timer1
    cfg1.counting_mode = TIMER_UP_COUNTING;
    cfg1.int_en = TIMER_INT_DISABLE;
    cfg1.mode = TIMER_PERIODIC_MODE;
    cfg1.oneshot_mode = TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;

    cfg1.ch0_capture_edge = TIMER_CAPTURE_POS_EDGE;
    cfg1.ch0_deglich_enable = TIMER_CAPTURE_DEGLICH_ENABLE;
    cfg1.ch0_int_enable = TIMER_CAPTURE_INT_ENABLE;
    cfg1.ch0_iosel = GPIO30;
    hosal_timer_capture_init(RT_TIMER1, cfg1, timer_cap_handler1);

    //Timer2
    cfg2.counting_mode = TIMER_UP_COUNTING;
    cfg2.int_en = TIMER_INT_DISABLE;
    cfg2.mode = TIMER_PERIODIC_MODE;
    cfg2.oneshot_mode = TIMER_ONE_SHOT_DISABLE;
    cfg2.prescale = TIMER_PRESCALE_16;
    cfg2.user_prescale = 0;

    cfg2.ch0_capture_edge = TIMER_CAPTURE_POS_EDGE;
    cfg2.ch0_deglich_enable = TIMER_CAPTURE_DEGLICH_ENABLE;
    cfg2.ch0_int_enable = TIMER_CAPTURE_INT_ENABLE;
    cfg2.ch0_iosel = GPIO31;
    cfg2.ch1_capture_edge = TIMER_CAPTURE_NEG_EDGE;
    cfg2.ch1_deglich_enable = TIMER_CAPTURE_DEGLICH_ENABLE;
    cfg2.ch1_int_enable = TIMER_CAPTURE_INT_ENABLE;
    cfg2.ch1_iosel = GPIO31;

    hosal_timer_capture_init(RT_TIMER2, cfg2, timer_cap_handler2);

    hosal_timer_start(RT_TIMER0, tick_cfg0);
    NVIC_EnableIRQ((IRQn_Type)(Timer0_IRQn));
    hosal_timer_capture_start(RT_TIMER1, 0, 4000000, true, false);
    NVIC_EnableIRQ((IRQn_Type)(Timer1_IRQn));
    hosal_timer_capture_start(RT_TIMER2, 0, 0xFFFFFFF, true, true);
    NVIC_EnableIRQ((IRQn_Type)(Timer2_IRQn));
}

void pin_init(void) {
    hosal_gpio_input_config_t pin_cfg;
    
    hosal_pin_set_mode(30, HOSAL_MODE_GPIO);
    hosal_pin_set_mode(31, HOSAL_MODE_GPIO);
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_NOINT;
    pin_cfg.usr_cb = NULL;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_pin_set_pullopt(GPIO30, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input_parameters(GPIO30, pin_cfg, true);

    hosal_pin_set_pullopt(GPIO31, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input_parameters(GPIO31, pin_cfg, true);

    return;
}


int main(void) {
    
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();
    pin_init();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/******************************/\r\n");
    printf("/*****Start Timer Capture******/\r\n");
    printf("/******************************/\r\n");
    printf("GPIO22 is output, GPIO30/31 is capture\r\n");

    timer_capture();

    while (1) {}
}
