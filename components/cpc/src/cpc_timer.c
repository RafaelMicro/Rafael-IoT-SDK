/**
 * @file cpc_timer.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "cpc_timer.h"
#include "FreeRTOS.h"
#include "cpc.h"
#include "log.h"
#include "mcu.h"
#include "task.h"
#include "timers.h"

#include "sw_timer.h"

static cpc_timer_callback_t p_cpc_timer_callback = NULL;

uint32_t cpc_timer_ms_to_tick(uint16_t time_ms) {
    return pdMS_TO_TICKS(time_ms);
}

uint32_t cpc_timer_tick_to_ms(uint32_t tick) {
    return (tick / portTICK_RATE_MS);
}

uint32_t cpc_timer_get_tick_count(void) {
    return (uint32_t)xTaskGetTickCount();
}

uint64_t cpc_timer_get_tick_count64(void) {
    return (uint64_t)xTaskGetTickCount();
}

void cpc_timer_callback(void* data) {
    if (p_cpc_timer_callback != NULL) {
        p_cpc_timer_callback(data);
    }
}

status_t cpc_timer_restart_timer(cpc_timer_handle_t** handle, uint32_t timeout,
                                 cpc_timer_callback_t callback,
                                 void* callback_data) {
    // log_info("T Start %p, %d", *handle, timeout);
    // log_info("handle %p, callback_data %p", *handle, callback_data);
    p_cpc_timer_callback = callback;
    if (*handle != NULL) {

        sw_timer_stop(*handle);
        sw_timer_delete(*handle);
    }

    *handle = sw_timer_create("1", timeout, false, SW_TIMER_PRIORITY_2,
                              SW_TIMER_EXECUTE_ONCE_FOR_DUPLICATE_TIMEOUT,
                              callback_data, cpc_timer_callback);
    sw_timer_start(*handle);
    return CPC_STATUS_OK;
}

status_t cpc_timer_stop_timer(cpc_timer_handle_t* handle) {
    // log_info("T Stop %p", handle);
    if (handle != NULL)
        sw_timer_stop(handle);
    return CPC_STATUS_OK;
}