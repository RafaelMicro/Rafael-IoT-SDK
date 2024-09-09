/**
 * Copyright (c) 2024 Rex Huang, All Rights Reserved.
 */
/** @file syslog.c
 *
 * @author Rex Huang
 * @version 0.1
 * @date 2024/08/14
 * @license
 * @description
 */

#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "syslog.h"
#include "task.h"

#include "log.h"

#ifndef CONFIG_SYSLOG_ENTRY_SIZE
#define CONFIG_SYSLOG_ENTRY_SIZE 800
#endif

#define CONFIG_SYSLOG_TASK_PRIORITY (configMAX_PRIORITIES - 2)
#define CONFIG_SYSLOG_STACK_SIZE    256 // task stack size (number of words)

#define SYSLOG_QUEUE_LENGTH 20

static syslog_t g_sys_log[CONFIG_SYSLOG_ENTRY_SIZE];
static print_fn g_print_fn[SYSLOG_MODULE_ALL];
static QueueHandle_t g_syslog_queue = NULL;

static uint32_t current_idx = 0;
volatile bool halt_log = false;

static void syslog_print_default(syslog_t* log) {
    printf("[%10u] %u-%u-%u: 0x%08x, 0x%08x\r\n", log->tick, log->module,
           log->type, log->sub_type, log->msg1, log->msg2);
}

void syslog_insert(syslog_module_t t_module, uint8_t u8_type,
                   uint16_t u16_sub_type, uint32_t msg1, uint32_t msg2) {
    syslog_t t_syslog = {
        .tick = xTaskGetTickCount(),
        .module = t_module,
        .type = u8_type,
        .sub_type = u16_sub_type,
        .msg1 = msg1,
        .msg2 = msg2,
    };
    while (xQueueSendToBack(g_syslog_queue, &t_syslog, portMAX_DELAY)
           != pdTRUE) {}
}

void syslog_insert_from_isr(syslog_module_t t_module, uint8_t u8_type,
                            uint16_t u16_sub_type, uint32_t msg1,
                            uint32_t msg2) {
    BaseType_t context_switch = pdFALSE;
    syslog_t t_syslog = {};

    t_syslog.tick = xTaskGetTickCountFromISR();
    t_syslog.module = t_module;
    t_syslog.type = u8_type;
    t_syslog.sub_type = u16_sub_type;
    t_syslog.msg1 = msg1;
    t_syslog.msg2 = msg2;

    xQueueSendToBackFromISR(g_syslog_queue, &t_syslog, &context_switch);

    portYIELD_FROM_ISR(context_switch);
}

void syslog_print_fn_register(syslog_module_t t_module, print_fn fn) {
    g_print_fn[t_module] = fn;
}

void syslog_print(syslog_module_t t_module, uint8_t u8_type) {
    uint32_t u32_log, u32_idx, u32_last_idx = 0xFFFF;
    uint32_t u32_duration_tick;

    u32_idx = (current_idx + 1) % CONFIG_SYSLOG_ENTRY_SIZE;
    for (u32_log = 0; u32_log < CONFIG_SYSLOG_ENTRY_SIZE; u32_log++) {
        if (g_sys_log[u32_idx].module) {
            if ((t_module == SYSLOG_MODULE_ALL)
                || ((t_module == g_sys_log[u32_idx].module)
                    && ((u8_type == g_sys_log[u32_idx].type)
                        || (u8_type == 0xFF)))) {
                u32_duration_tick = (u32_last_idx == 0xFFFF)
                                        ? 0
                                        : (g_sys_log[u32_idx].tick
                                           - g_sys_log[u32_last_idx].tick);
                u32_last_idx = u32_idx;
                printf("%8d, ", u32_duration_tick);
                if (g_print_fn[g_sys_log[u32_idx].module])
                    g_print_fn[g_sys_log[u32_idx].module](&g_sys_log[u32_idx]);
                else
                    syslog_print_default(&g_sys_log[u32_idx]);
            }
        }
        u32_idx = (u32_idx + 1) % CONFIG_SYSLOG_ENTRY_SIZE;
    }
}

void syslog_clear(void) {
    halt_log = true;
    current_idx = 0;
    memset((uint8_t*)&g_sys_log, 0x0, sizeof(g_sys_log));
    halt_log = false;
}

static void syslog_save(syslog_t* pt_syslog) {
    while (halt_log == true) {}
    halt_log = true;
    current_idx = (current_idx + 1) % CONFIG_SYSLOG_ENTRY_SIZE;
    g_sys_log[current_idx].tick = pt_syslog->tick;
    g_sys_log[current_idx].module = pt_syslog->module;
    g_sys_log[current_idx].type = pt_syslog->type;
    g_sys_log[current_idx].sub_type = pt_syslog->sub_type;
    g_sys_log[current_idx].msg1 = pt_syslog->msg1;
    g_sys_log[current_idx].msg2 = pt_syslog->msg2;
    halt_log = false;
}

static void syslog_handler(void* params) {
    while (1) {
        syslog_t t_syslog;
        if (xQueueReceive(g_syslog_queue, &t_syslog, portMAX_DELAY) == pdTRUE) {
            // error
            syslog_save(&t_syslog);
        }
    }
}

uint32_t syslog_init(void) {
    TaskHandle_t t_thread = NULL;
    g_syslog_queue = xQueueCreate(SYSLOG_QUEUE_LENGTH, sizeof(syslog_t));
    if (g_syslog_queue == NULL) {
        log_error("syslog queue create fail\n");
        return 1;
    }

    xTaskCreate(syslog_handler, "syslog", CONFIG_SYSLOG_STACK_SIZE, NULL,
                CONFIG_SYSLOG_TASK_PRIORITY, &t_thread);
    if (t_thread == NULL) {
        log_error("syslog task create fail\n");
        return 1;
    }
    return 0;
}