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

// Advertising device name
#define DEVICE_NAME                     'I', 'O', 'T', '_', 'T', 'E', 'S', 'T', '_', '5', '8', 'x'

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms


// GAP device name
static const uint8_t     DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x81, 0x82, 0x83, 0x84, 0x85, 0xC6 }
                                           };

#define GPIO_WAKE_UP_PIN                0
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef struct ble_information
{
    uint8_t connect_upate_state;
    uint16_t interval;
    uint16_t latency;
    uint16_t timeout;
    uint8_t phy_update_state;
    uint8_t tx_phy;
    uint8_t rx_phy;
    uint8_t sec_state;
} ble_information;

/**************************************************************************************************
 *    PUBLIC VARIABLES
 *************************************************************************************************/
uint8_t g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
ble_information device;
uint8_t app_test_case;
uint8_t hogp_test_case;
uint8_t att_HDL_HIDS_REPORT_MSI[8];
uint8_t hid_report_key_count;
uint8_t data_counter = 1;
uint8_t test_counter = 0;
uint8_t tx_array[244];
uint8_t hogp_test = 0;
uint8_t one_flag = 1;

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;
static xQueueHandle g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static void svcs_trsps_data_init(ble_svcs_trsps_data_t *p_data);
static ble_err_t ble_init(void);
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    lpm_low_power_mask(LOW_POWER_MASK_BIT_TASK_BLE_APP);
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
            app_test_case = 1;
            app_request_set(p_param->host_id, APP_REQUEST_TEST_START, false);
            break;

        case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
            break;

        default:
            break;
        }
    }
}

static void ble_svcs_hids_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT:
            break;

        case BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT:
            break;

        case BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT:
            break;

        case BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT:
            hogp_test = 1;
            app_request_set(p_param->host_id, APP_REQUEST_TEST_START, false);
            break;

        case BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_WRITE_EVENT:
            break;

        default:
            break;
        }
    }
}

// HIDS Peripheral
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status = BLE_ERR_OK;
    ble_sm_bonding_flag_param_t bond_flag;
    ble_sm_io_cap_param_t io_param;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;
    ble_gatt_data_param_t data_param;
    ble_gap_phy_update_param_t phy_param;
    ble_gap_conn_param_update_param_t connParam;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;


    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        // set preferred MTU size and data length
        g_trsp_mtu = BLE_GATT_ATT_MTU_MIN;
        status = ble_cmd_default_mtu_size_set(APP_TRSP_P_HOST_ID, BLE_GATT_ATT_MTU_MAX);
        if (status != BLE_ERR_OK)
        {
            printf("MTU/ Data Length set status = 0x%02x\n", status);
        }

        // service data init
        svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);
        svcs_trsps_data_init(&p_profile_info->svcs_info_trsps.server_info.data);

        bond_flag.bonding_flag = NO_BONDING;
        ble_cmd_bonding_flag_set(&bond_flag);
        io_param.io_caps_param = NOINPUT_NOOUTPUT;
        ble_cmd_io_capability_set(&io_param);

        device.connect_upate_state = 0;
        device.interval = 0;
        device.latency = 0;
        device.timeout = 0;
        device.phy_update_state = 0;
        device.tx_phy = 1;
        device.rx_phy = 1;
        device.sec_state = 0;
        app_test_case = 0;
        hogp_test_case = 0;
        hogp_test = 0;
        one_flag = 1;
        // enable advertising

        adv_init();
        status = adv_enable(APP_TRSP_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            printf("Adv. enable status = 0x%02x\n", status);
        }
        else
        {
            printf("Please check ADV & Scan Rsp raw data....\r\n");
        }
        break;

    case APP_REQUEST_TEST_START:
        if (app_test_case != 0)
        {
            switch (app_test_case)
            {
            case 0x01:
                if (device.tx_phy == 1)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY 1Mbps: Notify Data 1 ~ MAX Length   ----------    PASS\r\n");
                            app_test_case = 2;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to 1Mbps ---------------- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 2;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 1;
                        phy_param.rx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x02:
                if (device.tx_phy == 2)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY 2Mbps: Notify Data 1 ~ MAX Length   ----------    PASS\r\n");
                            app_test_case = 3;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to 2Mbps ---------------- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 3;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 2;
                        phy_param.rx_phy = 2;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x03:
                if (device.tx_phy == 3)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY S2: Notify Data 1 ~ MAX Length   ----------    PASS\r\n");
                            app_test_case = 4;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to S2 (500Kbps) --------- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 4;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 4;
                        phy_param.rx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S2;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x04:
                if (device.tx_phy == 3)
                {
                    if ((test_counter < 5) && (device.phy_update_state == 0))
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 1;
                        phy_param.rx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else
                    {
                        test_counter = 0;
                        app_test_case = 5;
                        data_counter = 1;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                else
                {
                    test_counter = 0;
                    app_test_case = 5;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                break;

            case 0x05:
                if (device.tx_phy == 3)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY S8: Notify Data 1 ~ MAX Length   ----------    PASS\r\n");
                            app_test_case = 6;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to S8 (125Kbps) --------- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 6;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 4;
                        phy_param.rx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S8;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x06:
                if (device.sec_state == 0)
                {
                    if (ble_cmd_security_request_set(host_id) == BLE_ERR_OK)
                    {
                        device.sec_state = 1;
                    }
                    else
                    {
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                else if (device.sec_state == 2)
                {
                    printf("TEST : SECURITY ENABLE  ---------------------------------    PASS\r\n");
                    device.sec_state = 3;
                    app_test_case = 7;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    Delay_ms(100);
                }
                break;

            case 0x07:
                if (device.tx_phy == 1)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY 1Mbps: Notify Data 1 ~ MAX Length  --SECURITY--   PASS\r\n");
                            app_test_case = 8;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to 1Mbps ----SECURITY---- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 8;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 1;
                        phy_param.rx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x08:
                if (device.tx_phy == 2)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY 2Mbps: Notify Data 1 ~ MAX Length  --SECURITY--   PASS\r\n");
                            app_test_case = 9;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to 2Mbps ----SECURITY----- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 9;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 2;
                        phy_param.rx_phy = 2;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x09:
                if (device.tx_phy == 3)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY S2: Notify Data 1 ~ MAX Length   --SECURITY--    PASS\r\n");
                            app_test_case = 10;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to S2 (500Kbps) --SECURITY-- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 10;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 4;
                        phy_param.rx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S2;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x0A:
                if (device.tx_phy == 3)
                {
                    if ((test_counter < 5) && (device.phy_update_state == 0))
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 1;
                        phy_param.rx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else
                    {
                        test_counter = 0;
                        app_test_case = 11;
                        data_counter = 1;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                else
                {
                    test_counter = 0;
                    app_test_case = 11;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                break;

            case 0x0B:
                if (device.tx_phy == 3)
                {
                    tx_array[data_counter - 1] = data_counter;
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                    data_param.length = data_counter;
                    data_param.p_data = tx_array;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        data_counter++;
                        if (data_counter == (g_trsp_mtu - 2))
                        {
                            printf("TEST : PHY S8: Notify Data 1 ~ MAX Length   --SECURITY--    PASS\r\n");
                            app_test_case = 12;
                            data_counter = 1;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to S8 (125Kbps) --SECURITY-- PENDING\r\n");
                    test_counter = 0;
                    app_test_case = 12;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.tx_phy = 4;
                        phy_param.rx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S8;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 0x0C:
                if (device.connect_upate_state == 0)
                {
                    if ((device.interval > 16) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 6;
                        connParam.ble_conn_param.max_conn_interval = 15;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval High Duty   ---------------    PENDING\r\n");
                        app_test_case = 13;
                        data_counter = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        tx_array[data_counter - 1] = data_counter;
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                        data_param.length = data_counter;
                        data_param.p_data = tx_array;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            data_counter++;
                            if (data_counter == (g_trsp_mtu - 2))
                            {
                                printf("TEST : Connection Interval High Duty   ------------------    PASS\r\n");
                                app_test_case = 13;
                                data_counter = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 0x0D:
                if (device.connect_upate_state == 0)
                {
                    if (((device.interval < 64) || (device.interval > 240)) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 100;
                        connParam.ble_conn_param.max_conn_interval = 150;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval Medium Duty   -------------    PENDING\r\n");
                        app_test_case = 14;
                        data_counter = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        tx_array[data_counter - 1] = data_counter;
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                        data_param.length = data_counter;
                        data_param.p_data = tx_array;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            data_counter++;
                            if (data_counter == (g_trsp_mtu - 2))
                            {
                                printf("TEST : Connection Interval Medium Duty   ----------------    PASS\r\n");
                                app_test_case = 14;
                                data_counter = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 0x0E:
                if (device.connect_upate_state == 0)
                {
                    if ((device.interval < 400) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 400;
                        connParam.ble_conn_param.max_conn_interval = 550;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval Low Duty   ----------------    PENDING\r\n");
                        app_test_case = 15;
                        data_counter = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        tx_array[data_counter - 1] = data_counter;
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_trsps.server_info.handles.hdl_udatni01;
                        data_param.length = data_counter;
                        data_param.p_data = tx_array;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            data_counter++;
                            if (data_counter == (g_trsp_mtu - 2))
                            {
                                printf("TEST : Connection Interval Low Duty   -------------------    PASS\r\n");
                                app_test_case = 15;
                                data_counter = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 0x0F:
                if (ble_cmd_conn_terminate(host_id) == BLE_ERR_OK)
                {
                    app_test_case = 16;
                }
                else
                {
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                break;

            default:
                break;
            }
        }
        else if (hogp_test != 0)
        {
            if (one_flag == 1)
            {
                printf("TEST : -------------HOGP Profile Test----------------------------\r\n");
                one_flag = 0;
                Delay_ms(100);
            }
            switch (hogp_test_case)
            {
            case 0:
                if (device.tx_phy == 1)
                {
                    if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                    {
                        att_HDL_HIDS_REPORT_MSI[0] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[1] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[2] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[3] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                    {
                        att_HDL_HIDS_REPORT_MSI[0] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[2] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[3] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                    {
                        att_HDL_HIDS_REPORT_MSI[0] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFF;
                    }
                    else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                    {
                        att_HDL_HIDS_REPORT_MSI[0] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[1] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFF;
                    }
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                    data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                    data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        hid_report_key_count++;                   //counter++
                        if (hid_report_key_count == 0x80)
                        {
                            printf("TEST : PHY 1Mbps: Notify Data ---------------------------    PASS\r\n");
                            hogp_test_case = 1;
                            hid_report_key_count = 0;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    printf("TEST : Central Role cant change to 1Mbps ---------------- PENDING\r\n");
                    test_counter = 0;
                    hogp_test_case = 1;
                    hid_report_key_count = 0;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.rx_phy = 1;
                        phy_param.tx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 1:
                if (device.tx_phy == 2)
                {
                    if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                    data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                    data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        hid_report_key_count++;                   //counter++
                        if (hid_report_key_count == 0x80)
                        {
                            printf("TEST : PHY 2Mbps: Notify Data ---------------------------    PASS\r\n");
                            hogp_test_case = 2;
                            hid_report_key_count = 0;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    //printf("TEST : PHY 1Mbps: Notify Data 1 ~ MAX Length   ----------    PASS\n");
                    printf("TEST : Central Role cant change to 2Mbps ---------------- PENDING\r\n");
                    test_counter = 0;
                    hogp_test_case = 2;
                    hid_report_key_count = 0;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.rx_phy = 2;
                        phy_param.tx_phy = 2;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 2:
                if (device.tx_phy == 3)
                {
                    if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                    data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                    data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        hid_report_key_count++;                   //counter++
                        if (hid_report_key_count == 0x80)
                        {
                            printf("TEST : PHY S2: Notify Data ---------------------------    PASS\r\n");
                            hogp_test_case = 3;
                            hid_report_key_count = 0;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    //printf("TEST : PHY 1Mbps: Notify Data 1 ~ MAX Length   ----------    PASS\n");
                    printf("TEST : Central Role cant change to S2 ---------------- PENDING\r\n");
                    test_counter = 0;
                    hogp_test_case = 3;
                    hid_report_key_count = 0;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.rx_phy = 4;
                        phy_param.tx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S2;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 3:
                if (device.tx_phy == 3)
                {
                    if ((test_counter < 5) && (device.phy_update_state == 0))
                    {
                        phy_param.host_id = host_id;
                        phy_param.rx_phy = 1;
                        phy_param.tx_phy = 1;
                        phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else
                    {
                        test_counter = 0;
                        hogp_test_case = 5;
                        data_counter = 1;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                else
                {
                    test_counter = 0;
                    hogp_test_case = 5;
                    data_counter = 1;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                break;

            case 5:
                if (device.tx_phy == 3)
                {
                    if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                        att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                    }
                    else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                        att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                    {
                        att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                        att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                        att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                        att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                    }
                    data_param.host_id = host_id;
                    data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                    data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                    data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                    if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                    {
                        hid_report_key_count++;                   //counter++
                        if (hid_report_key_count == 0x80)
                        {
                            printf("TEST : PHY S8: Notify Data ---------------------------    PASS\r\n");
                            hogp_test_case = 6;
                            hid_report_key_count = 0;
                            test_counter = 0;
                        }
                    }
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else if ((test_counter == 5) && (device.phy_update_state == 0))
                {
                    //printf("TEST : PHY 1Mbps: Notify Data 1 ~ MAX Length   ----------    PASS\n");
                    printf("TEST : Central Role cant change to S8 ---------------- PENDING\r\n");
                    test_counter = 0;
                    hogp_test_case = 6;
                    hid_report_key_count = 0;
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                else
                {
                    if (device.phy_update_state == 0)
                    {
                        phy_param.host_id = host_id;
                        phy_param.rx_phy = 4;
                        phy_param.tx_phy = 4;
                        phy_param.coded_phy_option = BLE_CODED_PHY_S8;
                        if (ble_cmd_phy_update(&phy_param) == BLE_ERR_OK)
                        {
                            device.phy_update_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                }
                break;

            case 6:
                if (device.connect_upate_state == 0)
                {
                    if ((device.interval > 16) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 6;
                        connParam.ble_conn_param.max_conn_interval = 15;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval High Duty   ---------------    PENDING\r\n");
                        hogp_test_case = 7;
                        hid_report_key_count = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                        data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                        data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            hid_report_key_count++;                   //counter++
                            if (hid_report_key_count == 0x80)
                            {
                                printf("TEST : Connection Interval High Duty   ------------------    PASS\r\n");
                                hogp_test_case = 7;
                                hid_report_key_count = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 7:
                if (device.connect_upate_state == 0)
                {
                    if (((device.interval < 64) || (device.interval > 240)) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 100;
                        connParam.ble_conn_param.max_conn_interval = 150;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval Medium Duty   ---------------    PENDING\r\n");
                        hogp_test_case = 8;
                        hid_report_key_count = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                        data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                        data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            hid_report_key_count++;                   //counter++
                            if (hid_report_key_count == 0x80)
                            {
                                printf("TEST : Connection Interval Medium Duty   ------------------    PASS\r\n");
                                hogp_test_case = 8;
                                hid_report_key_count = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 8:
                if (device.connect_upate_state == 0)
                {
                    if ((device.interval < 400) && (test_counter < 2))
                    {
                        connParam.host_id = host_id;
                        connParam.ble_conn_param.min_conn_interval = 400;
                        connParam.ble_conn_param.max_conn_interval = 550;
                        connParam.ble_conn_param.periph_latency = 0;
                        connParam.ble_conn_param.supv_timeout = 600;
                        if (ble_cmd_conn_param_update(&connParam) == BLE_ERR_OK)
                        {
                            device.connect_upate_state = 1;
                            test_counter++;
                        }
                        else
                        {
                            app_request_set(host_id, APP_REQUEST_TEST_START, false);
                        }
                    }
                    else if (test_counter == 2)
                    {
                        printf("TEST : Connection Interval Low Duty   ---------------    PENDING\r\n");
                        hogp_test_case = 9;
                        hid_report_key_count = 1;
                        test_counter = 0;
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                    else
                    {
                        if (hid_report_key_count <= 0x1F)     //counter 0~0x1F, mouse move right-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x3F) //counter 0x20~0x3F, mouse move left-down
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0x05; // down
                            att_HDL_HIDS_REPORT_MSI[4] = 0x00;
                        }
                        else if (hid_report_key_count <= 0x5F) //counter 0x40~0x5F, mouse move left-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0xFA; // Left
                            att_HDL_HIDS_REPORT_MSI[2] = 0xFF;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        else if (hid_report_key_count <= 0x7F) //counter 0x60~0x7F, mouse move right-up
                        {
                            att_HDL_HIDS_REPORT_MSI[1] = 0x05; // right
                            att_HDL_HIDS_REPORT_MSI[2] = 0x00;
                            att_HDL_HIDS_REPORT_MSI[3] = 0xFA; // up
                            att_HDL_HIDS_REPORT_MSI[4] = 0xFF;
                        }
                        data_param.host_id = host_id;
                        data_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                        data_param.length = sizeof(att_HDL_HIDS_REPORT_MSI);
                        data_param.p_data = att_HDL_HIDS_REPORT_MSI;
                        if (ble_cmd_gatt_notification(&data_param) == BLE_ERR_OK)
                        {
                            hid_report_key_count++;                   //counter++
                            if (hid_report_key_count == 0x80)
                            {
                                printf("TEST : Connection Interval Low Duty   ------------------    PASS\r\n");
                                hogp_test_case = 9;
                                hid_report_key_count = 1;
                                test_counter = 0;
                            }
                        }
                        app_request_set(host_id, APP_REQUEST_TEST_START, false);
                    }
                }
                break;

            case 9:
                if (ble_cmd_conn_terminate(host_id) == BLE_ERR_OK)
                {
                    hogp_test_case = 10;
                }
                else
                {
                    app_request_set(host_id, APP_REQUEST_TEST_START, false);
                }
                break;

            default:
                break;
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

        if (p_conn_param->status == BLE_ERR_CODE_TIMEOUT)
        {
            printf("CONN_UPDATE_TIMEOUT\r\n");
        }
        else if (p_conn_param->status == BLE_ERR_CODE_CONN_PARAM_UPDATE_REJECTED)
        {
            printf("CONN_UPDATE_REJECTED\r\n");
        }

        device.connect_upate_state = 0;
        device.interval = p_conn_param->conn_interval;
        device.latency = p_conn_param->periph_latency;
        device.timeout = p_conn_param->supv_timeout;
        app_request_set(p_conn_param->host_id, APP_REQUEST_TEST_START, false);
    }
    break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy;
        if (p_phy_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("PHY update/read failed, error code = 0x%02x\n", p_phy_param->status);
            device.phy_update_state = 0;
        }
        else
        {
            device.tx_phy = p_phy_param->tx_phy;
            device.rx_phy = p_phy_param->rx_phy;
            device.phy_update_state = 0;
            printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
        }
        app_request_set(p_phy_param->host_id, APP_REQUEST_TEST_START, false);
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu;

        // update MTU size
        g_trsp_mtu = p_mtu_param->mtu;

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
            if ((hogp_test_case == 10) || (app_test_case == 16))
            {
                printf("TEST : Disconnected    ----------------------------------    PASS\r\n");
                printf("\r\n");
                printf("--------------------- TEST END-----------------------------------\r\n");
            }
            else
            {
                printf("TEST : Disconnected    ----------------------------------    FAIL     ----0x%X\r\n", p_disconn_param->reason);
                printf("\r\n");
                printf("----------------------TEST FAIL------------------------------\r\n");
            }
            printf("\r\n");
            printf("\r\n");
            printf("\r\n");

            ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;

            // re-start adv
            app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false);
        }
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth_param = (ble_evt_sm_auth_status_t *)&p_param->event_param.ble_evt_sm.param.evt_auth_status;
        if (p_auth_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("auth failed, error code = 0x%02x\n", p_auth_param->status);
        }
        else
        {
            device.sec_state = 2;
            app_request_set(p_auth_param->host_id, APP_REQUEST_TEST_START, false);
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
    const uint8_t   SCANRSP_ADLENGTH  = (1) + sizeof(DEVICE_NAME_STR); //  1 byte data type
    ble_gap_addr_t addr_param;

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

    // get device address
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
    ble_err_t status;

    status = BLE_ERR_OK;
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

        // HIDS Related
        // -------------------------------------
        status = ble_svcs_hids_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_hids), ble_svcs_hids_evt_handler);
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
        status = server_profile_init(APP_TRSP_P_HOST_ID);
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
    printf("  IOT Test (P) demo: start...\n");
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
