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
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "app_hooks.h"
#include "uart_stdio.h"

#define RT_TIMER0     0
#define RT_TIMER1     1
#define RT_TIMER2     2

void timer_pwm(void) {
    hosal_timer_pwm_config_mode_t cfg0, cfg1, cfg2;
    hosal_timer_pwm_config_tick_t tick_cfg0, tick_cfg1, tick_cfg2;

    hosal_pin_set_mode(GPIO4, HOSAL_MODE_PWM0);      /*GPIO4 as PWM0*/
    hosal_pin_set_mode(GPIO5, HOSAL_MODE_PWM1);      /*GPIO5 as PWM1*/
    hosal_pin_set_mode(GPIO20, HOSAL_MODE_PWM2);     /*GPIO20 as PWM2*/
    hosal_pin_set_mode(GPIO21, HOSAL_MODE_PWM3);     /*GPIO21 as PWM3*/
    hosal_pin_set_mode(GPIO30, HOSAL_MODE_PWM4);     /*GPIO30 as PWM4*/

    //Timer0
    cfg0.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg0.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg0.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = HOSAL_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;
    cfg0.pwm0_enable = 1;
    cfg0.pwm1_enable = 0;
    cfg0.pwm2_enable = 0;
    cfg0.pwm3_enable = 0;
    cfg0.pwm4_enable = 0;
    hosal_timer_pwm_open(RT_TIMER0, cfg0);

    tick_cfg0.timeload_ticks = 0;
    tick_cfg0.timeout_ticks = 2000;
    tick_cfg0.threshold = 1000;
    tick_cfg0.phase = 0;
    hosal_timer_pwm_start(RT_TIMER0, tick_cfg0);

    //Timer1
    cfg1.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg1.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg1.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_TIMER_PRESCALE_16;
    cfg1.user_prescale = 0;
    cfg1.pwm0_enable = 0;
    cfg1.pwm1_enable = 1;
    cfg1.pwm2_enable = 1;
    cfg1.pwm3_enable = 0;
    cfg1.pwm4_enable = 0;
    hosal_timer_pwm_open(RT_TIMER1, cfg1);

    tick_cfg1.timeload_ticks = 0;
    tick_cfg1.timeout_ticks = 2000;
    tick_cfg1.threshold = 1000;
    tick_cfg1.phase = 1;
    hosal_timer_pwm_start(RT_TIMER1, tick_cfg1);

    //Timer2
    cfg2.counting_mode = HOSAL_TIMER_UP_COUNTING;
    cfg2.int_en = HOSAL_TIMER_INT_ENABLE;
    cfg2.mode = HOSAL_TIMER_PERIODIC_MODE;
    cfg2.oneshot_mode = HOSAL_TIMER_ONE_SHOT_DISABLE;
    cfg2.prescale = HOSAL_TIMER_PRESCALE_32;
    cfg2.user_prescale = 0;
    cfg2.pwm0_enable = 0;
    cfg2.pwm1_enable = 0;
    cfg2.pwm2_enable = 0;
    cfg2.pwm3_enable = 1;
    cfg2.pwm4_enable = 1;
    hosal_timer_pwm_open(RT_TIMER2, cfg2);

    tick_cfg2.timeload_ticks = 0;
    tick_cfg2.timeout_ticks = 2000;
    tick_cfg2.threshold = 1000;
    tick_cfg2.phase = 1;
    hosal_timer_pwm_start(RT_TIMER2, tick_cfg2);
}


int main(void) {
    
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/****************************/\r\n");
    printf("/******START Timer PWM*******/\r\n");
    printf("/****************************/\r\n");

    timer_pwm();

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO0);
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);

    while (1) {
        printf("Enter sleep\r\n");
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }

    return 0;
}
