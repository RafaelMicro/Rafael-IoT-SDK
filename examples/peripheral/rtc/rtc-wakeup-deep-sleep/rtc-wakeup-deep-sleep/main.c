#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_rtc.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
#include "hosal_dpd.h"
#endif
#include "app_hooks.h"
#include "uart_stdio.h"

volatile uint8_t rtc_alarm;

void rtc_callback(uint32_t rtc_status) {
    rtc_alarm = 1;
}

void global_var_init() {
    rtc_alarm = 0;
}

int32_t main(void) {
    uint32_t i;
    hosal_rtc_time_t current_time, alarm_tm;
    uint32_t alarm_mode;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    global_var_init();
    printf("/*******Start RTC Wakeup From Deep Sleep********/\r\n");
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
#if defined(CONFIG_RCO32K_ENABLE)
    hosal_rtc_set_clk(0x200000);
#elif defined(CONFIG_RCO16K_ENABLE)
    hosal_rtc_set_clk(0x100000);
#elif defined(CONFIG_RCO20K_ENABLE)
    hosal_rtc_set_clk(0x140000);
#endif
    hosal_rtc_get_time(&current_time);
    printf("unsetting time is %2d-%2d-%2d %2d:%2d:%2d.%3d\r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec,
           current_time.tm_msec);
#else
    hosal_rtc_set_clk(40000);
    hosal_rtc_get_time(&current_time);
    printf("unsetting time is %2d-%2d-%2d %2d:%2d:%2d\r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
#endif

#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    printf("Deep sleep wake up,%.8lx\r\n", hosal_get_all_reset_cause());
    if ( hosal_reset_by_deep_sleep() ) {
        hosal_clear_reset_cause();
    } else {
        current_time.tm_year = 23;
        current_time.tm_mon = 12;
        current_time.tm_day = 13;
        current_time.tm_hour = 12;
        current_time.tm_min = 20;
        current_time.tm_sec = 32;
        current_time.tm_msec = 0;
        hosal_rtc_set_time(&current_time);
    }
#else
    current_time.tm_year = 23;
    current_time.tm_mon = 12;
    current_time.tm_day = 13;
    current_time.tm_hour = 12;
    current_time.tm_min = 20;
    current_time.tm_sec = 32;

    hosal_rtc_set_time(&current_time);
#endif

    hosal_rtc_get_time(&current_time);
    #if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
            printf("setting time is %2d-%2d-%2d %2d:%2d:%2d.%3d\r\n",
                    current_time.tm_year, current_time.tm_mon, current_time.tm_day,
                    current_time.tm_hour, current_time.tm_min, current_time.tm_sec,
                    current_time.tm_msec);
            #else
            printf("setting time is %2d-%2d-%2d %2d:%2d:%2d \r\n",
                    current_time.tm_year, current_time.tm_mon, current_time.tm_day,
                    current_time.tm_hour, current_time.tm_min, current_time.tm_sec);

            #endif
    
    alarm_tm.tm_year = 23;
    alarm_tm.tm_mon = 12;
    alarm_tm.tm_day = 13;
    alarm_tm.tm_hour = 12;
    alarm_tm.tm_min = 21;
    alarm_tm.tm_sec = 35;
    alarm_mode = HOSAL_RTC_MODE_EVERY_MIN_INTERRUPT
                 | HOSAL_RTC_MODE_EN_MIN_INTERRUPT;
    printf("setting alarm time is every minute \r\n");

    NVIC_EnableIRQ(Rtc_IRQn);
    hosal_rtc_set_alarm(&alarm_tm, alarm_mode, rtc_callback);


#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_RTC_TIMER);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,
                     HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
#else
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_RTC_TIMER);
#endif
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_DEEP_SLEEP);
    
    while (1) {
        printf("sleep\r\n");
        hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);
    }
    return 0;
}
