/**************************************************************************************************
 * @file main.c
 * @brief Main application code for the BLE HOGP (Peripheral) demo.
 * @version 1.0
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 * @details This file includes the necessary includes, macros, constants, and function declarations
 *          for the BLE HOGP (Peripheral) demo. It also contains the implementation of the main application
 *          task, BLE event handlers, and other local functions.
 *************************************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "app_hooks.h"
#include "ble_app.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_host_cmd.h"
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "dump_boot_info.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"


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
#define DEVICE_NAME                     "HOGP_DEMO"

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
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)

#define GPIO_WAKE_UP_PIN                0

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
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool hids_sw_timer_start(void);
static bool hids_sw_timer_stop(void);
static void passkey_set(uint8_t *p_data, uint8_t length);

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
 * @brief Handles incoming UART data.
 *
 * This function processes the received character from the UART.
 *
 * @param ch The character received from the UART.
 * @return true if the system can enable sleep mode, false otherwise.
 */
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
        hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);

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

/**
 * @brief UART0 line status callback.
 *
 * This function is called when there is a change in the UART0 line status.
 *
 * @param p_arg Pointer to the argument passed to the callback function.
 * @return Always returns 0.
 */
static int uart0_line_status_cb(void *p_arg)
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
 * @brief UART0 receive callback.
 *
 * This function is called when data is received on UART0. It processes the received character
 * and determines if the system can enter sleep mode based on the received data.
 *
 * @param p_arg Pointer to the argument passed to the callback function.
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
        // Received '\n' or '\r' --> enable sleep mode.
        hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    }

    return 0;
}

/**
 * @brief HIDS timer handler.
 *
 * This function is called when the HIDS timer expires. It checks if the device is connected
 * and sends HIDS data if the connection is active.
 *
 * @param timer The handle of the timer that expired.
 * @return void This function does not return a value.
 */
static void hids_timer_handler(TimerHandle_t timer)
{
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(timer);

    // HIDS
    if (ble_app_link_info[APP_HID_P_HOST_ID].state == STATE_CONNECTED)
    {
        // send HIDS data
        if (ble_app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_NTF, false) == false)
        {
            // No Application queue buffer available. This means the request cannot be processed at this time, 
            // potentially leading to missed events or delayed processing.
        }
    }
}

/**
 * @brief Handles HIDS service events.
 *
 * This function processes events related to the HIDS service. It handles various
 * HIDS-related events such as CCCD write events for different input reports and
 * starts the HIDS timer if notifications are enabled.
 *
 * @param p_param Pointer to the event parameters.
 * @return void This function does not return a value.
 */
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

/**
 * @brief Handles events for the peripheral role.
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
static void handle_phy_event(ble_evt_gap_phy_t *p_phy_param)
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
 * @param p_disconn_param Pointer to the disconnection complete event parameters.
 * @return void This function does not return a value.
 */
static void handle_disconn_complete(ble_evt_gap_disconn_complete_t *p_disconn_param)
{
    if (p_disconn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
    {
        printf("Disconnect failed, error code = 0x%02x\n", p_disconn_param->status);
    }
    else
    {
        ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;

        // re-start adv
        if (ble_app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false) == false)
        {
            // No Application queue buffer. Error.
        }

        printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);
    }
}

/**
 * @brief Handles the STK generation method event.
 *
 * This function processes the event when the STK generation method is received.
 *
 * @param p_stk_gen_param Pointer to the STK generation method event parameters.
 *                        - key_gen_method: The method used for key generation (e.g., PASSKEY_ENTRY, PASSKEY_DISPLAY).
 *                        - other fields: Additional fields related to the event.
 * @return void This function does not return a value.
 */
static void handle_stk_generation_method(ble_evt_sm_stk_gen_method_t *p_stk_gen_param)
{
    switch (p_stk_gen_param->key_gen_method)
    {
    case PASSKEY_ENTRY:
        // I/O Capability is keyboard
        // Start scanning user-entered passkey.
        break;

    case PASSKEY_DISPLAY:
        // I/O Capability is display
        // Generate a 6-digit random code and display it for pairing.
#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)
        printf("BLE_PAIRING_KEY = %u\n", DEMO_HID_DISPLAY_PASSKEY);
        ble_passkey_confirmed_state = 2;
#endif
        break;

    default:
        break;
    }
}

/**
 * @brief Handles the passkey confirm event.
 *
 * This function processes the event when the passkey is confirmed.
 *
 * @param p_cfm_param Pointer to the passkey confirm event parameters.
 * @return void This function does not return a value.
 */
static void handle_passkey_confirm(ble_evt_sm_passkey_confirm_param_t *p_cfm_param)
{
#if (IO_CAPABILITY_SETTING != NOINPUT_NOOUTPUT)
    ble_sm_passkey_param_t ble_passkey_param;

    if (ble_passkey_confirmed_state == 2)
    {
        ble_passkey_param.host_id = p_cfm_param->host_id;
        ble_passkey_param.passkey = (uint32_t)DEMO_HID_DISPLAY_PASSKEY;
        // set passkey
        ble_cmd_passkey_set(&ble_passkey_param);
        ble_passkey_confirmed_state = 0;
    }
    else
    {
        ble_passkey_confirmed_state = 1;
        printf("Please enter the passkey for BLE pairing...\n");
    }
#endif
}

/**
 * @brief Handles the numeric comparison event.
 *
 * This function processes the event when a numeric comparison is required during pairing.
 *
 * @param p_cfm_param Pointer to the numeric comparison event parameters.
 *                    - comparison_value: The numeric value to be compared.
 *                    - host_id: The ID of the host that triggered the event.
 * @return void This function does not return a value.
 */
static void handle_numeric_comparison(ble_evt_sm_numeric_comparison_param_t *p_cfm_param)
{
#if ((IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
    ble_numeric_comp_state = 1;
    printf("Confirm the pairing values displayed on both devices:\n%06u (Enter 1 if matched, 0 if not matched)\n", p_cfm_param->comparison_value);
#endif
}

/**
 * @brief Handles the authentication status event.
 *
 * This function processes the event when the authentication status is received.
 * If the authentication is successful, it restores the CCCD configuration.
 * If the authentication fails, it resets the passkey confirmation state and numeric comparison state.
 *
 * @param p_auth_param Pointer to the authentication status event parameters.
 *                     - status: The status of the authentication (e.g., success or failure).
 *                     - host_id: The ID of the host that triggered the event.
 * @return void This function does not return a value.
 */
static void handle_ble_auth_status(ble_evt_sm_auth_status_t *p_auth_param)
{
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

/**
 * @brief Handles BLE application events.
 *
 * This function processes various BLE application events, including advertising, connection, 
 * disconnection, and other GAP and GATT related events.
 *
 * @param p_param Pointer to the event parameters. This parameter is a structure that contains 
 *                information about the BLE event, including the event type and associated data.
 * @return void This function does not return a value.
 * @note This function does not return any error codes. All errors are handled internally and logged.
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
        handle_phy_event((ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy);
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

    case BLE_SM_EVT_STK_GENERATION_METHOD:
        handle_stk_generation_method((ble_evt_sm_stk_gen_method_t *)&p_param->event_param.ble_evt_sm.param.evt_stk_gen_method);
        break;

    case BLE_SM_EVT_PASSKEY_CONFIRM:
        handle_passkey_confirm((ble_evt_sm_passkey_confirm_param_t *)&p_param->event_param.ble_evt_sm.param.evt_passkey_confirm_param);
        break;

    case BLE_SM_EVT_NUMERIC_COMPARISON:
        handle_numeric_comparison((ble_evt_sm_numeric_comparison_param_t *)&p_param->event_param.ble_evt_sm.param.evt_numeric_comparison_param);
        break;

    case BLE_SM_EVT_AUTH_STATUS:
        handle_ble_auth_status((ble_evt_sm_auth_status_t *)&p_param->event_param.ble_evt_sm.param.evt_auth_status);
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
 * @brief Starts the HIDS software timer.
 *
 * This function starts the HIDS software timer if it is not already active.
 *
 * @return true if the timer was successfully started, false otherwise.
 */
static bool hids_sw_timer_start(void)
{
    if ( xTimerIsTimerActive( g_hids_timer ) == pdFALSE )
    {
        if ( xTimerStart( g_hids_timer, 0 ) != pdPASS )
        {
            // The HIDS timer could not be set into the Active state.
            return false;
        }
    }
    return true;
}

/**
 * @brief Stops the HIDS software timer.
 *
 * This function stops the HIDS software timer if it is active.
 *
 * @return true if the timer was successfully stopped, false otherwise.
 */
static bool hids_sw_timer_stop(void)
{
    if ( xTimerIsTimerActive( g_hids_timer ) != pdFALSE )
    {
        if ( xTimerStop( g_hids_timer, 0 ) == pdFAIL )
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

    // adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
        0x03, GAP_AD_TYPE_SERVICE_MORE_16B_UUID, U16_LOWBYTE(GATT_SERVICES_HUMAN_INTERFACE_DEVICE), U16_HIGHBYTE(GATT_SERVICES_HUMAN_INTERFACE_DEVICE),
        0x03, GAP_AD_TYPE_APPEARANCE, U16_LOWBYTE(BLE_APPEARANCE_GENERIC_HID), U16_HIGHBYTE(BLE_APPEARANCE_GENERIC_HID),
    };

    //Scan response data
    uint8_t adv_scan_rsp_data[2 + (strlen(DEVICE_NAME_STR))];
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
        adv_param.own_addr_type = addr_param.addr_type; // Set the address type (public or random)
        adv_param.adv_interval_min = APP_ADV_INTERVAL_MIN; // Minimum advertising interval
        adv_param.adv_interval_max = APP_ADV_INTERVAL_MAX; // Maximum advertising interval
        adv_param.adv_channel_map = ADV_CHANNEL_ALL; // Use all advertising channels
        adv_param.adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL; // Accept all connection requests

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

#if ((IO_CAPABILITY_SETTING == KEYBOARD_ONLY) || (IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
/**
 * @brief Sets the passkey for BLE pairing.
 *
 * This function processes the received passkey data and sets it for BLE pairing.
 * It validates the input to ensure it is numeric and then sets the passkey.
 * If the passkey confirmation state is active, it sends the passkey entry request.
 *
 * @param p_data Pointer to the received passkey data.
 * @param length Length of the received passkey data.
 */
static void passkey_set(uint8_t *p_data, uint8_t length)
{
    if (ble_passkey_confirmed_state == 1)
    {
        ble_passkey_confirmed_state = 0;
        passkey = 0;

        // Validate that the input string is numeric
        bool is_numeric = true;
        for (uint8_t i = 0; i < length; i++) {
            if (!isdigit(p_data[i])) {
                is_numeric = false;
                break;
            }
        }

        if (is_numeric) {
            sscanf((char *)p_data, "%d", (int *)&passkey);
        } else {
            printf("Invalid passkey input. Please enter a numeric value.\n");
        }

        // send HIDS passkey
        if (ble_app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_PASSKEY_ENTRY, true) == false)
        {
            // No Application queue buffer. Error.
        }
    }
#if ((IO_CAPABILITY_SETTING == KEYBOARD_DISPLAY) || (IO_CAPABILITY_SETTING == DISPLAY_YESNO))
    if (ble_numeric_comp_state == 1)
    {
        ble_numeric_comp_state = 0;

        // Validate that the input string is numeric
        bool is_numeric = true;
        for (uint8_t i = 0; i < length; i++) {
            if (!isdigit(p_data[i])) {
                is_numeric = false;
                break;
            }
        }

        if (is_numeric) {
            sscanf((char *)p_data, "%d", (int *)&same_numeric);
        } else {
            printf("Invalid numeric comparison input. Please enter a numeric value.\n");
        }

        // send HIDS passkey
        if (ble_app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_HIDS_NUMERIC_COMP_ENTRY, true) == false)
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
 * @param p_param Pointer to the service event parameters.
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
    if (ble_app_request_set(APP_HID_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
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
    hosal_uart_callback_set(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_CALLBACK, uart0_line_status_cb, &uart0_dev);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT_RX);
    hosal_uart_ioctl(&uart0_dev, HOSAL_UART_RECEIVE_LINE_STATUS_ENABLE, (void *)NULL);

    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_UART_RX);

    __NVIC_SetPriority(Uart0_IRQn, 6);
}

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
 * @brief Initializes the HIDS service data.
 *
 * This function sets the initial values for the HIDS service data, specifically
 * setting the Client Characteristic Configuration Descriptors (CCCDs) for various
 * input reports to 0.
 *
 * @param p_data Pointer to the HIDS service data structure to be initialized.
 * @return void This function does not return a value.
 */
static void svcs_hids_data_init(ble_svcs_hids_data_t *p_data)
{
    p_data->boot_keyboard_input_report_cccd = 0;
    p_data->keyboard_input_report_cccd = 0;
    p_data->boot_mouse_input_report_cccd = 0;
    p_data->mouse_input_report_cccd = 0;
    p_data->consumer_input_report_cccd = 0;
}

/**
 * @brief Initializes the BLE profile for the server role.
 *
 * This function sets up the BLE profile for the server role, including the initialization
 * of GAP, GATT, DIS, and HIDS services. It configures the necessary parameters and 
 * event handlers for each service.
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

/**
 * @brief Initializes the BLE IO capabilities.
 *
 * This function initializes the BLE IO capabilities by setting the IO capabilities
 * to the specified parameter.
 *
 * @param param The IO capabilities to be set. Possible values are:
 *              - IO_CAP_DISPLAY_ONLY
 *              - IO_CAP_DISPLAY_YESNO
 *              - IO_CAP_KEYBOARD_ONLY
 *              - IO_CAP_NO_INPUT_NO_OUTPUT
 *              - IO_CAP_KEYBOARD_DISPLAY
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
 */
static ble_err_t io_capability_init(io_caps_t param)
{
    ble_err_t status;
    ble_sm_io_cap_param_t io_caps_param;

    // Set BLE IO capabilities
    io_caps_param.io_caps_param = param;

    status = ble_cmd_io_capability_set(&io_caps_param);

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

    // application SW timer, tick = 1ms
    g_hids_timer = xTimerCreate("HIDS_Timer", pdMS_TO_TICKS(50), pdTRUE, ( void * ) 0, hids_timer_handler);

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
