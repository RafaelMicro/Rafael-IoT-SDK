#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_rtc.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"

volatile uint8_t rtc_alarm;

void rtc_callback(uint32_t rtc_status) {
    printf("rtc_status %.8x\r\n", rtc_status);
    rtc_alarm = 1;
}

void global_var_init() {
    rtc_alarm = 0;
}

int32_t main(void) {
    uint32_t i;
    hosal_rtc_time_t current_time, alarm_tm;
    uint32_t alarm_mode;
    uint32_t status;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    global_var_init();
    printf("/*******Start RTC Event********/\r\n");
    
    hosal_rtc_get_time(&current_time);
#if defined(CONFIG_RT584S) || defined(CONFIG_RT584H) || defined(CONFIG_RT584L)
#if defined(CONFIG_RCO32K_ENABLE)
    hosal_rtc_set_clk(0x200000);
#elif defined(CONFIG_RCO16K_ENABLE)
    hosal_rtc_set_clk(0x100000);
#elif defined(CONFIG_RCO20K_ENABLE)
    hosal_rtc_set_clk(0x140000);
#endif
    printf("unsetting time is %2d-%2d-%2d %2d:%2d:%2d.%3d\r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec,
           current_time.tm_msec);
#else
    hosal_rtc_set_clk(40000);
    printf("unsetting time is %2d-%2d-%2d %2d:%2d:%2d\r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
#endif
    printf("set current time\r\n");
    current_time.tm_year = 23;
    current_time.tm_mon = 12;
    current_time.tm_day = 13;
    current_time.tm_hour = 12;
    current_time.tm_min = 20;
    current_time.tm_sec = 32;
#if defined(CONFIG_RT584S)
    current_time.tm_msec = 12;
#endif

    hosal_rtc_set_time(&current_time);

    hosal_rtc_get_time(&current_time);

#if defined(CONFIG_RT584S)
    printf("setting time is %2d-%2d-%2d %2d:%2d:%2d.%3d \r\n", current_time.tm_year,
           current_time.tm_mon, current_time.tm_day, current_time.tm_hour,
           current_time.tm_min, current_time.tm_sec, current_time.tm_msec);

    alarm_tm.tm_year = 23;
    alarm_tm.tm_mon = 12;
    alarm_tm.tm_day = 13;
    alarm_tm.tm_hour = 12;
    alarm_tm.tm_min = 21;
    alarm_tm.tm_sec = 35;
    alarm_tm.tm_msec = 382;
#else
    printf("setting time is %2d-%2d-%2d %2d:%2d:%2d\r\n", current_time.tm_year,
           current_time.tm_mon, current_time.tm_day, current_time.tm_hour,
           current_time.tm_min, current_time.tm_sec);

    alarm_tm.tm_year = 23;
    alarm_tm.tm_mon = 12;
    alarm_tm.tm_day = 13;
    alarm_tm.tm_hour = 12;
    alarm_tm.tm_min = 21;
    alarm_tm.tm_sec = 35;
#endif
    alarm_mode = HOSAL_RTC_MODE_HOUR_EVENT_INTERRUPT
                 | HOSAL_RTC_MODE_EVENT_INTERRUPT;

#if defined(CONFIG_RT584S)
            printf("setting alarm time is %2d-%2d-%2d %2d:%2d:%2d.%3d \r\n",
                   alarm_tm.tm_year, alarm_tm.tm_mon, alarm_tm.tm_day,
                   alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_sec,
                   alarm_tm.tm_msec);
#else
            printf("setting alarm time is %2d-%2d-%2d %2d:%2d:%2d\r\n",
                   alarm_tm.tm_year, alarm_tm.tm_mon, alarm_tm.tm_day,
                   alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_sec);
#endif

    hosal_rtc_set_alarm(&alarm_tm, alarm_mode, rtc_callback);
    NVIC_EnableIRQ(Rtc_IRQn);

#if defined(CONFIG_RT584S)
    current_time.tm_msec = 12;
#endif

    while (1) {
        if (rtc_alarm) {
            puts("specify time alarm\r\n");
            rtc_alarm = 0;
            hosal_rtc_get_time(&current_time);
#if defined(CONFIG_RT584S)
            printf("current time is %2d-%2d-%2d %2d:%2d:%2d.%3d \r\n",
                   current_time.tm_year, current_time.tm_mon,
                   current_time.tm_day, current_time.tm_hour,
                   current_time.tm_min, current_time.tm_sec,
                   current_time.tm_msec);
#else
            printf("current time is %2d-%2d-%2d %2d:%2d:%2d\r\n",
                   current_time.tm_year, current_time.tm_mon,
                   current_time.tm_day, current_time.tm_hour,
                   current_time.tm_min, current_time.tm_sec);
#endif
        }
    }
}
