/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file app_task.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * 
 */

#include <FreeRTOS.h>
#include <semphr.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_rf.h"
#include "hosal_rtc.h"
#include "hosal_sysctrl.h"
#include "hosal_wdt.h"
#include "log.h"
#include "main.h"
#include "uartConfig.h"
#include "util_string.h"

#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2

#define SRAM_RETENTION_ADDR                          0x20020100
#define SRAM_RETENTSION_ONLY_KEPT_SRAM4_IN_DEEPSLEEP 0x300

#define APP_EVENT_NOTIFY_HANDEL g_app_evt_var
#define APP_EVENT_NOTIFY_SIGNAL __app_event_signal()
#define APP_EVENT_NOTIFY_ISR(ebit)                                             \
    (APP_EVENT_NOTIFY_HANDEL |= ebit);                                         \
    APP_EVENT_NOTIFY_SIGNAL;
#define APP_EVENT_NOTIFY(ebit)                                                 \
    enter_critical_section();                                                  \
    APP_EVENT_NOTIFY_HANDEL |= ebit;                                           \
    leave_critical_section();                                                  \
    APP_EVENT_NOTIFY_SIGNAL;
#define APP_EVENT_GET_NOTIFY(ebit)                                             \
    enter_critical_section();                                                  \
    ebit = APP_EVENT_NOTIFY_HANDEL;                                            \
    APP_EVENT_NOTIFY_HANDEL = APP_EVENT_NONE;                                  \
    leave_critical_section();

typedef enum {
    APP_EVENT_NONE = 0,
    APP_EVENT_BTN0_PRESS = 0x00000001,
    APP_EVENT_BTN1_PRESS = 0x00000002,
    APP_EVENT_SCAN = 0x00000004,
    APP_EVENT_DETACH = 0x00000008,
    APP_EVENT_CHILD = 0x00000010,

    APP_EVENT_ALL = 0xffffffff,
} app_event_t;

static uint32_t m_allow_sleep = 0;
static uint16_t m_detach_count = 0;
static uint16_t m_scan_channel_retry;
static uint16_t m_scan_channel_index = 0;
static app_event_t g_app_evt_var;
// static TimerHandle_t scan_timer;
static SemaphoreHandle_t appSemHandle = NULL;
static TaskHandle_t app_taskHandle = NULL;

static hosal_gpio_input_config_t pin_cfg0, pin_cfg1, pin_cfg2;
static hosal_wdt_config_mode_t wdt_init_cfg;
static hosal_wdt_config_tick_t wdt_tick_cfg;

static rtc_time_t current_time, alarm_tm;
static uint32_t alarm_mode;

static scan_config_t thread_parameter;

static hosal_rf_tx_power_t sPhyPowerStage = {
#if CONFIG_APP_SUBG_SETTING_FREQUENCY_BAND_915
    .band_type = HOSAL_RF_BAND_SUBG_915M,
#elif CONFIG_APP_SUBG_SETTING_FREQUENCY_BAND_868
    .band_type = HOSAL_RF_BAND_SUBG_868M,
#elif CONFIG_APP_SUBG_SETTING_FREQUENCY_BAND_433
    .band_type = HOSAL_RF_BAND_SUBG_433M,
#elif CONFIG_APP_SUBG_SETTING_FREQUENCY_BAND_470
    .band_type = HOSAL_RF_BAND_SUBG_433M,
#endif
#if CONFIG_APP_RF_BAND_SUBG
    .power_index = CONFIG_APP_SUBG_SETTING_TX_POWER_INDEX,
#endif
};
static void __app_event_signal(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(app_taskHandle, &pxHigherPriorityTaskWoken);
    } else
        xTaskNotifyGive(app_taskHandle);
}

static void gpio_cb(uint32_t pin, void* isr_param) {

    if (pin == BUTTON_0) {
        APP_EVENT_NOTIFY_ISR(APP_EVENT_BTN0_PRESS);
    }

    if (pin == BUTTON_1) {
        APP_EVENT_NOTIFY_ISR(APP_EVENT_BTN1_PRESS);
    }

    return;
}

static void rtc_callback(uint32_t rtc_status) {}

// static void scan_timer_handler(void) { APP_EVENT_NOTIFY(APP_EVENT_SCAN); }

static void ot_stateChangeCallback(uint32_t flags, void* p_context) {
    char states[5][10] = {"disabled", "detached", "child", "router", "leader"};
    otInstance* instance = (otInstance*)p_context;
    uint8_t* p;

    if (flags & OT_CHANGED_THREAD_ROLE) {

        uint32_t role = otThreadGetDeviceRole(p_context);
        switch (role) {
            case OT_DEVICE_ROLE_CHILD: {
                // APP_EVENT_NOTIFY(APP_EVENT_CHILD);
                break;
            }
            case OT_DEVICE_ROLE_ROUTER: break;
            case OT_DEVICE_ROLE_LEADER: break;

            case OT_DEVICE_ROLE_DISABLED: break;
            case OT_DEVICE_ROLE_DETACHED: {
                APP_EVENT_NOTIFY(APP_EVENT_DETACH);
                break;
            }
            default: break;
        }

        if (role) {
            log_info("Current role       : %s",
                     states[otThreadGetDeviceRole(p_context)]);

            p = (uint8_t*)(otLinkGetExtendedAddress(instance)->m8);
            log_info("Extend Address     : %02x%02x-%02x%02x-%02x%02x-%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t*)(otThreadGetMeshLocalPrefix(instance)->m8);
            log_info("Local Prefx        : %02x%02x:%02x%02x:%02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t*)(otThreadGetLinkLocalIp6Address(instance)
                               ->mFields.m8);
            log_info("IPv6 Address       : "
                     "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"
                     "02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9],
                     p[10], p[11], p[12], p[13], p[14], p[15]);

            log_info("Rloc16             : %x", otThreadGetRloc16(instance));

            p = (uint8_t*)(otThreadGetRloc(instance)->mFields.m8);
            log_info("Rloc               : "
                     "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"
                     "02x%02x:%02x%02x",
                     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9],
                     p[10], p[11], p[12], p[13], p[14], p[15]);
        }
    }
}

static void set_retention_value(uint32_t index, uint32_t value) {
    if (index < 8) {
        outp32(((SYSCTRL_BASE + SYS_SCRATCH_OFFSET) + (index << 2)), value);
    }
}

static void get_retention_value(uint32_t index, uint32_t* value) {
    if (index < 8) {
        *value = inp32((SYSCTRL_BASE + SYS_SCRATCH_OFFSET) + (index << 2));
    } else {
        *value = 0; /*wrong index*/
    }
}

static void set_wakeup_sleep(size_t wakeup_time) {
    log_info("deep sleep(%d sec)", wakeup_time);
    // xTimerStop(scan_timer, 0);
    hosal_rtc_set_clk(40000);

    current_time.tm_year = 0;
    current_time.tm_mon = 0;
    current_time.tm_day = 0;
    current_time.tm_hour = 0;
    current_time.tm_min = 0;
    current_time.tm_sec = 0;
    hosal_rtc_set_time(&current_time);

    alarm_tm.tm_year = 0;
    alarm_tm.tm_mon = 0;
    alarm_tm.tm_day = 0;
    alarm_tm.tm_hour = 0;
    alarm_tm.tm_min = 0;
    alarm_tm.tm_sec = wakeup_time;
    alarm_mode = RTC_MODE_HOUR_EVENT_INTERRUPT | RTC_MODE_EVENT_INTERRUPT;
    hosal_rtc_set_alarm(&alarm_tm, alarm_mode, rtc_callback);
    // if (thread_parameter.thread_short_deeplseep_wakeup_time == wakeup_time) {
    //     lpm_set_sram_sleep_deepsleep_shutdown(SRAM_RETENTSION_ONLY_KEPT_SRAM4_IN_DEEPSLEEP);
    // }
    lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED13);
    lpm_set_low_power_level(LOW_POWER_LEVEL_SLEEP3);
}

static void set_network_config(otInstance* instance) {
    otError error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp.mSeconds = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    aDataset.mChannel = thread_parameter.scan_table[0];
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId = (otPanId)CONFIG_APP_THREAD_NETWORK_PANID;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC0, 0xDE, 0x1A, 0xB5,
                                            0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mExtendedPanId.m8, extPanId,
           sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    uint8_t key[OT_NETWORK_KEY_SIZE];

    memset(key, 0x0, OT_NETWORK_KEY_SIZE);
    for (int i = 0; i < 16; i++) {
        key[i] = (uint8_t)utility_strtox(
            (uint8_t*)&CONFIG_APP_THRREAD_NETWORK_MASTER_KEY[i * 2], 0, 2);
    }

    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    /* Set Network Name */
    size_t length = strlen(CONFIG_APP_THREAD_NETWORK_NAME);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, CONFIG_APP_THREAD_NETWORK_NAME, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    /* Set the Active Operational Dataset to this dataset */
    error = otDatasetSetActive(instance, &aDataset);
    if (error != OT_ERROR_NONE) {
        log_error("otDatasetSetActive failed with: %d, %s\r\n", error,
                  otThreadErrorToString(error));
        return;
    }
}

static void app_udp_cb(otMessage* otMsg, const otMessageInfo* otInfo) {
    uint8_t* p = NULL;

    uint16_t len;

    otInstance* instance = otrGetInstance();

    len = otMessageGetLength(otMsg) - otMessageGetOffset(otMsg);

    p = pvPortMalloc(len);

    do {
        if (p == NULL)
            break;

        otMessageRead(otMsg, otMessageGetOffset(otMsg), p, len);

        log_info("UDP Packet received, port: %d, len: %d", otInfo->mPeerPort,
                 len);
        log_info("UDP Packet data: %s", p);

    } while (0);

    if (p != NULL)
        vPortFree(p);
}

static void HandleActiveScanResult(otActiveScanResult* aResult) {
    otError error;
    otOperationalDataset aDataset;
    otInstance* instance = otrGetInstance();

    if (aResult == NULL) {
        log_info("Leader not found");
        // xTimerStart(scan_timer, 0);
        if (otThreadGetDeviceRole(instance) == OT_DEVICE_ROLE_DISABLED) {
            APP_EVENT_NOTIFY(APP_EVENT_SCAN);
        }
        return;
    }

    // xTimerStop(scan_timer, 0);

    log_info("Leader found");
    log_info("panid: %04x", aResult->mPanId);
    log_info("ext addr: %08x", aResult->mExtAddress);
    log_info("channel: %2u", aResult->mChannel);
    log_info("rssi: %3d", aResult->mRssi);
    log_info("lqi: %3u", aResult->mLqi);

    // otThreadSetEnabled(instance, false);
    memset(&aDataset, 0, sizeof(otOperationalDataset));
    error = otDatasetGetActive(instance, &aDataset);
    if (error != OT_ERROR_NONE) {
        log_error("otDatasetGetActive failed with: %d, %s\r\n", error,
                  otThreadErrorToString(error));
    }
    aDataset.mChannel = aResult->mChannel;
    aDataset.mPanId = aResult->mPanId;
    aDataset.mComponents.mIsChannelPresent = true;

    error = otDatasetSetActive(instance, &aDataset);
    if (error != OT_ERROR_NONE) {
        log_error("otDatasetSetActive failed with: %d, %s\r\n", error,
                  otThreadErrorToString(error));
    }
    otThreadSetEnabled(instance, true);
    
    log_info("Thread version     : %s", otGetVersionString());
    log_info("Link Mode          : %d, %d, %d",
             otThreadGetLinkMode(instance).mRxOnWhenIdle,
             otThreadGetLinkMode(instance).mDeviceType,
             otThreadGetLinkMode(instance).mNetworkData);
    log_info("Network name       : %s", otThreadGetNetworkName(instance));
    log_info("PAN ID             : %x", otLinkGetPanId(instance));
    log_info("channel            : %d", otLinkGetChannel(instance));
    app_sockInit(instance, app_udp_cb, CONFIG_APP_THREAD_UDP_LISTEN_PORT);
}

void otrInitUser(otInstance* instance) {

    otLinkModeConfig mode;
    uint32_t value;
    uint8_t scan_table[20] = {CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL01,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL02,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL03,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL04,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL05,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL06,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL07,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL08,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL09,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL10,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL11,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL12,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL13,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL14,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL15,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL16,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL17,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL18,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL19,
                              CONFIG_APP_THREAD_CHANNEL_SCAN_CHANNEL20};

#if 0
    scan_config_t test = {
        .power_stage = 29,
        .network_data_poll_period = 2000,
        .scan_number = 5,
        .thread_channel_scan_retry = 3,
        .thread_channel_scan_timeout = 200,
        .thread_long_deeplseep_wakeup_time = 5,
        .thread_short_deeplseep_wakeup_time = 1,
        .scan_number = 10,
        .scan_table = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20}
    };
    thread_config_set(&test);
    thread_config_reset();
#endif

    thread_config_get(&thread_parameter);
    if (thread_parameter.dirty == false) {
        log_info("default thread configuration");
#if CONFIG_APP_RF_BAND_SUBG
        thread_parameter.power_stage = CONFIG_APP_SUBG_SETTING_TX_POWER_INDEX;
#endif
        thread_parameter.network_data_poll_period =
            CONFIG_APP_THREAD_NETWORK_DATA_POLL_PERIOD;
        thread_parameter.thread_channel_scan_timeout =
            CONFIG_APP_THREAD_CHANNEL_SCAN_TIMEOUT;
        thread_parameter.thread_channel_scan_retry =
            CONFIG_APP_THREAD_CHANNEL_SCAN_RETRY;
        thread_parameter.thread_long_deeplseep_wakeup_time =
            CONFIG_APP_THREAD_LONG_DEEPSLEEP_WAKEUP_TIME;
        thread_parameter.thread_short_deeplseep_wakeup_time =
            CONFIG_APP_THREAD_SHORT_DEEPSLEEP_WAKEUP_TIME;
        thread_parameter.scan_number = CONFIG_APP_THREAD_CHANNEL_SCAN_NUMBER;
        memcpy(&thread_parameter.scan_table, &scan_table, sizeof(scan_table));
    }
#if CONFIG_APP_RF_BAND_SUBG
    sPhyPowerStage.power_index = thread_parameter.power_stage;
    if (hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_SET, &sPhyPowerStage)
        != HOSAL_RF_STATUS_SUCCESS) {
        log_info("set tx power error");
    }
#endif
    log_info("dirty: %s", (thread_parameter.dirty == true) ? "true" : "false");
#if CONFIG_APP_RF_BAND_SUBG
    log_info("power stage: %d", thread_parameter.power_stage);
#endif
    log_info("period(ms): %d", thread_parameter.network_data_poll_period);
    log_info("timeout(ms): %d", thread_parameter.thread_channel_scan_timeout);
    log_info("retry cout: %d", thread_parameter.thread_channel_scan_retry);
    log_info("long wakeup(s): %d",
             thread_parameter.thread_long_deeplseep_wakeup_time);
    log_info("short wakeup(s): %d",
             thread_parameter.thread_short_deeplseep_wakeup_time);
    log_info("scan number: %d", thread_parameter.scan_number);
    log_info("scan table");
    for (size_t i = 0; i < thread_parameter.scan_number; i++) {
        log_info("ch: %d", thread_parameter.scan_table[i]);
    }

    // scan_timer = xTimerCreate(
    //     "scan", pdMS_TO_TICKS(thread_parameter.thread_channel_scan_timeout),
    //     pdTRUE, (void*)0, (TimerCallbackFunction_t)scan_timer_handler);

    get_retention_value(1, &value);
    // log_info("retention register: %x", value);
    if (value == 0) {
        m_scan_channel_retry = 0;
    } else {
        m_scan_channel_retry = value;
    }

    // otAppCliInit((otInstance*)instance);

    // log_info("Thread version     : %s", otGetVersionString());

    memset(&mode, 0, sizeof(mode));

    mode.mDeviceType = 0;
    mode.mRxOnWhenIdle = 0;
    mode.mNetworkData = 0;

    otLinkSetPollPeriod(instance, thread_parameter.network_data_poll_period);

    otThreadSetLinkMode(instance, mode);

    set_network_config(instance);
    otIp6SetEnabled(instance, true);
    // otThreadSetEnabled(instance, true);

    // log_info("Link Mode          : %d, %d, %d",
    //          otThreadGetLinkMode(instance).mRxOnWhenIdle,
    //          otThreadGetLinkMode(instance).mDeviceType,
    //          otThreadGetLinkMode(instance).mNetworkData);
    // log_info("Network name       : %s", otThreadGetNetworkName(instance));
    // log_info("PAN ID             : %x", otLinkGetPanId(instance));
    // log_info("channel            : %d", otLinkGetChannel(instance));

    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);

    // app_sockInit(instance, app_udp_cb, CONFIG_APP_THREAD_UDP_LISTEN_PORT);

    APP_EVENT_NOTIFY(APP_EVENT_SCAN);
}

static void app_task_loop(void* parameters_ptr) {

    app_event_t sevent = APP_EVENT_NONE;

    while (1) {
        APP_EVENT_GET_NOTIFY(sevent);
        // log_info("evt: %d", sevent);

        if (sevent & APP_EVENT_BTN0_PRESS) {
            log_info("Button 0 pressed");
            m_allow_sleep = !m_allow_sleep;

            if (m_allow_sleep) {
                log_info("Allow sleep");
                lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED13);
            } else {
                log_info("Not allow sleep");
                lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED13);
            }
        }

        if (sevent & APP_EVENT_BTN1_PRESS) {
            log_info("Button 1 pressed");

            otInstance* instance = otrGetInstance();
            app_udpSend(instance, (uint8_t*)"Hello World", 12);
        }

        if (sevent & APP_EVENT_SCAN) {
            otInstance* instance = otrGetInstance();

            OT_THREAD_SAFE(do {
                if (m_scan_channel_index == thread_parameter.scan_number) {
                    m_scan_channel_index = 0;
                    if (++m_scan_channel_retry
                        >= thread_parameter.thread_channel_scan_retry) {
                        set_retention_value(1, 0);
                        set_wakeup_sleep(
                            thread_parameter.thread_long_deeplseep_wakeup_time);
                    } else {
                        set_retention_value(1, m_scan_channel_retry);
                        set_wakeup_sleep(
                            thread_parameter
                                .thread_short_deeplseep_wakeup_time);
                    }
                    log_info("Retry count        : %d", m_scan_channel_retry);
                } else {
                    log_info("scan channel(%d) %d ms",
                             thread_parameter.scan_table[m_scan_channel_index],
                             thread_parameter.thread_channel_scan_timeout);
                    otLinkActiveScan(
                        instance,
                        (1 << thread_parameter
                                  .scan_table[m_scan_channel_index++]),
                        thread_parameter.thread_channel_scan_timeout, 
                        HandleActiveScanResult, NULL);
                }
            } while (0);)
        }

        if (sevent & APP_EVENT_DETACH) {
            otInstance* instance = otrGetInstance();

            if (++m_detach_count >= 2) {
                m_detach_count = 0;
                otThreadSetEnabled(instance, false);
                APP_EVENT_NOTIFY(APP_EVENT_SCAN);
            }
            // log_info("Timer start");
            // xTimerStart(scan_timer, 0);
        }

        // if (sevent & APP_EVENT_CHILD) {
            // log_info("Timer stop");
            // xTimerStop(scan_timer, 0);
        // }

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void app_task(void) {
    /* BUTTON_0 pin setting */
    pin_cfg0.param = NULL;
    pin_cfg0.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg0.usr_cb = gpio_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(BUTTON_0);
    hosal_pin_set_pullopt(BUTTON_0, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(BUTTON_0, pin_cfg0);

    /* BUTTON_1 pin setting */
    pin_cfg1.param = NULL;
    pin_cfg1.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg1.usr_cb = gpio_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    hosal_gpio_debounce_enable(BUTTON_1);
    hosal_pin_set_pullopt(BUTTON_1, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(BUTTON_1, pin_cfg1);

    hosal_gpio_int_enable(BUTTON_0);
    hosal_gpio_int_enable(BUTTON_1);

    NVIC_SetPriority(Gpio_IRQn, 6);
    NVIC_EnableIRQ(Gpio_IRQn);

    lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED13);

    if (xTaskCreate(app_task_loop, "demo", 512, NULL, E_TASK_PRIORITY_NORMAL,
                    &app_taskHandle)
        != pdPASS)
        log_error("task create fail\n");
}