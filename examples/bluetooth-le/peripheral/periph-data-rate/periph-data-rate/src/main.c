/** @file
 *
 * @brief BLE HOGP peripheral role demo.
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

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define CHECK_STR(data,target_str)      (strncmp((char *)(data), (target_str), sizeof(target_str) - 1) == 0)
#define APP_TIMER_MS_TO_TICK(ms)        ((ms)*(4000))  // the input clock is 32M/s, so it will become 4M ticks per second

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define BLE_APP_CB_QUEUE_SIZE           32
#define APP_ISR_QUEUE_SIZE              2
#define APP_REQ_QUEUE_SIZE              6
#define APP_QUEUE_SIZE                  (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE)

#define APP_DATA_RATE_P_HOST_ID         0

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX

// Advertising device name
#define DEVICE_NAME                     'D', 'a', 't', 'a', '_', 'R', 'a', 't', 'e'

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms

// GAP device name
static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x71, 0x72, 0x73, 0x74, 0x75, 0xC6 }
                                           };

// Data rate test total length
#define DATARATE_TEST_LENGTH            1048712

// Data rate cmd identification
#define SET_PARAM_STR                   "set_param"         // Receive from central device, set data rate parameters for the test
#define GET_PARAM_STR                   "get_param"         // Receive from central device, send current data rate parameters for the central device
#define PRX_TEST_STR                    "pRxtest"           // Receive from central device, test C->P data rate
#define PTX_TEST_STR                    "pTxtest"           // Receive from central device, test P->C data rate
#define CANCEL_TEST_STR                 "canceltest"        // Receive from central device, cancel the test case
#define SET_LATENCY_STR                 "connLatencytest0"  // Receive from central device, set connection latency 0 for the test
// Data rate parameter set status identifier
#define STATUS_SET_PARAM_ID             0

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;

static uint8_t g_test_buffer[DEFAULT_MTU];              // TX test data buffer
static request_temp_param_t g_temp_test_param;          // temp test parameters from client request
static app_test_param_t g_curr_test_param;              // record the current test parameters

static uint32_t g_test_length = DATARATE_TEST_LENGTH;   // test total length
static uint32_t g_curr_rx_length = 0;                   // current received length
static uint32_t g_curr_tx_lenght = 0;                   // current transmitted length
static uint32_t g_time_ms = 0;                          // timer count

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static void trsps_read_handler(uint8_t host_id, uint16_t handle_num);
static void trsps_write_cmd_handler(uint8_t host_id, uint8_t length, uint8_t *data);
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
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

static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
        case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
            // process test command
            trsps_write_cmd_handler(p_param->host_id, p_param->length, p_param->data);
            break;

        case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
            // send read rsp with current test parameters
            trsps_read_handler(p_param->host_id, p_param->handle_num);
            break;

        default:
            break;
        }
    }
}

static void trsps_read_handler(uint8_t host_id, uint16_t handle_num)
{
    ble_err_t status;
    char data[100];
    int len;
    ble_gatt_data_param_t gatt_data_param;

    len = sprintf((char *)data, "%d,%d,%d,%d,%d,%d",
                  g_curr_test_param.phy,
                  g_curr_test_param.packet_data_len,
                  g_curr_test_param.mtu_size,
                  g_curr_test_param.conn_interval,
                  g_curr_test_param.conn_latency,
                  g_curr_test_param.conn_supervision_timeout);

    gatt_data_param.host_id = host_id;
    gatt_data_param.handle_num = handle_num;
    gatt_data_param.length = len;
    memcpy(gatt_data_param.p_data, (uint8_t *)data, len);

    status = ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &gatt_data_param);
    if (status != BLE_ERR_OK)
    {
        printf("ble_gatt_read_rsp status: %d\n", status);
    }
}

static void trsps_write_cmd_handler(uint8_t host_id, uint8_t length, uint8_t *data)
{
    uint32_t dataLen;

    if (CHECK_STR(data, CANCEL_TEST_STR))
    {
        // cancel the test
        ble_app_link_info[host_id].state = STATE_TEST_STANDBY;

        // stop timer
        timer_stop(APP_HW_TIMER_ID);

        printf("Cancel Test.\n");
    }

    if (ble_app_link_info[host_id].state == STATE_TEST_STANDBY)
    {
        if (CHECK_STR(data, PRX_TEST_STR))
        {
            // start device RX test
            ble_app_link_info[host_id].state = STATE_TEST_RXING;

            // init parameters
            g_time_ms = 0;
            g_curr_rx_length = 0;

            // start timer
            timer_start(APP_HW_TIMER_ID, APP_TIMER_MS_TO_TICK(1));

            // total test length follows the test string
            data[length] = 0;
            sscanf((char *)(data + strlen(PRX_TEST_STR)), "%d", &dataLen);
            g_test_length = dataLen;

            printf("Test length = %d\n", g_test_length);
            printf("Start RX...\n");
        }
        else if (CHECK_STR(data, PTX_TEST_STR))
        {
            // start device TX test
            ble_app_link_info[host_id].state = STATE_TEST_TXING;

            // init parameter
            g_curr_tx_lenght = 0;

            // total test length follows the test string
            data[length] = 0;
            sscanf((char *)(data + strlen(PTX_TEST_STR)), "%d", &dataLen);
            g_test_length = dataLen;

            // set TX test request
            if (app_request_set(host_id, APP_REQUEST_TX_TEST, false) == false)
            {
                // No Application queue buffer. Error.
            }

            printf("Test length = %d\n", g_test_length);
            printf("Start TX...\n");
        }
        else if (CHECK_STR(data, SET_PARAM_STR))
        {
            int state;

            // get Data Rate Parameters
            data[length] = 0;
            state = sscanf((char *)data, "set_param=%u,%u,%u,%u",
                           (unsigned int *) & (g_temp_test_param.phy),
                           (unsigned int *) & (g_temp_test_param.packet_data_len),
                           (unsigned int *) & (g_temp_test_param.conn_interval_min),
                           (unsigned int *) & (g_temp_test_param.conn_interval_max));

            if (state == -1)
            {
                printf("the params of set_param cmd is wrong!\n");
            }
            else
            {
                // set set parameteres request
                if (app_request_set(host_id, APP_REQUEST_TEST_PARAM_SET, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
        }
        else if (CHECK_STR(data, GET_PARAM_STR))
        {
            // set get parameteres request
            if (app_request_set(host_id, APP_REQUEST_TEST_PARAM_GET, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        else if (CHECK_STR(data, SET_LATENCY_STR))
        {
            // set get parameteres request
            if (g_curr_test_param.conn_latency != 0)
            {
                if (app_request_set(host_id, APP_REQUEST_LATENCY_0_SET, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
        }
    }
    else if (ble_app_link_info[host_id].state == STATE_TEST_RXING)
    {
        g_curr_rx_length += length;

        // RX test done
        if (g_curr_rx_length >= g_test_length)
        {
            double throughput;

            timer_stop(APP_HW_TIMER_ID);

            ble_app_link_info[host_id].state = STATE_TEST_STANDBY;
            throughput = (double)(g_test_length << 3) / (double)g_time_ms;

            printf("Stop RX\n");
            printf("Total Rx Received Time: %d ms\n", g_time_ms);
            printf("Total Rx Received %d Bytes\n", g_test_length);
            printf("Rx Through: %.3f bps\n",  throughput * 1000);
        }
    }
}

// TRSPS Peripheral
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

        break;

    case APP_REQUEST_CONN_UPDATE:
    {
        ble_gap_conn_param_update_param_t conn_param;

        conn_param.host_id = host_id;
        conn_param.ble_conn_param.min_conn_interval = g_curr_test_param.conn_interval;
        conn_param.ble_conn_param.max_conn_interval = g_curr_test_param.conn_interval;
        conn_param.ble_conn_param.periph_latency = 0;
        conn_param.ble_conn_param.supv_timeout = g_curr_test_param.conn_supervision_timeout;

        status = ble_cmd_conn_param_update(&conn_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_conn_param_update status = %d\n", status);
        }
    }
    break;

    case APP_REQUEST_TEST_PARAM_SET:
    {
        ble_err_t phy_status = BLE_ERR_OK;
        ble_err_t con_status = BLE_ERR_OK;
        ble_err_t mtu_status = BLE_ERR_OK;
        ble_gap_conn_param_update_param_t conn_param;
        uint8_t notify_data[100];
        uint32_t notify_len;

        // set phy
        if (g_temp_test_param.phy != 0)
        {
            ble_gap_phy_update_param_t phy_param;

            phy_param.host_id = host_id;
            phy_param.tx_phy = g_temp_test_param.phy;
            phy_param.rx_phy = g_temp_test_param.phy;
            switch (g_temp_test_param.phy)
            {
            case 3:
                phy_param.tx_phy = BLE_PHY_CODED;
                phy_param.rx_phy = BLE_PHY_CODED;
                phy_param.coded_phy_option = BLE_CODED_PHY_S2;
                break;

            case 4:
                phy_param.tx_phy = BLE_PHY_CODED;
                phy_param.rx_phy = BLE_PHY_CODED;
                phy_param.coded_phy_option = BLE_CODED_PHY_S8;
                break;

            default:
                phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                break;
            }

            phy_status = ble_cmd_phy_update(&phy_param);
            if (phy_status != BLE_ERR_OK)
            {
                printf("ble_cmd_phy_update status = %d\n", phy_status);
            }
        }

        // set TX data packet size
        if (g_temp_test_param.packet_data_len != 0)
        {
            if (g_temp_test_param.packet_data_len <= (g_curr_test_param.mtu_size - 3))
            {
                g_curr_test_param.packet_data_len = g_temp_test_param.packet_data_len;
            }
            else
            {
                mtu_status = BLE_ERR_INVALID_PARAMETER;
            }
        }

        Delay_ms(1000);
        // conn interval

        if ((g_curr_test_param.conn_interval < g_temp_test_param.conn_interval_min) ||
                (g_curr_test_param.conn_interval > g_temp_test_param.conn_interval_max))
        {
            conn_param.host_id = host_id;
            conn_param.ble_conn_param.min_conn_interval = g_temp_test_param.conn_interval_min;
            conn_param.ble_conn_param.max_conn_interval = g_temp_test_param.conn_interval_max;
            conn_param.ble_conn_param.periph_latency = 0;
            conn_param.ble_conn_param.supv_timeout = 600;

            con_status = ble_cmd_conn_param_update(&conn_param);
            if (con_status != BLE_ERR_OK)
            {
                printf("ble_cmd_conn_param_update status = %d\n", con_status);
            }
            Delay_ms(1000);
        }
        // send notification
        notify_len = sprintf((char *)notify_data, "%d,%d,%d,%d",
                             STATUS_SET_PARAM_ID, phy_status, mtu_status, con_status);

        status = ble_svcs_trsps_server_send(host_id, BLEGATT_CCCD_NOTIFICATION, p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01, notify_data, notify_len);
        if (status != BLE_ERR_OK)
        {
            printf("ble_svcs_trsps_server_send status = %d\n", status);
        }
    }
    break;


    case APP_REQUEST_TEST_PARAM_GET:
    {
        uint8_t notify_data[100];
        uint32_t notify_len;

        notify_len = sprintf((char *)notify_data, "%x,%x,%x,%x,%x,%x",
                             g_curr_test_param.phy,
                             g_curr_test_param.mtu_size,
                             g_curr_test_param.packet_data_len,
                             g_curr_test_param.conn_interval,
                             g_curr_test_param.conn_latency,
                             g_curr_test_param.conn_supervision_timeout);

        status = ble_svcs_trsps_server_send(host_id, BLEGATT_CCCD_NOTIFICATION, p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01, notify_data, notify_len);
        if (status != BLE_ERR_OK)
        {
            printf("ble_svcs_trsps_server_send status = %d\n", status);
        }
    }
    break;

    case APP_REQUEST_TX_TEST:
    {
        uint32_t packet_len = g_curr_test_param.packet_data_len;

        if ((g_curr_tx_lenght + g_curr_test_param.packet_data_len) > g_test_length)
        {
            packet_len = g_test_length - g_curr_tx_lenght;
        }

        status = ble_svcs_trsps_server_send(host_id,
                                            BLEGATT_CCCD_NOTIFICATION,
                                            ((ble_info_link0_t *)ble_app_link_info[host_id].profile_info)->svcs_info_trsps.server_info.handles.hdl_udatni01,
                                            g_test_buffer,
                                            packet_len);

        if (status == BLE_ERR_OK)
        {
            g_curr_tx_lenght += packet_len;

            if (g_curr_tx_lenght >= g_test_length)
            {
                // end of the TX test.
                ble_app_link_info[host_id].state = STATE_TEST_STANDBY;
                printf("Stop TX\n");
                return;
            }
        }

        // issue APP_REQUEST_TX_TEST again
        if (app_request_set(host_id, APP_REQUEST_TX_TEST, false) == false)
        {
            // No application queue buffer. Error.
        }
    }
    break;

    case APP_REQUEST_LATENCY_0_SET:
    {
        ble_gap_conn_param_update_param_t conn_param;
        ble_err_t con_status = BLE_ERR_OK;

        conn_param.host_id = host_id;
        conn_param.ble_conn_param.min_conn_interval = g_curr_test_param.conn_interval;
        conn_param.ble_conn_param.max_conn_interval = g_curr_test_param.conn_interval;
        conn_param.ble_conn_param.periph_latency = 0;
        conn_param.ble_conn_param.supv_timeout = 600;

        con_status = ble_cmd_conn_param_update(&conn_param);
        if (con_status != BLE_ERR_OK)
        {
            printf("ble_cmd_conn_param_update status = %d\n", con_status);
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
    case BLE_ADV_EVT_SET_ENABLE:
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = (ble_evt_adv_set_adv_enable_t *)&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable;

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
            ble_app_link_info[p_conn_param->host_id].state = STATE_TEST_STANDBY;

            // update test parameter
            g_curr_test_param.conn_interval = p_conn_param->conn_interval;
            g_curr_test_param.conn_latency = p_conn_param->periph_latency;
            g_curr_test_param.conn_supervision_timeout = p_conn_param->supv_timeout;
            g_curr_test_param.mtu_size = BLE_GATT_ATT_MTU_MIN;
            g_curr_test_param.phy = BLE_PHY_1M;
            g_curr_test_param.packet_data_len = (g_curr_test_param.mtu_size - 3);

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
            // update test parameter
            g_curr_test_param.conn_interval = p_conn_param->conn_interval;
            g_curr_test_param.conn_latency = p_conn_param->periph_latency;
            g_curr_test_param.conn_supervision_timeout = p_conn_param->supv_timeout;

            if (g_curr_test_param.conn_latency != 0)
            {
                // set latency to 0
                if (app_request_set(p_conn_param->host_id, APP_REQUEST_CONN_UPDATE, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }

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
            g_curr_test_param.phy = p_phy_param->tx_phy;
            printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu;

        // update MTU size
        g_curr_test_param.mtu_size = ((p_mtu_param->mtu) > 247) ? 247 : p_mtu_param->mtu;
        g_curr_test_param.packet_data_len = g_curr_test_param.mtu_size - 3;

        printf("MTU Exchanged, ID:%d, size: %d\n", p_mtu_param->host_id, p_mtu_param->mtu);
    }
    break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
    {
        ble_evt_suggest_data_length_set_t *p_data_len_param = (ble_evt_suggest_data_length_set_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set;

        if (p_data_len_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Write default data length, status: %d\n", p_data_len_param->status);
        }
        else
        {
            printf("Write default data length, status: %d\n", p_data_len_param->status);
        }
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

            // re-start adv
            if (app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false) == false)
            {
                // No application queue buffer. Error.
            }

            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
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
static ble_err_t adv_init(void)
{
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    ble_gap_addr_t addr_param;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + sizeof(DEVICE_NAME_STR); //  1 byte data type

    // adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
    };

    // scan response data
    uint8_t adv_scan_rsp_data[] =
    {
        SCANRSP_ADLENGTH,                   // AD length
        GAP_AD_TYPE_LOCAL_NAME_COMPLETE,    // AD data type
        DEVICE_NAME,                        // the name is shown on scan list
    };

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

static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
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

    // start adv
    if (app_request_set(APP_DATA_RATE_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
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
    // Link - Peripheral
    app_peripheral_handler(p_param);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data)
{
    p_data->udatni01_cccd = 0;
}

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
        status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, sizeof(DEVICE_NAME_STR));
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
        status = server_profile_init(APP_DATA_RATE_P_HOST_ID);
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

    // banner
    printf("------------------------------------------\n");
    printf("  BLE data rate (P) demo: start...\n");
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

    // generate TX data for data rate test
    for (uint8_t i = 0; i < DEFAULT_MTU; i++)
    {
        g_test_buffer[i] = i;
    }
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
