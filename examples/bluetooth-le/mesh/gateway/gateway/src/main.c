/** @file
 *
 * @brief FreeRTOSbySystem example file.
 *
 */


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "util.h"

#include "mesh_task.h"
#include "pib.h"
#include "ble_mesh_lib_api.h"

#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_event.h"
#include "ble_mesh_gateway.h"

#include "hosal_rf.h"
#include "hosal_sysctrl.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"

#include "uart_handler.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

// Advertising device name
#define DEVICE_NAME                 "MESH_GW"

//=============================================================================
//                Private ENUM
//=============================================================================
typedef enum
{
    APP_INIT_EVT,
    APP_IDLE_EVT,
} app_main_evt_t;

typedef enum
{
    APP_BLE_MESH_EVT = 0,
    APP_BUTTON_EVT,
    APP_QUEUE_UART_MSG_EVT,
} app_queue_evt_t;

//=============================================================================
//                Private Struct
//=============================================================================

typedef struct
{
    mesh_tlv_t *pt_tlv;
    uint32_t event;
    uint32_t data;
} app_queue_t;
//=============================================================================
//                Private Function Declaration
//=============================================================================
static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);

//=============================================================================
//                Private Global Variables
//=============================================================================
static uint32_t app_main_event = APP_INIT_EVT;
static mesh_app_cfg_t gt_app_cfg;
static const uint8_t  DEVICE_NAME_STR[] = {DEVICE_NAME};

xQueueHandle app_msg_q;
uint8_t (* const app_event_handler[])(uint32_t data, mesh_tlv_t *p_mesh_tlv) =
{
    app_mesh_event_handler,     //APP_BLE_MESH_EVT
    app_button_handler,         //APP_BUTTON_EVT
    ble_mesh_gateway_cmd_proc,  //APP_QUEUE_UART_MSG_EVT
};

//=============================================================================
//                Private Functions
//=============================================================================
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
extern void app_model_evt_parse(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc);
extern void cfg_model_evt_parse(mesh_cfg_mdl_evt_msg_idc_t *pt_msg_idc);
extern void auto_prov_device_start(uint16_t primary_addr, uint8_t *p_device_uuid);
extern void auto_prov_device_complete(uint16_t *p_primary_addr, mesh_prov_complete_idc_t *p_prov_complete_idc);
#endif

static void app_model_evt_msg_handler(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc)
{
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
    app_model_evt_parse(pt_msg_idc);
#else
    uint32_t i;
    printf("Recv APP model event messages[%04X][0x%04X -> 0x%04X] ... \n", BE2LE16(pt_msg_idc->opcode), pt_msg_idc->src_addr, pt_msg_idc->dst_addr);
    printf("Parameter:\n");
    for (i = 0; i < pt_msg_idc->parameter_len ; i++)
    {
        printf("%02x ", pt_msg_idc->parameter[i]);
    }
    printf("\n");

#endif
    ble_mesh_gateway_cmd_send(DEVICE_APPLICATION_SVC_CMD, pt_msg_idc->src_addr, pt_msg_idc->appkey_index,
                              pt_msg_idc->opcode, pt_msg_idc->parameter, pt_msg_idc->parameter_len);

}

static void cfg_model_evt_msg_handler(mesh_cfg_mdl_evt_msg_idc_t *pt_msg_idc)
{
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
    cfg_model_evt_parse(pt_msg_idc);
#else
    uint32_t i;
    printf("Recv configure model event messages[%04X][0x%04X] ... \n", pt_msg_idc->opcode, pt_msg_idc->src_addr);
    printf("Parameter:\n");
    for (i = 0; i < pt_msg_idc->parameter_len ; i++)
    {
        printf("%02x ", pt_msg_idc->parameter[i]);
    }
    printf("\n");
#endif

    ble_mesh_gateway_cmd_send(DEVICE_CONFIGURATION_SVC_CMD, pt_msg_idc->src_addr, 0,
                              pt_msg_idc->opcode, pt_msg_idc->parameter, pt_msg_idc->parameter_len);

}

static void ble_trsps_evt_msg_handler(uint16_t len, uint8_t *p_trsps_data)
{
    uint16_t i;
    bool tx_success;

    printf("Receive BLE TRSPS data:\n");
    for (i = 0; i < len ; i++)
    {
        printf("%02x ", p_trsps_data[i]);
    }
    printf("\n");

    tx_success = ble_trsps_data_set(len, p_trsps_data);

    printf("Loopback received BLE TRSPS data %d:\n", tx_success);
}

static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
    static uint16_t primary_addr = 0x0100;
#endif

    switch (p_mesh_tlv->type)
    {
    case TYPE_MESH_UNPROV_DEVICE_IDC:
    {
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
        mesh_unprov_device_idc_t *p_unprov_device_idc = (mesh_unprov_device_idc_t *)p_mesh_tlv->value;

        auto_prov_device_start(primary_addr, p_unprov_device_idc->uuid);
#endif
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0, MESH_NWK_OPCODE_UNPROV_DEVICE_LIST, p_mesh_tlv->value, p_mesh_tlv->length);
    }
    break;

    case TYPE_MESH_PROVISION_COMPLETE_IDC:
    {
#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
        mesh_prov_complete_idc_t *p_prov_complete_idc = (mesh_prov_complete_idc_t *)p_mesh_tlv->value;

        auto_prov_device_complete(&primary_addr, p_prov_complete_idc);
#endif
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0, MESH_NWK_OPCODE_DEVICE_PROV_STAUS, p_mesh_tlv->value, p_mesh_tlv->length);
    }
    break;

    case TYPE_MESH_APP_MDL_EVT_MSG_IDC:
    {
        app_model_evt_msg_handler((mesh_app_mdl_evt_msg_idc_t *)p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_CFG_MDL_EVT_MSG_IDC:
    {
        cfg_model_evt_msg_handler((mesh_cfg_mdl_evt_msg_idc_t *)p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_BLE_SVC_TRSPS_WRITE_IDC:
    {
        ble_trsps_evt_msg_handler(p_mesh_tlv->length, p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_BLE_TRSPS_CONNECT_CREATE_IDC:
    {
        printf("BLE TRSPS connection creating, status %d\n", p_mesh_tlv->value[0]);
    }
    break;

    case TYPE_MESH_BLE_TRSPS_CONNECT_CANCEL_IDC:
    {
        printf("BLE TRSPS connection cancel, status %d\n", p_mesh_tlv->value[0]);
    }
    break;

    case TYPE_MESH_BLE_TRSPS_CONNECTED_IDC:
    {
        printf("BLE TRSPS connected\n");
    }
    break;

    case TYPE_MESH_BLE_TRSPS_DISCONNECTED_IDC:
    {
        printf("BLE TRSPS disconnected\n");
    }
    break;

    case TYPE_MESH_FRIEND_ESTABLISED_IDC:
    {
        mesh_friend_established_idc_t *p_friend_establish;

        p_friend_establish = (mesh_friend_established_idc_t *)p_mesh_tlv->value;
        printf("Friendship is established, address: 0x%04x, element cnt %d\n", p_friend_establish->lpn_address,
                   p_friend_establish->element_cnt);
    }
    break;

    case TYPE_MESH_FRIEND_TERMINATED_IDC:
    {
        mesh_friend_terminated_idc_t *p_friend_terminated;

        p_friend_terminated = (mesh_friend_terminated_idc_t *)p_mesh_tlv->value;
        printf("Friendship is terminated, address: 0x%04x, reason %d\n", p_friend_terminated->lpn_address,
                   p_friend_terminated->terminate_reason);
    }
    break;

    default:
        break;
    }
    return true;
}

static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uint32_t pin = data;

    switch (pin)
    {
    case 0:
        ble_trsps_connection_create();
        break;
    case 1:
        ble_trsps_connection_cancel();
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }

    return false;
}

static void app_main_loop(void)
{
    switch (app_main_event)
    {
    case APP_INIT_EVT:
        app_main_event = APP_IDLE_EVT;
        break;

    case APP_IDLE_EVT:
        break;

    default:
        break;
    }
}


static void app_main_task(void)
{
    app_queue_t app_q;

    ble_mesh_device_name_set(strlen(DEVICE_NAME_STR), (uint8_t *)DEVICE_NAME_STR);

    for (;;)
    {
        app_main_loop();

        if (xQueueReceive(app_msg_q, &app_q, 20) == pdTRUE)
        {
            if (app_event_handler[app_q.event](app_q.data, app_q.pt_tlv) == true)
            {
                vPortFree(app_q.pt_tlv);
            }
        }
    }
}

static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    mesh_tlv_t *p_tlv;

    status = BLE_ERR_OK;

    do {
        p_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);
        if (p_tlv == NULL)
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            break;
        }

        p_tlv->type = MSG_TAG_BEARER_HCI_EVENT;
        memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);

        if (mesh_queue_sendto(MESH_LAYER_BEARER, p_tlv) != MESH_SUCCESS)
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    ble_evt_att_param_t *p_evt_att;
    mesh_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    do {

        p_evt_att = p_param;
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);
        if (p_tlv == NULL)
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            //xSemaphoreGive(semaphore_cb);
            break;
        }
        p_tlv->type = MSG_TAG_BEARER_HCI_ACL_DATA;
        memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

        if (mesh_queue_sendto(MESH_LAYER_BEARER, p_tlv) != MESH_SUCCESS)
        {
            status = BLE_BUSY;
            //xSemaphoreGive(semaphore_cb);
        }

    } while (0);

    return status;
}

static void app_uart_msg_recv(mesh_tlv_t *pt_tlv)
{
    app_queue_t app_q;

    app_q.event = APP_QUEUE_UART_MSG_EVT;
    app_q.pt_tlv = pt_tlv;

    xQueueSendToBack(app_msg_q, &app_q, 0);
}

static ble_err_t mesh_app_evt_indication_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t app_q;

    status = BLE_ERR_OK;

    do
    {
        if (p_param == NULL)
        {
            printf("[%s] null point get!\n", __func__);
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        app_q.pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + ((mesh_tlv_t*)p_param)->length);
        if (app_q.pt_tlv == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        app_q.event = APP_BLE_MESH_EVT;
        app_q.pt_tlv->length =  ((mesh_tlv_t*)p_param)->length;
        app_q.pt_tlv->type =  ((mesh_tlv_t*)p_param)->type;
        memcpy(app_q.pt_tlv->value,  ((mesh_tlv_t*)p_param)->value,  ((mesh_tlv_t*)p_param)->length);
        while (xQueueSendToBack(app_msg_q, &app_q, 20) != pdPASS)
        {
            printf("[%s] send fail\n", __func__);
            status = BLE_BUSY;
        };

        vPortFree(p_param);

    } while (0);

    return status;
}

//=============================================================================
//                Public Functions
//=============================================================================
void app_init(void)
{
    mesh_task_priority_t task_priority_cfg;
    ble_task_priority_t ble_task_level;
    uart_handler_parm_t uart_handler_param = {0};
    ble_gap_addr_t  device_addr = {.addr_type = RANDOM_STATIC_ADDR,
                                   .addr = {0x21, 0x11, 0x22, 0x33, 0x25, 0xC6}
                                  };

    pib_init(NULL, NULL, 1, &device_addr);

    app_msg_q = xQueueCreate(16, sizeof(app_queue_t));

    if (ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb) != BLE_ERR_OK)
    {
        printf("ble_host_callback_set(APP_GENERAL_EVENT) fail...\n");
    }

    if (ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb) != BLE_ERR_OK)
    {
        printf("ble_host_callback_set(APP_SERVICE_EVENT) fail...\n");
    }

    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;    
    if (ble_host_stack_init(&ble_task_level) == 0) {
        printf("BLE stack initial success...\n");
    }
    else
    {
        printf("BLE stack initial fail...\n");
    }

    task_priority_cfg.bearer_level = configMAX_PRIORITIES - 8;
    task_priority_cfg.mesh_level = configMAX_PRIORITIES - 9;
    if (mesh_stack_init(&task_priority_cfg, mesh_app_evt_indication_cb) == MESH_TASK_ERR_OK)
    {
        printf("BLE mesh stack initial success...\n");
    }
    else
    {
        printf("BLE mesh stack initial fail...\n");
    }

    uart_handler_param.UartParserCB[0] = ble_mesh_gateway_cmd_parser;
    uart_handler_param.UartRecvCB[0] = app_uart_msg_recv;

    uart_handler_init(&uart_handler_param, configMAX_PRIORITIES - 10);

#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)
    extern int cli_console_init(void);
    cli_console_init();
#endif
}

void button_cb(uint32_t pin, void* isr_param) 
{
    BaseType_t context_switch;
    app_queue_t t_app_q;
    uint32_t pin_value;

    hosal_gpio_pin_get(pin, &pin_value);
    printf("GPIO%d=%d\r\n", pin, pin_value);
    switch (pin)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        t_app_q.event = APP_BUTTON_EVT;
        t_app_q.data = pin;

        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
        break;

    default:
        break;
    }

    return;
}

void button_init(void)
{
    uint8_t i = 0;
    hosal_gpio_input_config_t pin_cfg;
    /* gpio0 pin setting */
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg.usr_cb = button_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    NVIC_SetPriority(Gpio_IRQn, 7);
    for (i = 0; i < 5; i++)
    {
        hosal_gpio_debounce_enable(i);
        hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
        hosal_gpio_cfg_input(i, pin_cfg);    
        hosal_gpio_int_enable(i);
    }
    NVIC_EnableIRQ(Gpio_IRQn);
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

    button_init();
    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/*this is pin mux setting*/
static void init_default_pin_mux(void)
{
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++)
    {
        if ((i != 16) && (i != 17) && (i != 28) && (i != 29))
        {
            hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
        }
    }

    /*uart0 pinmux*/
    hosal_pin_set_mode(16, HOSAL_MODE_UART0_RX);     /*GPIO16 as UART0 RX*/
    hosal_pin_set_mode(17, HOSAL_MODE_UART0_TX);     /*GPIO17 as UART0 TX*/

    return;
}


int main(void)
{
    init_default_pin_mux();
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

