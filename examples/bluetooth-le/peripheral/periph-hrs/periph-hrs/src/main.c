/**************************************************************************************************
 * @file main.c
 * @brief Main application code for the BLE HRS (P) demo.
 * @version 1.0
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * @details This file includes the necessary includes, macros, constants, and function declarations
 *          for the BLE HRS (P) demo. It also contains the implementation of the main application
 *          task, BLE event handlers, and other local functions.
 *************************************************************************************************/

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
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "app_hooks.h"
#include "uart_stdio.h"
#include "dump_boot_info.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
// uint16 convert to uint8 high byte and low byte
#define U16_HIGHBYTE(x)                 (uint8_t)((x >> 8) & 0xFF)
#define U16_LOWBYTE(x)                  (uint8_t)(x & 0xFF)

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define BLE_APP_CB_QUEUE_SIZE           32
#define APP_ISR_QUEUE_SIZE              2
#define APP_REQ_QUEUE_SIZE              6
#define APP_QUEUE_SIZE                  (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE)

#define APP_HRS_P_HOST_ID               0         // HRS: Peripheral

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MIN

// Advertising device name
#define DEVICE_NAME                     "HRS_DEMO"

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms

// GAP device name
static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x11, 0x12, 0x13, 0x14, 0x15, 0xC6 }
                                           };

#define GPIO_WAKE_UP_PIN                0
/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

static TimerHandle_t  g_hrs_timer;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_hrs_data_init(ble_svcs_hrs_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool hrs_sw_timer_start(void);
static bool hrs_sw_timer_stop(void);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/**
 * @brief GPIO interrupt handler.
 *
 * This function is called when a GPIO interrupt occurs. It masks the low power mode.
 *
 * @param pin The GPIO pin number that triggered the interrupt.
 * @param isr_param Additional parameters for the ISR (Interrupt Service Routine).
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

/**
 * @brief HRS timer handler.
 *
 * This function is called when the HRS timer expires. It sends a notification to the connected client.
 *
 * @param timer The timer handle.
 * @return void This function does not return a value.
 */
static void hrs_timer_handler(TimerHandle_t timer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( timer );

    // HRS
    if (ble_app_link_info[APP_HRS_P_HOST_ID].state == STATE_CONNECTED)
    {
        // send HRS data
        if (ble_app_request_set(APP_HRS_P_HOST_ID, APP_REQUEST_HRS_NTF, false) == false)
        {
            //No Application queue buffer. Error.
        }
    }
}

/**
 * @brief BLE HRS event handler.
 *
 * This function handles events related to the Heart Rate Service (HRS). It processes
 * client requests and manages notifications for heart rate measurements.
 *
 * @param p_param Pointer to the BLE event parameters.
 * @return void This function does not return a value.
 */
static void ble_svcs_hrs_evt_handler(ble_evt_att_param_t *p_param)
{
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[p_param->host_id].profile_info;

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_HRS_HEART_RATE_MEASUREMENT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HRS timer to send notification
                if (hrs_sw_timer_start() == false)
                {
                    printf("HRS timer start failed. \n");
                }
            }
            else
            {
                // stop HRS timer
                if (hrs_sw_timer_stop() == false)
                {
                    printf("HRS timer stop failed. \n");
                }
            }
            break;

        case BLESERVICE_HRS_BODY_SENSOR_LOCATION_READ_EVENT:
        {
            ble_err_t status;
            ble_gatt_data_param_t gatt_data_param;

            gatt_data_param.host_id = p_param->host_id;
            gatt_data_param.handle_num = p_param->handle_num;
            gatt_data_param.length = 1;
            gatt_data_param.p_data = &p_profile_info->svcs_info_hrs.server_info.data.body_sensor_location;

            status = ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &gatt_data_param);
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
 * @brief Handles peripheral role events.
 *
 * This function processes events related to the peripheral role, such as connection and disconnection events.
 *
 * @param p_param Pointer to the event parameters.
 *                - host_id: The ID of the host that triggered the event.
 *                - app_req: The type of request (e.g., start advertising, send data).
 *                - other fields: Additional fields related to the event.
 *
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
        do
        {
            // service data init
            svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
            svcs_hrs_data_init(&p_profile_info->svcs_info_hrs.server_info.data);

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

    case APP_REQUEST_HRS_NTF:
    {
        ble_gatt_data_param_t gatt_param;

        // send heart rate measurement value to client
        if ((p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[0] & BIT1) == 0)    //initial is 0x14 & 0x02 = 0, toggle "device detected" / "device not detected" information
        {
            p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[0] |= BIT1;          //set Sensor Contact Status bit
        }
        else
        {
            p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[0] &= ~BIT1;         //clear Sensor Contact Status bit
        }

        // set parameters
        gatt_param.host_id = host_id;
        gatt_param.handle_num = p_profile_info->svcs_info_hrs.server_info.handles.hdl_heart_rate_measurement;
        gatt_param.length = sizeof(p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement) / sizeof(p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[0]);
        gatt_param.p_data = p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement;

        // send notification
        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
        if (status == BLE_ERR_OK)
        {
            p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[1]++;  //+1, Heart Rate Data. Here just a simulation, increase 1 about every second
            p_profile_info->svcs_info_hrs.server_info.data.heart_rate_measurement[2]++;  //+1, Heart Rate RR-Interval
        }
    }
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
 * @brief BLE event handler.
 *
 * This function handles various BLE events such as advertising, connection, and data length changes.
 *
 * @param p_param Pointer to the BLE event parameters.
 * @return void This function does not return a value.
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
 * @brief Starts the HRS software timer.
 *
 * This function starts the Heart Rate Service (HRS) software timer if it is not already active.
 * The timer is used to periodically send heart rate measurements to the connected client.
 *
 * @return true if the timer was successfully started or is already active, false otherwise.
 */
static bool hrs_sw_timer_start(void)
{
    if ( g_hrs_timer != NULL && xTimerIsTimerActive( g_hrs_timer ) == pdFALSE )
    {
        if ( xTimerStart( g_hrs_timer, 0 ) != pdTRUE )
        {
            // The timer could not be set into the Active state.
            return false;
        }
    }
    return true;
}

/**
 * @brief Stops the HRS software timer.
 *
 * This function stops the Heart Rate Service (HRS) software timer if it is currently active.
 * The timer is used to periodically send heart rate measurements to the connected client.
 *
 * @return true if the timer was successfully stopped or is already inactive, false otherwise.
 */
static bool hrs_sw_timer_stop(void)
{
    if ( xTimerIsTimerActive( g_hrs_timer ) != pdFALSE )
    {
        if ( xTimerStop( g_hrs_timer, 0 ) != pdTRUE )
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Initializes the advertising parameters and data.
 *
 * This function sets up the advertising parameters and data for BLE advertising.
 * It configures the advertising type, interval, channel map, and filter policy.
 * It also sets the advertising data and scan response data.
 *
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
 */
static ble_err_t adv_init(void)
{
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    ble_gap_addr_t addr_param;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + strlen(DEVICE_NAME_STR); //  1 byte data type

    //Adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
        0x03, GAP_AD_TYPE_SERVICE_MORE_16B_UUID, U16_LOWBYTE(GATT_SERVICES_HEART_RATE), U16_HIGHBYTE(GATT_SERVICES_HEART_RATE),
        0x03, GAP_AD_TYPE_APPEARANCE, U16_LOWBYTE(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR), U16_HIGHBYTE(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR),
    };

    //Scan response data
    uint8_t adv_scan_rsp_data[2 + strlen(DEVICE_NAME_STR)];
    adv_scan_rsp_data[0] = SCANRSP_ADLENGTH;                      // AD length
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;       // AD data type
    memcpy(&adv_scan_rsp_data[2], DEVICE_NAME_STR, strlen(DEVICE_NAME_STR)); // Copy the name

    do
    {
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
        memcpy(adv_data_param.data, adv_data, adv_data_param.length);
        status = ble_cmd_adv_data_set(&adv_data_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_data_set() status = %d\n", status);
            break;
        }

        // set scan rsp data
        adv_scan_data_param.length = sizeof(adv_scan_rsp_data);
        memcpy(adv_scan_data_param.data, adv_scan_rsp_data, adv_scan_data_param.length);
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
 * This function enables BLE advertising for the given host ID.
 *
 * @param host_id The ID of the host for which advertising is to be enabled.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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
 * @brief Sets a BLE application request.
 *
 * This function sets a request for the BLE application, either from an ISR or a normal context.
 * It prepares a queue item with the request details and sends it to the application message queue.
 * If the request is from an ISR, it uses the appropriate FreeRTOS ISR-safe functions.
 *
 * @param host_id The ID of the BLE host.
 * @param request The application request to be set.
 * @param from_isr Indicates if the request is from an ISR (true) or not (false).
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
 * This function handles BLE service data events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
 */
static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
    {
        ble_evt_att_param_t *p_evt_att = p_param;
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);

        if (p_tlv != NULL)
        {
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
 * @brief Callback function for BLE L2CAP data events.
 *
 * This function handles L2CAP data events and queues them for processing.
 *
 * @param p_param Pointer to the L2CAP event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
 */
static ble_err_t ble_l2cap_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_l2cap_evt_param_t *p_evt_l2cap;

    status = BLE_ERR_OK;
    if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
    {
        p_evt_l2cap = p_param;
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);
        if (p_tlv != NULL)
        {
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
 * @brief Main task for the BLE application.
 *
 * This function initializes the BLE stack and profiles, and processes BLE events.
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

    // start adv
    if (ble_app_request_set(APP_HRS_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
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
 * @brief Initializes the GATT service data.
 *
 * This function initializes the GATT service data by setting the service changed CCCD to 0.
 *
 * @param p_data Pointer to the GATT service data structure to be initialized.
 * @return void This function does not return a value.
 */
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

/**
 * @brief Initializes the HRS service data.
 *
 * This function sets the initial values for the Heart Rate Service (HRS) data, including setting the CCCD to 0.
 *
 * @param p_data Pointer to the HRS service data structure to be initialized.
 * @return void This function does not return a value.
 */
static void svcs_hrs_data_init(ble_svcs_hrs_data_t *p_data)
{
    p_data->body_sensor_location = 0x02;
    p_data->heart_rate_measurement_cccd = 0;

    //[0]: HRS Flag; [1]: Heart Rate Data [2][3]: Heart Rate RR-Interval
    p_data->heart_rate_measurement[0] = 0x14; // HRS Flag
    p_data->heart_rate_measurement[1] = 0;    // Heart Rate Data
    p_data->heart_rate_measurement[2] = 0;    // Heart Rate RR-Interval
    p_data->heart_rate_measurement[3] = 0;    // Heart Rate RR-Interval
}

/**
 * @brief Initializes the BLE profile for the server role.
 *
 * This function initializes the BLE profile for the server role, including the GAP, GATT, DIS, and HRS services.
 *
 * @param host_id The ID of the host for which the profile is to be initialized.
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
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

        // HRS Related
        // -------------------------------------
        status = ble_svcs_hrs_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_hrs), ble_svcs_hrs_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Initializes the BLE stack and sets up callbacks.
 *
 * This function initializes the BLE stack, sets up necessary callbacks, and configures the device address.
 * It also initializes the PHY controller and sets the suggested data length.
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
        status = server_profile_init(APP_HRS_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Initializes the application.
 *
 * This function sets up the application by initializing the BLE stack and GPIO.
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
    printf("  BLE HRS (P) demo: start...\n");
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

    // application SW timer, tick = 1s
    g_hrs_timer = xTimerCreate("HRS_Timer", pdMS_TO_TICKS(1000), pdTRUE, ( void * ) 0, hrs_timer_handler);

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
