/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file
 *
 * @brief BLE Mesh lightness and TRSP element demo.
 *
 */
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "mesh_mdl_handler.h"
#include "ble_mesh_element.h"
#include "log.h"

#include "mmdl_opcodes.h"
#include "mmdl_common.h"
#include "mmdl_defs.h"
#include "hosal_rf.h"

#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_uart.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "mcu.h"

#include "ble_mesh_lib_api.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_event.h"
#include "mesh_task.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define MAX_TRSP_DATA_LEN           (377)

#define INVALID_DST_ADDRESS         (0x0000)

// Advertising device name
#define DEVICE_NAME                 "BLE_MESH"
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/** @brief UART command table entry used to recognize and dispatch typed user commands.
 * @details Each entry pairs a human-readable command example (for help text) with the
 *          matching command header prefix and the handler invoked when a line of UART
 *          input starts with that prefix.
 */
typedef struct
{
    const char          *p_cmd_example;    /**< Example string shown to the user, e.g. "DstAddr0x001c". */
    const char          *p_cmd_header;     /**< Command prefix that identifies this command, e.g. "DstAddr0x". */
    void                (*p_cmd_process)(uint8_t *p_data); /**< Handler invoked with the data following the prefix. */
} user_cmd_t;

/** @brief Message posted to the application event queue (app_msg_q).
 * @details Carries the event type and data for BLE mesh, button, and UART events consumed by
 *          app_main_task(); pt_tlv optionally carries a heap-allocated TLV payload which the
 *          handler must free once processed.
 */
typedef struct
{
    mesh_tlv_t *pt_tlv;    /**< Pointer to an allocated mesh TLV payload (used for mesh events). */
    uint32_t event;        /**< Index into app_event_handler[] selecting which handler to run. */
    uint32_t data;         /**< Event-specific data (e.g. GPIO pin number or UART character). */
} app_queue_t;

/** @brief Identifies which handler in app_event_handler[] should process a queued event.
 * @details Used as the app_queue_t::event value.
 */
typedef enum
{
    APP_BLE_MESH_EVT,   /**< Event originates from the BLE mesh stack. */
    APP_BUTTON_EVT,     /**< Event originates from a button press. */
    APP_UART_EVT,       /**< Event originates from UART character reception. */
} app_evt_t;


/** @brief States of the application's main provisioning/idle state machine.
 * @details Drives app_main_loop(); transitions between GATT/mesh provisioning, the
 *          in-progress provisioning wait, provisioning completion, and idle.
 */
typedef enum
{
    APP_GATT_PROVISION_EVT,   /**< Start (or resume) provisioning over the GATT bearer. */
    APP_MESH_PROVISION_EVT,   /**< Start (or resume) provisioning over the mesh (PB-ADV) bearer. */
    APP_PROVISIONING_EVT,     /**< Provisioning is in progress; waiting for it to complete. */
    APP_PROVISION_DONE_EVT,   /**< Provisioning just completed; one-time post-provision setup runs. */
    APP_IDLE_EVT,             /**< Device is provisioned and idle; nothing to do. */
} app_main_evt_t;
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/

extern ble_mesh_element_param_t g_element_info[];

#define UART0_OPERATION_PORT            0
#if (CONFIG_UART_STDIO_BAUDRATE == 9600)
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_9600)
#elif (CONFIG_UART_STDIO_BAUDRATE == 115200)
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)
#elif (CONFIG_UART_STDIO_BAUDRATE == 500000)
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_500000)
#elif (CONFIG_UART_STDIO_BAUDRATE == 1000000)
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_1000000)
#elif (CONFIG_UART_STDIO_BAUDRATE == 2000000)
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_2000000)
#endif
/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
extern int retarget_stdout_string(char *str, int length, uart_t *p_csr);
extern int retarget_stdout_char(int ch, uart_t *p_csr);
extern void app_init(void);


/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static uint8_t app_uart_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static void user_data_dst_address_set(uint8_t *p_rx_data);
static void user_data_tx_ack_set(uint8_t *p_rx_data);
/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/

static mesh_app_cfg_t gt_app_cfg;
uint8_t (* const app_event_handler[])(uint32_t data, mesh_tlv_t *p_mesh_tlv) =
{
    app_mesh_event_handler,
    app_button_handler,
    app_uart_handler,
};

static user_cmd_t g_user_cmd_list[] =
{
    {
        .p_cmd_example = "DstAddr0x001c",
        .p_cmd_header =  "DstAddr0x",
        .p_cmd_process = user_data_dst_address_set,
    },
    {
        .p_cmd_example = "TxAckEnable0",
        .p_cmd_header =  "TxAckEnable",
        .p_cmd_process = user_data_tx_ack_set,
    },
};

static uint32_t app_main_event = APP_GATT_PROVISION_EVT;
static uint16_t user_data_dst_addr = INVALID_DST_ADDRESS;
static uint16_t user_data_appkey_idx = 0xFFFF;
static uint8_t user_data_req_ack = 0;
static uint8_t user_cmd_num = (sizeof(g_user_cmd_list) / sizeof(user_cmd_t));

static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

static TimerHandle_t unprov_idc_tmr;
xQueueHandle app_msg_q;

/**
 * @brief Configures the default GPIO pin multiplexing for the board.
 *
 * Sets every GPIO pin (0-31) to plain GPIO mode, except GPIO16 and GPIO17 which are
 * reserved for other functions and left untouched.
 *
 * @return void This function does not return a value.
 */
static void init_default_pin_mux(void)
{
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++)
    {
        if ((i != 16) && (i != 17))
        {
            hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
        }
    }

    return;
}

/**
 * @brief Converts a single ASCII hex character to its 4-bit numeric value.
 *
 * @param c ASCII character to convert ('0'-'9', 'a'-'f', or 'A'-'F').
 * @return uint8_t The numeric value (0-15) of the hex digit.
 * @retval 0xFF c is not a valid hex digit.
 */
static uint8_t parse_hex_digit(char c)
{
    uint8_t result = 0xff;

    if ((c >= '0') && (c <= '9'))
    {
        result = c - '0';
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        result = c - 'a' + 10;
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        result = c - 'A' + 10;
    }

    return result;
}

/**
 * @brief Periodic timer callback that blinks the lightness elements' PWM outputs while unprovisioned.
 *
 * Called every 100 ms while the device is not yet provisioned; toggles the PWM duty cycle of
 * both lightness elements (ELEMENT0_PWN_ID, ELEMENT1_PWN_ID) between 0 and 100 to blink them
 * as a visual "unprovisioned" indicator. Once the device becomes provisioned, it turns both
 * outputs off and stops itself.
 *
 * @param t_timer Handle of the expired timer (unprov_idc_tmr).
 * @return void This function does not return a value.
 */
static void unprov_tmr_cb(TimerHandle_t t_timer)
{
    static uint8_t toggle;

    if (pib_is_provisioned())
    {
        hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, 0);
        hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 0);
        xTimerStop(unprov_idc_tmr, 0);
    }
    else
    {
        toggle ^= 1;
        if (toggle)
        {
            hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, 0);
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 0);
        }
        else
        {
            hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, 100);
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 100);
        }
    }
}

/**
 * @brief Parses a 4-digit hex string into the UART TRSP destination address.
 *
 * Called by uart_data_handler() when a "DstAddr0x" command is recognized. Converts the 4
 * hex characters following the command header into user_data_dst_addr and prints the result.
 *
 * @param p_rx_data Pointer to the 4 ASCII hex characters following the "DstAddr0x" prefix.
 * @return void This function does not return a value.
 */
static void user_data_dst_address_set(uint8_t *p_rx_data)
{
    uint8_t dst_addr[4];

    dst_addr[0] = parse_hex_digit((char)p_rx_data[0]);
    dst_addr[1] = parse_hex_digit((char)p_rx_data[1]);
    dst_addr[2] = parse_hex_digit((char)p_rx_data[2]);
    dst_addr[3] = parse_hex_digit((char)p_rx_data[3]);

    user_data_dst_addr = (((dst_addr[0] & 0x0F) << 12) |
                          ((dst_addr[1] & 0x0F) << 8) |
                          ((dst_addr[2] & 0x0F) << 4) | (dst_addr[3] & 0x0F));

    printf("Destination address of uart data: 0x%04x\n", user_data_dst_addr);
}

/**
 * @brief Parses a hex digit into the UART TRSP "request ack" flag.
 *
 * Called by uart_data_handler() when a "TxAckEnable" command is recognized. Sets
 * user_data_req_ack from the single hex character following the command header.
 *
 * @param p_rx_data Pointer to the ASCII hex character following the "TxAckEnable" prefix.
 * @return void This function does not return a value.
 */
static void user_data_tx_ack_set(uint8_t *p_rx_data)
{
    user_data_req_ack = parse_hex_digit((char)p_rx_data[0]);

    printf("Uart data tx with ack %d\n", user_data_req_ack);
}

/**
 * @brief Handles one received UART character, buffering it and processing complete lines.
 *
 * Accumulates characters from the UART into a static line buffer. On overflow, the buffer is
 * discarded and the index reset. On receiving '\n' or '\r', the buffered line is checked
 * against the known user commands (g_user_cmd_list) and dispatched to the matching command
 * handler if found; otherwise, if a valid destination address has been configured via the
 * "DstAddr0x" command and an app key has been bound to the Rafael TRSP client model, the line
 * is sent as a Rafael TRSP Set message (acknowledged or unacknowledged, per
 * user_data_req_ack) to that destination. If no destination address has been configured yet, a
 * usage message listing the available commands is printed instead. The line buffer index is
 * reset once the line has been processed.
 *
 * @param ch Character received from the UART.
 * @return void This function does not return a value.
 */
static void uart_data_handler(char ch)
{
    static uint8_t rx_buffer[MAX_TRSP_DATA_LEN + 1];
    static uint16_t index = 0;
    uint16_t i, cmd_processed = false;
    mmdl_transmit_info_t tx_info;
    raf_trsp_set_msg_t *p_raf_trsp_set_msg;
    int status;

    rx_buffer[index++] = ch;

    if (index > (MAX_TRSP_DATA_LEN + 1)) /* (+1) length '\r' or '\n' */
    {
        printf("Uart data out of memory %d\n", index - 1);
        index = 0;
    }
    else if ((ch == '\n') || (ch == '\r'))
    {
        for (i = 0 ; i < user_cmd_num; i++)
        {
            if ((index == strlen(g_user_cmd_list[i].p_cmd_example) + 1/* (+1) length '\r' or '\n' */) &&
                    (strncmp((char *)rx_buffer, g_user_cmd_list[i].p_cmd_header, strlen(g_user_cmd_list[i].p_cmd_header)) == 0))
            {
                g_user_cmd_list[i].p_cmd_process(rx_buffer + strlen(g_user_cmd_list[i].p_cmd_header));
                cmd_processed = true;
            }
        }

        if (cmd_processed == false)
        {
            if (user_data_dst_addr != INVALID_DST_ADDRESS)
            {
                if (user_data_appkey_idx == 0xFFFF)
                {
                    printf("No APP key bind to Rafael TRSP client model\n");
                }
                else if (pib_is_provisioned())
                {
                    printf("Send Rafael TRSP set to 0x%04x\n", user_data_dst_addr);

                    for (i = 0; i < (index - 1); i++)
                    {
                        printf("%02x ", rx_buffer[i]);
                    }
                    printf("\n... " );

                    p_raf_trsp_set_msg = pvPortMalloc(sizeof(raf_trsp_set_msg_t) + (index - 1/*(-1) removed '\n' or '\r'*/));
                    if (p_raf_trsp_set_msg == NULL)
                    {
                        printf("fail (no memory)\n");
                    }
                    else
                    {
                        tx_info.dst_addr = user_data_dst_addr;
                        tx_info.src_addr = pib_primary_address_get();
                        tx_info.appkey_index = user_data_appkey_idx;
                        p_raf_trsp_set_msg->data_len = (index - 1/*(-1) removed '\n' or '\r'*/);
                        memcpy(p_raf_trsp_set_msg->data, rx_buffer, p_raf_trsp_set_msg->data_len);

                        if (user_data_req_ack)
                        {
                            status = mmdl_rafael_trsp_send_set(tx_info, p_raf_trsp_set_msg);
                        }
                        else
                        {
                            status = mmdl_rafael_trsp_send_unack_set(tx_info, p_raf_trsp_set_msg);
                        }
                        printf("result %d\n", status);
                        vPortFree(p_raf_trsp_set_msg);
                    }
                }
                else
                {
                    printf("Device was not provision\n");
                }
            }
            else
            {
                printf("Set destination address of TRSP data.\n");
                printf("Command example:\n");
                for (i = 0 ; i < user_cmd_num; i++)
                {
                    printf("%s\n", g_user_cmd_list[i].p_cmd_example);
                }
            }
        }
        index = 0;
    }
}

/**
 * @brief Handles button-press events, restarting device provisioning.
 *
 * Invoked via app_event_handler[APP_BUTTON_EVT] for GPIO button pins 0-4. Pins 0 and 1
 * (re)start mesh or GATT provisioning respectively: any in-progress provisioning is stopped,
 * an already-provisioned device has its proxy service disabled, the element info and local
 * provisioning info are reset, and app_main_event is set to APP_MESH_PROVISION_EVT (pin 0) or
 * APP_GATT_PROVISION_EVT (pin 1) so app_main_loop() restarts provisioning. Pins 2-4 are
 * currently unused (no-op).
 *
 * @param data GPIO pin number that triggered the button event (0-4).
 * @param p_mesh_tlv Unused for button events.
 * @return uint8_t Always false; no TLV buffer needs to be freed by the caller for this event type.
 */
static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uint8_t memory_free = false;

    switch (data)
    {
    case 0: //Mesh provsion start
    case 1: //Gatt provsion start
        if (app_main_event == APP_PROVISIONING_EVT)
        {
            ble_mesh_provision_stop();
        }

        if (pib_is_provisioned())
        {
            ble_mesh_proxy_disable();
        }
        element_info_init();
        pib_provision_info_local_reset();
        app_main_event = (data == 0) ? APP_MESH_PROVISION_EVT : APP_GATT_PROVISION_EVT;
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

    return memory_free;
}

/**
 * @brief Handles queued UART character events.
 *
 * Invoked via app_event_handler[APP_UART_EVT]; forwards the received character to
 * uart_data_handler() for line buffering and command/data processing.
 *
 * @param data The received UART character, passed through as uint32_t.
 * @param p_mesh_tlv Unused for UART events.
 * @return uint8_t Always false; no TLV buffer needs to be freed by the caller for this event type.
 */
static uint8_t app_uart_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uart_data_handler(data);
    
    return false;
}

/**
 * @brief Handles BLE TRSPS characteristic write data received from the mesh event queue.
 *
 * Logs the raw bytes received from the BLE Transparent Service Profile Server (TRSPS), then
 * loops the same data back out through ble_trsps_data_set() (e.g. as a notification to a
 * connected central), logging whether the loopback send succeeded.
 *
 * @param len Length in bytes of the received TRSPS data.
 * @param p_trsps_data Pointer to the received TRSPS data buffer.
 * @return void This function does not return a value.
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
 * @brief Dispatches BLE mesh stack indications/confirmations received on the app message queue.
 *
 * Invoked via app_event_handler[APP_BLE_MESH_EVT] for every mesh_tlv_t delivered by
 * mesh_app_evt_indication_cb(). Handles: provisioning start confirmation (advances
 * app_main_event and records this node's element addresses), node reset, BLE TRSPS
 * characteristic writes (forwarded to ble_trsps_evt_msg_handler()), generic model event
 * messages (forwarded to mmdl_evt_msg_cb(), e.g. Lightness/TRSP model messages), app-key
 * bind indications (captures the app key index bound to the Rafael TRSP client model),
 * provisioning completion, and provisioning/proxy advertising enable results. Unrecognized
 * TLV types are ignored.
 *
 * @param data Unused for mesh events.
 * @param p_mesh_tlv Pointer to the mesh TLV describing the event type and its payload.
 * @return uint8_t Always true; the caller must free the TLV buffer (p_mesh_tlv) after this
 *         handler returns.
 */
static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    mesh_prov_start_cfm_t *p_start_cfm;
    uint8_t memory_free = true, i;

    switch (p_mesh_tlv->type)
    {
    case TYPE_MESH_PROV_START_CFM:
    {
        p_start_cfm = (mesh_prov_start_cfm_t *)p_mesh_tlv->value;
        if (p_start_cfm->status != 0)
        {
            printf("Provision timeout!\n");
            app_main_event = APP_GATT_PROVISION_EVT;
        }
        else
        {
            printf("Provision done! address 0x%04x\n", p_start_cfm->element_address);
            for (i = 0 ; i < ble_mesh_element_count ; i++)
            {
                g_element_info[i].element_address = p_start_cfm->element_address + i;
            }
            app_main_event = APP_PROVISION_DONE_EVT;
        }
    }
    break;

    case TYPE_MESH_NODE_RESET_IDC:
    {
        printf("Recv CfgNodeRest ... \n");
        element_info_init();
        app_main_event = APP_GATT_PROVISION_EVT;
    }
    break;

    case TYPE_MESH_BLE_SVC_TRSPS_WRITE_IDC:
    {
        ble_trsps_evt_msg_handler(p_mesh_tlv->length, p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_APP_MDL_EVT_MSG_IDC:
    {
        mmdl_evt_msg_cb((mesh_app_mdl_evt_msg_idc_t *)p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_CFG_MDL_APP_BIND_IDC:
    {
        mesh_cfg_mdl_app_bind_idc_t *p_app_bind_idc;
        p_app_bind_idc = (mesh_cfg_mdl_app_bind_idc_t *)p_mesh_tlv->value;

        printf("Appkey %d bind to model 0x%04x\n", p_app_bind_idc->appkey_index, p_app_bind_idc->model_id);
        if (p_app_bind_idc->model_id == MMDL_RAFAEL_TRSP_CL_MDL_ID)
        {
            user_data_appkey_idx = p_app_bind_idc->appkey_index;
        }
    }
    break;

    case TYPE_MESH_PROVISION_COMPLETE_IDC:
    {
        mesh_prov_complete_idc_t *p_prov_device;

        p_prov_device = (mesh_prov_complete_idc_t *)p_mesh_tlv->value;
        if (p_prov_device->status == 1)
        {
            printf("Device provision fail  ... \n");
        }
    }
    break;

    case TYPE_MESH_PROV_ADV_ENABLED_IDC:
    {
        mesh_prov_adv_enable_idc_t *p_adv_enable_status;
        p_adv_enable_status = (mesh_prov_adv_enable_idc_t *)p_mesh_tlv->value;
        printf("Provision ADV enable result: %d\n", p_adv_enable_status->status);
    }
    break;

    case TYPE_MESH_PROXY_ENABLE_IDC:
    {
        mesh_proxy_adv_enable_idc_t *p_adv_enable_status;
        p_adv_enable_status = (mesh_proxy_adv_enable_idc_t *)p_mesh_tlv->value;
        //printf("Proxy ADV enable result: %d\n", p_adv_enable_status->status);
    }
    break;

    default:
        break;
    }
    return memory_free;
}

/**
 * @brief Runs one step of the application's provisioning/idle state machine.
 *
 * Called repeatedly from app_main_task()'s loop. Starts mesh or GATT provisioning when in the
 * corresponding *_PROVISION_EVT state (if not already provisioned) and arms the unprovisioned
 * indication timer; once provisioning completes, enables the mesh proxy service and
 * transitions to the idle state. No action is taken while provisioning is in progress or once
 * idle.
 *
 * @return void This function does not return a value.
 */
static void app_main_loop(void)
{
    switch (app_main_event)
    {
    case APP_MESH_PROVISION_EVT:
        if (pib_is_provisioned() == 0)
        {
            ble_mesh_provision_start(0);
            printf("Start mesh provision!\n");
            app_main_event = APP_PROVISIONING_EVT;
            xTimerStart(unprov_idc_tmr, 0);
        }
        else
        {
            app_main_event = APP_IDLE_EVT;
        }
        break;

    case APP_GATT_PROVISION_EVT:
        if (pib_is_provisioned() == 0)
        {
            ble_mesh_provision_start(1);
            printf("Start gatt provision!\n");
            app_main_event = APP_PROVISIONING_EVT;
            xTimerStart(unprov_idc_tmr, 0);
        }
        else
        {
            app_main_event = APP_IDLE_EVT;
        }
        break;

    case APP_PROVISIONING_EVT:
        break;

    case APP_PROVISION_DONE_EVT:
        printf("Provision Complete!!\n");

        ble_mesh_proxy_enable();
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
 * Sets the GATT device name, and if the device is already provisioned on entry, resolves the
 * app key bound to the Rafael TRSP client model for each element and enables the mesh proxy
 * service. Then runs forever, driving the provisioning state machine via app_main_loop() and
 * dispatching any event queued on app_msg_q to the matching handler in app_event_handler[],
 * freeing the event's TLV buffer when the handler indicates it is no longer needed.
 *
 * @return void This function does not return a value.
 * @note Never returns; this is the body of the main application FreeRTOS task.
 */
static void app_main_task(void)
{
    ble_mesh_model_param_t  *p_model;
    app_queue_t app_q;
    uint8_t i, j;

    ble_mesh_device_name_set(strlen(DEVICE_NAME_STR), (uint8_t *)DEVICE_NAME_STR);
    if (pib_is_provisioned())
    {
        for (i = 0 ; i < ble_mesh_element_count ; i++)
        {
            g_element_info[i].element_address = pib_primary_address_get() + i;

            //search the appkey index which bind to model "MMDL_RAFAEL_TRSP_CL_MDL_ID"
            if ((user_data_appkey_idx == 0xFFFF) &&
                    (search_model(g_element_info + i, MMDL_RAFAEL_TRSP_CL_MDL_ID, &p_model)))
            {
                for (j = 0 ; j < RAF_BLE_MESH_MODEL_BIND_LIST_SIZE ; j++)
                {
                    if (p_model->binding_list[j] != 0xFFFF)
                    {
                        user_data_appkey_idx = p_model->binding_list[j];
                        break;
                    }
                }
            }
        }

        ble_mesh_proxy_enable();
        printf("Device provisioned ... primary address 0x%04x\n", pib_primary_address_get());
    }

    for (;;)
    {
        app_main_loop();

        if (xQueueReceive(app_msg_q, &app_q, 0) == pdTRUE)
        {
            if (app_event_handler[app_q.event](app_q.data, app_q.pt_tlv) == true)
            {
                vPortFree(app_q.pt_tlv);
            }
        }
    }
}

/**
 * @brief Mesh stack indication callback; forwards a mesh TLV event onto the app message queue.
 *
 * Registered with mesh_stack_init() as the mesh event indication callback. Makes a heap copy
 * of the TLV pointed to by p_param, queues it as an APP_BLE_MESH_EVT event on app_msg_q for
 * app_main_task()/app_mesh_event_handler() to process, then frees the original p_param buffer.
 * If the queue is full, it keeps retrying the send (20 ticks per attempt) until it succeeds.
 *
 * @param p_param Pointer to the mesh_tlv_t event delivered by the mesh stack; must not be NULL.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK The event was copied and queued successfully.
 * @retval BLE_ERR_INVALID_PARAMETER p_param was NULL.
 * @retval BLE_ERR_ALLOC_MEMORY_FAIL Failed to allocate memory for the queued TLV copy.
 * @retval BLE_BUSY Returned if the queue send had to be retried at least once before
 *         succeeding (the event is still queued once the retry succeeds).
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

/**
 * @brief BLE host general-event callback; forwards HCI events into the mesh bearer layer.
 *
 * Registered with ble_host_callback_set(APP_GENERAL_EVENT, ...). Wraps the BLE event
 * parameters in a mesh_tlv_t tagged MSG_TAG_BEARER_HCI_EVENT and forwards it to the mesh
 * stack's bearer layer via mesh_queue_sendto().
 *
 * @param p_param Pointer to the ble_evt_param_t event data provided by the BLE host stack.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK The event was forwarded to the mesh bearer layer successfully.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the TLV buffer for the event.
 * @retval BLE_BUSY mesh_queue_sendto() failed to queue the event.
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
 * @brief BLE host service-event (ATT/GATT) callback; forwards ACL data into the mesh bearer layer.
 *
 * Registered with ble_host_callback_set(APP_SERVICE_EVENT, ...). Wraps the BLE ATT event
 * parameters in a mesh_tlv_t tagged MSG_TAG_BEARER_HCI_ACL_DATA and forwards it to the mesh
 * stack's bearer layer via mesh_queue_sendto().
 *
 * @param p_param Pointer to the ble_evt_att_param_t event data provided by the BLE host stack.
 * @return ble_err_t Result of the operation.
 * @retval BLE_ERR_OK The event was forwarded to the mesh bearer layer successfully.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Failed to allocate the TLV buffer for the event.
 * @retval BLE_BUSY mesh_queue_sendto() failed to queue the event.
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
 * @brief Initializes the BLE mesh stack, model layer, and application queue/timer.
 *
 * Initializes the provisioning info base (PIB) and mesh models (mmdl_init()), creates the
 * application event queue (app_msg_q) and the unprovisioned-indication timer
 * (unprov_idc_tmr), registers the general and service BLE host event callbacks, and starts
 * the BLE host stack and the mesh stack (registering mesh_app_evt_indication_cb() as the mesh
 * event callback). Prints a status message for each initialization step.
 *
 * @return void This function does not return a value.
 */
void app_init(void)
{
    mesh_task_priority_t task_priority_cfg;
    ble_task_priority_t ble_task_level;

    pib_init(NULL, g_element_info, ble_mesh_element_count, NULL);

    mmdl_init();

    app_msg_q = xQueueCreate(32, sizeof(app_queue_t));

    unprov_idc_tmr = xTimerCreate("100ms", pdMS_TO_TICKS(100), pdTRUE, (void *)0, unprov_tmr_cb);

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
        printf("BLE Mesh stack initial success...\n");
    }
    else
    {
        printf("BLE Mesh stack initial fail...\n");
    }
}

/**
 * @brief GPIO interrupt callback for the button pins.
 *
 * Reads and logs the current level of the triggering pin, and for pins 0-4 queues an
 * APP_BUTTON_EVT event (carrying the pin number) onto app_msg_q from ISR context for
 * app_button_handler() to process.
 *
 * @param pin GPIO pin number that triggered the interrupt.
 * @param isr_param Unused ISR callback parameter.
 * @return void This function does not return a value.
 * @note Called from GPIO ISR context.
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
 * @brief Configures GPIO pins 0-4 as debounced, pulled-up button inputs.
 *
 * Sets the GPIO debounce time, configures pins 0-4 for rising-edge interrupts with
 * button_cb() as the callback and a 100K pull-up, and enables the GPIO interrupt in the NVIC.
 *
 * @return void This function does not return a value.
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
 * @brief UART0 receive interrupt callback.
 *
 * Drains all bytes currently available in the UART0 receive FIFO, queuing an APP_UART_EVT
 * event (carrying the character) onto app_msg_q from ISR context for each byte received.
 *
 * @param p_arg Unused callback parameter.
 * @return int Always returns 0.
 * @note Called from UART0 ISR context.
 */
static int uart0_rx_callback(void *p_arg)
{
    BaseType_t context_switch;
    app_queue_t t_app_q;
    char ch;
    
    while(hosal_uart_receive(&uart0_dev, &ch, 1) > 0)
    {
        t_app_q.event = APP_UART_EVT;
        t_app_q.data = ch;
        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
    }
    return 0;
}

/**
 * @brief UART0 receive-line-status interrupt callback.
 *
 * Invoked when the UART0 line status changes. If a break condition (UART_LSR_BI) is detected,
 * reads and discards one byte from the UART to clear it and logs the break.
 *
 * @param p_arg Unused callback parameter.
 * @return int Not used by the caller.
 * @note Called from UART0 ISR context.
 */
static int uart0_receive_line_callback(void* p_arg)
{
    if (hosal_uart_get_lsr(&uart0_dev) & UART_LSR_BI)
    {
        char ch;

        hosal_uart_receive(&uart0_dev, &ch, 1);
        //hosal_uart_receive(&uart0_dev, &ch, 1);
        printf("break\n");
    }
}

/**
 * @brief Initializes UART0 for stdio-style command/data input.
 *
 * Initializes the UART0 device, registers the TX DMA, RX, and receive-line-status callbacks,
 * configures UART0 for interrupt-driven RX mode with receive-line-status detection enabled,
 * and sets the UART0 interrupt priority.
 *
 * @return void This function does not return a value.
 */
static void uart_init(void)
{
    /*Init UART In the first place*/
    hosal_uart_init(&uart0_dev);


    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_TX_DMA_CALLBACK, NULL, &uart0_dev);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_RX_CALLBACK, uart0_rx_callback, &uart0_dev);
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK, uart0_receive_line_callback, &uart0_dev);
    
    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);
    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, (void *)NULL);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

/**
 * @brief Main application entry task.
 *
 * Initializes the low-power manager and the RF module for BLE, configures the PWM outputs
 * driving the two lightness elements' brightness (ELEMENT0_PWN_ID on pin 20, ELEMENT1_PWN_ID
 * on pin 21), initializes the buttons and UART, then runs app_init() and app_main_task()
 * (which never returns).
 *
 * @param pvParameters Pointer to parameters passed to the task (not used).
 * @return void This function does not return a value.
 * @note The trailing infinite loop is unreachable in practice because app_main_task() never
 *       returns.
 */
static void app_main_entry(void* pvParameters)
{
    hosal_pwm_dev_t pwm_dev;
    hosal_lpm_init();
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);

    pwm_dev.config.id = ELEMENT0_PWN_ID;
    pwm_dev.config.frequency = 16000;//16K
    pwm_dev.config.pin_out = 20;	
    pwm_dev.config.count_end_val = 3000;
    hosal_pwm_init_fmt0(&pwm_dev); //for element 0

    pwm_dev.config.id = ELEMENT1_PWN_ID;
    pwm_dev.config.pin_out = 21;
    hosal_pwm_init_fmt0(&pwm_dev); //for element 1

    button_init();
    uart_stdio_deinit();
    uart_init();
    app_init();
    app_main_task();
    while (1)
    {
    }
}

/**
 * @brief Application entry point.
 *
 * Configures the default pin multiplexing, initializes UART stdio and the heap regions, dumps
 * boot info, creates the main application FreeRTOS task (app_main_entry), and starts the
 * FreeRTOS scheduler.
 *
 * @return int Not used; the FreeRTOS scheduler does not return.
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

