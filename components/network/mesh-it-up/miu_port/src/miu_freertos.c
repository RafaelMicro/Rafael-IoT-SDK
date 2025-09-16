
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/ncp.h>
#include <openthread/platform/entropy.h>
#include <openthread/tasklet.h>
#include <semphr.h>
#include <task.h>


#include "log.h"
#include "miu_port.h"


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

void otSysProcessDrivers(otInstance* aInstance, ot_system_event_t sevent) {
    ot_alarmTask(sevent);
#ifndef CONFIG_OT_RCP_EZMESH
    otrAppProcess(sevent);
#endif
    ot_uartTask(sevent);
    ot_radioTask(sevent);
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

static void* ot_calloc(size_t n, size_t size) {
    size_t total = n * size;
    void* ptr = pvPortMalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

static void otrStackTask(void* aContext) {
    /** This task is an example to handle both main event loop of openthread task lets and 
     * hardware drivers for openthread, such as radio, alarm timer and also uart shell.
     * Customer can implement own task for both of two these missions with other privoded APIs.  */

    OT_THREAD_SAFE(ot_entropy_init(); ot_alarmInit(); ot_radioInit();
                   otrStackInit();
                   mbedtls_platform_set_calloc_free(ot_calloc, vPortFree);

#if OPENTHREAD_ENABLE_DIAG
                   otDiagInit(ot_instance);
#endif
                   otrInitUser(ot_instance);
#ifdef CONFIG_OT_RCP_EZMESH
                   rf_ot_cpc_init();
#endif
    );

    while (true) {
        ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;
        OT_GET_NOTIFY(sevent);

        if (sevent & OT_SYSTEM_EVENT_OT_TASKLET) {
            otTaskletsProcess(ot_instance);
        }
        otSysProcessDrivers(ot_instance, sevent);
        if (!otTaskletsArePending(ot_instance)) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        wdt_kick();
    }

    otInstanceFinalize(ot_instance);
    ot_instance = NULL;

    vTaskDelete(NULL);
}

void miuStart(void) {
    ot_extLock = xSemaphoreCreateMutexStatic(&stackLock);
    configASSERT(ot_extLock != NULL);

    OT_THREAD_SAFE(xTaskCreate(otrStackTask, "miu-openthread-task",
                               CONFIG_MIU_OPENTHREAD_TASK_SIZE, ot_instance,
                               E_TASK_PRIORITY_MIU_OPENTHREAD, &ot_taskHandle);)
}
