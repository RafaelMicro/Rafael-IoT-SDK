#include <FreeRTOS.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include <timers.h>
#include "hosal_uart.h"
#include "mcu.h"
#include "uart_stdio.h"

extern uint8_t _heap_start;
extern uint8_t _heap_size;
extern uint8_t _heap2_start;
extern uint8_t _heap2_size;

#ifndef CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE
#define CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE 8192
#endif

#ifndef CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_PRIORITY
#define CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_PRIORITY 5
#endif
static HeapRegion_t xHeapRegions[] = {
    {&_heap_start, (unsigned int)&_heap_size}, //set on runtime
    {&_heap2_start, (unsigned int)&_heap2_size},
    {NULL, 0}, /* Terminates the array. */
    {NULL, 0}  /* Terminates the array. */
};

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask,
                                                         char* pcTaskName) {
    puts("Stack Overflow checked\r\n");
    if (pcTaskName) {
        printf("Stack name %s\r\n", pcTaskName);
    }
    taskDISABLE_INTERRUPTS();
    NVIC_DisableIRQ(Wdt_IRQn);
    while (1) {
        /*empty here*/
    }
}

void __attribute__((weak)) vApplicationMallocFailedHook(void) {
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize());
    taskDISABLE_INTERRUPTS();
    NVIC_DisableIRQ(Wdt_IRQn);
    while (1) {
        /*empty here*/
    }
}

void __attribute__((weak)) vApplicationIdleHook(void) {
    __WFI();
    /*empty*/
}

#if (configUSE_TICKLESS_IDLE != 0)
void __attribute__((weak)) vApplicationSleep(TickType_t xExpectedIdleTime) {
    /*empty*/
}
#endif

#if (configUSE_TICK_HOOK != 0)
void __attribute__((weak)) vApplicationTickHook(void) { /*empty*/ }
#endif

void __attribute__((weak))
vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                              StackType_t** ppxIdleTaskStackBuffer,
                              uint32_t* pulIdleTaskStackSize) {
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void __attribute__((weak))
vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                               StackType_t** ppxTimerTaskStackBuffer,
                               uint32_t* pulTimerTaskStackSize) {
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void __attribute__((weak)) vAssertCalled(const char* const pcFileName,
                                         unsigned long ulLine) {
    char* current_task_name = (char*)pcTaskGetTaskName(
        xTaskGetCurrentTaskHandle());
    printf("assert: [%s] %s:%ld\n", current_task_name, pcFileName, ulLine);
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

static void pin_mux_init(void) {
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        pin_set_mode(i, MODE_GPIO);
    }
    return;
}

static void _dump_boot_info(void) {
    puts("\r\n");
    puts("------------------------------------------------------------\r\n");
    puts("ARM Cortex-M3 ");
    puts("SoC: ");
    puts(CHIP_NAME);
    puts(" Target Board: ");
    puts(CONFIG_TARGET_BOARD);

#ifdef HOSAL_SOC_TARGET_CUSTOMER
    puts(" Customer: ");
    puts(HOSAL_SOC_TARGET_CUSTOMER);
#endif
    puts("\r\n");

    puts("Build Version: ");
    puts(RAFAEL_SDK_VER);
    puts("\r\n");

    puts("Build Date: ");
    puts(__DATE__);
    puts("\r\n");
    puts("Build Time: ");
    puts(__TIME__);
    puts("\r\n");

    printf("Heap %u@%p, %u@%p\r\n", (unsigned int)&_heap_size, &_heap_start,
           (unsigned int)&_heap2_size, &_heap2_start);

    puts("------------------------------------------------------------\r\n");
}

static void app_main_entry(void* pvParameters) {
    extern int main();
    main();
    vTaskDelete(NULL);
}

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
static void __init_sleep() {
    timern_t* TIMER;
#if (CONFIG_HOSAL_SOC_SLEEP_TIMER_ID == 4)
    TIMER = TIMER4;
    NVIC_DisableIRQ((IRQn_Type)(Timer4_IRQn));
    NVIC_SetPriority((IRQn_Type)(Timer4_IRQn), 1);
#elif (CONFIG_HOSAL_SOC_SLEEP_TIMER_ID == 3)
    TIMER = TIMER3;
    NVIC_DisableIRQ((IRQn_Type)(Timer3_IRQn));
    NVIC_SetPriority((IRQn_Type)(Timer3_IRQn), 1);
#endif

    TIMER->LOAD = 0;
    TIMER->CLEAR = 0;
    TIMER->CONTROL.reg = 0;

    TIMER->CONTROL.bit.PRESCALE = 0;
    TIMER->CONTROL.bit.MODE = 0;
    TIMER->CONTROL.bit.EN = 0;

    Lpm_Set_Low_Power_Level(LOW_POWER_LEVEL_SLEEP0);
    Lpm_Enable_Low_Power_Wakeup(LOW_POWER_WAKEUP_GPIO);
    Lpm_Enable_Low_Power_Wakeup(LOW_POWER_WAKEUP_32K_TIMER);
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime_ms) {
    TickType_t xModifiableIdleTime;
#if (CONFIG_HOSAL_SOC_SLEEP_TIMER_ID == 4)
    timern_t* TIMER = TIMER4;
#elif (CONFIG_HOSAL_SOC_SLEEP_TIMER_ID == 3)
    timern_t* TIMER = TIMER3;
#endif
    uint32_t now_v;

    __disable_irq();

    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        __enable_irq();
        return;
    }

    if (xExpectedIdleTime_ms > 0) {
        if (Lpm_Get_Low_Power_Mask_Status() != LOW_POWER_NO_MASK) {
            __WFI();
        } else {
            TIMER->LOAD = ((xExpectedIdleTime_ms) * 40) - 1;
            TIMER->CLEAR = 1;
            TIMER->CONTROL.bit.INT_ENABLE = 1;
            TIMER->CONTROL.bit.EN = 1;

            Lpm_Enter_Low_Power_Mode();

            TIMER->CONTROL.bit.EN = 0;
            TIMER->CONTROL.bit.INT_ENABLE = 0;
            TIMER->CLEAR = 1;
            Delay_us(250);
            now_v = (TIMER->VALUE / 40);

            if (now_v > (xExpectedIdleTime_ms)) {
                now_v = 0;
            }
            xModifiableIdleTime = (xExpectedIdleTime_ms)-now_v;

            vTaskStepTick(xModifiableIdleTime);
        }
    }

    __enable_irq();
}

#endif

int rt582_main(void) {
    pin_mux_init();

    vPortDefineHeapRegions(xHeapRegions);

    uart_stdio_init();

    _dump_boot_info();

    dma_init();

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    __init_sleep();
#endif

    printf("Starting RT582 now %d.... \r\n",
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    if (xTaskCreate(app_main_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }

    puts("[OS] Starting OS Scheduler...\r\n");

    vTaskStartScheduler();
    while (1)
        ;

    return 0;
}