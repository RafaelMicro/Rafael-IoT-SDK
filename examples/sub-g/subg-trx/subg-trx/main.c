/** @file
 *
 * @brief BLE example file.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "rfb_sample.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include "cli.h"
#include "hosal_dma.h"
#include "hosal_gpio.h"
#include "hosal_rf.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "hosal_uart.h"
#include "lmac15p4.h"
#include "log.h"
#include "mac_frame_gen.h"
#include "app_hooks.h"
#include "uart_stdio.h"
/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
/*
 * Remark: UART_BAUDRATE_115200 is not 115200...Please don't use 115200 directly
 * Please use macro define  UART_BAUDRATE_XXXXXX
 */
#define PRINTF_BAUDRATE         UART_BAUDRATE_115200 //UART_BAUDRATE_2000000//

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/

subg_test_case_t rfb_pci_test_case;
/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
void set_priotity(void)
{
    NVIC_SetPriority(Uart0_IRQn, 0x01);
    NVIC_SetPriority(CommSubsystem_IRQn, 0x00);
}

static void tx_timer_timeout() {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_TX_TIMER_EVT;
    t_app_q.data = rfb_pci_test_case;
    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
}

static void rx_timer_timeout() {
    app_queue_t t_app_q;
    BaseType_t context_switch;

    t_app_q.event = APP_RX_TIMER_EVT;
    t_app_q.data = 0;
    xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
}

static void app_main_task(void) {
    app_queue_t app_q;
    for (;;) {
        if (xQueueReceive(app_msg_q, &app_q, 0) == pdTRUE) {
            switch (app_q.event) {
                case APP_TX_TIMER_EVT: app_tx_process(app_q.data); break;
                case APP_RX_TIMER_EVT: app_rx_process(); break;
                default: break;
            }
        }
    }
}

static void app_main_entry(void* pvParameters)
{
    /* Init RFB */
    rfb_sample_init(rfb_pci_test_case);
    app_main_task();

    while (1) {
    }
}
/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
int32_t main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
    /* RF system priority set */
    set_priotity();

    /* Init debug pin*/
    //init_default_pin_mux();

    /* initil dama*/
    hosal_dma_init();

    /* initil cli*/
    cli_init();

    app_msg_q = xQueueCreate(5, sizeof(app_queue_t));

    /*tx timer*/
    tx_timer = xTimerCreate("tx_timer", pdMS_TO_TICKS(30), pdFALSE, (void*)0,
                            tx_timer_timeout);

    /*rx timer*/
    rx_timer = xTimerCreate("rx_timer", pdMS_TO_TICKS(1000), pdFALSE, (void*)0,
                            rx_timer_timeout);

    /* Set RFB test case
    1. SUBG_BURST_TX_TEST: Tester sends a certain number of packets
    2. SUBG_SLEEP_TX_TEST: Tester sends a certain number of packets and sleeps between each tx
    3. SUBG_RX_TEST: Tester receives and verify packets
    */
    rfb_pci_test_case = SUBG_SLEEP_TX_TEST;
    printf("Test Case:%X\n", rfb_pci_test_case);

    if (xTaskCreate(app_main_entry, (char*)"main",
                    256, NULL, E_TASK_PRIORITY_APP, NULL) != pdPASS) {
        printf("Task create fail....\r\n");
    }

    vTaskStartScheduler();
}




