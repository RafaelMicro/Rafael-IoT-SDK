/** @file main.c
 *
 * @brief BLE AT command demo.
 * @details AT command: One for central role with TRSP service in client/server role and \n
 *                another one for Peripheral role with TRSP service in client and server role.
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "ble_app.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "hosal_rf.h"
#include "atcmd.h"
#include "shell.h"
#include "ble_host_ref.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define BLE_APP_CB_QUEUE_SIZE           32
#define APP_ISR_QUEUE_SIZE              2
#define APP_REQ_QUEUE_SIZE              6
#define APP_QUEUE_SIZE                  (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE)

#define GPIO_WAKE_UP_PIN                0

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static atcmd_t ble_atcmd;

HOSAL_UART_DEV_DECL(uart_dev, CONFIG_UART_STDIO_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
bool app_request_set(app_queue_param_type type, bool from_isr);
static ble_err_t ble_init(void);
static void ble_app_main(void);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
static int uart0_receive_line_callback(void* p_arg)
{
    if (hosal_uart_get_lsr(&uart_dev) & UART_LSR_BI)
    {
        extern void wake_up_cli_console_isr(void);
        char ch;

        hosal_uart_receive(&uart_dev, &ch, 1);
        wake_up_cli_console_isr();
        hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }
}
#endif

static void ble_evt_handler(ble_module_evt_t event, void *p_param)
{
    atcmd_event_handle(&ble_atcmd, event, p_param);
}

static void ble_evt_svcs_gaps_handler(ble_evt_att_param_t *p_param)
{
    atcmd_gap_service_handle(&ble_atcmd, p_param);
}

static void ble_evt_svcs_atcmd_handler(ble_evt_att_param_t *p_param)
{
    atcmd_atcmd_service_handle(&ble_atcmd, p_param);
}

/* ------------------------------
 *  Methods
 * ------------------------------
 */
bool app_request_set(app_queue_param_type type, bool from_isr)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = type;

    if (from_isr == false)
    {
        if (xSemaphoreTake(semaphore_app, 0) == pdTRUE)
        {
            p_app_q.from_isr = false;
            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
            {
                // send error
                xSemaphoreGive(semaphore_app);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        BaseType_t context_switch = pdFALSE;

        if (xSemaphoreTakeFromISR(semaphore_isr, &context_switch) == pdTRUE)
        {
            p_app_q.from_isr = true;
            context_switch = pdFALSE;
            if (xQueueSendToBackFromISR(g_app_msg_q, &p_app_q, &context_switch) != pdTRUE)
            {
                context_switch = pdFALSE;
                xSemaphoreGiveFromISR(semaphore_isr, &context_switch);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

/* ------------------------------
 *  Application Task
 * ------------------------------
 */
static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
    {
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);
        if (p_tlv != NULL)
        {
            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_GENERAL_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            xSemaphoreGive(semaphore_cb);
        }
    }
    else
    {
        status = BLE_BUSY;
    }

    return status;
}

static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_evt_att_param_t *p_evt_att;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_att = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_SERVICE_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

static ble_err_t ble_l2cap_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_l2cap_evt_param_t *p_evt_l2cap;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_l2cap = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_L2CAP_DATA_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

static void app_main_task(void)
{
    ble_err_t status;
    app_queue_t p_app_q;

    status = BLE_ERR_OK;

    // BLE default setting and profile init
    status = ble_init();
    if (status != BLE_ERR_OK)
    {
        printf("[DEBUG_ERR] ble_init() fail: %d\n", status);
        while (1);
    }

    bool check = jump_to_main();
    CHECK_BOOL(check);

    for (;;)
    {
        if (xQueueReceive(g_app_msg_q, &p_app_q, portMAX_DELAY) == pdTRUE)
        {
            switch (p_app_q.param_type)
            {
            case QUEUE_TYPE_APP_REQ:
            {
                if (p_app_q.from_isr == true)
                {
                    xSemaphoreGive(semaphore_isr);
                }
                else
                {
                    xSemaphoreGive(semaphore_app);
                }
                ble_app_main();
            }
            break;

            case QUEUE_TYPE_OTHERS:
            {
                if (p_app_q.param.pt_tlv != NULL)
                {
                    switch (p_app_q.param.pt_tlv->type)
                    {
                    case APP_GENERAL_EVENT:
                    {
                        ble_evt_param_t *evt_param = (ble_evt_param_t *)p_app_q.param.pt_tlv->value;
                        ble_evt_handler(evt_param->event, (void *)&evt_param->event_param);
                    }
                    break;

                    case APP_SERVICE_EVENT:
                    {
                        ble_evt_att_param_t *p_svcs_param = (ble_evt_att_param_t *)p_app_q.param.pt_tlv->value;
                        if (p_svcs_param->gatt_role == BLE_GATT_ROLE_CLIENT)
                        {
                            att_db_link[p_svcs_param->host_id].p_client_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                        }
                        else if (p_svcs_param->gatt_role == BLE_GATT_ROLE_SERVER)
                        {
                            att_db_link[p_svcs_param->host_id].p_server_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                        }
                    }
                    break;

                    case APP_L2CAP_DATA_EVENT:
                        break;

                    default:
                        break;
                    }

                    // free
                    vPortFree(p_app_q.param.pt_tlv);
                    xSemaphoreGive(semaphore_cb);
                }
            }
            break;

            default:
                break;
            }
        }
    }
}

static void ble_app_main(void)
{
    atcmd_main_handle(&ble_atcmd);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */

static ble_err_t ble_init(void)
{
    ble_err_t status;
    ble_unique_code_format_t unique_code_param;
    ble_gap_addr_t device_addr_param;

    status = BLE_ERR_OK;
    do
    {
        status = ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        atcmd_init(&ble_atcmd);

        //Init the ble param
        status = atcmd_ble_param_init(&ble_atcmd);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = atcmd_profile_init(&ble_atcmd, ble_evt_svcs_gaps_handler, ble_evt_svcs_atcmd_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    // register shell command
    extern sh_cmd_t g_cli_cmd_atplus;
    shell_register_cmd(&g_cli_cmd_atplus);

    return status;
}

static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;
    extern int cli_console_init(void);

    // banner
    printf("Reset MCU, Chip_ID=0x67\n");
    printf("OK\n");

    // application queue & semaphore
    g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
    semaphore_cb = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
    semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
    semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);

    // BLE Stack init
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    if (ble_host_stack_init(&ble_task_level) == 0) {
        //printf("BLE stack initial success...\n");
    }
    else {
        //printf("BLE stack initial fail...\n");
    }
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    hosal_uart_callback_set(&uart_dev, HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK, uart0_receive_line_callback, &uart_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, (void *)NULL);

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_UART_RX);
#endif

    cli_console_init();

    // wake up pin
    input_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    input_cfg.usr_cb = app_gpio_handler;
    input_cfg.param = NULL;
    hosal_gpio_cfg_input(GPIO_WAKE_UP_PIN, input_cfg);
    hosal_gpio_debounce_enable(GPIO_WAKE_UP_PIN);
    hosal_gpio_int_enable(GPIO_WAKE_UP_PIN);
}

/**
 * @brief Initializes the pin multiplexing.
 *
 * This function sets all GPIO pins to GPIO mode, except for GPIO16 and GPIO17,
 * which are reserved for specific functions.
 *
 * @return void This function does not return a value.
 */
static void pin_mux_init(void) 
{
    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (int i = 0; i < 32; i++) {
        if (i == 16 || i == 17) {
            continue; // Skip GPIO16 and GPIO17
        }
        hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
    }
}

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the RF module, starts the application initialization,
 * and enters an infinite loop to keep the application running.
 *
 * @param pvParameters Pointer to parameters passed to the task (not used).
 * @return void This function does not return a value.
 */
static void app_main_entry(void* pvParameters)
{
    hosal_lpm_init();
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);
    
    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void uart_rx_data_handle(uint8_t *data, uint8_t length)
{
    extern void sleep_cli_console(void);

    atcmd_uart_handle(&ble_atcmd, data, length);
#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
    sleep_cli_console();
    hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
#endif
}

/**
 * @brief Application entry point.
 *
 * Initializes hardware, configures peripherals, and starts the main application task.
 *
 * @return int Not used.
 */
int main(void)
{
    pin_mux_init();
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();

    if (xTaskCreate(app_main_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }
    puts("[OS] Starting OS Scheduler...\r\n");
    puts("\r\n");
    vTaskStartScheduler();
    while (1) {
    }

    return 0;
}
