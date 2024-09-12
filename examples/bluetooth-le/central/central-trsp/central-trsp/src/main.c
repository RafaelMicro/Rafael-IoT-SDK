/** @file
 *
 * @brief BLE TRSP central role demo.
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
#include "timers.h"
#include "ble_app.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_profile.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "uart_stdio.h"
#include "hosal_gpio.h"

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

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MIN
#define UART_BUFF_DEPTH                 2

// Device name
#define DEVICE_NAME                     'T', 'R', 'S', 'P', 'C'

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

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x41, 0x42, 0x43, 0x44, 0x45, 0xC6 }
                                           };

#define UART0_OPERATION_PORT            0
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, 16, 17, UART_BAUDRATE_115200)

#define GPIO_WAKE_UP_PIN                0

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static ble_gap_addr_t g_target_addr;
static uint8_t g_rx_buffer[UART_BUFF_DEPTH][DEFAULT_MTU];
static uint8_t g_rx_buffer_length[UART_BUFF_DEPTH];
static uint8_t g_uart_index = 0;
static uint8_t g_uart_transmit_index = 0;
static uint8_t g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
static bool g_data_send_pending   = false;    // true: indicates that there is data pending, still send old data and skip new data from UART.

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static ble_err_t ble_init(void);
static void ble_app_main(app_req_param_t *p_param);
static void trsp_data_send_from_isr(uint8_t *p_data, uint8_t length);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    lpm_low_power_mask(LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

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

static int uart0_receive_line_callback(void *p_arg)
{
    if (hosal_uart_get_lsr(&uart0_dev) & UART_LSR_BI)
    {
        char ch;

        hosal_uart_receive(&uart0_dev, &ch, 1);
        lpm_low_power_mask(LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }
}

static int uart0_rx_callback(void *p_arg)
{
    char ch;

    hosal_uart_receive(&uart0_dev, &ch, 1);
    if (uart_data_handler(ch) == true)
    {
        // recieved '\n' or '\r' --> enable sleep mode
        lpm_low_power_unmask(LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }

    return 0;
}

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

static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        /* ----------------- Handle event from server ----------------- */
        switch (p_param->event)
        {

        case BLESERVICE_TRSPS_UDATNI01_NOTIFY_EVENT:
        case BLESERVICE_TRSPS_UDATNI01_INDICATE_EVENT:
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

        default:
            break;
        }
    }
}

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

// TRSPS Central
static void app_central_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;
    ble_gap_addr_t addr_param;

    status = BLE_ERR_OK;
    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_SCAN_START:
    {
        ble_scan_param_t scan_param;

        // set  data length
        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK)
        {
            printf("MTU/ Data Length set status = 0x%02x\n", status);
        }

        // service data init
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.client_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.client_info.data);

        // get device address
        ble_cmd_device_addr_get(&addr_param);

        // set scan parameter
        scan_param.scan_type = SCAN_TYPE;
        scan_param.own_addr_type = addr_param.addr_type;
        scan_param.scan_filter_policy = SCAN_FILTER;
        scan_param.scan_interval = SCAN_INTERVAL;
        scan_param.scan_window = SCAN_WINDOW;
        status = ble_cmd_scan_param_set(&scan_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_scan_param_set status = 0x%02x\n", status);
        }

        //set scan enable
        status = ble_cmd_scan_enable();
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_scan_enable status = 0x%02x\n", status);
        }

        g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
    }
    break;

    case APP_REQUEST_SCAN_STOP:
        // disable scan
        status = ble_cmd_scan_disable();
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_scan_disable status = 0x%02x\n", status);
        }
        break;

    case APP_REQUEST_CREATE_CONN:
    {
        ble_gap_create_conn_param_t conn_create_param;

        if (ble_app_link_info[host_id].state == STATE_INITIATING)
        {
            conn_create_param.host_id = host_id;
            conn_create_param.own_addr_type = DEVICE_ADDR.addr_type;
            conn_create_param.scan_interval = SCAN_INTERVAL;
            conn_create_param.scan_window = SCAN_WINDOW;
            conn_create_param.init_filter_policy = INIT_FILTER_POLICY_ACCEPT_ALL;
            memcpy(&conn_create_param.peer_addr, &g_target_addr, sizeof(ble_gap_peer_addr_t));
            conn_create_param.conn_param.min_conn_interval = CONN_INTERVAL_MIN;
            conn_create_param.conn_param.max_conn_interval = CONN_INTERVAL_MAX;
            conn_create_param.conn_param.periph_latency = CONN_PERIPHERAL_LATENCY;
            conn_create_param.conn_param.supv_timeout = CONN_SUPERVISION_TIMEOUT;

            status = ble_cmd_conn_create(&conn_create_param);
            if (status != BLE_ERR_OK)
            {
                printf("ble_cmd_conn_create status = 0x%02x\n", status);
            }
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
                printf("connection terminated, reconnect server to restart TX/RX data. \n");
            }
        }
        break;

    case APP_REQUEST_TRSPC_DATA_SEND:
        if ((BLE_TRSP_WRITE_TYPE != BLEGATT_WRITE) && (BLE_TRSP_WRITE_TYPE != BLEGATT_WRITE_WITHOUT_RSP))
        {
            printf("Error write type. \n");
        }
        else
        {
            status = ble_svcs_trsps_client_send(host_id,
                                                BLE_TRSP_WRITE_TYPE,
                                                p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01,
                                                &g_rx_buffer[g_uart_transmit_index][0],
                                                g_rx_buffer_length[g_uart_transmit_index]);

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
                if (app_request_set(host_id, APP_REQUEST_TRSPC_DATA_SEND, false) == false)
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

static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_SCAN_EVT_SET_ENABLE:
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)&p_param->event_param.ble_evt_scan.param.evt_set_scan_enable;

        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_scan_enable->scan_enabled == true)
            {
                ble_app_link_info[APP_TRSP_C_HOST_ID].state = STATE_SCANNING;
                printf("Scanning...\n");
            }
            else
            {
                if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_INITIATING)
                {
                    if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_CREATE_CONN, false) == false)
                    {
                        // No application queue buffer. Error.
                    }
                }
                else
                {
                    ble_app_link_info[APP_TRSP_C_HOST_ID].state = STATE_STANDBY;
                }
                printf("Idle.\n");
            }
        }
        else
        {
            printf("Scan enable failed.\n");
        }
    }
    break;

    case BLE_SCAN_EVT_ADV_REPORT:
    {
        ble_evt_scan_adv_report_t *p_scan_param = (ble_evt_scan_adv_report_t *)&p_param->event_param.ble_evt_scan.param.evt_adv_report;
        uint8_t name[31];
        uint8_t length;

        // show device name, address and RSSI value
        if ((ble_cmd_scan_report_adv_data_parsing(p_scan_param, GAP_AD_TYPE_LOCAL_NAME_COMPLETE, name, &length) == BLE_ERR_OK) ||
                (ble_cmd_scan_report_adv_data_parsing(p_scan_param, GAP_AD_TYPE_LOCAL_NAME_SHORTENED, name, &length) == BLE_ERR_OK))
        {
            name[length] = '\0';
            printf("Found [Name:%s] [Address: %02x:%02x:%02x:%02x:%02x:%02x] [RSSI= %d]\n",
                       name,
                       p_scan_param->peer_addr.addr[5], p_scan_param->peer_addr.addr[4],
                       p_scan_param->peer_addr.addr[3], p_scan_param->peer_addr.addr[2],
                       p_scan_param->peer_addr.addr[1], p_scan_param->peer_addr.addr[0],
                       p_scan_param->rssi);

            if (((length == strlen((char *)TARGET_DEVICE_NAME)) && (strncmp((char *)name, (char *)TARGET_DEVICE_NAME, length) == 0)))
            {
                // set peer address type
                g_target_addr.addr_type = p_scan_param->peer_addr.addr_type;
                memcpy(g_target_addr.addr, &p_scan_param->peer_addr.addr, BLE_ADDR_LEN);

                // update state
                ble_app_link_info[APP_TRSP_C_HOST_ID].state = STATE_INITIATING;

                // stop scan and go to connect with this peer address.
                if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_SCAN_STOP, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
        }
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

            // re-start scan
            if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_SCAN_START, false) == false)
            {
                // No application queue buffer. Error.
            }

            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE:
    {
        ble_err_t status;
        ble_evt_att_db_parse_complete_t *p_parsing_param = (ble_evt_att_db_parse_complete_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_att_db_parse_complete;

        if (p_parsing_param->host_id == APP_TRSP_C_HOST_ID)
        {
            printf("DB Parsing completed, ID:%d status:0x%02x\n", p_parsing_param->host_id, p_parsing_param->result);

            // Get LINK 0 all service handles and related information
            status = link0_svcs_handles_get(p_parsing_param->host_id);

            if (status != BLE_ERR_OK)
            {
                printf("Get handle fail, ID:%d status:%d\n", p_parsing_param->host_id, status);
            }

            // Do GATT commands
            if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_TRSPC_MULTI_CMD, false) == false)
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
        if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_TRSPC_DATA_SEND, true) == false)
        {
            // No application queue buffer. Error.
        }
    }
}

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
static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_param_t));
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_GENERAL_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t));

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

    //Initial Parameters
    g_uart_index = 0;
    g_uart_transmit_index = 0;

    // scan
    if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_SCAN_START, false) == false)
    {
        // No application queue buffer. Error.
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

static void ble_app_main(app_req_param_t *p_param)
{
    // Link 0 (Central)
    app_central_handler(p_param);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
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

    lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_UART_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data)
{
    p_data->udatni01_cccd = 0;
}

static ble_err_t client_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

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

        status = ble_cmd_lesc_init();
        if (status != BLE_ERR_OK)
        {
            break;
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
        status = client_profile_init(APP_TRSP_C_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

    } while (0);

    return status;
}

static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;

    // banner
    printf("------------------------------------------\n");
    printf("  BLE TRSP (C) demo: start...\n");
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
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
int main(void)
{
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);

    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}
