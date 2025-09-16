/**
 * @file ot_alarm.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// #include "openthread-system.h"
#include <assert.h>
#include <miu_port.h>
#include <openthread/config.h>
#include <openthread/link.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread/thread.h>
#include <stdbool.h>
#include <stdint.h>

// #include "common/logging.hpp"

#include "code_utils.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "lmac15p4.h"
#include "log.h"
#include "mcu.h"

static TimerHandle_t otAlarm_timerHandle = NULL;
static uint32_t otAlarm_offset = 0xFFFFFFF;

#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
#define ALARM_TIMER_TICK_TO_MICRO_SEC(n) (n * 25ul)
#define ALARM_MICRO_SEC_TO_TIMER_TICK(n)                                       \
    (((n % 25ul) > 13ul) ? ((n / 25ul) + 1ul) : (n / 25ul))
#define ALRAM_TIMER_COUNTER_COMPARE1(n1, n2)                                   \
    ((n1 - n2) <= (0xFFFFFFFF / 2)) ? (n1 - n2) : 0
#define ALRAM_TIMER_COUNTER_COMPARE2(n1, n2)                                   \
    ((n2 - n1) > (0xFFFFFFFF / 2)) ? ((0xFFFFFFFF - n2) + n1) : 0
#define ALRAM_TIMER_COUNTER_CHECK(n1, n2)                                      \
    (n1 > n2) ? ALRAM_TIMER_COUNTER_COMPARE1(n1, n2)                           \
              : ALRAM_TIMER_COUNTER_COMPARE2(n1, n2)
#define ALRAM_SLEEP_WAKE_UP_COST_MS   (3ul)
#define ALRAM_SLEEP_MINIMUM_PERIOD_MS (7ul)
#endif

static void otPlatALarm_msTimerCallback(TimerHandle_t xTimer) {
    // log_info("[AlarmMS] Callback triggered");
    OT_NOTIFY(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
static void otPlatALarm_usTimerCallback() {
    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}
#endif

void _utick_set_clear() {
    timern_t* TIMER = TIMER2;

    TIMER->load = 0xFFFFFFFE;
    TIMER->clear = 1;
    TIMER->control.reg = 0;

    TIMER->control.bit.prescale = TIMER_PRESCALE_32;
    TIMER->control.bit.mode = TIMER_PERIODIC_MODE;
    TIMER->control.bit.int_enable = 1;
    TIMER->control.bit.en = 1;
}

void ot_alarmInit(void) {
    _utick_set_clear();
    otAlarm_timerHandle = xTimerCreate("miu_timer", 1, pdFALSE,
                                       (void*)otAlarm_timerHandle,
                                       otPlatALarm_msTimerCallback);
#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
    timer_config_mode_t cfg;
    timern_t* TIMER = TIMER3;
    NVIC_DisableIRQ((IRQn_Type)(Timer3_IRQn));
    NVIC_SetPriority((IRQn_Type)(Timer3_IRQn), 2);

    TIMER->load = 0;
    TIMER->clear = 1;
    TIMER->control.reg = 0;

    TIMER->control.bit.prescale = TIMER_PRESCALE_1;
    TIMER->control.bit.mode = TIMER_FREERUN_MODE;
    TIMER->control.bit.en = 0;

    timer_callback_register(3, otPlatALarm_usTimerCallback);

    // Timer_Int_Callback_Register(3, otPlatALarm_usTimerCallback);

    NVIC_EnableIRQ((IRQn_Type)Timer3_IRQn);
#endif
}

void ot_alarmTask(ot_system_event_t sevent) {
    if (!(OT_SYSTEM_EVENT_ALARM_ALL_MASK & sevent)) {
        return;
    }

    if (OT_SYSTEM_EVENT_ALARM_MS_EXPIRED & sevent) {
        // log_info("[AlarmMS] Event dispatched to otPlatAlarmMilliFired");
        otPlatAlarmMilliFired(otrGetInstance());
    }

#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
    if (OT_SYSTEM_EVENT_ALARM_US_EXPIRED & sevent) {
        otPlatAlarmMicroFired(otrGetInstance());
    }
#endif
}

uint32_t otPlatTimeGetXtalAccuracy(void) { return SystemCoreClock; }

void otPlatAlarmMilliStartAt(otInstance* aInstance, uint32_t aT0,
                             uint32_t aDt) {
    BaseType_t ret;
    uint32_t now = otPlatAlarmMilliGetNow();
    uint32_t elapseTime = now - aT0;
    uint32_t t = pdMS_TO_TICKS(aDt - elapseTime);

    if (elapseTime < aDt && t > 0) {
        // log_info("[AlarmMS] StartAt: now=%u, aT0=%u, aDt=%u, expire=%u, "
        //          "remaining=%d",
        //          now, aT0, aDt, elapseTime, t);
        ret = xTimerChangePeriod(otAlarm_timerHandle, t, 0);
        configASSERT(ret == pdPASS);

        return;
    }

    OT_NOTIFY(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

void otPlatAlarmMilliStop(otInstance* aInstance) {
    if (xTimerIsTimerActive(otAlarm_timerHandle) == pdTRUE) {
        BaseType_t ret = xTimerStop(otAlarm_timerHandle, 0);
        configASSERT(ret == pdPASS);
    }
}

uint32_t otPlatAlarmMilliGetNow(void) {
    return xTaskGetTickCount() * portTICK_RATE_MS;
}

#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
inline uint32_t otPlatAlarmMicroGetNow(void) {
    uint32_t rtc_curr_time = 0U;
    lmac15p4_rtc_time_read((uint32_t*)&rtc_curr_time);

    return rtc_curr_time;
}

void otPlatAlarmMicroStartAt(otInstance* aInstance, uint32_t aT0,
                             uint32_t aDt) {
    OT_UNUSED_VARIABLE(aInstance);
    timern_t* TIMER = TIMER3;

    uint32_t otExpectedIdleTime_us = (aT0 + aDt);
    uint32_t Curr_us = otPlatTimeGet();
    uint32_t UsRemainingTime = ALRAM_TIMER_COUNTER_CHECK(otExpectedIdleTime_us,
                                                         Curr_us);

#if (CONFIG_HOSAL_SOC_IDLE_SLEEP == 1)
    if (UsRemainingTime > (ALRAM_SLEEP_WAKE_UP_COST_MS * 1000)) {
        UsRemainingTime -= (ALRAM_SLEEP_WAKE_UP_COST_MS * 1000);
    }
    if (UsRemainingTime > (ALRAM_SLEEP_MINIMUM_PERIOD_MS * 1000))
#else
    if (otExpectedIdleTime_us > Curr_us
        && (otExpectedIdleTime_us - Curr_us) > 3000)
#endif
    {
        TIMER->load = ALARM_MICRO_SEC_TO_TIMER_TICK(UsRemainingTime);

        TIMER->clear = 1;
        TIMER->control.bit.int_enable = 1;
        TIMER->control.bit.en = 1;
    } else {
        OT_NOTIFY(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
    }
}

void otPlatAlarmMicroStop(otInstance* aInstance) {
    OT_UNUSED_VARIABLE(aInstance);
    timern_t* TIMER = TIMER3;
    TIMER->control.bit.en = 0;
    TIMER->control.bit.int_enable = 0;
    TIMER->clear = 1; /*clear interrupt*/
}
#endif

uint64_t otPlatTimeGet(void) {
    static uint32_t timerWraps = 0U;
    static uint32_t prev32Time = 0U;
    uint32_t now32Time;
    uint64_t now64Time;
#if (OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE == 1)
    now32Time = otPlatAlarmMicroGetNow();
#else
    // now32Time = lmac15p4_rtc_time_read();
    now32Time = otPlatAlarmMilliGetNow();
#endif
    if (now32Time < prev32Time) {
        timerWraps += 1U;
    }
    prev32Time = now32Time;
    now64Time = ((uint64_t)timerWraps << 32) + now32Time;

    return now64Time;
}