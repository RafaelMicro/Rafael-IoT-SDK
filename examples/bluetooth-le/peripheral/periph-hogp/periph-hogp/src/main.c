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
#include "hosal_uart.h"

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

#define APP_HID_P_HOST_ID               0         // HOGP: Peripheral

#define DEFAULT_MTU                     23

// Advertising device name
#define DEVICE_NAME                     'H', 'O', 'G', 'P', '_', 'D', 'E', 'M', 'O'

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms


// GAP device name
static const uint8_t     DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0x31, 0x32, 0x33, 0x34, 0x35, 0xC6 }
                                           };

//HIDS Consumer keycode table
const uint8_t hids_consumer_report_keycode_demo[][2] =
{
    {0xE9, 0x00,},  //vol+
    {0xEA, 0x00,},  //vol-
    {0xE2, 0x00,},  //Mute
    {0xB0, 0x00,},  //Play
    {0xB1, 0x00,},  //Pause
    {0xB3, 0x00,},  //Fast forward
    {0xB4, 0x00,},  //Rewind
    {0xB5, 0x00,},  //Scan next track
    {0xB6, 0x00,},  //Scan previous track
    {0xB7, 0x00,},  //Stop
    {0xB8, 0x00,},  //Eject
    {0x8A, 0x01,},  //Email reader
    {0x96, 0x01,},  //Internet browser
    {0x9E, 0x01,},  //Terminal lock/screensaver
    {0xC6, 0x01,},  //Research/search browser
    {0x2D, 0x02,},  //Zoom in
};

#define STATE_HID_REPORT_CS_INITIAL             0
#define STATE_HID_REPORT_CS_DATA_UPD            0x01

#define STATE_HID_REPORT_KB_INITIAL             0
#define STATE_HID_REPORT_KB_DATA_UPD            0x01

#define HDL_HIDS_REPORT_TAB_CSKEY_L             0
#define HDL_HIDS_REPORT_TAB_CSKEY_H             1

#define HDL_HIDS_REPORT_TAB_KEY_L_R             0
#define HDL_HIDS_REPORT_TAB_DIR_L_R_L           1
#define HDL_HIDS_REPORT_TAB_DIR_L_R_H           2
#define HDL_HIDS_REPORT_TAB_DIR_U_D_L           3
#define HDL_HIDS_REPORT_TAB_DIR_U_D_H           4
#define HDL_HIDS_REPORT_TAB_ROL_U_D             5
#define HDL_HIDS_REPORT_TAB_ROL_L_R_L           6
#define HDL_HIDS_REPORT_TAB_ROL_L_R_H           7

#define HDL_HIDS_REPORT_TAB_KEY_CTRL            0
#define HDL_HIDS_REPORT_TAB_KEY_DATA0           2
#define HDL_HIDS_REPORT_TAB_KEY_DATA1           3
#define HDL_HIDS_REPORT_TAB_KEY_DATA2           4
#define HDL_HIDS_REPORT_TAB_KEY_DATA3           5
#define HDL_HIDS_REPORT_TAB_KEY_DATA4           6
#define HDL_HIDS_REPORT_TAB_KEY_DATA5           7

#define DEMO_HID_DISPLAY_PASSKEY                654321

#define UART0_OPERATION_PORT            0
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, 16, 17, UART_BAUDRATE_115200)

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static TimerHandle_t  g_hids_timer;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;

static uint8_t hid_report_count;

static uint8_t hid_consumer_report_state;      //consumer report state
static uint8_t hid_consumer_report_count;      //consumer control value. Here use to control volume

static uint8_t hid_keyboard_report_state;      //keyboard report state
static uint8_t hid_keyboard_report_count;      //keyboard control value. Here use to control keycode.

#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)
static uint8_t ble_passkey_confirmed_state = 0;  //wait to 1 to set scanned Passkey and 2 for display.
#endif
#if ((IO_CAPABILITY_SETTING == KEYBOARD_ONLY) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
static uint32_t passkey = 0;                   //passkey value
#endif

#if ((IO_CAPABILITY_SETTING == DISPLAY_YESNO) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY))
static uint8_t ble_numeric_comp_state = 0;  //wait to 1 to set scanned numeric comparison result.
static uint8_t same_numeric = 0xff;         //numeric is same
#endif

/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_hids_data_init(ble_svcs_hids_data_t *p_data);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool hids_sw_timer_start(void);
static bool hids_sw_timer_stop(void);
static void passkey_set(uint8_t *p_data, uint8_t length);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static bool uart_data_handler(char ch)
{
    bool status = false;
#if ((IO_CAPABILITY_SETTING == KEYBOARD_ONLY) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO) )
    static uint8_t rx_buffer[6];
    static uint8_t index = 0;

    if ((ch == '\n') || (ch == '\r'))
    {     
        // set passkey
        passkey_set(rx_buffer, index);

        // reset index
        index = 0;

        // enable sleep mode
        lpm_low_power_unmask(LOW_POWER_MASK_BIT_TASK_BLE_APP);

        // set status to true indicates the system can enable sleep mode
        status = true;
    }
    else
    {
        if (index == 6)
        {
            index = 0;
        }
        rx_buffer[index++] = ch;
    }
#endif
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

static void hids_timer_handler( TimerHandle_t timer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( timer );

    // HIDS
    if (ble_app_link_info[APP_HID_P_HOST_ID].state == STATE_CONNECTED)
    {
        // send HIDS data
        if (app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_NTF, false) == false)
        {
            // No Application queue buffer. Error.
        }
    }
}

static void ble_svcs_hids_evt_handler(ble_evt_att_param_t *p_param)
{
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[p_param->host_id].profile_info;

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_HIDS_BOOT_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hids.server_info.data.boot_keyboard_input_report_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HIDS timer to send notification
                if (hids_sw_timer_start() == false)
                {
                    printf("HIDS timer start failed. \n");
                }
            }
            break;

        case BLESERVICE_HIDS_KEYBOARD_INPUT_REPORT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hids.server_info.data.keyboard_input_report_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HIDS timer to send notification
                if (hids_sw_timer_start() == false)
                {
                    printf("HIDS timer start failed. \n");
                }
            }
            break;

        case BLESERVICE_HIDS_BOOT_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hids.server_info.data.boot_mouse_input_report_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HIDS timer to send notification
                if (hids_sw_timer_start() == false)
                {
                    printf("HIDS timer start failed. \n");
                }
            }
            break;

        case BLESERVICE_HIDS_MOUSE_INPUT_REPORT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hids.server_info.data.mouse_input_report_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HIDS timer to send notification
                if (hids_sw_timer_start() == false)
                {
                    printf("HIDS timer start failed. \n");
                }
            }
            break;

        case BLESERVICE_HIDS_CONSUMER_INPUT_REPORT_CCCD_WRITE_EVENT:
            if ((p_profile_info->svcs_info_hids.server_info.data.consumer_input_report_cccd & BLEGATT_CCCD_NOTIFICATION) != 0)
            {
                // notify enabled -> start HIDS timer to send notification
                if (hids_sw_timer_start() == false)
                {
                    printf("HIDS timer start failed. \n");
                }
            }
            break;

        default:
            break;
        }
    }
}

// HIDS Peripheral
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
            svcs_hids_data_init(&p_profile_info->svcs_info_hids.server_info.data);

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

            // reset report count
            hid_report_count = 0;
        } while (0);

        break;

    case APP_REQUEST_HIDS_PASSKEY_ENTRY:
    {
#if ((IO_CAPABILITY_SETTING == KEYBOARD_ONLY) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY))
        ble_sm_passkey_param_t param;

        param.host_id = host_id;
        param.passkey = (uint32_t)passkey;

        printf("BLE_PAIRING_KEY = %06d\n", passkey);          // show the passkey
        ble_cmd_passkey_set(&param);
#endif
    }
    break;

    case APP_REQUEST_HIDS_NUMERIC_COMP_ENTRY:
    {
#if ((IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
        ble_sm_numeric_comp_result_param_t param;

        param.host_id = host_id;
        param.same_numeric = same_numeric;

        printf("Numeric matched: %d\n", same_numeric);
        ble_cmd_numeric_comp_result_set(&param);
#endif
    }
    break;


    case APP_REQUEST_HIDS_NTF:
    {
        ble_gatt_data_param_t gatt_param;

        // send heart rate measurement value to client
        if ((hid_report_count & 0x3F) != 0x3F)    //(counter value!=0x3F or 0x7F or 0xBF or 0xFF)
        {
            if (hid_report_count < 0x80)
            {
                if (p_profile_info->svcs_info_hids.server_info.data.mouse_input_report_cccd != 0)
                {
                    if (hid_report_count <= 0x1F)    //counter 0~0x1F, mouse move right-down
                    {
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_L] = 0x05;    // right
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_H] = 0x00;
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_L] = 0x05;    // down
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_H] = 0x00;
                    }
                    else if (hid_report_count <= 0x3F)    //counter 0x20~0x3F, mouse move left-down
                    {
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_L] = 0xFA;    // left
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_H] = 0xFF;
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_L] = 0x05;    // down
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_H] = 0x00;
                    }
                    else if (hid_report_count <= 0x5F)    //counter 0x40~0x5F, mouse move left-up
                    {
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_L] = 0xFA;    // left
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_H] = 0xFF;
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_L] = 0xFA;    // up
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_H] = 0xFF;
                    }
                    else if (hid_report_count <= 0x7F)    //counter 0x60~0x7F, mouse move right-up
                    {
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_L] = 0x05;    // right
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_L_R_H] = 0x00;
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_L] = 0xFA;    // up
                        p_profile_info->svcs_info_hids.server_info.data.mouse_input_report[HDL_HIDS_REPORT_TAB_DIR_U_D_H] = 0xFF;
                    }

                    // set parameters
                    gatt_param.host_id = host_id;
                    gatt_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_mouse_input_report;
                    gatt_param.length = sizeof(p_profile_info->svcs_info_hids.server_info.data.mouse_input_report);
                    gatt_param.p_data = p_profile_info->svcs_info_hids.server_info.data.mouse_input_report;

                    // send notification
                    status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
                    if (status == BLE_ERR_OK)
                    {
                        hid_report_count++;    //counter++
                    }
                }
                else
                {
                    hid_report_count++;    //counter++
                }
            }
            else
            {
                hid_report_count++;    //counter++
            }
        }
        else    //(counter vlaue==0x3F or 0x7F or 0xBF or 0xFF)
        {
            if ((hid_report_count == 0x3F) || (hid_report_count == 0xBF))    //control keyboard when counter=0x3F, 0xBF
            {
                if (p_profile_info->svcs_info_hids.server_info.data.keyboard_input_report_cccd != 0)
                {
                    if ((hid_keyboard_report_state & STATE_HID_REPORT_KB_DATA_UPD) == 0)   //check keyboard report status
                    {
                        if ((hid_keyboard_report_count <= 0x04) || (hid_keyboard_report_count >= 0x27))
                        {
                            hid_keyboard_report_count = 0x04;    //0x04 mean 'a'; 0x27 mean '9'; see USB HID spec.
                        }
                        p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report[HDL_HIDS_REPORT_TAB_KEY_DATA0] = hid_keyboard_report_count;    // repeat keyCode: 'a' 'b' ~ 'z' ~ '1' '2'  ~ '9'

                        // set parameters
                        gatt_param.host_id = host_id;
                        gatt_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_keyboard_input_report;
                        gatt_param.length = sizeof(p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report);
                        gatt_param.p_data = p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report;

                        // send notification
                        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
                        if (status == BLE_ERR_OK)
                        {
                            hid_keyboard_report_state |= STATE_HID_REPORT_KB_DATA_UPD;
                            hid_keyboard_report_count++;    //keyboard keycode
                        }
                    }
                    else    //release key
                    {
                        p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report[HDL_HIDS_REPORT_TAB_KEY_DATA0] = 0x00;    // release key

                        // set parameters
                        gatt_param.host_id = host_id;
                        gatt_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_keyboard_input_report;
                        gatt_param.length = sizeof(p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report);
                        gatt_param.p_data = p_profile_info->svcs_info_hids.server_info.data.keyboard_intput_report;

                        // send notification
                        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
                        if (status == BLE_ERR_OK)
                        {
                            hid_keyboard_report_state &= ~STATE_HID_REPORT_KB_DATA_UPD;
                            hid_report_count++;
                        }
                    }
                }
                else
                {
                    hid_report_count++;
                }
            }
            if ((hid_report_count == 0x7F) || (hid_report_count == 0xFF))    //control volume when counter=0x7F, 0xFF
            {
                if (p_profile_info->svcs_info_hids.server_info.data.consumer_input_report_cccd != 0)
                {
                    if ((hid_consumer_report_state & STATE_HID_REPORT_CS_DATA_UPD) == 0)    // check consumer report status
                    {
                        if ((hid_consumer_report_count & 0x01) == 0x01)
                        {
                            p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[0] = hids_consumer_report_keycode_demo[0][0];    // vol+
                            p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[1] = hids_consumer_report_keycode_demo[0][1];
                        }
                        else
                        {
                            p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[0] = hids_consumer_report_keycode_demo[2][0];    // mute
                            p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[1] = hids_consumer_report_keycode_demo[2][1];
                        }


                        // set parameters
                        gatt_param.host_id = host_id;
                        gatt_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_consumer_input_report;
                        gatt_param.length = sizeof(p_profile_info->svcs_info_hids.server_info.data.consumer_input_report);
                        gatt_param.p_data = p_profile_info->svcs_info_hids.server_info.data.consumer_input_report;

                        // send notification
                        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
                        if (status == BLE_ERR_OK)
                        {
                            hid_consumer_report_state |= STATE_HID_REPORT_CS_DATA_UPD;
                            hid_consumer_report_count++;    // just counter for send another consumer data
                        }
                    }
                    else    //release key
                    {
                        p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[0] = 0x00;    // release key
                        p_profile_info->svcs_info_hids.server_info.data.consumer_input_report[1] = 0x00;


                        // set parameters
                        gatt_param.host_id = host_id;
                        gatt_param.handle_num = p_profile_info->svcs_info_hids.server_info.handles.hdl_consumer_input_report;
                        gatt_param.length = sizeof(p_profile_info->svcs_info_hids.server_info.data.consumer_input_report);
                        gatt_param.p_data = p_profile_info->svcs_info_hids.server_info.data.consumer_input_report;

                        // send notification
                        status = ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &gatt_param);
                        if (status == BLE_ERR_OK)
                        {
                            hid_consumer_report_state &= ~STATE_HID_REPORT_CS_DATA_UPD;
                            hid_report_count++;
                        }
                    }
                }
                else
                {
                    hid_report_count++;
                }
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

            // stop HIDS timer
            if (hids_sw_timer_stop() == false)
            {
                printf("HIDS timer stop failed. \n");
            }

            // re-start adv
            if (app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false) == false)
            {
                // No Application queue error. Error.
            }

            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
        }
    }
    break;

    case BLE_SM_EVT_STK_GENERATION_METHOD:
    {
        ble_evt_sm_stk_gen_method_t *p_stk_gen_param = (ble_evt_sm_stk_gen_method_t *)&p_param->event_param.ble_evt_sm.param.evt_stk_gen_method;
        if (p_stk_gen_param->key_gen_method == PASSKEY_ENTRY)
        {
            // I/O Capability is keyboard
            // Start scanning user-entered passkey.
        }
        else if (p_stk_gen_param->key_gen_method == PASSKEY_DISPLAY)
        {
            // I/O Capability is display
            // Generate a 6-digit random code and display it for pairing.
#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)
            printf("BLE_PAIRING_KEY = %d\n", (uint32_t)DEMO_HID_DISPLAY_PASSKEY);
            ble_passkey_confirmed_state = 2;
#endif
        }
    }
    break;

    case BLE_SM_EVT_PASSKEY_CONFIRM:
    {
        //enter a scanned Passkey or use a randomly generated passkey.
#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)

        ble_evt_sm_passkey_confirm_param_t *p_cfm_param = (ble_evt_sm_passkey_confirm_param_t *)&p_param->event_param.ble_evt_sm.param.evt_passkey_confirm_param;
        ble_sm_passkey_param_t passkey_param;

        if (ble_passkey_confirmed_state == 2)
        {
            passkey_param.host_id = p_cfm_param->host_id;
            passkey_param.passkey = (uint32_t)DEMO_HID_DISPLAY_PASSKEY;
            // set passkey
            ble_cmd_passkey_set(&passkey_param);
            ble_passkey_confirmed_state = 0;
        }
        else
        {
            ble_passkey_confirmed_state = 1;
            printf("Please enter passkey...\n");
        }
#endif
    }
    break;

    case BLE_SM_EVT_NUMERIC_COMPARISON:
    {
#if ((IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
        ble_evt_sm_numeric_comparison_param_t *p_cfm_param = (ble_evt_sm_numeric_comparison_param_t *)&p_param->event_param.ble_evt_sm.param.evt_numeric_comparison_param;

        ble_numeric_comp_state = 1;
        printf("confirm pairing values:\n%06d (1: matched, 0: not matched)\n", p_cfm_param->comparison_value);
#endif
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth_param = (ble_evt_sm_auth_status_t *)&p_param->event_param.ble_evt_sm.param.evt_auth_status;

        if (p_auth_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            ble_cmd_cccd_restore(p_auth_param->host_id);
        }
        else
        {
#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)
            ble_passkey_confirmed_state = 0;
#endif
#if ((IO_CAPABILITY_SETTING == DISPLAY_YESNO) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY))
            ble_numeric_comp_state = 0;
#endif
        }
        printf("BLE authentication status = %d\n", p_auth_param->status);
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
static bool hids_sw_timer_start(void)
{
    if ( xTimerIsTimerActive( g_hids_timer ) == pdFALSE )
    {
        if ( xTimerStart( g_hids_timer, 0 ) != pdPASS )
        {
            // The timer could not be set into the Active state.
            return false;
        }
    }
    return true;
}

static bool hids_sw_timer_stop(void)
{
    if ( xTimerIsTimerActive( g_hids_timer ) != pdFALSE )
    {
        if ( xTimerStop( g_hids_timer, 0 ) != pdPASS )
        {
            return false;
        }
    }
    return true;
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
        0x03, GAP_AD_TYPE_SERVICE_MORE_16B_UUID, U16_LOWBYTE(GATT_SERVICES_HUMAN_INTERFACE_DEVICE), U16_HIGHBYTE(GATT_SERVICES_HUMAN_INTERFACE_DEVICE),
        0x03, GAP_AD_TYPE_APPEARANCE, U16_LOWBYTE(BLE_APPEARANCE_GENERIC_HID), U16_HIGHBYTE(BLE_APPEARANCE_GENERIC_HID),
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

#if ((IO_CAPABILITY_SETTING == KEYBOARD_ONLY) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
static void passkey_set(uint8_t *p_data, uint8_t length)
{
    if (ble_passkey_confirmed_state == 1)
    {
        ble_passkey_confirmed_state = 0;
        passkey = 0;

        sscanf((char *)p_data, "%d", (int *)&passkey);

        // send HIDS passkey
        if (app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_PASSKEY_ENTRY, true) == false)
        {
            // No Application queue buffer. Error.
        }
    }

#if ((IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
    if (ble_numeric_comp_state == 1)
    {
        ble_numeric_comp_state = 0;

        sscanf((char *)p_data, "%d", (int *)&same_numeric);

        // send HIDS passkey
        if (app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_NUMERIC_COMP_ENTRY, true) == false)
        {
            // No Application queue buffer. Error.
        }
    }
#endif

}
#endif

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
    if (app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
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

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

static void svcs_hids_data_init(ble_svcs_hids_data_t *p_data)
{
    p_data->boot_keyboard_input_report_cccd = 0;
    p_data->keyboard_input_report_cccd = 0;
    p_data->boot_mouse_input_report_cccd = 0;
    p_data->mouse_input_report_cccd = 0;
    p_data->consumer_input_report_cccd = 0;
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

        // HIDS Related
        // -------------------------------------
        status = ble_svcs_hids_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_hids), ble_svcs_hids_evt_handler);
        if (status != BLE_ERR_OK)
        {
            return status;
        }
    } while (0);

    return status;
}

static ble_err_t io_capability_init(io_caps_t param)
{
    ble_err_t status;
    ble_sm_io_cap_param_t io_caps_param;

    //set BLE IO capabilities
    io_caps_param.io_caps_param = param;

    status = ble_cmd_io_capability_set(&io_caps_param);

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
        status = server_profile_init(APP_HID_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        //set BLE IO capabilities
        status = io_capability_init(IO_CAPABILITY_SETTING);
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
    printf("  BLE HID (P) demo: start...\n");
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
    g_hids_timer = xTimerCreate("HIDS_Timer", pdMS_TO_TICKS(50), pdTRUE, ( void * ) 0, hids_timer_handler);

    app_uart_init();
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
