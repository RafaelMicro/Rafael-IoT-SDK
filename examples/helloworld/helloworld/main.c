#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
#include "app_hooks.h"
#endif
#include "uart_stdio.h"
#include "hosal_uart.h"
#include "hosal_sysctrl.h"
#include "hosal_dma.h"
#include "app_hooks.h"
#include "hosal_trng.h"
 
TaskHandle_t xTas1kHandle,xTas2kHandle;

static void app_task_1(void *parameters_ptr);
static void app_task_2(void *parameters_ptr);

int main(void) {
 
    uart_stdio_init();
    vHeapRegionsInt();
    hosal_dma_init();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    if (xTaskCreate(app_task_1, (char*)"apptask_1",
                    256, NULL,
                    E_TASK_PRIORITY_LOWEST, &xTas1kHandle)
        != pdPASS) {
        printf("Task 1 create fail....\r\n");
    }

    if (xTaskCreate(app_task_2, (char*)"apptask_2",
                    256, NULL,
                    E_TASK_PRIORITY_NORMAL, &xTas2kHandle)
        != pdPASS) {
        printf("Task 2 create fail....\r\n");
    }

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
    while (1) {}
}

void app_task_1(void *parameters_ptr)
{
    uint32_t test[1];
    uint32_t i = 0;

    test[0] = 0x00;

    for(;;){
        printf("task 1 running\r\n");    
        hosal_trng_get_random_number(test, 1);
        printf("%.8x\r\n", test[i]);
        vTaskDelay(500);
    } 
}


void app_task_2(void *parameters_ptr)
{

    for(;;){
        printf("task 2 running\r\n");
        vTaskDelay(1000);
    } 
}
