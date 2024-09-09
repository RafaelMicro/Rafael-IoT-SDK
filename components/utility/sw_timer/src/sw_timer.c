/**
 * Copyright (c) 2020 Rex Huang, All Rights Reserved.
 */
/** @file sw_timer.c
 *
 * @author Rex Huang
 * @version 0.1
 * @date 2020/03/16
 * @license
 * @description
 */

//=============================================================================
//                Include
//=============================================================================
#include "sw_timer.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "log.h"
#include "mcu.h"
#include "util_list.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================
#define CONFIG_SW_TIMER_TASK_PRIORITY         (E_TASK_PRIORITY_SW_TIMER)
#define CONFIG_SW_TIMER_STACK_SIZE            256 // task stack size (number of words)
#define CONFIG_SW_TIMER_RUNNING_TASK_PRIORITY (E_TASK_PRIORITY_SW_TIMER - 1)
#define CONFIG_SW_TIMER_RUNNING_STACK_SIZE                                     \
    256 // task stack size (number of words)

#define SW_TIMER_TRIGGER_TASK_TIMEOUT   100 // 100 ms
#define SW_TIMER_CMD_QUEUE_LENGTH       40
#define SW_TIMER_CMD_QUEUE_ITEM_SIZES   (sizeof(sw_timer_queue_t))
#define SW_TIMER_CMD_QUEUE_SEND_TIMEOUT 5 // 5 ms

#define SW_TIMER_RUNNING_QUEUE_LENGTH     40
#define SW_TIMER_RUNNING_QUEUE_ITEM_SIZES (sizeof(void*))

#define SW_TIMER_RUNNING_RESERVED_LENGTH 10

#define TIMER_ERR_LOG() log_error("SW TIMER ERROR!")

#define SW_TIMER_START(timer) timer_cmd_send(timer, SW_TIMER_CMD_START, 0, 0)
#define SW_TIMER_STOP(timer)  timer_cmd_send(timer, SW_TIMER_CMD_STOP, 0, 0)
#define SW_TIMER_RESET(timer) timer_cmd_send(timer, SW_TIMER_CMD_RESET, 0, 0)
#define SW_TIMER_CHANGE_PERIOD(timer, period)                                  \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_PERIOD, period, 0)
#define SW_TIMER_CHANGE_PRIORITY(timer, priority)                              \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_PRIORITY, priority, 0)
#define SW_TIMER_CHANGE_EXECUTE_MODE(timer, execute_mode)                      \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_EXECUTE_MODE, execute_mode, 0)
#define SW_TIMER_DELETE(timer) timer_cmd_send(timer, SW_TIMER_CMD_DELETE, 0, 0)

#define SW_TIMER_START_FROM_ISR(timer)                                         \
    timer_cmd_send(timer, SW_TIMER_CMD_START, 0, 1)
#define SW_TIMER_STOP_FROM_ISR(timer)                                          \
    timer_cmd_send(timer, SW_TIMER_CMD_STOP, 0, 1)
#define SW_TIMER_RESET_FROM_ISR(timer)                                         \
    timer_cmd_send(timer, SW_TIMER_CMD_RESET, 0, 1)
#define SW_TIMER_CHANGE_PERIOD_FROM_ISR(timer, period)                         \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_PERIOD, period, 1)
#define SW_TIMER_CHANGE_PRIORITY_FROM_ISR(timer, priority)                     \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_PRIORITY, priority, 1)
#define SW_TIMER_CHANGE_EXECUTE_MODE_FROM_ISR(timer, execute_mode)             \
    timer_cmd_send(timer, SW_TIMER_CMD_CHANGE_EXECUTE_MODE, execute_mode, 1)
#define SW_TIMER_DELETE_FROM_ISR(timer)                                        \
    timer_cmd_send(timer, SW_TIMER_CMD_DELETE, 0, 1)
#define SW_TIMER_TASK_WAKE_UP_FROM_ISR                                         \
    timer_cmd_send((sw_timer_t*)0x12345678, SW_TIMER_CMD_TASK_WAKE_UP, 0, 1)

//=============================================================================
//                Private ENUM
//=============================================================================
typedef enum SW_TIMER_CMD {
    SW_TIMER_CMD_START = 0,
    SW_TIMER_CMD_STOP = 1,
    SW_TIMER_CMD_RESET = 2,
    SW_TIMER_CMD_CHANGE_PERIOD = 3,
    SW_TIMER_CMD_CHANGE_PRIORITY = 4,
    SW_TIMER_CMD_CHANGE_EXECUTE_MODE = 5,
    SW_TIMER_CMD_DELETE = 6,
    SW_TIMER_CMD_TASK_WAKE_UP = 7,
} sw_timer_cmd_t;

//=============================================================================
//                Private Struct
//=============================================================================
typedef struct SW_TIMER_CONTROL {
    utils_dlist_t working_list;
    uint32_t current_time; // ms
    uint32_t alarm_time;
} sw_timer_ctrl_t;

/*************************************************************/
/*********************** VT Timer Queue ************************/
/*************************************************************/
typedef struct SW_TIMER_QUEUE {
    sw_timer_t* timer;
    uint32_t cmd_op    : 8;
    uint32_t cmd_value : 24;
} sw_timer_queue_t;

//=============================================================================
//                Private Function Declaration
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================
static sw_timer_ctrl_t g_timer_ctrl;
static QueueHandle_t g_timer_cmd_queue;
static QueueHandle_t g_timer_running_queue[SW_TIMER_PRIORITY_MAX];
static uint32_t u32_debug_timer;

//=============================================================================
//                Public Global Variables
//=============================================================================

//=============================================================================
//                Private Definition of Compare/Operation/Inline funciton/
//=============================================================================

//=============================================================================
//                Functions
//=============================================================================

#ifdef CONFIG_SW_TIMER_ENABLE_DEBUG
void sw_timer_state_print(void) { u32_debug_timer = 1; }

static void debug_timer_cb(void* cb_param) {
    if (u32_debug_timer == 0)
        return;
    sw_timer_t* timer_list = NULL;
    u32_debug_timer = 0;
    log_info("current_time %u, alarm_time %u\n", g_timer_ctrl.current_time,
             g_timer_ctrl.alarm_time);
    utils_dlist_for_each_entry(&g_timer_ctrl.working_list, timer_list,
                               sw_timer_t, list) {
        log_info("TIMER :%s (TO %u, Pri %u, Period %u, Auto %u)",
                 timer_list->name, timer_list->timeout, timer_list->priority,
                 timer_list->period, timer_list->auto_reload);
    }
}

static uint32_t debug_timer_init(void) {
    sw_timer_t* debug_timer;
    sw_timer_err_t t_timer_return;

    debug_timer = sw_timer_create(
        "BUG_t", SW_TIMER_SECOND_TO_TICK(1), 1, SW_TIMER_PRIORITY_0,
        SW_TIMER_EXECUTE_ONCE_FOR_EACH_TIMEOUT, NULL, debug_timer_cb);

    if (debug_timer == NULL) {
        log_error("dbgtimer create fail");
        return 1;
    }

    t_timer_return = sw_timer_start(debug_timer);
    if (t_timer_return != SW_TIMER_PASS) {
        log_error("dbgtimer_op fail(%d)", t_timer_return);
        return 1;
    }
    return 0;
}
#endif // CONFIG_SW_TIMER_ENABLE_DEBUG

static void timer_running_handler(void* params) {
    uint32_t u32_running_task = (uint32_t)params;
    sw_timer_t* timer;

    log_info("%s(%u)", __FUNCTION__, u32_running_task);
    while (1) {
        if (xQueueReceive(g_timer_running_queue[u32_running_task],
                          (void*)&timer, portMAX_DELAY)
            != pdTRUE) {
            TIMER_ERR_LOG();
            break;
        }

        //log_info("running(%d)(%s:%p)\n", u32_running_task, timer->name, timer);

        if (timer->wating_for_execution > 1)
            log_info("timer(%s:%p) duplicate timeout(%d), tick %d", timer->name,
                     timer, timer->wating_for_execution, xTaskGetTickCount());

        timer->cb_function(timer->cb_param);

        if (timer->wating_for_execution) {
            timer->wating_for_execution--;
            if ((timer->receive_delete_cmd == 1)
                && (timer->wating_for_execution == 0))
                sw_timer_delete(timer);
        } else
            TIMER_ERR_LOG();
    }
}

static uint32_t timer_running_task_create(void) {
    static char* task_name[SW_TIMER_PRIORITY_MAX] = {"RT_0", "RT_1", "RT_2"};
    uint32_t u32_idx, u32_priority;
    TaskHandle_t t_thread;
    for (u32_idx = 0; u32_idx < SW_TIMER_PRIORITY_MAX; u32_idx++) {
        g_timer_running_queue[u32_idx] = xQueueCreate(
            SW_TIMER_RUNNING_QUEUE_LENGTH, SW_TIMER_RUNNING_QUEUE_ITEM_SIZES);
        if (g_timer_running_queue[u32_idx] == NULL) {
            log_error("run_queue[%d] create fail", u32_idx);
            return 1;
        }
        u32_priority = CONFIG_SW_TIMER_RUNNING_TASK_PRIORITY
                       - (SW_TIMER_PRIORITY_MAX - u32_idx - 1);

        xTaskCreate(timer_running_handler, "syslog",
                    CONFIG_SW_TIMER_RUNNING_STACK_SIZE, (void*)u32_idx,
                    u32_priority, &t_thread);
        if (t_thread == NULL) {
            log_error("run_thread[%d] create fail", u32_idx);
            return 1;
        }
    }
    return 0;
}

static sw_timer_err_t timer_parameter_check(sw_timer_cmd_t cmd_op,
                                            uint32_t value) {
    switch (cmd_op) {
        case SW_TIMER_CMD_CHANGE_PERIOD:
            if (value > SW_TIMER_MAX_PERIOD) {
                TIMER_ERR_LOG();
                return SW_TIMER_PERIOD_FAIL;
            }
            break;
        case SW_TIMER_CMD_CHANGE_PRIORITY:
            if (value > SW_TIMER_PRIORITY_MAX) {
                TIMER_ERR_LOG();
                return SW_TIMER_PRIORITY_FAIL;
            }
            break;
        case SW_TIMER_CMD_CHANGE_EXECUTE_MODE:
            if (value > SW_TIMER_EXECUTE_ONCE_FOR_DUPLICATE_TIMEOUT) {
                TIMER_ERR_LOG();
                return SW_TIMER_EXECUTE_MODE_FAIL;
            }
            break;
        case SW_TIMER_CMD_START:
        case SW_TIMER_CMD_STOP:
        case SW_TIMER_CMD_RESET:
        case SW_TIMER_CMD_DELETE:
        case SW_TIMER_CMD_TASK_WAKE_UP:
        default: break;
    }
    return SW_TIMER_PASS;
}

static sw_timer_err_t timer_cmd_send(sw_timer_t* timer, sw_timer_cmd_t cmd_op,
                                     uint32_t cmd_value, uint32_t is_isr) {
    sw_timer_err_t t_result = SW_TIMER_PASS;
    BaseType_t context_switch = pdFALSE;
    // log_error("[%s]", __func__);

    sw_timer_queue_t timer_queue;
    if (timer == NULL) {
        TIMER_ERR_LOG();
        return SW_TIMER_NULL;
    }

    t_result = timer_parameter_check(cmd_op, cmd_value);
    if (t_result < 0) {
        TIMER_ERR_LOG();
        return t_result;
    }

    timer_queue.timer = timer;
    timer_queue.cmd_op = cmd_op;
    timer_queue.cmd_value = cmd_value;

    if (is_isr) {
        xQueueSendToBackFromISR(g_timer_cmd_queue, &timer_queue,
                                &context_switch);
        portYIELD_FROM_ISR(context_switch);
        return t_result;
    }

    if (xQueueSend(g_timer_cmd_queue, &timer_queue,
                   SW_TIMER_CMD_QUEUE_SEND_TIMEOUT)
        != pdTRUE) {
        TIMER_ERR_LOG();
        return SW_TIMER_CMD_SEND_FAIL;
    }
}

__STATIC_INLINE void alarm_time_reset(void) {
    sw_timer_t* timer_list = utils_dlist_first_entry(&g_timer_ctrl.working_list,
                                                     sw_timer_t, list);
    g_timer_ctrl.alarm_time = (timer_list) ? timer_list->timeout : VT_MAX_TIMER;
}

static void _timer_start(sw_timer_t* timer) {
    utils_dlist_del(&timer->list);
    timer->timeout = g_timer_ctrl.current_time + timer->period;
    if (utils_dlist_empty(&g_timer_ctrl.working_list)) {
        utils_dlist_add(&timer->list, &g_timer_ctrl.working_list);
        g_timer_ctrl.alarm_time = timer->timeout;
        return;
    }
    sw_timer_t* timer_list;
    utils_dlist_for_each_entry(&g_timer_ctrl.working_list, timer_list,
                               sw_timer_t, list) {
        if (timer_list->timeout > timer->timeout) {
            utils_dlist_add_tail(&timer->list, &timer_list->list);
            break;
        } else if ((timer_list->timeout == timer->timeout)
                   && (timer_list->priority < timer->priority)) {
            utils_dlist_add_tail(&timer->list, &timer_list->list);
            break;
        }
    }
    if (utils_dlist_empty(&timer->list))
        utils_dlist_add_tail(&timer->list, &g_timer_ctrl.working_list);

    alarm_time_reset();
}

static void _timer_stop(sw_timer_t* timer) { utils_dlist_del(&timer->list); }

static void _timer_reset(sw_timer_t* timer) {
    if (!utils_dlist_empty(&timer->list))
        _timer_start(timer);
}

static void _timer_change_period(sw_timer_t* timer, uint32_t period) {
    timer->period = period;
    _timer_reset(timer);
}

static void _timer_change_priority(sw_timer_t* timer, uint32_t priority) {
    timer->priority = priority;
    _timer_reset(timer);
}

static void _timer_change_execute_mode(sw_timer_t* timer,
                                       uint32_t execute_mode) {
    timer->execute_mode = execute_mode;
    _timer_reset(timer);
}

static void _timer_delete(sw_timer_t* timer) {
    _timer_stop(timer);
    // in hplc_timer_handler, if auto_reload is one, than it will auto restart
    memset(timer, 0x0, sizeof(sw_timer_t));
    vPortFree(timer);
}

static void _timer_shift(uint32_t shift_time) {
    sw_timer_t* timer_list;
    g_timer_ctrl.current_time -= shift_time;

    if (utils_dlist_empty(&g_timer_ctrl.working_list))
        g_timer_ctrl.alarm_time = VT_MAX_TIMER;
    else {
        utils_dlist_for_each_entry(&g_timer_ctrl.working_list, timer_list,
                                   sw_timer_t, list) {
            timer_list->timeout = (timer_list->timeout <= shift_time)
                                      ? 0
                                      : (timer_list->timeout - shift_time);
        }
        alarm_time_reset();
    }
}

static void switch_to_running_task(sw_timer_t* timer) {
    if (timer->cb_function == NULL) {
        // running event already in running task queue
        // log_error("Timer(%s) cb = NULL", timer->name);
        return;
    }

    if ((timer->execute_mode == SW_TIMER_EXECUTE_ONCE_FOR_DUPLICATE_TIMEOUT)
        && (timer->wating_for_execution > 0)) {
        // running event already in running task queue
        log_error("Timer(%s) Waiting for execution", timer->name);
        return;
    }

    if (timer->receive_delete_cmd == 1) {
        log_error("Timer(%s) will be delete, not run again", timer->name);
        return;
    }

    timer->wating_for_execution++;
    if ((uxQueueSpacesAvailable(g_timer_running_queue[timer->priority])
         < SW_TIMER_RUNNING_RESERVED_LENGTH)
        && (timer->wating_for_execution > 2)) {
        // give up current timer timeout
        timer->wating_for_execution--;
        log_error("Timer(%s) expire give up", timer->name);
        return;
    }
    if (xQueueSendToBack(g_timer_running_queue[timer->priority], &timer,
                         SW_TIMER_CMD_QUEUE_SEND_TIMEOUT)
        != pdTRUE) {
        timer->wating_for_execution--;
        TIMER_ERR_LOG();
    }
}

static void timer_cmd_queue_check(void) {
    // check cmd queue
    do {
        sw_timer_queue_t timer_queue;
        if (xQueueReceive(g_timer_cmd_queue, &timer_queue, portMAX_DELAY)
            != pdTRUE) {
            TIMER_ERR_LOG();
            break;
        }

        if (timer_queue.cmd_op == SW_TIMER_CMD_TASK_WAKE_UP) {
            log_debug("Get timer wake up");
        } else {
            log_debug("Get timer:%s %p cmd %d", timer_queue.timer->name,
                      timer_queue.timer, timer_queue.cmd_op);
        }
        log_debug("[%s] timer_queue.cmd_op = %d\n", __func__,
                  timer_queue.cmd_op);
        switch (timer_queue.cmd_op) {
            case SW_TIMER_CMD_START: _timer_start(timer_queue.timer); break;
            case SW_TIMER_CMD_STOP: _timer_stop(timer_queue.timer); break;
            case SW_TIMER_CMD_RESET: _timer_reset(timer_queue.timer); break;
            case SW_TIMER_CMD_CHANGE_PERIOD:
                _timer_change_period(timer_queue.timer, timer_queue.cmd_value);
                break;
            case SW_TIMER_CMD_CHANGE_PRIORITY:
                _timer_change_priority(timer_queue.timer,
                                       timer_queue.cmd_value);
                break;
            case SW_TIMER_CMD_CHANGE_EXECUTE_MODE:
                _timer_change_execute_mode(timer_queue.timer,
                                           timer_queue.cmd_value);
                break;
            case SW_TIMER_CMD_DELETE:
                if (timer_queue.timer->wating_for_execution) {
                    // Waiting for the timeout event to complete
                    timer_queue.timer->receive_delete_cmd = 1;
                    log_debug("timer %p wait to run\n", timer_queue.timer);
                } else
                    _timer_delete(timer_queue.timer);
                break;
            case SW_TIMER_CMD_TASK_WAKE_UP:
            default: break;
        }
    } while (uxQueueMessagesWaiting(g_timer_cmd_queue));
}

static void timer_list_check(void) {
    sw_timer_t* timer_list = NULL;

    if (g_timer_ctrl.current_time > SW_TIMER_SHIFT_THRESHOLD)
        _timer_shift(SW_TIMER_SHIFT_THRESHOLD);

    // check timer list
    while (g_timer_ctrl.current_time >= g_timer_ctrl.alarm_time) {
        timer_list = utils_dlist_first_entry(&g_timer_ctrl.working_list,
                                             sw_timer_t, list);
        if (timer_list) {
            if (g_timer_ctrl.current_time > timer_list->timeout)
                utils_dlist_del(&timer_list->list);
            else
                timer_list = NULL;
        } else {
            TIMER_ERR_LOG();
            g_timer_ctrl.alarm_time = VT_MAX_TIMER;
            break;
        }

        if (timer_list) {
            switch_to_running_task(timer_list);
            if (timer_list->auto_reload)
                _timer_start(timer_list);
        }

        alarm_time_reset();
    }
}

static void sw_timer_isr(uint32_t timer_id) {
    static uint32_t last_alarm_time = 0, u32_same_alarm_time = 0;
    g_timer_ctrl.current_time++;
    if ((g_timer_ctrl.current_time >= g_timer_ctrl.alarm_time)
        || (g_timer_ctrl.current_time > SW_TIMER_SHIFT_THRESHOLD)) {
        if (last_alarm_time != g_timer_ctrl.alarm_time) {
            last_alarm_time = g_timer_ctrl.alarm_time;
            u32_same_alarm_time = 0;
        } else {
            u32_same_alarm_time++;
            if ((u32_same_alarm_time % SW_TIMER_TRIGGER_TASK_TIMEOUT) == 1)
                SW_TIMER_TASK_WAKE_UP_FROM_ISR;
        }
    }
}

uint32_t sw_timer_hw_init(uint32_t u32_timer_id) {

    uint32_t t_return = 0;
    timern_t* TIMER;
    timern_t* Timer_Base[3] = {TIMER0, TIMER1, TIMER2};
    timer_config_mode_t cfg;

    cfg.int_en = ENABLE;
    cfg.mode = TIMER_PERIODIC_MODE;
    /*the input clock is 32M/s, so it will become 4M ticks per second */
    cfg.prescale = TIMER_PRESCALE_1;

    TIMER = Timer_Base[u32_timer_id];
    TIMER->clear = 1; /*clear interrupt*/

    timer_open(u32_timer_id, cfg, sw_timer_isr);
    NVIC_SetPriority((IRQn_Type)(Timer0_IRQn + u32_timer_id), 3);
    timer_start(u32_timer_id, 32000); /*so each tick is 1ms, 1000HZ*/
    return t_return;
}

static void timer_handler(void* params) {
    uint32_t u32_timer_id = (uint32_t)params;

    log_info("%s start\n", __FUNCTION__);
    if (sw_timer_hw_init(u32_timer_id) != 0) {
        log_error("########################\n");
        log_error("# HW Timer %d init fail #\n", u32_timer_id);
        log_error("########################\n");
        //break;
    }
    while (1) {
        timer_cmd_queue_check();
        timer_list_check();
    }
}

uint32_t sw_timer_init(uint32_t u32_timer_id) {
    uint32_t t_return = 0;
    TaskHandle_t t_thread;

    do {
        memset(&g_timer_ctrl, 0x0, sizeof(g_timer_ctrl));
        g_timer_ctrl.alarm_time = VT_MAX_TIMER;
        INIT_UTILS_DLIST_HEAD(&g_timer_ctrl.working_list);

        g_timer_cmd_queue = xQueueCreate(SW_TIMER_CMD_QUEUE_LENGTH,
                                         SW_TIMER_CMD_QUEUE_ITEM_SIZES);
        if (g_timer_cmd_queue == NULL) {
            log_error("timer_queue create fail");
            t_return = 1;
            break;
        }

        xTaskCreate((TaskFunction_t)timer_handler, "htime",
                    CONFIG_SW_TIMER_STACK_SIZE, (void*)u32_timer_id,
                    CONFIG_SW_TIMER_TASK_PRIORITY, &t_thread);

        if (t_thread == NULL) {
            log_error("timer_thread create fail");
            t_return = 1;
            break;
        }

        t_return = timer_running_task_create();
        if (t_return != 0)
            break;
#ifdef CONFIG_SW_TIMER_ENABLE_DEBUG
        t_return = debug_timer_init();
        if (t_return != 0)
            break;
#endif
    } while (0);

    return t_return;
}

sw_timer_t* sw_timer_create(const char* name, uint32_t period,
                            uint32_t auto_reload, sw_timer_pri_t priority,
                            uint32_t execute_mode, void* cb_param,
                            sw_timer_cb cb_function) {
    sw_timer_t* new_timer;

    if (timer_parameter_check(SW_TIMER_CMD_CHANGE_PERIOD, period) < 0) {
        TIMER_ERR_LOG();
        return NULL;
    }

    if (timer_parameter_check(SW_TIMER_CMD_CHANGE_PRIORITY, priority) < 0) {
        TIMER_ERR_LOG();
        return NULL;
    }

    if (timer_parameter_check(SW_TIMER_CMD_CHANGE_EXECUTE_MODE, execute_mode)
        < 0) {
        TIMER_ERR_LOG();
        return NULL;
    }

    new_timer = (sw_timer_t*)pvPortMalloc(sizeof(sw_timer_t));

    if (new_timer) {
        memset(new_timer, 0x0, sizeof(sw_timer_t));
        INIT_UTILS_DLIST_HEAD(&new_timer->list);
        new_timer->name = name;
        new_timer->period = period;
        new_timer->auto_reload = auto_reload ? 1 : 0;
        new_timer->priority = priority;
        new_timer->execute_mode = execute_mode;
        new_timer->cb_param = cb_param;
        new_timer->cb_function = cb_function;
    }
    return new_timer;
}

sw_timer_err_t sw_timer_start(sw_timer_t* timer) {
    return SW_TIMER_START(timer);
}

sw_timer_err_t sw_timer_stop(sw_timer_t* timer) { return SW_TIMER_STOP(timer); }

sw_timer_err_t sw_timer_reset(sw_timer_t* timer) {
    return SW_TIMER_RESET(timer);
}

sw_timer_err_t sw_timer_change_period(sw_timer_t* timer, uint32_t period) {
    return SW_TIMER_CHANGE_PERIOD(timer, period);
}

sw_timer_err_t sw_timer_change_priority(sw_timer_t* timer, uint32_t priority) {
    return SW_TIMER_CHANGE_PRIORITY(timer, priority);
}

sw_timer_err_t sw_timer_change_execute_mode(sw_timer_t* timer,
                                            uint32_t execute_mode) {
    return SW_TIMER_CHANGE_EXECUTE_MODE(timer, execute_mode);
}

sw_timer_err_t sw_timer_delete(sw_timer_t* timer) {
    return SW_TIMER_DELETE(timer);
}

const char* sw_timer_get_timer_name(sw_timer_t* timer) { return timer->name; }

sw_timer_err_t sw_timer_start_from_isr(sw_timer_t* timer) {
    return SW_TIMER_START_FROM_ISR(timer);
}

sw_timer_err_t sw_timer_stop_from_isr(sw_timer_t* timer) {
    return SW_TIMER_STOP_FROM_ISR(timer);
}

sw_timer_err_t sw_timer_reset_from_isr(sw_timer_t* timer) {
    return SW_TIMER_RESET_FROM_ISR(timer);
}

sw_timer_err_t sw_timer_change_period_from_isr(sw_timer_t* timer,
                                               uint32_t period) {
    return SW_TIMER_CHANGE_PERIOD_FROM_ISR(timer, period);
}

sw_timer_err_t sw_timer_change_priority_from_isr(sw_timer_t* timer,
                                                 uint32_t priority) {
    return SW_TIMER_CHANGE_PRIORITY_FROM_ISR(timer, priority);
}

sw_timer_err_t sw_timer_change_execute_mode_from_isr(sw_timer_t* timer,
                                                     uint32_t execute_mode) {
    return SW_TIMER_CHANGE_EXECUTE_MODE_FROM_ISR(timer, execute_mode);
}

sw_timer_err_t sw_timer_delete_from_isr(sw_timer_t* timer) {
    return SW_TIMER_DELETE_FROM_ISR(timer);
}
