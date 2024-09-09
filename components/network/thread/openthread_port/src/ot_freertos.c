
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
// #include <openthread-core-config.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/ncp.h>
#include <openthread/tasklet.h>

// #include "mbedtls/platform.h"
#include "openthread_port.h"

// #include <mbedtls/platform.h>

#include "log.h"

ot_system_event_t ot_system_event_var = OT_SYSTEM_EVENT_NONE;
static SemaphoreHandle_t ot_extLock = NULL;
static otInstance* ot_instance = NULL;
static TaskHandle_t ot_taskHandle = NULL;

static StaticQueue_t stackLock;

// static StaticTask_t ot_task;

// static StackType_t ot_stackTask_stack[OT_TASK_SIZE];

__attribute__((weak)) void otrAppProcess(ot_system_event_t sevent) {}

void otTaskletsSignalPending(otInstance* aInstance) {
    if (aInstance) {
        OT_NOTIFY(OT_SYSTEM_EVENT_OT_TASKLET);
    }
}

otInstance* otrGetInstance() { return ot_instance; }

void otSysProcessDrivers(otInstance* aInstance) {
    ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;

    OT_GET_NOTIFY(sevent);
    ot_alarmTask(sevent);
    ot_uartTask(sevent);
    ot_radioTask(sevent);
    otrAppProcess(sevent);
    // ota_event_handler(sevent);
}

void otSysEventSignalPending(void) {
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(ot_taskHandle, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(ot_taskHandle);
    }
}

void otrLock(void) {
    if (ot_extLock) {
        xSemaphoreTake(ot_extLock, portMAX_DELAY);
    }
}

void otrUnlock(void) {
    if (ot_extLock) {
        xSemaphoreGive(ot_extLock);
    }
}

void otrStackInit(void) {
    ot_instance = otInstanceInitSingle();
    assert(ot_instance);
}

extern void rf_ot_cpc_rcp_process(void);
extern int mbedtls_platform_set_calloc_free(void* (*calloc_func)(size_t,
                                                                 size_t),
                                            void (*free_func)(void*));

static void otrStackTask(void* aContext) {
    /** This task is an example to handle both main event loop of openthread task lets and 
     * hardware drivers for openthread, such as radio, alarm timer and also uart shell.
     * Customer can implement own task for both of two these missions with other privoded APIs.  */

    OT_THREAD_SAFE(ot_entropy_init();
                   // #if USE_PURE_OT_RCP
                   //         ot_uart_init();
                   // #endif
                   ot_alarmInit(); ot_radioInit(); otrStackInit();
                   mbedtls_platform_set_calloc_free(pvPortCalloc, vPortFree);

#if OPENTHREAD_ENABLE_DIAG
                   otDiagInit(ot_instance);
#endif
                   otrInitUser(ot_instance);
#ifdef CONFIG_OT_RCP_EZMESH
                   rf_ot_cpc_init();
#endif
    );

    while (true) {
        // log_info("ot-run");
        OT_THREAD_SAFE(otTaskletsProcess(ot_instance);
                       otSysProcessDrivers(ot_instance);

#ifdef CONFIG_OT_RCP_EZMESH
                       rf_ot_cpc_rcp_process();
#endif
        );
        wdt_kick();
        if (otTaskletsArePending(ot_instance) == false) {
            // log_info("ot-block");
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    }

    otInstanceFinalize(ot_instance);
    ot_instance = NULL;

    vTaskDelete(NULL);
}

void otrStart(void) {
    ot_extLock = xSemaphoreCreateMutexStatic(&stackLock);
    configASSERT(ot_extLock != NULL);

    OT_THREAD_SAFE(xTaskCreate(otrStackTask, "threadTask",
                               CONFIG_OPENTHREAD_TASK_SIZE, ot_instance,
                               (configMAX_PRIORITIES - 16), &ot_taskHandle);)

    // OT_THREAD_SAFE(ot_taskHandle = xTaskCreateStatic(
    //                    otrStackTask, "threadTask", OT_TASK_SIZE, ot_instance,
    //                    OT_TASK_PRORITY, ot_stackTask_stack, &ot_task););
}
