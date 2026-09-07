/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 * Standalone application entry for this CMSIS-DSP example.
 *
 * The ARM example .c in this folder is compiled with the per-file definitions
 * `main=dsp_example_run` and `SEMIHOSTING` (see CMakeLists.txt), turning its
 * main() into a callable dsp_example_run() that prints "SUCCESS"/"FAILURE" via
 * printf (routed to the UART stdio port). This file brings up the SoC and
 * FreeRTOS and runs it.
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

/* ARM example main(), renamed via -Dmain=dsp_example_run at compile time. */
extern int32_t dsp_example_run(void);

static TaskHandle_t dsp_task_handle;

static void dsp_task(void *parameters_ptr)
{
    (void)parameters_ptr;

    printf("[DSP] running CMSIS-DSP example: %s\r\n", CONFIG_BUILD_PROJECT);
    dsp_example_run();  /* prints SUCCESS / FAILURE */
    printf("[DSP] example finished.\r\n");

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

    printf("Starting CMSIS-DSP example on %s ....\r\n", CONFIG_CHIP);

    if (xTaskCreate(dsp_task, (char *)"dsp", 1024, NULL,
                    E_TASK_PRIORITY_NORMAL, &dsp_task_handle) != pdPASS)
    {
        printf("dsp task create fail....\r\n");
    }

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
    while (1) {}
}
