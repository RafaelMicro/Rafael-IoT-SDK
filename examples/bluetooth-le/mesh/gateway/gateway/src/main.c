/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file
 *
 * @brief BLE Mesh gateway application.
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
/** @brief Application state machine states.
 *  @details Tracked by app_main_event and stepped by app_main_loop().
 */
typedef enum
{
    APP_INIT_EVT,   /**< Initial state; transitions to APP_IDLE_EVT on the first loop pass. */
    APP_IDLE_EVT,   /**< Idle state; no periodic action is taken. */
} app_main_evt_t;

/** @brief Identifiers for the events dispatched through app_msg_q.
 *  @details Used as the index into app_event_handler[] to select the handler
 *           that processes a queued app_queue_t message.
 */
typedef enum
{
    APP_BLE_MESH_EVT = 0,   /**< A mesh_tlv_t indication was received from the mesh stack. */
    APP_BUTTON_EVT,         /**< A button GPIO interrupt was posted by button_cb(). */
    APP_QUEUE_UART_MSG_EVT, /**< A gateway command was parsed from UART input. */
} app_queue_evt_t;

//=============================================================================
//                Private Struct
//=============================================================================

/** @brief Message format posted to app_msg_q.
 *  @details Carries the event type and its associated data to the handler
 *           selected from app_event_handler[] in app_main_task().
 */
typedef struct
{
    mesh_tlv_t *pt_tlv; /**< TLV buffer associated with the event, or NULL if unused; freed by
                          *   app_main_task() when the handler returns true. */
    uint32_t event;     /**< Event type, one of app_queue_evt_t. */
    uint32_t data;      /**< Event-specific scalar data (e.g. GPIO pin number for APP_BUTTON_EVT). */
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

/**
 * @brief Handles an application model event message indication from the mesh stack.
 *
 * When CLI support is enabled, delegates parsing to app_model_evt_parse(); otherwise
 * prints the opcode, source/destination addresses and raw parameter bytes for debugging.
 * In both cases, forwards the event to the connected host over the gateway UART interface
 * via ble_mesh_gateway_cmd_send() using the DEVICE_APPLICATION_SVC_CMD service.
 *
 * @param pt_msg_idc Pointer to the application model event message indication received
 *                   from the mesh stack.
 */
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

/**
 * @brief Handles a configuration model event message indication from the mesh stack.
 *
 * When CLI support is enabled, delegates parsing to cfg_model_evt_parse(); otherwise
 * prints the opcode, source address and raw parameter bytes for debugging. In both cases,
 * forwards the event to the connected host over the gateway UART interface via
 * ble_mesh_gateway_cmd_send() using the DEVICE_CONFIGURATION_SVC_CMD service (the appkey
 * index is always reported as 0, since configuration messages are not appkey-bound).
 *
 * @param pt_msg_idc Pointer to the configuration model event message indication received
 *                   from the mesh stack.
 */
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

/**
 * @brief Handles data received on the BLE Transparent Service (TRSPS) write characteristic.
 *
 * Prints the received bytes for debugging, then loops the data back by writing it to the
 * TRSPS notify characteristic via ble_trsps_data_set().
 *
 * @param len Length of the received data, in bytes.
 * @param p_trsps_data Pointer to the received data buffer.
 */
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

/**
 * @brief Dispatches a mesh stack event indication queued via mesh_app_evt_indication_cb().
 *
 * Inspects the TLV type carried by @p p_mesh_tlv and handles it accordingly: forwards
 * unprovisioned-device and provisioning-complete indications to the host over the gateway
 * UART interface (and, when CLI support is enabled, drives the auto-provisioning flow via
 * auto_prov_device_start()/auto_prov_device_complete()), dispatches application and
 * configuration model events and TRSPS write data to their respective handlers, and logs
 * TRSPS connection and mesh friendship state-change indications.
 *
 * @param data Unused for this handler.
 * @param p_mesh_tlv Pointer to the TLV-encoded mesh event indication to process.
 * @return Always returns true, indicating app_main_task() should free @p p_mesh_tlv after
 *         this call.
 */
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

/**
 * @brief Handles a button-press event queued from button_cb().
 *
 * Maps the GPIO pin number carried in @p data to an action: pin 0 creates a BLE TRSPS
 * connection, pin 1 cancels a pending BLE TRSPS connection attempt; pins 2-4 currently
 * take no action.
 *
 * @param data GPIO pin number that triggered the button event.
 * @param p_mesh_tlv Unused for this handler.
 * @return Always returns false, indicating app_main_task() should not attempt to free
 *         any TLV buffer (none is allocated for this event).
 */
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

/**
 * @brief Runs one iteration of the application's main state machine.
 *
 * Called on every pass of app_main_task()'s loop. Currently only transitions the
 * application state from APP_INIT_EVT to APP_IDLE_EVT on the first call; the idle state
 * takes no further action.
 */
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


/**
 * @brief Main application task loop.
 *
 * Sets the BLE mesh device advertising name, then repeatedly runs the application state
 * machine (app_main_loop()) and services app_msg_q: each dequeued message is dispatched
 * to the handler registered in app_event_handler[] for its event type, and the
 * associated mesh_tlv_t buffer is freed if the handler returns true. Never returns.
 */
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

/**
 * @brief BLE host general-event callback registered for APP_GENERAL_EVENT.
 *
 * Wraps the incoming HCI event parameters in a mesh_tlv_t tagged with
 * MSG_TAG_BEARER_HCI_EVENT and forwards it to the mesh bearer layer via
 * mesh_queue_sendto().
 *
 * @param p_param Pointer to the ble_evt_param_t event data supplied by the BLE host stack.
 * @return BLE_ERR_OK on success.
 * @retval BLE_ERR_DATA_MALLOC_FAIL if allocating the TLV buffer failed.
 * @retval BLE_BUSY if the event could not be queued to the mesh bearer layer.
 */
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
            vPortFree(p_tlv);
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

/**
 * @brief BLE host service-data callback registered for APP_SERVICE_EVENT.
 *
 * Wraps the incoming ATT service data in a mesh_tlv_t tagged with
 * MSG_TAG_BEARER_HCI_ACL_DATA and forwards it to the mesh bearer layer via
 * mesh_queue_sendto().
 *
 * @param p_param Pointer to the ble_evt_att_param_t data supplied by the BLE host stack.
 * @return BLE_ERR_OK on success.
 * @retval BLE_ERR_DATA_MALLOC_FAIL if allocating the TLV buffer failed.
 * @retval BLE_BUSY if the data could not be queued to the mesh bearer layer.
 */
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
            vPortFree(p_tlv);
            //xSemaphoreGive(semaphore_cb);
        }

    } while (0);

    return status;
}

/**
 * @brief UART receive callback that queues a parsed gateway command for processing.
 *
 * Registered as the UART handler's receive callback. Wraps @p pt_tlv in an
 * APP_QUEUE_UART_MSG_EVT message and posts it to app_msg_q for app_main_task() to
 * dispatch to ble_mesh_gateway_cmd_proc().
 *
 * @param pt_tlv Pointer to the TLV-encoded command parsed from UART input by
 *               ble_mesh_gateway_cmd_parser().
 */
static void app_uart_msg_recv(mesh_tlv_t *pt_tlv)
{
    app_queue_t app_q;

    app_q.event = APP_QUEUE_UART_MSG_EVT;
    app_q.pt_tlv = pt_tlv;

    xQueueSendToBack(app_msg_q, &app_q, 0);
}

/**
 * @brief Mesh stack event-indication callback registered via mesh_stack_init().
 *
 * Copies the TLV event indication pointed to by @p p_param into a newly allocated
 * mesh_tlv_t, queues it to app_msg_q as an APP_BLE_MESH_EVT message for
 * app_mesh_event_handler() to process, and frees the original @p p_param buffer owned
 * by the mesh stack. Retries posting to the queue until it succeeds.
 *
 * @param p_param Pointer to the mesh_tlv_t event indication allocated and owned by the
 *                mesh stack.
 * @return BLE_ERR_OK on success.
 * @retval BLE_ERR_INVALID_PARAMETER if @p p_param is NULL.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL if allocating the queued TLV buffer failed.
 * @retval BLE_BUSY if a queue post attempt failed (the function keeps retrying until it
 *         succeeds, so this may be returned even though the message was ultimately
 *         delivered).
 */
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
/**
 * @brief Initializes the gateway application.
 *
 * Sets the local BLE device address and PIB, creates the application message queue,
 * registers the BLE host general-event and service-data callbacks, initializes the BLE
 * host stack and the mesh stack (with mesh_app_evt_indication_cb() as the mesh event
 * callback), and initializes the UART handler with the gateway command parser and
 * receive callback. When CLI support is enabled, also initializes the debug console CLI.
 */
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

/**
 * @brief GPIO interrupt callback for the buttons.
 *
 * Called from GPIO ISR context when a configured button pin transitions. Reads and
 * prints the pin's current level, and for pins 0-4 posts an APP_BUTTON_EVT message
 * carrying the pin number to app_msg_q for app_button_handler() to process.
 *
 * @param pin GPIO pin number that triggered the interrupt.
 * @param isr_param Unused ISR callback parameter.
 * @note Called from interrupt context.
 */
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

/**
 * @brief Configures GPIO pins 0-4 as debounced button inputs.
 *
 * Sets debounce time, pull-up, and rising-edge interrupt configuration for GPIO pins
 * 0 through 4, registers button_cb() as their interrupt handler, and enables the GPIO
 * interrupt in the NVIC.
 */
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
        hosal_gpio_cfg_input_parameters(i, pin_cfg, true);
        hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
    }
    NVIC_EnableIRQ(Gpio_IRQn);
}

/**
 * @brief FreeRTOS task entry point for the application.
 *
 * Initializes low-power mode support and the RF/BLE controller, configures the button
 * GPIOs, then performs application initialization (app_init()) and enters the main
 * application task loop (app_main_task()), which never returns.
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
/**
 * @brief Initializes the default pin multiplexing.
 *
 * Sets all GPIO pins to GPIO mode except GPIO16, GPIO17, GPIO28 and GPIO29, then
 * configures GPIO16/GPIO17 as UART0 RX/TX.
 *
 * @return void This function does not return a value.
 */
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


/**
 * @brief Main function.
 *
 * Entry point of the application. Initializes the default pin multiplexing, UART stdio,
 * heap regions and boot-info dump, then creates the app_main_entry() FreeRTOS task and
 * starts the scheduler.
 *
 * @return int This function does not return a value.
 */
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

