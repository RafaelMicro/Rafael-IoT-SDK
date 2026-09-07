/*
 * Copyright (c) 2026 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 * Standalone application entry for the TFLM hello_world example.
 *
 * Brings up the SoC and FreeRTOS, then runs the TFLM inference from a task.
 * tflm_hello_world_run() (hello_world_run.cc) prints its results via printf,
 * routed to the UART stdio port. Mirrors the CMSIS-DSP example's main.c.
 */

#include <stdint.h>
#include <stdio.h>

#if CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "app_hooks.h"
#endif

#include "uart_stdio.h"
#include "hosal_sysctrl.h"
#include "hosal_dma.h"
#include "dump_boot_info.h"

/* Inference entry point, implemented in C++ (hello_world_run.cc). */
extern void tflm_hello_world_run(void);

static TaskHandle_t tflm_task_handle;

static void tflm_task(void *parameters_ptr)
{
    (void)parameters_ptr;

    printf("[TFLM] running hello_world example: %s\r\n", CONFIG_BUILD_PROJECT);
    tflm_hello_world_run();
    printf("[TFLM] example finished.\r\n");

    for (;;)
    {
        vTaskDelay(5000);
    }
}

int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();
    hosal_dma_init();

    printf("Starting TFLM example on %s ....\r\n", CONFIG_CHIP);

    /* TFLM interpreter + arena are static; a 2048-word stack is ample. */
    if (xTaskCreate(tflm_task, (char *)"tflm", 2048, NULL,
                    E_TASK_PRIORITY_NORMAL, &tflm_task_handle) != pdPASS)
    {
        printf("tflm task create fail....\r\n");
    }

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
    while (1) {}
}
