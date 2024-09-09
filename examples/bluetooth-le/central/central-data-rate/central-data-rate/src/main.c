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
#include "timer.h"
#include "ctrl_cmd.h"
#include "hosal_uart.h"

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

// Scan parameters
#define APP_SCAN_WINDOW                 50U       //50*0.625ms=31.25ms
#define APP_SCAN_INTERVAL               60U       //60*0.625ms=37.5ms

// Connection parameters
#define APP_CONN_INTERVAL_MIN           40U       //40*1.25ms=50ms
#define APP_CONN_INTERVAL_MAX           40U       //40*1.25ms=50ms
#define APP_CONN_LATENCY                0
#define APP_CONN_TIMEOUT                600U      //600*10ms=6000ms=6s

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX

// BLE application uses Write or Write Without Response
#define BLE_TRSP_WRITE_TYPE             BLEGATT_WRITE_WITHOUT_RSP  // BLEGATT_WRITE; BLEGATT_WRITE_WITHOUT_RSP

// Default PHY
#define APP_PHY                         BLE_PHY_1M

// Default data length
#define APP_DATA_LEN                    20

// Default TX test total length
#define DATARATE_TEST_LENGTH            1048712

// Data rate cmd identification
#define SET_PARAM_STR                   "set_param"         // Receive from central device, set data rate parameters for the test
#define PRX_TEST_STR                    "pRxtest"           // Receive from central device, test C->P data rate
#define PTX_TEST_STR                    "pTxtest"           // Receive from central device, test P->C data rate
#define CANCEL_TEST_STR                 "canceltest"  // Receive from central device, cancel the test case

#define APP_TIMER_MS_TO_TICK(ms)        ((ms)*(4000))       // the input clock is 32M/s, so it will become 4M ticks per second      

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x41, 0x42, 0x43, 0x44, 0x45, 0xC6 }
                                           };

#define UART0_OPERATION_PORT            0
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, 16, 17, UART_BAUDRATE_115200)
/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static uint32_t g_time_ms = 0;
static uint8_t g_uart_buffer[250];
static uint8_t g_uart_length;

ble_gap_addr_t g_target_addr  = {.addr_type = RANDOM_ADDR,
                                 .addr = {0x71, 0x72, 0x73, 0x74, 0x75, 0xC6}
                                };

uint8_t txDataBuffer[247];                 // Transmit data buffer

uint32_t                receivedTotalDataLength = 0;                // Current received data length
uint32_t                transmittedTotalDataLength = 0;             // Current transmitted data length
uint32_t                print_percentage_base;
link0_connect_info      connect_info;
app_dataRateTest_info   app_DR_test_info;

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static ble_err_t ble_init(void);
static void ble_app_main(app_req_param_t *p_param);
static void dataRate_Test_Param_Init(void);
static void daraRate_Test_PercentageBase_Set(void);
static void RunTxTest(uint8_t host_id);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
static bool uart_data_handler(char ch)
{
    bool status = false;
    static uint8_t rx_buffer[250];
    static uint8_t index = 0;

    rx_buffer[index++] = ch;

    if ((ch == '\n') || (ch == '\r'))
    {
        rx_buffer[index] = '\0';
        memcpy(g_uart_buffer, rx_buffer, index);
        g_uart_length = index;
        if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_PROCESS_UART_CMD, true) == false)
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

static void timer_count_update(void)
{
    g_time_ms++;
}


static void timer_handler(uint32_t timer_id)
{
    if (timer_id == APP_HW_TIMER_ID)
    {
        timer_count_update();
    }
}

/* timer init*/
static void hw_timer0_init(void)
{
    timer_config_mode_t cfg;

    cfg.int_en = ENABLE;
    cfg.mode = TIMER_PERIODIC_MODE;
    /*the input clock is 32M/s, so it will become 4M ticks per second */
    cfg.prescale = TIMER_PRESCALE_8;

    timer_open(APP_HW_TIMER_ID, cfg, timer_handler);
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
            if (app_DR_test_info.test_mode == DATARATE_MODE_RX)
            {
                if (app_DR_test_info.test_state == DATARATE_STATE_WAITING)
                {
                    app_DR_test_info.test_state = DATARATE_STATE_SETTING;
                    if (app_DR_test_info.test_mode == DATARATE_MODE_RX)
                    {
                        // start timer
                        timer_start(APP_HW_TIMER_ID, APP_TIMER_MS_TO_TICK(1));
                        g_time_ms = 0;
                    }
                    if (app_request_set(p_param->host_id, APP_DATA_RATE_TEST_RUN, false) == false)
                    {
                        // No application queue buffer. Error.
                    }
                }
                else if (app_DR_test_info.test_state == DATARATE_STATE_RUNNING)
                {
                    receivedTotalDataLength += p_param->length;

                    // show received data count
                    if (receivedTotalDataLength % print_percentage_base == 0)
                    {
                        printf("Rx:%ld\n", receivedTotalDataLength);
                    }

                    // RX test done
                    if (receivedTotalDataLength >= app_DR_test_info.total_Tx_test_len)
                    {
                        double throughput;

                        // stop timer
                        timer_stop(APP_HW_TIMER_ID);

                        printf("Stop RX\n");

                        receivedTotalDataLength = 0;
                        app_DR_test_info.test_state = DATARATE_STATE_IDLE;

                        printf("Total Rx Received Time: %ld ms\n", g_time_ms);
                        printf("Total Rx Received %ld Bytes\n", app_DR_test_info.total_Tx_test_len);
                        throughput = (double)(app_DR_test_info.total_Tx_test_len << 3) / (double)g_time_ms;
                        printf("Rx Through: %.3f bps\n",  throughput * 1000);
                    }
                }
            }
            break;

        case BLESERVICE_TRSPS_UDATRW01_WRITE_RSP_EVENT:
        {
            app_DR_test_info.test_state = DATARATE_STATE_RUNNING;
            if (app_request_set(p_param->host_id, APP_DATA_RATE_TEST_RUN, false) == false)
            {
                // No application queue buffer. Error.
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

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_CREATE_CONN:
    {
        if (ble_app_link_info[host_id].state == STATE_STANDBY)
        {
            ble_gap_create_conn_param_t create_conn_param;

            //------------------------------------------------------------------------
            // init LINK0 GAP/ DIS/ UDF01S services parameter and register callback function
            //------------------------------------------------------------------------
            connect_info.conn_interval = APP_CONN_INTERVAL_MAX;
            connect_info.data_length = APP_DATA_LEN;
            connect_info.phy = APP_PHY;

            svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.client_info.data);
            svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.client_info.data);
            ble_cmd_device_addr_get(&addr_param);

            create_conn_param.host_id = APP_TRSP_C_HOST_ID;
            create_conn_param.own_addr_type = addr_param.addr_type;
            create_conn_param.scan_interval = APP_SCAN_INTERVAL;
            create_conn_param.scan_window = APP_SCAN_WINDOW;
            create_conn_param.init_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED;
            create_conn_param.peer_addr.addr_type = RANDOM_STATIC_ADDR;
            memcpy(create_conn_param.peer_addr.addr, g_target_addr.addr, 6);
            create_conn_param.conn_param.min_conn_interval = APP_CONN_INTERVAL_MIN;
            create_conn_param.conn_param.max_conn_interval = APP_CONN_INTERVAL_MAX;
            create_conn_param.conn_param.periph_latency = APP_CONN_LATENCY;
            create_conn_param.conn_param.supv_timeout = APP_CONN_TIMEOUT;
            status = ble_cmd_conn_create(&create_conn_param);

            if (status == BLE_ERR_OK)
            {
                printf("Connecting...");
                ble_app_link_info[host_id].state = STATE_INITIATING;
            }
            else
            {
                if (app_request_set(host_id, APP_REQUEST_CREATE_CONN, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
        }
    }
    break;

    case APP_REQUEST_CREATE_CONN_CANCEL:
        if (ble_app_link_info[host_id].state == STATE_INITIATING)
        {
            status = ble_cmd_conn_create_cancel();
            if (status != BLE_ERR_OK)
            {
                if (app_request_set(host_id, APP_REQUEST_CREATE_CONN_CANCEL, false) == false)
                {
                    // No application queue buffer. Error.
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

    case APP_REQUEST_PROCESS_UART_CMD:
        handle_ctrl_cmd(g_uart_buffer, g_uart_length);
        break;

    case APP_DATA_RATE_TEST_RUN:
        if (app_DR_test_info.test_state == DATARATE_STATE_IDLE)
        {
            transmittedTotalDataLength = 0;
            receivedTotalDataLength = 0;
            // stop timer
            timer_stop(APP_HW_TIMER_ID);
        }
        else if (app_DR_test_info.test_state == DATARATE_STATE_SETTING)
        {
            if (app_DR_test_info.test_mode == DATARATE_MODE_RX)
            {
                char string_data[17] = PTX_TEST_STR;

                // send test identifier to peripheral device
                sprintf(&string_data[strlen(PTX_TEST_STR)], "%ld", app_DR_test_info.total_Tx_test_len);

                if (p_profile_info->svcs_info_trsps.client_info.data.udatni01_cccd == 0)
                {
                    if (BLE_ERR_OK == ble_svcs_cccd_set(host_id, p_profile_info->svcs_info_trsps.client_info.data.udatni01_cccd, BLEGATT_CCCD_NOTIFICATION))
                    {
                        p_profile_info->svcs_info_trsps.client_info.data.udatni01_cccd = BLEGATT_CCCD_NOTIFICATION;
                    }
                }

                if (BLE_ERR_OK == ble_svcs_trsps_client_send(host_id, BLEGATT_WRITE, p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01, (uint8_t *) &string_data, strlen(string_data)))
                {
                    app_DR_test_info.test_state = DATARATE_STATE_WAITING;
                    printf("Test Total Rx Length = %ld\n", app_DR_test_info.total_Tx_test_len);
                }
            }
        }
        else if (app_DR_test_info.test_state == DATARATE_STATE_RUN)
        {
            if (app_DR_test_info.test_mode == DATARATE_MODE_RX)
            {
                char param_string_data[100] = SET_PARAM_STR;

                // set percentage base value
                daraRate_Test_PercentageBase_Set();

                // set test parameters by sending "SET_PARAM_STR" to slave
                sprintf(&param_string_data[strlen(SET_PARAM_STR)], "=%d,%d,%d,%d", connect_info.phy, connect_info.data_length, connect_info.conn_interval, connect_info.conn_interval);

                param_string_data[strlen(param_string_data)] = 0;
                printf("Data:%s\n", param_string_data);

                if (BLE_ERR_OK == ble_svcs_trsps_client_send(host_id, BLEGATT_WRITE_WITHOUT_RSP, p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01, (uint8_t *) &param_string_data, strlen(param_string_data)))
                {
                    app_DR_test_info.test_state = DATARATE_STATE_WAITING;
                }
            }
            else if (app_DR_test_info.test_mode == DATARATE_MODE_TX)
            {
                char string_data[17] = PRX_TEST_STR;

                // set percentage base value
                daraRate_Test_PercentageBase_Set();

                // send test identifier to peripheral device
                sprintf(&string_data[strlen(PRX_TEST_STR)], "%ld", app_DR_test_info.total_Tx_test_len);

                if (BLE_ERR_OK == ble_svcs_trsps_client_send(host_id, BLEGATT_WRITE, p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01, (uint8_t *) &string_data, strlen(string_data)))
                {
                    app_DR_test_info.test_state = DATARATE_STATE_WAITING;
                    printf("Test Total Tx Length = %ld\n", app_DR_test_info.total_Tx_test_len);
                }
            }
        }
        else // RUNNING
        {
            if (app_DR_test_info.test_mode == DATARATE_MODE_TX)
            {
                // do TX test
                RunTxTest(host_id);
                if (app_request_set(host_id, APP_DATA_RATE_TEST_RUN, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
        }
        break;

    case APP_DATA_RATE_TEST_STOP_RUN:
    {
        char string_data[11] = CANCEL_TEST_STR;

        status = ble_svcs_trsps_client_send(host_id, BLEGATT_WRITE_WITHOUT_RSP, p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01, (uint8_t *) &string_data, strlen(string_data));
        if (BLE_ERR_OK == status)
        {
            printf("Cancel Test\n");
        }
        else
        {
            app_request_set(host_id, APP_DATA_RATE_TEST_STOP_RUN, false);
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
    case BLE_GAP_EVT_CONN_CANCEL:
        ble_app_link_info[APP_TRSP_C_HOST_ID].state = STATE_STANDBY;
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

        printf("MTU Exchanged, ID:%d, size: %d\n", p_mtu_param->host_id, p_mtu_param->mtu);
    }
    break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
    {
        ble_evt_data_length_change_t *p_data_len_param = (ble_evt_data_length_change_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change;
        printf("Data length changed, ID: %d\n", p_data_len_param->host_id);
        printf("MaxTxOctets: %d  MaxTxTime:%d\n", p_data_len_param->max_tx_octets, p_data_len_param->max_tx_time);
        printf("MaxRxOctets: %d  MaxRxTime:%d\n", p_data_len_param->max_rx_octets, p_data_len_param->max_rx_time);

        connect_info.data_length = (p_data_len_param->max_tx_octets - 7);

        print_app_data_len_param();
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

    case BLE_GAP_EVT_READ_CHANNEL_MAP:
    {
        ble_evt_gap_ch_map_read_t *p_ch_map_param = (ble_evt_gap_ch_map_read_t *)&p_param->event_param.ble_evt_gap.param.evt_channel_map;

        printf("Status = %d, Channel Map:%02x:%02x:%02x:%02x:%02x \n", p_ch_map_param->status,
                   p_ch_map_param->channel_map[0],
                   p_ch_map_param->channel_map[1],
                   p_ch_map_param->channel_map[2],
                   p_ch_map_param->channel_map[3],
                   p_ch_map_param->channel_map[4]);
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
static void RunTxTest(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    uint32_t dataLen = connect_info.data_length;
    uint16_t hdlNum;
    ble_info_link0_t *p_profile_info;

    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    if ((transmittedTotalDataLength + dataLen) > app_DR_test_info.total_Tx_test_len)
    {
        dataLen = app_DR_test_info.total_Tx_test_len - transmittedTotalDataLength;
    }

    // Send out data by RF
    hdlNum = p_profile_info->svcs_info_trsps.client_info.handles.hdl_udatrw01;
    status = ble_svcs_trsps_client_send(host_id, BLEGATT_WRITE_WITHOUT_RSP, hdlNum, txDataBuffer, dataLen);

    if (status == BLE_ERR_OK)
    {
        transmittedTotalDataLength += dataLen;

        if (transmittedTotalDataLength >= app_DR_test_info.total_Tx_test_len)
        {
            printf("Stop TX\n");
            app_DR_test_info.test_state = DATARATE_STATE_IDLE;
            return;
        }

        if (transmittedTotalDataLength % print_percentage_base == 0)
        {
            printf("Tx:%ld\n", transmittedTotalDataLength);
        }
    }
    else if (status == BLE_ERR_INVALID_PARAMETER)
    {
        printf("Packet Data Length > MTU Size\n");
    }

    if (app_request_set(host_id, APP_DATA_RATE_TEST_RUN, false) == false)
    {
        // No application queue buffer. Error.
    }
}

static void daraRate_Test_PercentageBase_Set(void)
{
    // 5%
    uint32_t temp_val;

    if (connect_info.data_length != 0)
    {
        temp_val = (uint16_t)(app_DR_test_info.total_Tx_test_len / connect_info.data_length) * 0.05;

        print_percentage_base = temp_val * connect_info.data_length;

        if (print_percentage_base == 0)
        {
            print_percentage_base = connect_info.data_length;
        }
        printf("print_percentage_base=%ld\n", print_percentage_base);
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

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

static void dataRate_Test_Param_Init(void)
{
    // init
    app_DR_test_info.total_Tx_test_len = DATARATE_TEST_LENGTH;
    app_DR_test_info.test_state = DATARATE_STATE_IDLE;
    app_DR_test_info.test_mode = DATARATE_MODE_TX;
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
        //------------------------------------------------------------------------
        // init LINK0 GAP/ DIS/ UDF01S services parameter and register callback function
        //------------------------------------------------------------------------
        connect_info.conn_interval = APP_CONN_INTERVAL_MAX;
        connect_info.data_length = APP_DATA_LEN;
        connect_info.phy = APP_PHY;

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

        /* init data rate application parameters.  */
        dataRate_Test_Param_Init();

        print_ctrl_cmd_help();

    } while (0);

    return status;
}

static void app_init(void)
{
    ble_task_priority_t ble_task_level;

    // banner
    printf("------------------------------------------\n");
    printf("  BLE demo: DataRate (Central) start...\n");
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

    app_uart_init();
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
int main(void)
{
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);

    /* HW timer0 init */
    hw_timer0_init();

    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}
