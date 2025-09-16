/**************************************************************************************************
 * @file main.c
 * @brief BLE Transparent Service Profile (TRSP) Peripheral Demonstration.
 * @version 1.0
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * @details This file demonstrates the implementation of a BLE Transparent Service Profile (TRSP) 
 *          in a peripheral role. It includes features for BLE-based data transmission and Firmware 
 *          Over-The-Air (FOTA) updates. The application handles BLE service initialization, UART 
 *          communication, and BLE events such as advertising, connection management, and data exchange.
 **************************************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_event.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_fota.h"
#include "hosal_gpio.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
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

#define APP_TRSP_P_HOST_ID              0

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX
#define UART_BUFF_DEPTH                 2

// Advertising device name
#define DEVICE_NAME                    "TRSP_FOTA_DEMO"

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms

// GAP device name
static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x91, 0x92, 0x93, 0x94, 0x95, 0xC6}
                                           };

#define UART0_OPERATION_PORT            0
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)

#define GPIO_WAKE_UP_PIN                0

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static TimerHandle_t  g_fota_timer;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;
static uint8_t g_rx_buffer[UART_BUFF_DEPTH][DEFAULT_MTU];
static uint8_t g_rx_buffer_length[UART_BUFF_DEPTH];
static uint8_t g_uart_index = 0;
static uint8_t g_uart_transmit_index = 0;
static uint8_t g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
static bool g_data_send_pending   = false;    // true: indicates that there is data pending, still send old data and skip new data from UART.


/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_fota_data_init(ble_svcs_fotas_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool fota_sw_timer_start(void);
static void trsp_data_send_from_isr(uint8_t *p_data, uint8_t length);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
/**
 * @brief GPIO interrupt handler.
 *
 * This function is triggered when a GPIO interrupt occurs. It masks the low-power mode.
 *
 * @param pin The GPIO pin number that triggered the interrupt.
 * @param isr_param Additional parameters for the ISR (Interrupt Service Routine).
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

/**
 * @brief Handles incoming UART data.
 *
 * This function processes each character received from the UART. It stores the character in a buffer
 * and sends the buffer via the TRSP service when a certain condition is met (e.g., buffer is full or
 * a newline character is received). It also resets the buffer index and returns a status indicating
 * whether the system can enable sleep mode.
 *
 * @param ch The character received from the UART.
 * @return true if the system can enable sleep mode, false otherwise.
 */
static bool uart_data_handler(char ch)
{
    bool status = false;
    static uint8_t rx_buffer[DEFAULT_MTU];
    static uint8_t index = 0;

    rx_buffer[index++] = ch;

    if (index >= (g_trsp_mtu - 3)) // 3 bytes header
    {
        // send data via TRSP service
        trsp_data_send_from_isr(rx_buffer, index);

        // reset index
        index = 0;
    }
    else if ((ch == '\n') || (ch == '\r'))
    {
        // send data via TRSP service
        trsp_data_send_from_isr(rx_buffer, (index - 1)); // (-1) removed '\n' or '\r'

        // reset index
        index = 0;

        // set status to true indicates the system can enable sleep mode
        status = true;
    }
    return status;
}

/**
 * @brief UART receive line callback.
 *
 * This function is called when there is a line status change in the UART.
 *
 * @param p_arg Pointer to the argument passed to the callback function.
 * @return Always returns 0.
 */
static int uart0_receive_line_callback(void *p_arg)
{
    if (hosal_uart_get_lsr(&uart0_dev) & UART_LSR_BI)
    {
        char ch;

        hosal_uart_receive(&uart0_dev, &ch, 1);
        hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }

    return 0;
}

/**
 * @brief UART receive callback function.
 *
 * This function is invoked when data is received on the UART. It processes the received character
 * and determines whether the system can transition to sleep mode based on the received data.
 *
 * @param p_arg Pointer to the argument provided to the callback function.
 * @return Always returns 0.
 */
static int uart0_rx_callback(void *p_arg)
{
    char ch;

    // Receive a character from UART0
    hosal_uart_receive(&uart0_dev, &ch, 1);

    // Process the received character
    if (uart_data_handler(ch) == true)
    {
        // received '\n' or '\r' --> enable sleep mode
        hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }

    return 0;
}

/**
 * @brief Handles FOTA timer expiration.
 *
 * This function is invoked when the FOTA timer expires. It verifies the connection state
 * and processes the FOTA timer expiry event accordingly.
 *
 * @param timer The handle of the expired timer.
 * @return void This function does not return any value.
 */
static void fota_timer_handler(TimerHandle_t timer)
{
    configASSERT(timer);

    // FOTA
    if (ble_app_link_info[APP_TRSP_P_HOST_ID].state == STATE_CONNECTED)
    {
        // FOTA timer tick and check if timer is expired
        if (ble_fota_timertick() == EXPIRED)
        {
            ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_FOTA_TIMER_EXPIRY, false);
        }
    }
}

/**
 * @brief Handles TRSPS events from the client.
 *
 * This function processes events related to the Transparent Service Profile Server (TRSPS),
 * such as read and write requests from the client. It ensures proper handling of client
 * interactions with the TRSPS service, including data writes and reads.
 *
 * @param p_param Pointer to the BLE event parameters.
 *                - Contains details about the event, such as the type of event, the data involved,
 *                  and the host ID of the client.
 * @return void This function does not return any value.
 */
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
        case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
        {
            uint8_t *p_data;

            p_data = pvPortMalloc(p_param->length + 1);
            if (p_data != NULL)
            {
                memcpy(p_data, p_param->data, p_param->length);
                *(p_data + p_param->length) = '\0';
                printf("%s\n", p_data);
                vPortFree(p_data);
            }
        }
        break;

        case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
        {
            ble_err_t status;
            const uint8_t readData[] = "UDATRW01 data";
            ble_gatt_data_param_t gatt_data_param;

            gatt_data_param.host_id = p_param->host_id;
            gatt_data_param.handle_num = p_param->handle_num;
            gatt_data_param.length = SIZE_STRING(readData);
            gatt_data_param.p_data = (uint8_t *)readData;

            status = ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &gatt_data_param);;
            if (status != BLE_ERR_OK)
            {
                printf("ble_gatt_read_rsp status: %d\n", status);
            }
        }
        break;

        default:
            break;
        }
    }
}

/**
 * @brief Processes FOTA service events from the client.
 *
 * This function handles events related to the Firmware Over-The-Air (FOTA) service,
 * including data and command writes initiated by the client.
 *
 * @param p_param Pointer to the BLE event parameters.
 *                - Provides details about the event, such as the event type, associated data,
 *                  and the host ID of the client.
 * @return void This function does not return any value.
 */
static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT:
        {
            ble_fota_data(p_param->host_id, p_param->length, p_param->data);
            fota_sw_timer_start();
        }
        break;

        case BLESERVICE_FOTAS_COMMAND_WRITE_EVENT:
        {
            ble_fota_cmd(p_param->host_id, p_param->length, p_param->data);
            fota_sw_timer_start();
        }

        default:
            break;
        }
    }
}

/**
 * @brief Handles application requests for the peripheral role.
 * 
 * This function processes various application requests, such as starting advertising,
 * sending data, and handling FOTA timer expiration. It manages the state of the
 * peripheral device and interacts with the BLE stack to perform the requested actions.
 * 
 * @param p_param Pointer to the request parameters.
 *                - host_id: The ID of the host that triggered the request.
 *                - app_req: The type of request (e.g., start advertising, send data).
 * @return void This function does not return any value.
 * @note This function is called from the main application loop and is responsible for
 * handling the requests related to the peripheral role of the BLE device.
 * @note The function uses a switch-case structure to handle different types of requests.
 */
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    status = BLE_ERR_OK;
    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        do
        {
            // service data init
            svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
            svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.server_info.data);
            svcs_fota_data_init(&p_profile_info->svcs_info_fotas.server_info.data);

            // set preferred MTU size and data length
            status = ble_cmd_default_mtu_size_set(host_id, DEFAULT_MTU);
            if (status != BLE_ERR_OK)
            {
                printf("ble_cmd_default_mtu_size_set() status = %d\n", status);
                break;
            }

            // enable advertising
            status = adv_init();
            if (status != BLE_ERR_OK)
            {
                printf("adv_init() status = %d\n", status);
                break;
            }

            status = adv_enable(host_id);
            if (status != BLE_ERR_OK)
            {
                printf("adv_enable() status = %d\n", status);
                break;
            }
        } while (0);

        g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
        break;

    case APP_REQUEST_TRSPS_DATA_SEND:
        // send data to client
        if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
        {
            status = ble_svcs_trsps_server_send(  host_id,
                                                  BLEGATT_CCCD_NOTIFICATION,
                                                  p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                  &g_rx_buffer[g_uart_transmit_index][0],
                                                  g_rx_buffer_length[g_uart_transmit_index]);
        }
        else if ((p_profile_info->svcs_info_trsps.server_info.data.udatni01_cccd & BLEGATT_CCCD_INDICATION) != 0)
        {
            status = ble_svcs_trsps_server_send(  host_id,
                                                  BLEGATT_CCCD_INDICATION,
                                                  p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                                  &g_rx_buffer[g_uart_transmit_index][0],
                                                  g_rx_buffer_length[g_uart_transmit_index]);
        }

        if (status == BLE_ERR_OK)
        {
            g_data_send_pending = false;

            g_uart_transmit_index++;
            if (g_uart_transmit_index == UART_BUFF_DEPTH)
            {
                g_uart_transmit_index = 0;
            }
        }
        else
        {
            g_data_send_pending = true;

            // re-send
            if (ble_app_request_set(host_id, APP_REQUEST_TRSPS_DATA_SEND, false) == false)
            {
                // No Application queue buffer. Error.
            }
        }
        break;

    case APP_REQUEST_FOTA_TIMER_EXPIRY:
        // handle FOTA timer expired event
        ble_fota_timerexpiry_handler(host_id);
        break;

    default:
        break;
    }
}

/**
 * @brief Handles the advertising set enable event.
 *
 * This function processes the event when advertising is enabled or disabled.
 *
 * @param p_adv_enable Pointer to the advertising enable event parameters.
 * @return void This function does not return a value.
 */
static void handle_adv_set_enable(ble_evt_adv_set_adv_enable_t *p_adv_enable)
{
    if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
    {
        if (p_adv_enable->adv_enabled == true)
        {
            if (g_advertising_host_id != BLE_HOSTID_RESERVED)
            {
                ble_app_link_info[g_advertising_host_id].state = STATE_ADVERTISING;
            }
            printf("Advertising...\n");
        }
        else
        {
            if (g_advertising_host_id != BLE_HOSTID_RESERVED)
            {
                ble_app_link_info[g_advertising_host_id].state = STATE_STANDBY;
            }
            printf("Idle.\n");
        }
    }
    else
    {
        printf("Advertising enable failed.\n");
    }
}

/**
 * @brief Handles the connection complete event.
 *
 * This function processes the event when a connection is established or failed.
 *
 * @param p_conn_param Pointer to the connection complete event parameters.
 */
static void handle_conn_complete(ble_evt_gap_conn_complete_t *p_conn_param)
{
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

/**
 * @brief Handles the connection parameter update event.
 *
 * This function processes the event when a connection parameter update is successful or failed.
 *
 * @param p_conn_param Pointer to the connection parameter update event parameters.
 * @return void This function does not return a value.
 */
static void handle_conn_param_update(ble_evt_gap_conn_param_update_t *p_conn_param)
{
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

/**
 * @brief Handles the PHY update event.
 *
 * This function processes the event when the PHY is updated or read.
 *
 * @param p_phy_param Pointer to the PHY event parameters.
 * @return void This function does not return a value.
 */
static void handle_phy_update(ble_evt_gap_phy_t *p_phy_param)
{
    if (p_phy_param->status != BLE_HCI_ERR_CODE_SUCCESS)
    {
        printf("PHY update/read failed, error code = 0x%02x\n", p_phy_param->status);
    }
    else
    {
        printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
    }
}

/**
 * @brief Handles the MTU exchange event.
 *
 * This function processes the event when the MTU is exchanged.
 *
 * @param p_mtu_param Pointer to the MTU exchange event parameters.
 * @return void This function does not return a value.
 */
static void handle_mtu_exchange(ble_evt_mtu_t *p_mtu_param)
{
    g_trsp_mtu = p_mtu_param->mtu; // update to real mtu size

    printf("MTU Exchanged, ID:%d, size: %d\n", p_mtu_param->host_id, p_mtu_param->mtu);
}

/**
 * @brief Handles the write suggested default data length event.
 *
 * This function processes the event when the default data length is written.
 *
 * @param p_data_len_param Pointer to the write suggested default data length event parameters.
 *                         - status: The status of the write operation.
 *                         - other fields: Additional fields related to the event.
 * @return void This function does not return a value.
 */
static void handle_write_suggested_default_data_length(ble_evt_suggest_data_length_set_t *p_data_len_param)
{
    printf("Write default data length, status: %d\n", p_data_len_param->status);
}

/**
 * @brief Handles the data length change event.
 *
 * This function processes the event when the data length is changed.
 *
 * @param p_data_len_param Pointer to the data length change event parameters, which includes the new data length values.
 * @return void This function does not return a value.
 */
static void handle_data_length_change(ble_evt_data_length_change_t *p_data_len_param)
{
    printf("Data length changed, ID: %d\n", p_data_len_param->host_id);
    printf("MaxTxOctets: %d  MaxTxTime:%d\n", p_data_len_param->max_tx_octets, p_data_len_param->max_tx_time);
    printf("MaxRxOctets: %d  MaxRxTime:%d\n", p_data_len_param->max_rx_octets, p_data_len_param->max_rx_time);
}

/**
 * @brief Handles the disconnection complete event.
 *
 * This function processes the event when a disconnection is complete.
 * The STATE_STANDBY state indicates that the device is not connected and is ready to start advertising again.
 *
 * @param p_disconnect_param Pointer to the disconnection complete event parameters.
 * @return void This function does not return a value.
 */
static void handle_disconn_complete(ble_evt_gap_disconn_complete_t *p_disconnect_param)
{
    if (p_disconnect_param->status != BLE_HCI_ERR_CODE_SUCCESS)
    {
        printf("Disconnect failed, error code = 0x%02x\n", p_disconnect_param->status);
    }
    else
    {
        ble_fota_disconnect();
        ble_app_link_info[p_disconnect_param->host_id].state = STATE_STANDBY;

        // re-start adv
        if (ble_app_request_set(p_disconnect_param->host_id, APP_REQUEST_ADV_START, false) == false)
        {
            // No Application queue buffer. Error.
        }

        printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconnect_param->host_id, p_disconnect_param->reason);
    }
}

/**
 * @brief Handles BLE events.
 *
 * This function processes various BLE events, such as advertising enable/disable,
 * connection completion, disconnection, and data length changes. It updates the
 * application state and prints relevant information to the console.
 *
 * @param p_param Pointer to the BLE event parameters.
 *                - Contains details about the event, such as the event type and associated data.
 * @return void This function does not return any value.
 */
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_ADV_EVT_SET_ENABLE:
        handle_adv_set_enable((ble_evt_adv_set_adv_enable_t *)&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable);
        break;

    case BLE_GAP_EVT_CONN_COMPLETE:
        handle_conn_complete((ble_evt_gap_conn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_complete);
        break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        handle_conn_param_update((ble_evt_gap_conn_param_update_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_param_update);
        break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
        handle_phy_update((ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy);
        break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
        handle_mtu_exchange((ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu);
        break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
        handle_write_suggested_default_data_length((ble_evt_suggest_data_length_set_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set);
        break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
        handle_data_length_change((ble_evt_data_length_change_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change);
        break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
        handle_disconn_complete((ble_evt_gap_disconn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_disconn_complete);
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
 * @brief Starts the FOTA software timer.
 *
 * This function ensures that the Firmware Over-The-Air (FOTA) software timer is active.
 * If the timer is not already running, it attempts to start it. The timer triggers
 * periodic FOTA-related tasks by invoking the fota_timer_handler function.
 *
 * @return true if the timer was successfully started or is already active, false otherwise.
 */
static bool fota_sw_timer_start(void)
{
    if ( xTimerIsTimerActive( g_fota_timer ) == pdFALSE )
    {
        if ( xTimerStart( g_fota_timer, 0 ) != pdTRUE )
        {
            // The timer could not be set into the Active state.
            return false;
        }
    }
    return true;
}

/**
 * @brief Sends data from the UART buffer to the TRSP service in an ISR context.
 *
 * This function is responsible for transmitting data from the UART buffer to the
 * Transparent Service Profile (TRSP) service. It ensures that pending data is sent
 * before processing new data. The function operates in an interrupt service routine (ISR)
 * context and manages the UART buffer indices for data transmission.
 *
 * @param p_data Pointer to the data buffer to be sent.
 * @param length Length of the data to be sent.
 */
static void trsp_data_send_from_isr(uint8_t *p_data, uint8_t length)
{
    // if there is data pending, still send old data and skip new data
    if (g_data_send_pending == false)
    {
        g_rx_buffer_length[g_uart_index] = length;
        memcpy(&g_rx_buffer[g_uart_index][0], p_data, length);

        g_uart_index++;
        if (g_uart_index == UART_BUFF_DEPTH)
        {
            g_uart_index = 0;
        }
        // send queue to task_bla_app
        if (ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_TRSPS_DATA_SEND, true) == false)
        {
            // No Application queue buffer. Error.
        }
    }
}

/**
 * @brief Initializes the advertising parameters and data.
 *
 * This function sets up the advertising parameters, including the advertising type,
 * interval, channel map, and filter policy. It also configures the advertising data
 * and scan response data to be used during BLE advertising.
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
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

    do {
        status = ble_cmd_device_addr_get(&addr_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_device_addr_get() status = %d\n", status);
            break;
        }

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
 * @brief Enables advertising for the specified host ID.
 *
 * This function starts advertising for the specified host ID. It sends a command to the BLE stack
 * to enable advertising and checks the status of the operation.
 *
 * @param host_id The ID of the host for which to enable advertising.
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
 * @brief Sets the application request for the specified host ID.
 *
 * This function adds a request to the application queue for the specified host ID.
 * It can be called from both normal and ISR contexts. The function handles the
 * synchronization of the queue and semaphore appropriately.
 *
 * @param host_id The ID of the host for which to set the request.
 * @param request The application request to set.
 * @param from_isr Indicates whether the function is called from an ISR context (true) or not (false).
 * @return true if the request was successfully set, false otherwise.
 */
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
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
 * @brief Callback function for BLE application events.
 *
 * This function handles general BLE application events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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

/**
 * @brief Callback function for BLE service data events.
 *
 * This function handles service data events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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
 * This function handles L2CAP data events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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
 * This function initializes the BLE stack, starts advertising, and processes incoming
 * events from the application queue. It handles requests related to the TRSP service,
 * including data transmission and FOTA timer expiration.
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

    // Initial parameters
    g_uart_index = 0;
    g_uart_transmit_index = 0;

    // start adv
    if (ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
    {
        // No Application queue buffer. Error.
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
 * @brief Main application handler for BLE requests.
 *
 * This function handles various BLE requests for the peripheral role.
 * It processes requests such as starting advertising and sending data.
 *
 * @param p_param Pointer to the request parameters.
 */
static void ble_app_main(app_req_param_t *p_param)
{
    // Link - Peripheral
    app_peripheral_handler(p_param);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */
/**
 * @brief Initializes the UART.
 * 
 * This function initializes the UART for communication.
 * It configures the UART to interrupt mode and sets the UART Rx interrupt callback function.
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
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, (void *)NULL);

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_UART_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

/**
 * @brief Initializes the GATT service data.
 * 
 * This function initializes the GATT service data structure for the peripheral role.
 * It sets the service changed characteristic client configuration descriptor (CCCD) to 0.
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
 * This function initializes the TRSPS service data structure for the peripheral role.
 * It sets the user data notification characteristic client configuration descriptor (CCCD) to 0.
 * 
 * @param p_data Pointer to the TRSPS service data structure.
 * @return void This function does not return a value.
 */
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data)
{
    p_data->udatni01_cccd = 0;
}

/**
 * @brief Initializes the FOTAS service data.
 * 
 * This function initializes the FOTAS service data structure for the peripheral role.
 * It sets the command and data characteristic client configuration descriptors (CCCD) to 0.
 * 
 * @param p_data Pointer to the FOTAS service data structure.
 * @return void This function does not return a value.
 */
static void svcs_fota_data_init(ble_svcs_fotas_data_t *p_data)
{
    p_data->command_cccd = 0;
    p_data->data_cccd = 0;
}

/**
 * @brief Initializes the server profile for the specified host ID.
 * 
 * This function initializes the server profile for the specified host ID.
 * It sets up the GATT, GAP, DIS, TRSPS, and FOTAS services.
 * 
 * @param host_id The ID of the host for which to initialize the server profile.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
 */
static ble_err_t server_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

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
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_dis), NULL);
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

        // FOTAS Related
        // -------------------------------------
        status = ble_svcs_fotas_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_fotas), ble_svcs_fotas_evt_handler);
        if (status != BLE_ERR_OK)
        {
            return status;
        }

    } while (0);

    return status;
}

/**
 * @brief Initializes the BLE stack and sets up callbacks.
 *
 * This function initializes the BLE stack, sets up necessary callbacks, and configures the device address.
 * It also initializes the PHY controller, LESC, and resolvable address, and sets the suggested data length.
 *
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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

        // BLE profile init --> only 1 link (host id = 0)
        status = server_profile_init(APP_TRSP_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        ble_fota_fw_buffer_flash_check();
        ble_fota_init();

    } while (0);

    return status;
}

/**
 * @brief Initializes the application.
 *
 * This function sets up the application by initializing the BLE stack, UART, and GPIO.
 * It also creates the necessary queues and semaphores for handling BLE events and requests.
 * 
 * @return void This function does not return a value.
 */
static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;

    // banner
    printf("------------------------------------------\n");
    printf("  BLE TRSP + FOTA (P) demo: start...\n");
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

    // application SW timer, tick = 1s
    g_fota_timer = xTimerCreate("FOTA_Timer", pdMS_TO_TICKS(1000), pdTRUE, ( void * ) 0, fota_timer_handler);

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
/**
 * @brief Main function.
 *
 * This is the entry point of the application. It initializes the RF module,
 * application, and starts the main application task.
 *
 * @return int This function does not return a value.
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
    while (1) {};

    return 0;
}
