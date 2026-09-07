/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file main.c
 *
 * @brief BLE TRSP 1C1P demo.
 * @details 1C1P: One for central role with TRSP service in client role and \n
 *                another one for Peripherl role with TRSP service in server role.
 *
 * @note
 *  [TRSP_Central] demo "TARGET_DEVICE_NAME" shall be modified to the device name of "DEVICE_NAME" in demo TRSP_1C1P.
 *
 *                    Host ID                    Host ID
 *  |-----------------|     |--------------------|     |-----------------|
 *  | [TRSP_Periph]   |     |    [TRSP_1C1P]     |     |  [TRSP_Central] |
 *  |                 |  0  |                    |     |                 |
 *  | TRSPS (server) <|-----|> TRSPS-C (client)  |  1  |                 |
 *  |                 |     |  TRSPS-P (server) <|-----|> TRSPS (client) |
 *  |                 |     |                    |     |                 |
 *  |-----------------|     |--------------------|     |-----------------|
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
#include "semphr.h"
#include "timers.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_event.h"
#include "ble_profile.h"
#include "hosal_rf.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ctrl_cmd.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "uart_stdio.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
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

// LINK please refer to "ble_profile_def.c" --> att_db_mapping
#define APP_TRSP_C_HOST_ID              0         // TRSPC: Central
#define APP_TRSP_P_HOST_ID              1         // TRSPS: Peripheral

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MIN
#define UART_BUFF_DEPTH                 5

// Device name
#define DEVICE_NAME                     "TRSP1C1P"


// Target peer device name
#define TARGET_DEVICE_NAME              (uint8_t *)"TRSP_DEMO"


// BLE application uses Write or Write Without Response
#define BLE_TRSP_WRITE_TYPE             BLEGATT_WRITE_WITHOUT_RSP  // BLEGATT_WRITE; BLEGATT_WRITE_WITHOUT_RSP


// Scan parameters
#define SCAN_TYPE                       SCAN_TYPE_ACTIVE
#define SCAN_FILTER                     SCAN_FILTER_POLICY_BASIC_UNFILTERED
#define SCAN_WINDOW                     10U       // 10*0.625ms=6.25ms
#define SCAN_INTERVAL                   10U       // 10*0.625ms=6.25ms

// Connection parameters
#define CONN_INTERVAL_MIN               38U       // 38*1.25ms=47.5ms
#define CONN_INTERVAL_MAX               42U       // 42*1.25ms=52.5ms
#define CONN_PERIPHERAL_LATENCY         0U
#define CONN_SUPERVISION_TIMEOUT        100U      // 100*10ms=1s


// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms


// GAP device name
static const uint8_t     DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x71, 0x72, 0x73, 0x74, 0x75, 0xC6 }
                                           };

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
#define GPIO_WAKE_UP_PIN                0

/**************************************************************************************************
 *    PUBLIC VARIABLES
 *************************************************************************************************/
uint8_t g_trsp_mtu[2];
uint8_t tx_data_buff[2][1024];
uint16_t tx_index[2];

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static uint8_t g_rx_buffer[UART_BUFF_DEPTH][DEFAULT_MTU + 10];
static uint16_t g_rx_buffer_length[UART_BUFF_DEPTH];
static uint8_t g_uart_index;
static uint8_t g_process_index;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;
static uint8_t g_scanning_host_id = BLE_HOSTID_RESERVED;
static uint8_t g_disconn_host_id = BLE_HOSTID_RESERVED;

static ble_gap_addr_t g_target_addr;
static uint16_t txed_index[2];

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static ble_err_t conn_create(uint8_t host_id, ble_gap_addr_t *p_peer);
static void scan_adv_report_print(ble_evt_scan_adv_report_t *p_report_data);
static ble_err_t adv_start(uint8_t host_id);
static ble_err_t ble_init(void);
static void ble_app_main(app_req_param_t *p_param);
void app_uart_cmd_help_show(void);
static ble_err_t scan_start(void);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
/**
 * @brief GPIO interrupt handler for the wake-up pin.
 *
 * This function is registered as the user callback for the wake-up GPIO pin
 * (GPIO_WAKE_UP_PIN). When the pin triggers a falling-edge interrupt it unmasks
 * the BLE application task's low power mask bit so the system stays awake.
 *
 * @param pin GPIO pin number that triggered the interrupt.
 * @param isr_param User parameter registered with the callback (unused, NULL).
 * @return void This function does not return a value.
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

/**
 * @brief Processes one received UART character into the current line buffer.
 *
 * This function appends the character to g_rx_buffer[g_uart_index]. When the buffer
 * is about to overflow or a line terminator ('\n' or '\r') is received, it NUL-terminates
 * the line, records its length in g_rx_buffer_length[], posts an APP_REQUEST_PROCESS_UART_CMD
 * application request (from ISR context), and advances/wraps g_uart_index to the next buffer.
 *
 * @param ch The character just received from the UART.
 * @return bool
 * @retval true A complete line was received (safe to allow low power sleep mode).
 * @retval false The line is not yet complete.
 */
static bool uart_data_handler(char ch)
{
    bool status = false;
    static uint16_t index = 0;

    g_rx_buffer[g_uart_index][index++] = ch;

    if (index == (sizeof(g_rx_buffer[0]) - 1))
    {
        g_rx_buffer[g_uart_index][index++] = '\0';
        g_rx_buffer_length[g_uart_index] = index;
        if (app_request_set(0xFF, APP_REQUEST_PROCESS_UART_CMD, true) == false)
        {
            // No application queue buffer. Error.
        }

        // reset index
        index = 0;

        // update uart buf index
        g_uart_index++;
        if (g_uart_index == UART_BUFF_DEPTH)
        {
            g_uart_index = 0;
        }
    }
    else if ((ch == '\n') || (ch == '\r'))
    {
        g_rx_buffer[g_uart_index][index] = '\0';
        g_rx_buffer_length[g_uart_index] = index;
        if (app_request_set(0xFF, APP_REQUEST_PROCESS_UART_CMD, true) == false)
        {
            // No application queue buffer. Error.
        }

        // reset index
        index = 0;

        // update uart buf index
        g_uart_index++;
        if (g_uart_index == UART_BUFF_DEPTH)
        {
            g_uart_index = 0;
        }

        // set status to true indicates the system can enable sleep mode
        status = true;
    }

    return status;
}

/**
 * @brief UART receive-line-status callback, used to detect a break condition.
 *
 * This function is registered as the HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK. When
 * the line status register reports a break indicator (UART_LSR_BI), it drains one
 * byte from the UART and masks the BLE application task's low power mask bit to
 * keep the system awake.
 *
 * @param p_arg User parameter registered with the callback (unused).
 * @return int Unused return value (required by the HOSAL UART callback signature).
 */
static int uart0_receive_line_callback(void *p_arg)
{
    if (hosal_uart_get_lsr(&uart0_dev) & UART_LSR_BI)
    {
        char ch;

        hosal_uart_receive(&uart0_dev, &ch, 1);
        hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }
}

/**
 * @brief UART Rx interrupt callback.
 *
 * This function is registered as the HOSAL_UART_RX_CALLBACK. It reads one received
 * byte from the UART and forwards it to uart_data_handler(). If a complete line has
 * just been received, it unmasks the BLE application task's low power mask bit to
 * allow the system to enter sleep mode again.
 *
 * @param p_arg User parameter registered with the callback (unused).
 * @return int Always returns 0.
 */
static int uart0_rx_callback(void *p_arg)
{
    char ch;

    hosal_uart_receive(&uart0_dev, &ch, 1);
    if (uart_data_handler(ch) == true)
    {
        // received '\n' or '\r' --> enable sleep mode
        hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }

    return 0;
}

/**
 * @brief Device Information Service (DIS) event handler.
 *
 * This function handles DIS ATT events. It only processes events on the client role
 * (i.e. this device acting as a GATT client toward a DIS server): it prints the
 * manufacturer name string and the firmware revision string read responses received
 * from the connected server.
 *
 * @param p_param Pointer to the ATT event parameters, including gatt_role, event, data and length.
 * @return void This function does not return a value.
 */
static void ble_svcs_dis_evt_handler(ble_evt_att_param_t *p_param)
{
    uint8_t *p_data;

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_DIS_MANUFACTURER_NAME_STRING_READ_RSP_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("Manufacturer name: %s\n", p_data);
                vPortFree(p_data);
            }
            break;

        case BLESERVICE_DIS_FIRMWARE_REVISION_STRING_READ_RSP_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("FW rev.: %s\n", p_data);
                vPortFree(p_data);
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief TRSP Service (TRSPS) event handler.
 *
 * This function handles TRSPS ATT events for both GATT roles. On the client role it
 * prints data received via notify/indicate from the connected server. On the server
 * role it prints data received via write/write-without-response from the connected
 * client, and replies to a characteristic read request with a fixed "UDATRW01 data" value.
 *
 * @param p_param Pointer to the ATT event parameters, including gatt_role, event, data and length.
 * @return void This function does not return a value.
 */
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    uint8_t *p_data;

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATNI01_NOTIFY_EVENT:
        case BLESERVICE_TRSPS_UDATNI01_INDICATE_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("From Host Id: %d, Data: %s\n", p_param->host_id, p_data);
                vPortFree(p_data);
            }
            break;

        default:
            break;
        }
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
        case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("From Host Id: %d, Data: %s\n", p_param->host_id, p_data);
                vPortFree(p_data);
            }
            break;

        case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
        {
            ble_gatt_data_param_t p_data_param;
            uint8_t readData[] = "UDATRW01 data";

            p_data_param.host_id = p_param->host_id;
            p_data_param.handle_num = p_param->handle_num;
            p_data_param.p_data = readData;
            p_data_param.length = sizeof(readData) / sizeof(readData[0]) - 1;
            ble_cmd_gatt_read_rsp(&p_data_param);
        }
        break;

        default:
            break;
        }
    }
}

/**
 * @brief Runs the TRSP client post-connection command sequence, one step per call.
 *
 * This function drives a small state machine (tracked by the static cmd_index) that,
 * after the ATT database has been parsed, issues in order: MTU exchange, data length
 * update, TRSPS CCCD configuration (enable notification from the server), GATT service
 * changed CCCD configuration (enable indication from the server), a read of the DIS
 * manufacturer name string, and a read of the DIS firmware revision string. If the link
 * is not connected, cmd_index is reset to 0. The caller is expected to invoke this
 * function again (via APP_REQUEST_TRSPC_MULTI_CMD) until it reports completion.
 *
 * @param host_id The central link's host id.
 * @return ble_err_t
 * @retval BLE_ERR_OK All commands in the sequence have completed (cmd_index reset to 0).
 * @retval BLE_BUSY The sequence is still in progress; call again to continue.
 */
static ble_err_t svcs_trspc_multi_cmd_handler(uint8_t host_id)
{
    // after received "BLECMD_EVENT_ATT_DATABASE_PARSING_FINISHED" event, do
    // 0. MTU exchange
    // 1. data length update
    // 2. set TRSPS cccd value
    // 3. set GATT cccd value
    // 4. read DIS manufacturer name from server
    // 5. read DIS firmware revision from server

    ble_err_t status;
    ble_info_link0_t *p_profile_info;
    static uint8_t cmd_index = 0;

    if (ble_app_link_info[host_id].state == STATE_CONNECTED)
    {
        p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

        switch (cmd_index)
        {
        case 0:
            // send MTU exchange to server
            status = ble_cmd_mtu_size_update(host_id, DEFAULT_MTU);
            break;

        case 1:
            // send data length updated to server
            status = ble_cmd_data_len_update(host_id, (DEFAULT_MTU + 4)); // 4 bytes header
            break;

        case 2:
            // set TRSPS cccd
            p_profile_info->svcs_info_trsps.client_info.data.udatni01_cccd = BLEGATT_CCCD_NOTIFICATION;

            // send config TRSPS cccd to server to enable to receive notifications from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatni01_cccd,
                                       p_profile_info->svcs_info_trsps.client_info.data.udatni01_cccd);
            break;

        case 3:
            // set GATT cccd
            p_profile_info->svcs_info_gatts.client_info.data.service_changed_cccd = BLEGATT_CCCD_INDICATION;

            // send config GATT cccd to server to enable to receive indication from server
            status = ble_svcs_cccd_set(host_id,
                                       p_profile_info->svcs_info_gatts.client_info.handles.hdl_service_changed_cccd,
                                       p_profile_info->svcs_info_gatts.client_info.data.service_changed_cccd);
            break;

        case 4:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_dis.client_info.handles.hdl_manufacturer_name_string;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        case 5:
        {
            ble_gatt_read_req_param_t read_param;

            read_param.host_id = host_id;
            read_param.handle_num = p_profile_info->svcs_info_dis.client_info.handles.hdl_firmware_revision_string;
            status = ble_cmd_gatt_read_req(&read_param);
        }
        break;

        default:
            // process commands are done.
            cmd_index = 0;
            return BLE_ERR_OK;
        }

        // if command status is BLE_STATUS_SUCCESS then go to do next command (index++)
        if (status == BLE_ERR_OK)
        {
            cmd_index++;
        }
    }
    else
    {
        cmd_index = 0;
    }

    return BLE_BUSY;
}

/**
 * @brief Handles application requests for the central-role link (TRSPS Central).
 *
 * This function processes APP_REQUEST_SCAN_START/STOP, APP_REQUEST_CREATE_CONN,
 * APP_REQUEST_TRSPC_MULTI_CMD, APP_REQUEST_DATA_SEND and APP_REQUEST_DISCONNECT for
 * the link acting as the GATT/GAP central role: it starts/stops scanning, creates the
 * connection, drives the TRSP client command sequence, sends pending TX data to the
 * connected server, and terminates the connection.
 *
 * @param p_param Pointer to the application request parameters (host id and request event).
 * @return void This function does not return a value.
 */
// TRSPS Central
static void app_central_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_SCAN_START:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_CENTRAL) && (ble_app_link_info[host_id].state == STATE_STANDBY))
        {
            g_scanning_host_id = host_id;
        }
        else
        {
            g_scanning_host_id = BLE_HOSTID_RESERVED; // scan only, no auto connect.
        }
        // set  data length
        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK)
        {
            printf("MTU/ Data Length set status = 0x%02x\n", status);
        }

        // service data init
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.client_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.client_info.data);

        //set scan enable
        status = scan_start();
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_scan_enable status = 0x%02x\n", status);
        }
        tx_index[APP_TRSP_C_HOST_ID] = 0;
        txed_index[APP_TRSP_C_HOST_ID] = 0;
        g_trsp_mtu[APP_TRSP_C_HOST_ID] = BLE_GATT_ATT_MTU_MIN;
        break;

    case APP_REQUEST_SCAN_STOP:
        // disable scan
        status = ble_cmd_scan_disable();
        if (status != BLE_ERR_OK)
        {
            printf("ble_scan_disable status = 0x%02x\n", status);
        }
        break;

    case APP_REQUEST_CREATE_CONN:

        if (ble_app_link_info[host_id].state == STATE_INITIATING)
        {
            status = conn_create(g_scanning_host_id, &g_target_addr);
            if (status != BLE_ERR_OK)
            {
                printf("conn_create status = 0x%02x\n", status);
            }
        }
        break;

    case APP_REQUEST_TRSPC_MULTI_CMD:
        if (svcs_trspc_multi_cmd_handler(host_id) == BLE_ERR_OK)
        {
            // Multiple commands set finished
            printf("Ready to TX/RX data to/from the connected server. \n");
        }
        else
        {
            if (ble_app_link_info[host_id].state == STATE_CONNECTED)
            {
                if (app_request_set(host_id, APP_REQUEST_TRSPC_MULTI_CMD, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
            else
            {
                printf("connection terminated. reconnect server to restart TX/RX data.\n");
            }
        }
        break;

    case APP_REQUEST_DATA_SEND:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_CENTRAL) && (ble_app_link_info[host_id].state == STATE_CONNECTED))
        {
            if ((BLE_TRSP_WRITE_TYPE != BLEGATT_WRITE) && (BLE_TRSP_WRITE_TYPE != BLEGATT_WRITE_WITHOUT_RSP))
            {
                printf("Error write type. \n");
            }
            else
            {
                uint16_t len;

                len = tx_index[host_id] - txed_index[host_id];
                if (len > (g_trsp_mtu[host_id] - 3))
                {
                    len = (g_trsp_mtu[host_id] - 3);
                }
                status = ble_svcs_trsps_client_send( host_id,
                                                     BLE_TRSP_WRITE_TYPE,
                                                     p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01,
                                                     (uint8_t *)&tx_data_buff[host_id][txed_index[host_id]],
                                                     len);

                if (status != BLE_ERR_OK)
                {
                    if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
                    {
                        // No application queue buffer. Error.
                    }
                }
                else
                {
                    txed_index[host_id] += len;
                    if (txed_index[host_id] == tx_index[host_id])
                    {
                        tx_index[host_id] = 0;
                        txed_index[host_id] = 0;
                    }
                    else
                    {
                        if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
                        {
                            // No application queue buffer. Error.
                        }
                    }
                }
            }
        }
        break;

    case APP_REQUEST_DISCONNECT:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_CENTRAL) &&
                (ble_app_link_info[host_id].state == STATE_CONNECTED))
        {
            g_disconn_host_id = host_id;
            status = ble_cmd_conn_terminate(g_disconn_host_id);
            if (status != BLE_ERR_OK)
            {
                printf("Disconnect failed, status = 0x%02x\n", status);
            }
        }
        else
        {
            printf("Central Role isn't connection state.\n");
        }
        break;

    default:
        break;
    }
}

/**
 * @brief Handles application requests for the peripheral-role link (TRSPS Peripheral).
 *
 * This function processes APP_REQUEST_ADV_START/STOP, APP_REQUEST_DATA_SEND and
 * APP_REQUEST_DISCONNECT for the link acting as the GATT/GAP peripheral role: it
 * starts/stops advertising, sends pending TX data to the connected client via
 * notification or indication (depending on the client's CCCD setting), and
 * terminates the connection.
 *
 * @param p_param Pointer to the application request parameters (host id and request event).
 * @return void This function does not return a value.
 */
// TRSPS Peripheral
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link1_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link1_t *)ble_app_link_info[host_id].profile_info;

    status = BLE_ERR_OK;
    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_PERIPHERAL) && (ble_app_link_info[host_id].state == STATE_STANDBY))
        {
            do
            {
                // service data init
                svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
                svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.server_info.data);

                // set preferred MTU size and data length
                status = ble_cmd_default_mtu_size_set(host_id, DEFAULT_MTU);
                if (status != BLE_ERR_OK)
                {
                    printf("ble_cmd_default_mtu_size_set() status = %d\n", status);
                    break;
                }

                // enable advertising
                g_advertising_host_id = host_id;
                status = adv_start(g_advertising_host_id);
                if (status != BLE_ERR_OK)
                {
                    printf("Adv start status = 0x%02x\n", status);
                }
                tx_index[APP_TRSP_P_HOST_ID] = 0;
                txed_index[APP_TRSP_P_HOST_ID] = 0;
                g_trsp_mtu[APP_TRSP_P_HOST_ID] = BLE_GATT_ATT_MTU_MIN;
            } while (0);
        }
        else
        {
            printf("Already in advertising or there is no any free link for peripheral. \n");
        }
        break;

    case APP_REQUEST_ADV_STOP:
        status = ble_cmd_adv_disable();
        if (status != BLE_ERR_OK)
        {
            printf("Adv. disable status = 0x%02x\n", status);
        }
        break;

    case APP_REQUEST_DATA_SEND:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_PERIPHERAL) && (ble_app_link_info[host_id].state == STATE_CONNECTED))
        {
            uint8_t len;

            len = 0;
            // send data to client
            if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                len = tx_index[host_id] - txed_index[host_id];
                if (len > (g_trsp_mtu[host_id] - 3))
                {
                    len = (g_trsp_mtu[host_id] - 3);
                }
                status = ble_svcs_trsps_server_send( host_id,
                                                     BLEGATT_CCCD_NOTIFICATION,
                                                     p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                     (uint8_t *)&tx_data_buff[host_id][txed_index[host_id]],
                                                     len);


            }
            else if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_INDICATION) != 0)
            {
                len = tx_index[host_id] - txed_index[host_id];
                if (len > (g_trsp_mtu[host_id] - 3))
                {
                    len = (g_trsp_mtu[host_id] - 3);
                }

                status = ble_svcs_trsps_server_send(  host_id,
                                                      BLEGATT_CCCD_INDICATION,
                                                      p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                      (uint8_t *)&tx_data_buff[host_id][txed_index[host_id]],
                                                      len);
            }
            if (status != BLE_ERR_OK)
            {
                if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
            else
            {
                txed_index[host_id] += len;
                if (txed_index[host_id] == tx_index[host_id])
                {
                    tx_index[host_id] = 0;
                    txed_index[host_id] = 0;
                }
                else
                {
                    if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
                    {
                        // No application queue buffer. Error.
                    }
                }
            }
        }
        break;


    case APP_REQUEST_DISCONNECT:
        if ((ble_app_link_info[host_id].gap_role == BLE_GAP_ROLE_PERIPHERAL) &&
                (ble_app_link_info[host_id].state == STATE_CONNECTED))
        {
            g_disconn_host_id = host_id;
            status = ble_cmd_conn_terminate(g_disconn_host_id);
            if (status != BLE_ERR_OK)
            {
                printf("Disconnect failed, status = 0x%02x\n", status);
            }
        }
        else
        {
            printf("Peripheral Role isn't connection state.\n");
        }
        break;

    default:
        break;
    }
}

/**
 * @brief General BLE stack event handler.
 *
 * This function dispatches BLE host events (advertising/scan enable, advertising
 * report, connection complete, connection parameter update, PHY read/update, MTU
 * exchange, data length change, disconnection complete, and ATT database parsing
 * complete), updates the corresponding link's state (ble_app_link_info[].state) and
 * the module-level scanning/advertising host id trackers, and prints status information.
 * On ATT database parsing complete it also retrieves the service handles and kicks off
 * the TRSP client command sequence via APP_REQUEST_TRSPC_MULTI_CMD.
 *
 * @param p_param Pointer to the BLE event parameters, including the event type and its payload.
 * @return void This function does not return a value.
 */
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_ADV_EVT_SET_ENABLE:
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = (ble_evt_adv_set_adv_enable_t *)&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable;

        if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_adv_enable->adv_enabled == true)
            {
                ble_app_link_info[g_advertising_host_id].state = STATE_ADVERTISING;
                printf("Advertising...\n");
            }
            else
            {
                ble_app_link_info[g_advertising_host_id].state = STATE_STANDBY;
                g_advertising_host_id = BLE_HOSTID_RESERVED;
                printf("Idle.\n");
            }
        }
        else
        {
            g_advertising_host_id = BLE_HOSTID_RESERVED;
            printf("Advertising enable failed.\n");
        }
    }
    break;

    case BLE_SCAN_EVT_SET_ENABLE:
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)&p_param->event_param.ble_evt_scan.param.evt_set_scan_enable;
        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_scan_enable->scan_enabled == true)
            {
                ble_app_link_info[g_scanning_host_id].state = STATE_SCANNING;
                printf("Scanning...\n");
            }
            else
            {
                if ((ble_app_link_info[g_scanning_host_id].state == STATE_INITIATING) && (g_scanning_host_id != BLE_HOSTID_RESERVED))
                {
                    if (app_request_set(g_scanning_host_id, APP_REQUEST_CREATE_CONN, false) == false)
                    {
                        // No application queue buffer. Error.
                    }
                }
                else
                {
                    ble_app_link_info[g_scanning_host_id].state = STATE_STANDBY;
                    g_scanning_host_id = BLE_HOSTID_RESERVED;
                }
                printf("Idle.\n");
            }
        }
        else
        {
            g_scanning_host_id = BLE_HOSTID_RESERVED;
            printf("Scan enable failed.\n");
        }
    }
    break;

    case BLE_SCAN_EVT_ADV_REPORT:
    {
        ble_evt_scan_adv_report_t *p_scan_param = (ble_evt_scan_adv_report_t *)&p_param->event_param.ble_evt_scan.param.evt_adv_report;

        // show device name, address and RSSI value
        scan_adv_report_print(p_scan_param);
    }
    break;

    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_complete;

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connect failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            ble_app_link_info[p_conn_param->host_id].state = STATE_CONNECTED;

            printf("Connected, ID=%d, Connected to %02x:%02x:%02x:%02x:%02x:%02x\n",
                       p_conn_param->host_id,
                       p_conn_param->peer_addr.addr[5],
                       p_conn_param->peer_addr.addr[4],
                       p_conn_param->peer_addr.addr[3],
                       p_conn_param->peer_addr.addr[2],
                       p_conn_param->peer_addr.addr[1],
                       p_conn_param->peer_addr.addr[0]);
        }
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        ble_evt_gap_conn_param_update_t *p_conn_param = (ble_evt_gap_conn_param_update_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_param_update;

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connection update failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            printf("Connection updated\n");
            printf("ID: %d, ", p_conn_param->host_id);
            printf("Interval: %d, ", p_conn_param->conn_interval);
            printf("Latency: %d, ", p_conn_param->periph_latency);
            printf("Supervision Timeout: %d\n", p_conn_param->supv_timeout);
        }
    }
    break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy;
        if (p_phy_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("PHY update/read failed, error code = 0x%02x\n", p_phy_param->status);
        }
        else
        {
            printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu;

        // update MTU size
        g_trsp_mtu[p_mtu_param->host_id] = p_mtu_param->mtu;

        printf("MTU Exchanged, ID:%d, size: %d\n", p_mtu_param->host_id, p_mtu_param->mtu);
    }
    break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
    {
        ble_evt_data_length_change_t *p_data_len_param = (ble_evt_data_length_change_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change;
        printf("Data length changed, ID: %d\n", p_data_len_param->host_id);
        printf("MaxTxOctets: %d  MaxTxTime:%d\n", p_data_len_param->max_tx_octets, p_data_len_param->max_tx_time);
        printf("MaxRxOctets: %d  MaxRxTime:%d\n", p_data_len_param->max_rx_octets, p_data_len_param->max_rx_time);
    }
    break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
    {
        ble_evt_gap_disconn_complete_t *p_disconn_param = (ble_evt_gap_disconn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_disconn_complete;
        if (p_disconn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Disconnect failed, error code = 0x%02x\n", p_disconn_param->status);
        }
        else
        {
            ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;
            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_evt_att_db_parse_complete_t *p_parsing_param = (ble_evt_att_db_parse_complete_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_att_db_parse_complete;

        // Get all service handles and related information
        link0_svcs_handles_get(p_parsing_param->host_id);

        // Do GATT commands
        if (app_request_set(p_parsing_param->host_id, APP_REQUEST_TRSPC_MULTI_CMD, false) == false)
        {
            // No application queue buffer. Error.
        }

        printf("DB Parsing completed, ID:%d status:0x%02x\n", p_parsing_param->host_id, p_parsing_param->result);
    }
    break;

    default:
        break;
    }
}

/* ------------------------------
 *  Methods
 * ------------------------------
 */
/**
 * @brief Configures the scan parameters and enables scanning.
 *
 * This function reads the device's own address, builds the scan parameters (type,
 * own address type, interval, window and filter policy), applies them, and enables
 * scanning.
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
 */
static ble_err_t scan_start(void)
{
    ble_err_t      status;
    ble_scan_param_t  scan_param;
    ble_gap_addr_t addr_param;
    status = BLE_ERR_OK;

    ble_cmd_device_addr_get(&addr_param);
    do
    {
        scan_param.scan_type = SCAN_TYPE;
        scan_param.own_addr_type = addr_param.addr_type;
        scan_param.scan_interval = SCAN_INTERVAL;
        scan_param.scan_window = SCAN_WINDOW;
        scan_param.scan_filter_policy = SCAN_FILTER;

        // set scan parameter
        status = ble_cmd_scan_param_set(&scan_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // set scan enable
        status = ble_cmd_scan_enable();
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Parses and prints an advertising report, and initiates connection to the target peer.
 *
 * This function extracts the complete or shortened local name from the advertising
 * report and prints the found device's name, address and RSSI. If the name matches
 * TARGET_DEVICE_NAME, it saves the peer address into g_target_addr, sets the scanning
 * link's state to STATE_INITIATING, and posts an APP_REQUEST_SCAN_STOP request, which
 * leads to stopping the scan and creating the connection.
 *
 * @param p_report_data Pointer to the advertising report event data.
 * @return void This function does not return a value.
 */
static void scan_adv_report_print(ble_evt_scan_adv_report_t *p_report_data)
{
    uint8_t name[32];
    uint8_t length;

    if ((ble_cmd_scan_report_adv_data_parsing(p_report_data, GAP_AD_TYPE_LOCAL_NAME_COMPLETE, name, &length) == BLE_ERR_OK) ||
            (ble_cmd_scan_report_adv_data_parsing(p_report_data, GAP_AD_TYPE_LOCAL_NAME_SHORTENED, name, &length) == BLE_ERR_OK))
    {
        name[length] = '\0';
        printf("Found [Name:%s] [Address: %02x:%02x:%02x:%02x:%02x:%02x] [RSSI= %d]\n",
                   name,
                   p_report_data->peer_addr.addr[5], p_report_data->peer_addr.addr[4],
                   p_report_data->peer_addr.addr[3], p_report_data->peer_addr.addr[2],
                   p_report_data->peer_addr.addr[1], p_report_data->peer_addr.addr[0],
                   p_report_data->rssi);

        if (((length == strlen((char *)TARGET_DEVICE_NAME)) && (strncmp((char *)name, (char *)TARGET_DEVICE_NAME, length) == 0)))
        {
            // set peer address type
            g_target_addr.addr_type = p_report_data->peer_addr.addr_type;
            memcpy(g_target_addr.addr, &p_report_data->peer_addr.addr, BLE_ADDR_LEN);

            // update state
            ble_app_link_info[g_scanning_host_id].state = STATE_INITIATING;

            // stop scan and go to connect with this peer address.
            if (app_request_set(g_scanning_host_id, APP_REQUEST_SCAN_STOP, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
    }
}

/**
 * @brief Creates a connection to the given peer address.
 *
 * This function builds the connection creation parameters (own address type, scan
 * interval/window, accept-all initiator filter policy, peer address, and the preset
 * connection interval/latency/supervision timeout) and issues the connection create
 * command. It then resets g_scanning_host_id to BLE_HOSTID_RESERVED.
 *
 * @param host_id The link's host id to use for the new connection.
 * @param p_peer Pointer to the target peer's BLE address.
 * @return ble_err_t Status returned by ble_cmd_conn_create().
 */
static ble_err_t conn_create(uint8_t host_id, ble_gap_addr_t *p_peer)
{
    ble_err_t             status;
    ble_gap_create_conn_param_t  create_conn_param;
    ble_gap_addr_t addr_param;

    ble_cmd_device_addr_get(&addr_param);

    create_conn_param.host_id = host_id;
    create_conn_param.own_addr_type = addr_param.addr_type;
    create_conn_param.scan_interval = SCAN_INTERVAL;
    create_conn_param.scan_window = SCAN_WINDOW;
    create_conn_param.init_filter_policy = INIT_FILTER_POLICY_ACCEPT_ALL;
    memcpy(&create_conn_param.peer_addr, p_peer, sizeof(ble_gap_addr_t));

    create_conn_param.conn_param.max_conn_interval = CONN_INTERVAL_MAX;
    create_conn_param.conn_param.min_conn_interval = CONN_INTERVAL_MIN;
    create_conn_param.conn_param.periph_latency = CONN_PERIPHERAL_LATENCY;
    create_conn_param.conn_param.supv_timeout = CONN_SUPERVISION_TIMEOUT;

    status = ble_cmd_conn_create(&create_conn_param);

    // reset
    g_scanning_host_id = BLE_HOSTID_RESERVED;

    return status;
}

/**
 * @brief Initializes the advertising parameters and data.
 *
 * This function sets up the advertising parameters (type, own address type, interval,
 * channel map and filter policy), the advertising data (limited discoverable flags),
 * and the scan response data (the device's local name).
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
 */
static ble_err_t adv_init(void)
{
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    ble_gap_addr_t addr_param;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + strlen(DEVICE_NAME_STR); //  1 byte data type

    // adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
    };

    //Scan response data
    uint8_t adv_scan_rsp_data[2 + strlen(DEVICE_NAME_STR)];

    adv_scan_rsp_data[0] = SCANRSP_ADLENGTH;                      // AD length
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;       // AD data type
    memcpy(&adv_scan_rsp_data[2], DEVICE_NAME_STR, strlen(DEVICE_NAME_STR)); // Copy the name

    ble_cmd_device_addr_get(&addr_param);
    do
    {
        adv_param.adv_type = ADV_TYPE_ADV_IND;
        adv_param.own_addr_type = addr_param.addr_type;
        adv_param.adv_interval_min = APP_ADV_INTERVAL_MIN;
        adv_param.adv_interval_max = APP_ADV_INTERVAL_MAX;
        adv_param.adv_channel_map = ADV_CHANNEL_ALL;
        adv_param.adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;

        // set adv parameter
        status = ble_cmd_adv_param_set(&adv_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_param_set() status = %d\n", status);
            break;
        }

        // set adv data
        adv_data_param.length = sizeof(adv_data);
        memcpy(&adv_data_param.data, &adv_data, sizeof(adv_data));
        status = ble_cmd_adv_data_set(&adv_data_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_data_set() status = %d\n", status);
            break;
        }

        // set scan rsp data
        adv_scan_data_param.length = sizeof(adv_scan_rsp_data);
        memcpy(&adv_scan_data_param.data, &adv_scan_rsp_data, sizeof(adv_scan_rsp_data));
        status = ble_cmd_adv_scan_rsp_set(&adv_scan_data_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_scan_rsp_set() status = %d\n", status);
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Enables advertising for the specified host id.
 *
 * This function sends a command to the BLE stack to enable advertising and checks
 * the status of the operation.
 *
 * @param host_id The link's host id for which to enable advertising.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
 */
static ble_err_t adv_enable(uint8_t host_id)
{
    ble_err_t status;

    status = ble_cmd_adv_enable(host_id);
    if (status != BLE_ERR_OK)
    {
        printf("adv_enable() status = %d\n", status);
    }

    return status;
}

/**
 * @brief Starts advertising for the specified host id.
 *
 * This function initializes the advertising parameters/data via adv_init() and then
 * enables advertising via adv_enable().
 *
 * @param host_id The link's host id for which to start advertising.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
 */
static ble_err_t adv_start(uint8_t host_id)
{
    ble_err_t status;

    status = BLE_ERR_OK;

    do
    {
        // set adv parameter and data
        status = adv_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // set adv enable
        status = adv_enable(host_id);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Queues an application request event for processing by the main application task.
 *
 * Builds an app_queue_t entry of type QUEUE_TYPE_APP_REQ from host_id/request and posts it
 * to g_app_msg_q, taking the appropriate counting semaphore first (semaphore_app for normal
 * calls, semaphore_isr for calls made from an ISR) to bound the number of outstanding requests.
 *
 * @param host_id The connection/host id the request applies to (0xFF for UART command requests).
 * @param request The application request event to post.
 * @param from_isr true if called from interrupt context (uses the FromISR queue/semaphore
 *        APIs), false for normal task context.
 * @return bool
 * @retval true The request was successfully queued.
 * @retval false No semaphore/queue slot was available; the request was dropped.
 * @note Safe to call from ISR context when from_isr is true.
 */
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = QUEUE_TYPE_APP_REQ;
    p_app_q.param.app_req.host_id = host_id;
    p_app_q.param.app_req.app_req = request;

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
/**
 * @brief Callback registered for general BLE host events (APP_GENERAL_EVENT).
 *
 * This function copies the incoming event parameters into a heap-allocated TLV buffer
 * and posts it to g_app_msg_q as a QUEUE_TYPE_OTHERS/APP_GENERAL_EVENT entry, guarded by
 * the semaphore_cb counting semaphore so the number of outstanding buffers is bounded.
 * The buffer is freed later by app_main_task() after ble_evt_handler() processes it.
 *
 * @param p_param Pointer to the BLE event parameters (ble_evt_param_t) to enqueue.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Memory allocation for the TLV buffer failed.
 * @retval BLE_BUSY No semaphore slot was available, or the queue send failed.
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
                vPortFree(p_tlv);
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

/**
 * @brief Callback registered for BLE service ATT events (APP_SERVICE_EVENT).
 *
 * This function copies the incoming ATT event parameters (including its variable-length
 * data payload) into a heap-allocated TLV buffer and posts it to g_app_msg_q as a
 * QUEUE_TYPE_OTHERS/APP_SERVICE_EVENT entry, guarded by the semaphore_cb counting
 * semaphore. The buffer is later dispatched to the relevant service's att_handler by
 * app_main_task() and freed.
 *
 * @param p_param Pointer to the ATT event parameters (ble_evt_att_param_t) to enqueue.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Memory allocation for the TLV buffer failed.
 * @retval BLE_BUSY No semaphore slot was available, or the queue send failed.
 */
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
                vPortFree(p_tlv);
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

/**
 * @brief Callback registered for BLE L2CAP data events (APP_L2CAP_DATA_EVENT).
 *
 * This function copies the incoming L2CAP event parameters (including its variable-length
 * data payload) into a heap-allocated TLV buffer and posts it to g_app_msg_q as a
 * QUEUE_TYPE_OTHERS/APP_L2CAP_DATA_EVENT entry, guarded by the semaphore_cb counting
 * semaphore.
 *
 * @param p_param Pointer to the L2CAP event parameters (ble_l2cap_evt_param_t) to enqueue.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Memory allocation for the TLV buffer failed.
 * @retval BLE_BUSY No semaphore slot was available, or the queue send failed.
 */
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
                vPortFree(p_tlv);
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

/**
 * @brief Main application task loop.
 *
 * This function initializes the BLE stack/profiles via ble_init(), prints the UART
 * control command help message, and then loops forever pulling entries from
 * g_app_msg_q: QUEUE_TYPE_APP_REQ entries are dispatched to ble_app_main(), and
 * QUEUE_TYPE_OTHERS entries (APP_GENERAL_EVENT, APP_SERVICE_EVENT, APP_L2CAP_DATA_EVENT)
 * are dispatched to the corresponding event handler and then freed. This function
 * never returns; it halts in an infinite loop if ble_init() fails.
 *
 * @return void This function does not return a value.
 */
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

    /* show UART control command help message */
    print_ctrl_cmd_help();
    g_uart_index = 0;
    g_process_index = 0;
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
                ble_app_main(&p_app_q.param.app_req);
            }
            break;

            case QUEUE_TYPE_OTHERS:
            {
                if (p_app_q.param.pt_tlv != NULL)
                {
                    switch (p_app_q.param.pt_tlv->type)
                    {
                    case APP_GENERAL_EVENT:
                        ble_evt_handler((ble_evt_param_t *)p_app_q.param.pt_tlv->value);
                        break;

                    case APP_SERVICE_EVENT:
                    {
                        ble_evt_att_param_t *p_svcs_param = (ble_evt_att_param_t *)p_app_q.param.pt_tlv->value;

                        switch (p_svcs_param->gatt_role)
                        {
                        case BLE_GATT_ROLE_CLIENT:
                            att_db_link[p_svcs_param->host_id].p_client_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        case BLE_GATT_ROLE_SERVER:
                            att_db_link[p_svcs_param->host_id].p_server_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        default:
                            break;
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

/**
 * @brief Dispatches a dequeued application request to the appropriate handler.
 *
 * If p_param->host_id is 0xFF, this is a UART command request: on
 * APP_REQUEST_PROCESS_UART_CMD it forwards the buffered line at g_rx_buffer[g_process_index]
 * to handle_ctrl_cmd() and advances/wraps g_process_index. Otherwise the request is routed
 * by host id to app_central_handler() (APP_TRSP_C_HOST_ID) or app_peripheral_handler()
 * (APP_TRSP_P_HOST_ID).
 *
 * @param p_param Pointer to the application request parameters (host id and request event).
 * @return void This function does not return a value.
 */
static void ble_app_main(app_req_param_t *p_param)
{
    if (p_param->host_id == 0xFF)
    {
        if (p_param->app_req == APP_REQUEST_PROCESS_UART_CMD)
        {
            handle_ctrl_cmd(&g_rx_buffer[g_process_index][0], g_rx_buffer_length[g_process_index]);
        }

        g_process_index++;
        if (g_process_index == UART_BUFF_DEPTH)
        {
            g_process_index = 0;
        }
    }
    else
    {
        if (p_param->host_id == APP_TRSP_C_HOST_ID)
        {
            app_central_handler(p_param);
        }
        else if (p_param->host_id == APP_TRSP_P_HOST_ID)
        {
            app_peripheral_handler(p_param);
        }
    }
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */
/**
 * @brief Initializes the UART.
 *
 * This function initializes UART0 for communication, registers the Rx and receive-line-status
 * interrupt callbacks, configures the UART for interrupt mode, and enables the UART Rx pin as
 * a low power wake-up source.
 *
 * @return void This function does not return a value.
 */
static void app_uart_init(void)
{
    /*Init UART In the first place*/
    hosal_uart_init(&uart0_dev);

    /* Configure UART Rx interrupt callback function */
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_RX_CALLBACK, uart0_rx_callback, &uart0_dev);
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_TX_DMA_CALLBACK, NULL, &uart0_dev);
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK, uart0_receive_line_callback, &uart0_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);
    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, (void *)NULL);

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_UART_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

/**
 * @brief Initializes the GATT service data.
 *
 * This function initializes the GATT service data structure by setting the service
 * changed characteristic's client configuration descriptor (CCCD) to 0.
 *
 * @param p_data Pointer to the GATT service data structure.
 * @return void This function does not return a value.
 */
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

/**
 * @brief Initializes the TRSPS service data.
 *
 * This function initializes the TRSPS service data structure by setting the user data
 * notification characteristic's client configuration descriptor (CCCD) to 0.
 *
 * @param p_data Pointer to the TRSPS service data structure.
 * @return void This function does not return a value.
 */
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data)
{
    p_data->udatni01_cccd = 0;
}

/**
 * @brief Initializes the server-role (peripheral) profile for the specified host id.
 *
 * This function sets the link's state to STATE_STANDBY, then initializes the GAP
 * service (sets the device name), GATT service, DIS service, and TRSPS service, all
 * in the server (BLE_GATT_ROLE_SERVER) role, for the given host id.
 *
 * @param host_id The link's host id for which to initialize the server profile.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
 */
static ble_err_t server_profile_init(uint8_t host_id)
{
    ble_err_t status;
    ble_info_link1_t *p_profile_info;

    status = BLE_ERR_OK;
    p_profile_info = (ble_info_link1_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;

    do
    {
        // GAP Related
        // -------------------------------------
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gaps), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // set GAP device name
        status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // GATT Related
        // -------------------------------------
        status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gatts), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // DIS Related
        // -------------------------------------
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_dis), ble_svcs_dis_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // TRSPS Related
        // -------------------------------------
        status = ble_svcs_trsps_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_trsps), ble_svcs_trsps_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}


/**
 * @brief Initializes the client-role (central) profile for the specified host id.
 *
 * This function sets the link's state to STATE_STANDBY, then initializes the GAP
 * service (sets the device name), GATT service, DIS service, and TRSPS service, all
 * in the client (BLE_GATT_ROLE_CLIENT) role, for the given host id.
 *
 * @param host_id The link's host id for which to initialize the client profile.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
 */
static ble_err_t client_profile_init(uint8_t host_id)
{
    ble_err_t status;
    ble_info_link0_t *p_profile_info;

    status = BLE_ERR_OK;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;

    do
    {
        // GAP Related
        // -------------------------------------
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_gaps), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // set GAP device name
        status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // GATT Related
        // -------------------------------------
        status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_gatts), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }


        // DIS Related
        // -------------------------------------
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_dis), ble_svcs_dis_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // TRSPS Related
        // -------------------------------------
        status = ble_svcs_trsps_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_trsps), ble_svcs_trsps_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Performs the default BLE stack and profile initialization.
 *
 * This function registers the general/service/L2CAP event callbacks, initializes the
 * PHY controller, sets the device address (from the factory unique code if available,
 * falling back to DEVICE_ADDR) and identity resolving key, initializes resolvable
 * address generation, sets the suggested data length, and initializes the server
 * (peripheral, APP_TRSP_P_HOST_ID) and client (central, APP_TRSP_C_HOST_ID) profiles.
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code from the
 *         first failing step on failure).
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

        status = ble_cmd_phy_controller_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_read_unique_code(&unique_code_param);
        if (status == BLE_ERR_OK)
        {
            device_addr_param.addr_type = unique_code_param.addr_type;
            memcpy(&device_addr_param.addr, &unique_code_param.ble_addr, 6);
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&device_addr_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }
            status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&DEVICE_ADDR);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }

        status = ble_cmd_resolvable_address_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // BLE Server profile init --> Peripheral (host id = 1)
        status = server_profile_init(APP_TRSP_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // BLE Client profile init --> Central (host id = 0)
        status = client_profile_init(APP_TRSP_C_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Initializes the application-level resources.
 *
 * This function prints the startup banner, creates the application message queue and
 * its counting semaphores (semaphore_cb, semaphore_isr, semaphore_app), initializes the
 * BLE host stack with the configured task priorities, re-initializes UART0 for the
 * application's own use (after deinitializing the stdio UART), and configures the
 * wake-up GPIO pin (falling-edge interrupt with app_gpio_handler as callback) as a
 * low power wake-up source.
 *
 * @return void This function does not return a value.
 */
static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;

    // banner
    printf("---------------------------------------------------\n");
    printf("  BLE TRSP 1C1P demo: start...\n");
    printf("---------------------------------------------------\n");
    printf("***************************************************\n");
    printf("1C1P Host id List\n");
    printf("Role : Central, Host id = %d\n", APP_TRSP_C_HOST_ID);
    printf("Role : Peripheral, Host id = %d\n", APP_TRSP_P_HOST_ID);
    printf("***************************************************\n");

    // application queue & semaphore
    g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
    semaphore_cb = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
    semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
    semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);

    // BLE Stack init  
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    if (ble_host_stack_init(&ble_task_level) == 0) {
        printf("BLE stack initial success...\n");
    }
    else {
        printf("BLE stack initial fail...\n");
    }

    uart_stdio_deinit();
    app_uart_init();

    // wake up pin
    input_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    input_cfg.usr_cb = app_gpio_handler;
    input_cfg.param = NULL;
    hosal_gpio_cfg_input(GPIO_WAKE_UP_PIN, input_cfg);
    hosal_gpio_debounce_enable(GPIO_WAKE_UP_PIN);
    hosal_gpio_int_enable(GPIO_WAKE_UP_PIN);
    NVIC_EnableIRQ(Gpio_IRQn);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO);
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
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
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
