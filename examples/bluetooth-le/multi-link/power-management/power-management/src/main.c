/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file
 *
 * @brief BLE Power Measurement demo: a dual-link application that runs a TRSP
 *        central role (host id 0) and a TRSP peripheral role (host id 1)
 *        simultaneously, together with low-power mode (HOSAL LPM) transitions
 *        driven by GPIO and UART wake-up sources.
 *
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
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "ctrl_cmd.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"
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

//Scan parameters
#define SCAN_TYPE                       SCAN_TYPE_ACTIVE
#define SCAN_FILTER                     SCAN_FILTER_POLICY_BASIC_UNFILTERED
#define SCAN_WINDOW                     32U    //32*0.625ms=20ms
#define SCAN_INTERVAL                   32U    //32*0.625ms=20ms

//Initial connection parameters
#define APP_CONN_INTERVAL_MIN           24U    //24*1.25ms=30ms
#define APP_CONN_INTERVAL_MAX           24U    //24*1.25ms=30ms      
#define CONN_SUPERVISION_TIMEOUT        500U    //500*10ms=5000ms
#define CONN_SLAVE_LATENCY              0U

//Advertising parameters
#define APP_ADV_TYPE                    ADV_TYPE_ADV_IND
#define APP_ADV_INTERVAL_MIN            160U   //160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U   //160*0.625ms=100ms

#define APP_PHY                         BLE_PHY_1M

// Advertising device name
#define DEVICE_NAME                     "POWER_DEMO"

// GAP device name
static const uint8_t     DEVICE_NAME_STR[] = {DEVICE_NAME};

// BLE application uses Write or Write Without Response
#define BLE_TRSP_WRITE_TYPE             BLEGATT_WRITE_WITHOUT_RSP  // BLEGATT_WRITE; BLEGATT_WRITE_WITHOUT_RSP

// Default data length
#define APP_DATA_LEN                    20


// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xC6 }
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
// demo application parameter
AppParam        appParam;

ble_gap_addr_t g_target_addr  = {.addr_type = RANDOM_ADDR,
                                 .addr = {0x71, 0x72, 0x73, 0x74, 0x75, 0xC6}
                                };

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static uint8_t g_trsp_mtu = DEFAULT_MTU;
static uint8_t g_uart_buffer[250];
static uint8_t g_uart_length;


/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static ble_err_t ble_init(void);
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t adv_start(uint8_t host_id);
static ble_err_t scan_start(void);
static ble_err_t conn_create(uint8_t host_id, ble_gap_addr_t *p_peer);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
/**
 * @brief GPIO wake-up pin interrupt handler.
 *
 * Registered as the callback for GPIO_WAKE_UP_PIN (falling-edge triggered).
 * When the wake-up pin toggles, it masks (blocks) the BLE application task's
 * low-power sleep bit so the system stays awake while the GPIO event is handled.
 *
 * @param pin The GPIO pin number that triggered the interrupt.
 * @param isr_param User parameter registered together with the callback (not used).
 * @note Runs in interrupt context.
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

/**
 * @brief Handles a single character received from the UART, in ISR context.
 *
 * Accumulates characters into a local static buffer. When a line terminator
 * ('\n' or '\r') is received, the buffered line (including the terminator) is
 * copied into g_uart_buffer/g_uart_length and an APP_REQUEST_PROCESS_UART_CMD
 * request (host_id 0xFF) is queued from ISR context so the application task
 * can process the command later. The internal buffer index is then reset.
 *
 * @param ch The character received from the UART.
 * @return true if a complete line was received (so the caller can re-enable
 *         low-power sleep mode), false otherwise.
 * @note Called from the UART RX interrupt (uart0_rx_callback).
 */
static bool uart_data_handler(char ch)
{
    bool status = false;
    static uint8_t rx_buffer[250];
    static uint8_t index = 0;

    rx_buffer[index++] = ch;

    if ((ch == '\n') || (ch == '\r'))
    {
        memcpy(g_uart_buffer, rx_buffer, index);
        g_uart_length = index;
        if (app_request_set(0xFF, APP_REQUEST_PROCESS_UART_CMD, true) == false)
        {
            // No application queue buffer. Error.
        }
        // reset index
        index = 0;

        // set status to true indicates the system can enable sleep mode
        status = true;
    }

    return status;
}

/**
 * @brief UART receive line status callback.
 *
 * Invoked when the UART line status changes. If a break indicator (BI) is
 * present, the pending byte is drained and the low-power sleep mode is
 * masked (blocked) for the BLE application task.
 *
 * @param p_arg Pointer to the argument passed to the callback function (not used).
 * @return This callback has no explicit return statement, so the returned
 *         value is unspecified; it is not used by the UART driver.
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
 * @brief UART receive callback function.
 *
 * Reads one byte from UART0 and forwards it to uart_data_handler(). If a
 * complete line has been received, the low-power sleep mask for the BLE
 * application task is cleared (unmasked), allowing the system to sleep again.
 *
 * @param p_arg Pointer to the argument provided to the callback function (not used).
 * @return Always returns 0.
 * @note Runs in interrupt context (UART0 RX interrupt).
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
 * @brief Handles DIS (Device Information Service) client-role events.
 *
 * Processes read-response events for the manufacturer name string and
 * firmware revision string characteristics received from a connected DIS
 * server, and prints them to the console.
 *
 * @param p_param Pointer to the BLE ATT event parameters.
 * @return void This function does not return a value.
 * @note Only handles events where p_param->gatt_role is BLE_GATT_ROLE_CLIENT.
 */
static void ble_svcs_dis_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_DIS_MANUFACTURER_NAME_STRING_READ_RSP_EVENT:
            p_param->data[p_param->length] = '\0';
            printf("Manufacturer name: %s\n", p_param->data);
            break;

        case BLESERVICE_DIS_FIRMWARE_REVISION_STRING_READ_RSP_EVENT:
            p_param->data[p_param->length] = '\0';
            printf("FW rev.: %s\n", p_param->data);
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Handles TRSPS (Transparent Service Profile) client-role events.
 *
 * Dispatches notify, indicate, and write-response events received from a
 * connected TRSPS server. In this demo the handled cases are placeholders
 * and do not perform any additional processing.
 *
 * @param p_param Pointer to the BLE ATT event parameters.
 * @return void This function does not return a value.
 * @note Only handles events where p_param->gatt_role is BLE_GATT_ROLE_CLIENT.
 */
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATNI01_NOTIFY_EVENT:
        case BLESERVICE_TRSPS_UDATNI01_INDICATE_EVENT:
            break;

        case BLESERVICE_TRSPS_UDATRW01_WRITE_RSP_EVENT:
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Runs the post-connection TRSP client setup command sequence.
 *
 * Called repeatedly (once per invocation) after the ATT database parsing has
 * finished, to step through the sequence: MTU exchange, data length update,
 * set TRSPS CCCD, set GATT service-changed CCCD, read DIS manufacturer name,
 * and read DIS firmware revision. Each call issues the next command in the
 * sequence identified by an internal static index, which only advances when
 * the previous command returned BLE_ERR_OK.
 *
 * @param host_id The connection's host id.
 * @return ble_err_t
 * @retval BLE_ERR_OK The full command sequence has completed (the internal
 *         step index is reset to 0).
 * @retval BLE_BUSY The link is not connected (the internal step index is also
 *         reset to 0 in this case), or the link is connected but the sequence
 *         has not finished yet; the caller is expected to invoke this function
 *         again to continue.
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
            status = ble_cmd_mtu_size_update(host_id, g_trsp_mtu);
            break;

        case 1:
            // send data length updated to server
            status = ble_cmd_data_len_update(host_id, (g_trsp_mtu + 4)); // 4 bytes header
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

// TRSPS Central
/**
 * @brief Handles application requests for the central role.
 *
 * Processes the application requests targeted at the central-role link
 * (APP_POWER_MEASUREMENT_C_HOST_ID): creating/cancelling a connection,
 * running the post-connection TRSPC multi-command sequence, starting/stopping
 * scanning, disconnecting, and updating the connection parameters or PHY.
 *
 * @param p_param Pointer to the request parameters (host id and request type).
 * @return void This function does not return a value.
 */
static void app_central_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_CREATE_CONN:
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.client_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.client_info.data);
        status = conn_create(host_id, &g_target_addr);
        if (status != BLE_ERR_OK)
        {
            printf("conn_create status = %d\n", status);
        }
        break;

    case APP_REQUEST_CREATE_CONN_CANCEL:
        status = ble_cmd_conn_create_cancel();
        if (status != BLE_ERR_OK)
        {
            printf("conn_create cancel status = %d\n", status);
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
        }
        break;

    case APP_REQUEST_SCAN_START:
    {
        //set scan enable
        status = scan_start();
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_scan_enable status = 0x%02x\n", status);
        }
    }
    break;

    case APP_REQUEST_SCAN_STOP:
        // disable scan
        status = ble_cmd_scan_disable();
        if (status != BLE_ERR_OK)
        {
            printf("ble_scan_disable status = %d\n", status);
        }
        break;

    case APP_REQUEST_DISCONNECT:
        status = ble_cmd_conn_terminate(host_id);
        if (status != BLE_ERR_OK)
        {
            printf("Disconnect failed, status = %d\n", status);
        }
        break;

    case APP_REQUEST_CONN_UPDATE_PARAM:
    {
        ble_gap_conn_param_update_param_t conn_param;

        conn_param.host_id = host_id;
        conn_param.ble_conn_param.min_conn_interval = appParam.conn_param.ble_conn_param.min_conn_interval;
        conn_param.ble_conn_param.max_conn_interval = appParam.conn_param.ble_conn_param.max_conn_interval;
        conn_param.ble_conn_param.periph_latency = appParam.conn_param.ble_conn_param.periph_latency;
        conn_param.ble_conn_param.supv_timeout = appParam.conn_param.ble_conn_param.supv_timeout;

        status = ble_cmd_conn_param_update(&conn_param);
        if (status != BLE_ERR_OK)
        {
            printf("Connection parameter update fail, status = %d\n", status);
        }
    }
    break;

    case APP_REQUEST_PHY_UPDATE:
    {
        ble_gap_phy_update_param_t phy_param;
        phy_param.host_id = host_id;
        phy_param.tx_phy = appParam.rf_phy;
        phy_param.rx_phy = appParam.rf_phy;
        phy_param.coded_phy_option = appParam.phy_option;
        status = ble_cmd_phy_update(&phy_param);

        if (status != BLE_ERR_OK)
        {
            printf("phy update fail, status = %d\n", status);
        }
    }
    break;

    default:
        break;
    }
}

// TRSPS Peripheral
/**
 * @brief Handles application requests for the peripheral role.
 *
 * Processes the application requests targeted at the peripheral-role link
 * (APP_POWER_MEASUREMENT_P_HOST_ID): starting/stopping advertising,
 * disconnecting, and updating the connection parameters or PHY.
 *
 * @param p_param Pointer to the request parameters (host id and request type).
 * @return void This function does not return a value.
 */
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.server_info.data);

        // enable advertising
        status = adv_start(host_id);
        if (status != BLE_ERR_OK)
        {
            printf("Adv start status = %d\n", status);
        }
        break;

    case APP_REQUEST_ADV_STOP:
        status = ble_cmd_adv_disable();
        if (status != BLE_ERR_OK)
        {
            printf("Adv. disable status = %d\n", status);
        }
        break;

    case APP_REQUEST_DISCONNECT:
        status = ble_cmd_conn_terminate(host_id);
        if (status != BLE_ERR_OK)
        {
            printf("Disconnect failed, status = %d\n", status);
        }
        break;

    case APP_REQUEST_CONN_UPDATE_PARAM:
    {
        ble_gap_conn_param_update_param_t conn_param;

        conn_param.host_id = host_id;
        conn_param.ble_conn_param.min_conn_interval = appParam.conn_param.ble_conn_param.min_conn_interval;
        conn_param.ble_conn_param.max_conn_interval = appParam.conn_param.ble_conn_param.max_conn_interval;
        conn_param.ble_conn_param.periph_latency = appParam.conn_param.ble_conn_param.periph_latency;
        conn_param.ble_conn_param.supv_timeout = appParam.conn_param.ble_conn_param.supv_timeout;

        status = ble_cmd_conn_param_update(&conn_param);
        if (status != BLE_ERR_OK)
        {
            printf("Connection parameter update fail, status = %d\n", status);
        }
    }
    break;

    case APP_REQUEST_PHY_UPDATE:
    {
        ble_gap_phy_update_param_t phy_param;
        phy_param.host_id = host_id;
        phy_param.tx_phy = appParam.rf_phy;
        phy_param.rx_phy = appParam.rf_phy;
        phy_param.coded_phy_option = appParam.phy_option;
        status = ble_cmd_phy_update(&phy_param);

        if (status != BLE_ERR_OK)
        {
            printf("phy update fail, status = %d\n", status);
        }
    }
    break;

    default:
        break;
    }
}

/**
 * @brief Handles general BLE events for both the central and peripheral links.
 *
 * Processes connection-cancel, connection-complete, connection-parameter-update,
 * PHY read/update, MTU exchange, data-length-change, disconnection-complete, and
 * ATT database parsing-complete events, updating link state and printing status
 * information. On DB-parse-complete for the central link, it also fetches the
 * link's service handles and kicks off the TRSPC multi-command sequence.
 *
 * @param p_param Pointer to the BLE event parameters.
 * @return void This function does not return a value.
 */
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_GAP_EVT_CONN_CANCEL:
        ble_app_link_info[APP_POWER_MEASUREMENT_C_HOST_ID].state = STATE_STANDBY;
        appParam.active_role = APP_ACTINE_NONE;
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

        g_trsp_mtu = p_mtu_param->mtu; // update to real mtu size, 3 bytes header

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
        appParam.active_role = APP_ACTINE_NONE;
    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_err_t status;
        ble_evt_att_db_parse_complete_t *p_parsing_param = (ble_evt_att_db_parse_complete_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_att_db_parse_complete;

        if (p_parsing_param->host_id == APP_POWER_MEASUREMENT_C_HOST_ID)
        {
            printf("DB Parsing completed, ID:%d status:0x%02x\n", p_parsing_param->host_id, p_parsing_param->result);

            // Get LINK 0 all service handles and related information
            status = link0_svcs_handles_get(p_parsing_param->host_id);

            if (status != BLE_ERR_OK)
            {
                printf("Get handle fail, ID:%d status:%d\n", p_parsing_param->host_id, status);
            }

            // Do GATT commands
            if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_TRSPC_MULTI_CMD, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
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
 * @brief Sets the scan parameters and enables scanning.
 *
 * Copies the current scan parameters from appParam.app_param_c.scan_param,
 * applies them via ble_cmd_scan_param_set(), and then enables scanning via
 * ble_cmd_scan_enable().
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing command otherwise).
 */
static ble_err_t scan_start(void)
{
    ble_err_t      status;
    ble_scan_param_t  scan_param;

    status = BLE_ERR_OK;

    do
    {
        scan_param.scan_type = appParam.app_param_c.scan_param.scan_type;
        scan_param.own_addr_type = appParam.app_param_c.scan_param.own_addr_type;
        scan_param.scan_interval = appParam.app_param_c.scan_param.scan_interval;
        scan_param.scan_window = appParam.app_param_c.scan_param.scan_window;
        scan_param.scan_filter_policy = appParam.app_param_c.scan_param.scan_filter_policy;

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
 * @brief Issues a connection-create request to a given peer address.
 *
 * Builds the create-connection parameters (own address, scan interval/window,
 * "accept all" initiator filter policy, peer address, and the connection
 * parameters from appParam.conn_param) and sends the create-connection command.
 *
 * @param host_id The host id that will own the resulting connection.
 * @param p_peer Pointer to the peer device address to connect to.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from ble_cmd_conn_create() otherwise).
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

    create_conn_param.conn_param.max_conn_interval = appParam.conn_param.ble_conn_param.max_conn_interval;
    create_conn_param.conn_param.min_conn_interval = appParam.conn_param.ble_conn_param.min_conn_interval;
    create_conn_param.conn_param.periph_latency = appParam.conn_param.ble_conn_param.periph_latency;
    create_conn_param.conn_param.supv_timeout = appParam.conn_param.ble_conn_param.supv_timeout;

    status = ble_cmd_conn_create(&create_conn_param);

    return status;
}

/**
 * @brief Initializes the advertising parameters and data for the peripheral role.
 *
 * Sets the advertising parameters from appParam.app_param_p.adv_param (plus a
 * fixed "all channels" map and "accept all" filter policy), sets the
 * advertising data (limited-discoverable-mode flags), and sets the scan
 * response data (the device's complete local name).
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing command otherwise).
 */
static ble_err_t adv_init(void)
{
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + strlen(DEVICE_NAME_STR); //  1 byte data type

    // adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
    };

    //Scan response data
    uint8_t adv_scan_rsp_data[2 + strlen(DEVICE_NAME_STR)]; // Adjust the size as needed
    adv_scan_rsp_data[0] = SCANRSP_ADLENGTH;                      // AD length
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;       // AD data type
    memcpy(&adv_scan_rsp_data[2], DEVICE_NAME_STR, strlen(DEVICE_NAME_STR)); // Copy the name

    do
    {
        adv_param.adv_type = appParam.app_param_p.adv_param.adv_type;
        adv_param.own_addr_type = appParam.app_param_p.adv_param.own_addr_type;
        adv_param.adv_interval_min = appParam.app_param_p.adv_param.adv_interval_min;
        adv_param.adv_interval_max = appParam.app_param_p.adv_param.adv_interval_max;
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
 * @param host_id The ID of the host for which to enable advertising.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from ble_cmd_adv_enable() otherwise).
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
 * Convenience wrapper that first initializes the advertising parameters and
 * data via adv_init(), then enables advertising via adv_enable().
 *
 * @param host_id The ID of the host for which to start advertising.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing step otherwise).
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
 * @brief Queues an application request for the specified host id.
 *
 * Adds a request to the application message queue for the specified host id.
 * Can be called from both normal task context and ISR context; the function
 * takes a counting semaphore (semaphore_app or semaphore_isr, respectively)
 * to reserve a queue slot before enqueuing, giving the semaphore back if the
 * enqueue fails.
 *
 * @param host_id The ID of the host for which to set the request.
 * @param request The application request to set.
 * @param from_isr Indicates whether the function is called from an ISR
 *                 context (true) or not (false).
 * @return true if the request was successfully queued, false if no queue
 *         slot/semaphore was available or the enqueue failed.
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
 * @brief Callback function for general BLE application events.
 *
 * Copies the incoming event (including its extended data) into a heap-allocated
 * TLV buffer and queues it (as QUEUE_TYPE_OTHERS / APP_GENERAL_EVENT) for
 * processing by the application task. Takes the semaphore_cb counting
 * semaphore to bound the number of in-flight buffers.
 *
 * @param p_param Pointer to the BLE event parameters.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Buffer allocation failed.
 * @retval BLE_BUSY No semaphore/queue slot was available, or the enqueue failed.
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
 * @brief Callback function for BLE service (ATT/GATT) data events.
 *
 * Copies the incoming service event (including its variable-length data)
 * into a heap-allocated TLV buffer and queues it (as QUEUE_TYPE_OTHERS /
 * APP_SERVICE_EVENT) for processing by the application task. Takes the
 * semaphore_cb counting semaphore to bound the number of in-flight buffers.
 *
 * @param p_param Pointer to the BLE ATT event parameters.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Buffer allocation failed.
 * @retval BLE_BUSY No semaphore/queue slot was available, or the enqueue failed.
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
 * @brief Callback function for BLE L2CAP data events.
 *
 * Copies the incoming L2CAP event (including its variable-length data) into
 * a heap-allocated TLV buffer and queues it (as QUEUE_TYPE_OTHERS /
 * APP_L2CAP_DATA_EVENT) for processing by the application task. Takes the
 * semaphore_cb counting semaphore to bound the number of in-flight buffers.
 *
 * @param p_param Pointer to the BLE L2CAP event parameters.
 * @return ble_err_t
 * @retval BLE_ERR_OK The event was successfully queued.
 * @retval BLE_ERR_DATA_MALLOC_FAIL Buffer allocation failed.
 * @retval BLE_BUSY No semaphore/queue slot was available, or the enqueue failed.
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
 * @brief Main task for the BLE application.
 *
 * Performs the BLE stack/profile initialization via ble_init(), then loops
 * forever receiving from the application message queue and dispatching each
 * entry: application requests (QUEUE_TYPE_APP_REQ) are routed to
 * ble_app_main(), while queued BLE/service/L2CAP events (QUEUE_TYPE_OTHERS)
 * are routed to ble_evt_handler() or the relevant service attribute handler.
 *
 * @return void This function does not return (infinite loop); if ble_init()
 *         fails it prints an error and halts in an infinite loop instead.
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
 * @brief Main application-request dispatcher.
 *
 * Routes a dequeued application request either to the UART command processor
 * (when host_id is 0xFF and the request is APP_REQUEST_PROCESS_UART_CMD), or
 * to app_central_handler()/app_peripheral_handler() based on whether host_id
 * matches the central or peripheral link.
 *
 * @param p_param Pointer to the request parameters (host id and request type).
 * @return void This function does not return a value.
 */
static void ble_app_main(app_req_param_t *p_param)
{
    if (p_param->host_id == 0xFF)
    {
        if (p_param->app_req == APP_REQUEST_PROCESS_UART_CMD)
        {
            handle_ctrl_cmd(g_uart_buffer, g_uart_length);
        }
    }
    else
    {
        if (p_param->host_id == APP_POWER_MEASUREMENT_C_HOST_ID)
        {
            app_central_handler(p_param);
        }
        else if (p_param->host_id == APP_POWER_MEASUREMENT_P_HOST_ID)
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
 * @brief Initializes UART0 for the demo's command console.
 *
 * Initializes the UART device, registers the RX and receive-line-status
 * callbacks, configures interrupt-driven RX mode, and enables UART RX as a
 * HOSAL low-power wake-up source so an incoming byte can wake the system
 * from sleep.
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
 * Resets the service-changed characteristic's client configuration
 * descriptor (CCCD) to 0.
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
 * Resets the user data notification characteristic's client configuration
 * descriptor (CCCD) to 0.
 *
 * @param p_data Pointer to the TRSPS service data structure.
 * @return void This function does not return a value.
 */
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data)
{
    p_data->udatni01_cccd = 0;
}

/**
 * @brief Initializes the client-role profile (GAP/GATT/DIS/TRSPS) for a host id.
 *
 * Sets the link's state to STATE_STANDBY, then initializes the GAP, GATT,
 * DIS, and TRSPS services in the BLE_GATT_ROLE_CLIENT role, registering
 * ble_svcs_dis_evt_handler() and ble_svcs_trsps_evt_handler() for the DIS and
 * TRSPS service events respectively.
 *
 * @param host_id The ID of the host (link) to initialize as a client.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing service init otherwise).
 */
static ble_err_t client_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;
    do
    {
        //------------------------------------------------------------------------
        // init LINK0 GAP/ DIS/ UDF01S services parameter and register callback function
        //------------------------------------------------------------------------

        // GAP Related
        // -------------------------------------
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_CLIENT, &(p_profile_info->svcs_info_gaps), NULL);
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
 * @brief Initializes the server-role profile (GAP/GATT/DIS/TRSPS) for a host id.
 *
 * Sets the link's state to STATE_STANDBY, sets the GAP device name, then
 * initializes the GAP, GATT, DIS, and TRSPS services in the
 * BLE_GATT_ROLE_SERVER role, registering ble_svcs_dis_evt_handler() and
 * ble_svcs_trsps_evt_handler() for the DIS and TRSPS service events
 * respectively.
 *
 * @param host_id The ID of the host (link) to initialize as a server.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing service init otherwise).
 */
static ble_err_t server_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;
    do
    {
        //------------------------------------------------------------------------
        // init LINK0 GAP/ DIS/ UDF01S services parameter and register callback function
        //------------------------------------------------------------------------

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
 * @brief Initializes the BLE stack, device address, and both link profiles.
 *
 * Registers the general/service/L2CAP event callbacks, initializes the PHY
 * controller, sets the device address (from the factory-programmed unique
 * code if available, falling back to DEVICE_ADDR) and identity resolving key,
 * initializes resolvable address generation and the suggested data length,
 * then initializes the central-role profile (client_profile_init) on
 * APP_POWER_MEASUREMENT_C_HOST_ID and the peripheral-role profile
 * (server_profile_init) on APP_POWER_MEASUREMENT_P_HOST_ID. Finally it
 * populates appParam with the default scan/adv/connection/PHY settings and
 * prints the UART control command help text.
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error
 *         code from the failing step otherwise).
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

        // BLE profile init --> client(host id = 0)
        status = client_profile_init(APP_POWER_MEASUREMENT_C_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // BLE profile init --> server(host id = 1)
        status = server_profile_init(APP_POWER_MEASUREMENT_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

    } while (0);

    ble_cmd_device_addr_get(&device_addr_param);

    appParam.active_role = APP_ACTINE_NONE;
    appParam.app_param_c.scan_param.scan_type = SCAN_TYPE;
    appParam.app_param_c.scan_param.own_addr_type = device_addr_param.addr_type;
    appParam.app_param_c.scan_param.scan_interval = SCAN_INTERVAL;
    appParam.app_param_c.scan_param.scan_window = SCAN_WINDOW;
    appParam.app_param_c.scan_param.scan_filter_policy = SCAN_FILTER;

    appParam.app_param_p.adv_param.adv_channel_map = ADV_CHANNEL_ALL;
    appParam.app_param_p.adv_param.adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;
    appParam.app_param_p.adv_param.adv_interval_min = APP_ADV_INTERVAL_MIN;
    appParam.app_param_p.adv_param.adv_interval_max = APP_ADV_INTERVAL_MAX;
    appParam.app_param_p.adv_param.adv_type = APP_ADV_TYPE;
    appParam.app_param_p.adv_param.own_addr_type = device_addr_param.addr_type;

    appParam.conn_param.ble_conn_param.min_conn_interval = APP_CONN_INTERVAL_MIN;
    appParam.conn_param.ble_conn_param.max_conn_interval = APP_CONN_INTERVAL_MAX;
    appParam.conn_param.ble_conn_param.periph_latency = CONN_SLAVE_LATENCY;
    appParam.conn_param.ble_conn_param.supv_timeout = CONN_SUPERVISION_TIMEOUT;

    appParam.rf_phy = BLE_PHY_1M;
    appParam.phy_option = BLE_CODED_PHY_NO_PREFERRED;
    // show AT CMD HELP
    print_ctrl_cmd_help();

    return status;
}

/**
 * @brief Initializes the application: queues, semaphores, BLE stack, UART, and wake-up GPIO.
 *
 * Creates the application message queue and its associated counting
 * semaphores, initializes the BLE host stack with the configured task
 * priorities, re-initializes UART0 for the command console (via
 * app_uart_init()), and configures GPIO_WAKE_UP_PIN as a debounced,
 * falling-edge interrupt source (handled by app_gpio_handler()) that is
 * also registered as a HOSAL low-power wake-up source.
 *
 * @return void This function does not return a value.
 */
static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;

    // banner
    printf("------------------------------------------\n");
    printf("  BLE demo: Power Measurement Tool\n");
    printf("------------------------------------------\n");

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
 * @brief Main entry point for the application FreeRTOS task.
 *
 * This function initializes the HOSAL low-power module and sets the default
 * low-power sleep level to HOSAL_LOW_POWER_LEVEL_SLEEP0, initializes the RF
 * module for the BLE controller, starts the application initialization
 * (app_init()) and main task (app_main_task()), and finally enters an
 * infinite loop (which is not reached in practice since app_main_task()
 * itself never returns).
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
 * Configures the pin mux, initializes the stdio UART, heap regions, and boot
 * info dump, creates the main FreeRTOS task (app_main_entry), and starts the
 * FreeRTOS scheduler.
 *
 * @return int Not used; execution does not normally return from
 *         vTaskStartScheduler().
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
