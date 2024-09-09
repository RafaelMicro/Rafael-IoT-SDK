// ---------------------------------------------------------------------------
// Copyright (c) 2020, Vango Technologies, Inc. - http://www.vangotech.com
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __SW_TIMER_H__
#define __SW_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include <stdint.h>
#include <string.h>
#include "util_list.h"
//=============================================================================
//                Public Definitions of const value
//=============================================================================
#define VT_MAX_TIMER             0xFFFFFFFF
#define SW_TIMER_MAX_PERIOD      0x0A4CB800 // 172800000 ms = 48 hour
#define SW_TIMER_SHIFT_THRESHOLD 0x80000000 // About 24.8 day

//############################################################
// VT Timer Tick = 0.1 ms
// MAX Time = 48 Hour = 2880 min = 172800 sec =  172800000 ms
//############################################################
#define SW_TIMER_HOUR_TO_TICK(h)         ((h) * 3600000)
#define SW_TIMER_MINUTE_TO_TICK(m)       ((m) * 60000)
#define SW_TIMER_SECOND_TO_TICK(s)       ((s) * 1000)
#define SW_TIMER_MILLISECOND_TO_TICK(ms) (ms)

//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum SW_TIMER_PRIORITY {
    SW_TIMER_PRIORITY_0 = 0, // Low
    SW_TIMER_PRIORITY_1,     // Normal
    SW_TIMER_PRIORITY_2,     // High
    SW_TIMER_PRIORITY_MAX,
} sw_timer_pri_t;

typedef enum SW_TIMER_EXECUTE_MODE {
    SW_TIMER_EXECUTE_ONCE_FOR_EACH_TIMEOUT = 0,
    SW_TIMER_EXECUTE_ONCE_FOR_DUPLICATE_TIMEOUT = 1,
} sw_timer_mode_t;

typedef enum SW_TIMER_ERRNO {
    SW_TIMER_EXECUTE_MODE_FAIL = -5,
    SW_TIMER_PERIOD_FAIL = -4,
    SW_TIMER_PRIORITY_FAIL = -3,
    SW_TIMER_CMD_SEND_FAIL = -2,
    SW_TIMER_NULL = -1,
    SW_TIMER_PASS = 0,
} sw_timer_err_t;

//=============================================================================
//                Public Struct
//=============================================================================
typedef void (*sw_timer_cb)(void* p_param);

typedef struct SW_TIMER {
    uint32_t period;
    const char* name;

    uint32_t auto_reload          : 1;
    uint32_t priority             : 2;
    uint32_t execute_mode         : 1; // follow enum sw_timer_execute_mode_e
    uint32_t receive_delete_cmd   : 1;
    uint32_t wating_for_execution : 8;
    uint32_t reserved             : 19;

    void* cb_param;
    sw_timer_cb cb_function;

    uint32_t timeout;
    utils_dlist_t list;
} sw_timer_t;

//=============================================================================
//                Public Function Declaration
//=============================================================================
sw_timer_t* sw_timer_create(const char* name, uint32_t period,
                            uint32_t auto_reload, sw_timer_pri_t priority,
                            uint32_t execute_mode, void* cb_param,
                            sw_timer_cb cb_function);
sw_timer_err_t sw_timer_start(sw_timer_t* timer);
sw_timer_err_t sw_timer_stop(sw_timer_t* timer);
sw_timer_err_t sw_timer_reset(sw_timer_t* timer);
sw_timer_err_t sw_timer_change_period(sw_timer_t* timer, uint32_t period);
sw_timer_err_t sw_timer_change_priority(sw_timer_t* timer, uint32_t priority);
sw_timer_err_t sw_timer_change_execute_mode(sw_timer_t* timer,
                                            uint32_t execute_mode);
sw_timer_err_t sw_timer_delete(sw_timer_t* timer);
const char* sw_timer_get_timer_name(sw_timer_t* timer);
sw_timer_err_t sw_timer_start_from_isr(sw_timer_t* timer);
sw_timer_err_t sw_timer_stop_from_isr(sw_timer_t* timer);
sw_timer_err_t sw_timer_reset_from_isr(sw_timer_t* timer);
sw_timer_err_t sw_timer_change_period_from_isr(sw_timer_t* timer,
                                               uint32_t period);
sw_timer_err_t sw_timer_change_priority_from_isr(sw_timer_t* timer,
                                                 uint32_t priority);
sw_timer_err_t sw_timer_change_execute_mode_from_isr(sw_timer_t* timer,
                                                     uint32_t execute_mode);
sw_timer_err_t sw_timer_delete_from_isr(sw_timer_t* timer);
uint32_t sw_timer_init(uint32_t u32_timer_id);
uint32_t sw_timer_hw_init(uint32_t u32_timer_id);
#ifdef CONFIG_SW_TIMER_ENABLE_DEBUG
void sw_timer_state_print(void);
#endif
#ifdef __cplusplus
};
#endif
#endif /* __SW_TIMER_H__ */
