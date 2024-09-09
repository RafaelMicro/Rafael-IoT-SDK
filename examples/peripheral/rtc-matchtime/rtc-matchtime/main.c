#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_rtc.h"

volatile uint8_t rtc_alarm;

void rtc_callback(uint32_t rtc_status)
{
    rtc_alarm = 1;
}

void global_var_init() {
    rtc_alarm = 0;
}

int32_t main(void)
{
    uint32_t i;
    rtc_time_t current_time, alarm_tm;
    uint32_t alarm_mode;

    global_var_init();
    puts("/*******Start RTC Match Time********/\r\n");

    hosal_rtc_set_clk(40000);
    hosal_rtc_get_time(&current_time);
    printf("unsetting time is %2d-%2d-%2d %2d:%2d:%2d \r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec);

    current_time.tm_year = 23;
    current_time.tm_mon = 12;
    current_time.tm_day = 13;
    current_time.tm_hour = 12;
    current_time.tm_min = 20;
    current_time.tm_sec = 32;

    hosal_rtc_set_time(&current_time);

    hosal_rtc_get_time(&current_time);
    printf("setting time is %2d-%2d-%2d %2d:%2d:%2d \r\n",
           current_time.tm_year, current_time.tm_mon, current_time.tm_day,
           current_time.tm_hour, current_time.tm_min, current_time.tm_sec);

    alarm_tm.tm_sec  = 35;
    alarm_mode = RTC_MODE_MATCH_SEC_INTERRUPT | RTC_MODE_EN_SEC_INTERRUPT;

    printf("setting match %2d second alarm\r\n", alarm_tm.tm_sec);

    hosal_rtc_set_alarm(&alarm_tm, alarm_mode, rtc_callback);

    while (1) {
        if (rtc_alarm) {
            puts("match time alarm\r\n");
            rtc_alarm = 0;
            hosal_rtc_get_time(&current_time);
            printf("current time is %2d-%2d-%2d %2d:%2d:%2d \r\n",
                   current_time.tm_year, current_time.tm_mon, current_time.tm_day,
                   current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
        }
    }
}
