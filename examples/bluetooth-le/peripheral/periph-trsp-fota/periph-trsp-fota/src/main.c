/** @file
 *
 * @brief BLE TRSP peripheral role demo.
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
#include "ble_profile.h"
#include "hosal_rf.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "hosal_uart.h"
#include "uart_stdio.h"
#include "ble_fota.h"
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

#define APP_TRSP_P_HOST_ID              0

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX
#define UART_BUFF_DEPTH                 2

// Advertising device name
#define DEVICE_NAME                     'T', 'R', 'S', 'P','_', 'F', 'O', 'T', 'A' ,'_', 'D', 'E', 'M', 'O'

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms

// GAP device name
static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x91, 0x92, 0x93, 0x94, 0x95, 0xC6 }
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
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool fota_sw_timer_start(void);
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

static void fota_timer_handler( TimerHandle_t timer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( timer );

    // fota
    if (ble_app_link_info[APP_TRSP_P_HOST_ID].state == STATE_CONNECTED)
    {
        // FOTA timer tick and check if timer is expired
        if (ble_fota_timertick() == EXPIRED)
        {
            app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_FOTA_TIMER_EXPIRY, false);
        }
    }
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
            if (app_request_set(host_id, APP_REQUEST_TRSPS_DATA_SEND, false) == false)
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

        g_trsp_mtu = p_mtu_param->mtu; // update to real mtu size

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
            ble_fota_disconnect();
            ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;

            // re-start adv
            if (app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false) == false)
            {
                // No Application queue buffer. Error.
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
        if (app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_TRSPS_DATA_SEND, true) == false)
        {
            // No Application queue buffer. Error.
        }
    }
}

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

    // Initial parameters
    g_uart_index = 0;
    g_uart_transmit_index = 0;

    // start adv
    if (app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
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

static void svcs_fota_data_init(ble_svcs_fotas_data_t *p_data)
{
    p_data->command_cccd = 0;
    p_data->data_cccd = 0;
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
