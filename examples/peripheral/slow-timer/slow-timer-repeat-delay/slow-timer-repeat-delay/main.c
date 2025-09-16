#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_slow_timer.h"
#include "hosal_lpm.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "hosal_sysctrl.h"

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
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

    hosal_slow_timer_current_get(timer_id, &value);
    printf("Slow Timer%d, value:%d\r\n", timer_id, value);
    return;
}

/* slow timer1 callback function */
void timer1_cb(uint32_t timer_id) {
    uint32_t value;

    hosal_slow_timer_current_get(timer_id, &value);
    printf("Slow Timer%d, value:%d\r\n", timer_id, value);
    return;
}


void slow_timer_periodic(void) {
    hosal_slow_timer_config_t cfg0, cfg1;
    hosal_slow_timer_tick_config_t tick_cfg0, tick_cfg1;

    /* This setting is 500ms timeout */
    cfg0.counting_mode = HOSAL_SLOW_TIMER_DOWN_COUNTING;
    cfg0.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg0.mode = HOSAL_SLOW_TIMER_PERIODIC_MODE;
    cfg0.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_DISABLE;
    cfg0.prescale = HOSAL_SLOW_TIMER_PRESCALE_16;
    cfg0.user_prescale = 0;
    cfg0.repeat_delay = 9;

    
    tick_cfg0.timeload_ticks = 1000;
    tick_cfg0.timeout_ticks = 0;

    /* This setting is 1s timeout */
    cfg1.counting_mode = HOSAL_SLOW_TIMER_DOWN_COUNTING;
    cfg1.int_enable = HOSAL_SLOW_TIMER_INT_ENABLE;
    cfg1.mode = HOSAL_SLOW_TIMER_PERIODIC_MODE;
    cfg1.one_shot_mode = HOSAL_SLOW_TIMER_ONE_SHOT_DISABLE;
    cfg1.prescale = HOSAL_SLOW_TIMER_PRESCALE_32;
    cfg1.user_prescale = 0;
    cfg1.repeat_delay = 4;

    tick_cfg1.timeload_ticks = 1000;
    tick_cfg1.timeout_ticks = 0;
    

    hosal_slow_timer_init(RT_SLOW_TIMER0, cfg0, timer0_cb);
    hosal_slow_timer_init(RT_SLOW_TIMER1, cfg1, timer1_cb);
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    NVIC_EnableIRQ((IRQn_Type)(SlowTimer0_IRQn));
    NVIC_EnableIRQ((IRQn_Type)(SlowTimer1_IRQn));
#else
    NVIC_EnableIRQ((IRQn_Type)(Timer3_IRQn));
    NVIC_EnableIRQ((IRQn_Type)(Timer4_IRQn));
#endif

    hosal_slow_timer_start(RT_SLOW_TIMER0, tick_cfg0);
    hosal_slow_timer_start(RT_SLOW_TIMER1, tick_cfg1);
}

int32_t main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    printf("/*****Start Slow Timer Repeat Delay*****/\r\n");
    
    slow_timer_periodic();
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_SLOW_TIMER);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_SLOW_TIMER);
#endif
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    
    while (1) {
        printf("sleep\r\n");
        hosal_delay_ms(1);
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
    return 0;
}

