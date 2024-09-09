#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ctrl_cmd.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_security_manager.h"
#include "timer.h"

typedef enum App_BleAction
{
    BLEACTION_IDLE,
    BLEACTION_CONNECT,
    BLEACTION_CONNECT_CANCEL,
    BLEACTION_DISCONNECT,
    BLEACTION_CONNECT_INT_SET,
    BLEACTION_PHY_SET,
    BLEACTION_DATA_LENGTH_SET,
    BLEACTION_SEC_REQ_SET,
    BLEACTION_CH_CLASSIFICATION_SET,
    BLEACTION_CH_MAP_READ,
} App_BleAction;


typedef enum App_DataTestAction
{
    TESTACTION_IDLE,
    TESTACTION_TOTAL_TEST_TX_LEN_SET,
    TESTACTION_MODE_SET,
    TESTACTION_TEST_START,
    TESTACTION_TEST_STOP,
} App_DataTestAction;


/**************************************************************************
* Private Functions
**************************************************************************/
/** hex string to hex byte array converter. */
void convert_hexStr_to_hexArry(char *hexStr, uint8_t strLen, char *hexArray)
{
    int count = 0;

    for (count = 0; count < strLen; count++)
    {
        sscanf(hexStr, "%2hhx", &hexArray[count]);
        hexStr += 2;
    }
}

/** string to uint32 converter. */
void convert_str_to_uint32(char *hexStr, uint32_t *value)
{
    sscanf(hexStr, "%ld", value);
}


/** Qruey ctrl command and retrun data with parameter only. */
CTRL_CMD_EValue chekc_ctrl_cmd(uint8_t **data, uint16_t length)
{
    uint8_t i = 0;
    uint8_t new_len = 0;
    int commandSize = sizeof(ctrl_cmd_table) / sizeof(ctrl_cmd_table[0]);


    for (i = 0; i < commandSize; i++)
    {
        if (strncmp(ctrl_cmd_table[i], (char *)(*data), strlen(ctrl_cmd_table[i])) == 0)
        {
            // FOUND!
            *data = *data + strlen(ctrl_cmd_table[i]); // trim CMD string
            new_len = strlen((char *)*data);

            if (new_len > 0)
            {
                *data = *data + 1;  // keep parameter only
                return (CTRL_CMD_EValue)i;
            }
            else if (new_len == 0)
            {
                return (CTRL_CMD_EValue)i;
            }
            else
            {
                return CMD_NONE;
            }
        }
    }
    return CMD_NONE;
}

/** handle local device address setting. */
void handle_targetAddr_command(uint8_t *param)
{
    char tempHexArray[6];

    // convert address
    convert_hexStr_to_hexArry((char *)param, (6 * 2), tempHexArray);
    memcpy(g_target_addr.addr, tempHexArray, 6);

    // print result
    print_targetAddr_param();
}

/** handle target device address setting. */
void handle_localAddr_command(uint8_t *param)
{
    ble_err_t status;
    ble_gap_addr_t addr;
    char tempHexArray[6];

    // convert address
    addr.addr_type = RANDOM_ADDR;
    convert_hexStr_to_hexArry((char *)param, (6 * 2), tempHexArray);
    memcpy(addr.addr, tempHexArray, 6);

    // set address
    status = ble_cmd_device_addr_set(&addr);
    if (status != BLE_ERR_OK)
    {
        PRINT_CTRL_CMD_ERROR("error address.");
    }
    else
    {
        // print result
        print_localAddr_param();
    }
}

/** handle BLE common commands. */
void handle_BLE_cmd(App_BleAction action, uint8_t *param)
{
    bool action_status = false;

    if (app_DR_test_info.test_state != DATARATE_STATE_IDLE)
    {
        PRINT_CTRL_CMD_ERROR("can not handle command while data rate test is running.");
        return;
    }

    // handle actions
    switch (action)
    {
    case BLEACTION_CONNECT:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_STANDBY)
        {
            action_status = true;

            if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_CREATE_CONN, false) == false)
            {
                // NO application queue buffer. Error.
            }
        }
        break;

    case BLEACTION_CONNECT_CANCEL:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_INITIATING)
        {
            action_status = true;

            if (app_request_set(APP_TRSP_C_HOST_ID, APP_REQUEST_CREATE_CONN_CANCEL, false) == false)
            {
                // No application queue buffer. Error.
            }
            // stop timer
            timer_stop(APP_HW_TIMER_ID);
        }
        break;

    case BLEACTION_DISCONNECT:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED)
        {
            action_status = true;
            ble_cmd_conn_terminate(APP_TRSP_C_HOST_ID);
        }
        break;

    case BLEACTION_CONNECT_INT_SET:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED)
        {
            ble_err_t ble_status;
            ble_gap_conn_param_update_param_t conn_param;

            action_status = true;
            convert_str_to_uint32((char *)param, (uint32_t *)&connect_info.conn_interval);

            conn_param.host_id = APP_TRSP_C_HOST_ID;
            conn_param.ble_conn_param.max_conn_interval = connect_info.conn_interval;
            conn_param.ble_conn_param.min_conn_interval = connect_info.conn_interval;
            conn_param.ble_conn_param.periph_latency = 0;
            conn_param.ble_conn_param.supv_timeout = 1000; // 10s

            ble_status = ble_cmd_conn_param_update(&conn_param);
            if (ble_status != BLE_ERR_OK)
            {
                printf("setBLEGATT_ExchangeMtuRequest failed: %d.\n", ble_status);
                return;
            }

            // show result
            print_app_conn_param();
        }
        break;


    case BLEACTION_PHY_SET:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED)
        {
            ble_err_t ble_status;
            ble_gap_phy_update_param_t phy_param;

            action_status = true;

            if (param[0] == '1')
            {
                connect_info.phy = BLE_PHY_1M;
                phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
            }
            else if (param[0] == '2')
            {
                connect_info.phy = BLE_PHY_2M;
                phy_param.coded_phy_option = BLE_CODED_PHY_NO_PREFERRED;
            }
            else if (param[0] == '3')
            {
                connect_info.phy = BLE_PHY_CODED;
                phy_param.coded_phy_option = BLE_CODED_PHY_S2;
            }
            else if (param[0] == '4')
            {
                connect_info.phy = BLE_PHY_CODED;
                phy_param.coded_phy_option = BLE_CODED_PHY_S8;
            }
            else
            {
                PRINT_CTRL_CMD_ERROR("error parameter.");
                return;
            }
            phy_param.host_id = APP_TRSP_C_HOST_ID;
            phy_param.rx_phy = connect_info.phy;
            phy_param.tx_phy = connect_info.phy;
            ble_status = ble_cmd_phy_update(&phy_param);
            if (ble_status != BLE_ERR_OK)
            {
                printf("setBLE_Phy failed: %d.\n", ble_status);
                return;
            }

            // show result
            print_app_phy_param();
        }
        break;

    case BLEACTION_DATA_LENGTH_SET:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED)
        {
            uint32_t len;
            ble_err_t ble_status;

            action_status = true;

            convert_str_to_uint32((char *)param, &len);

            if ((len < 20) || (len > 244))
            {
                PRINT_CTRL_CMD_ERROR("error parameter.");
                return;
            }
            ble_status = ble_cmd_data_len_update(APP_TRSP_C_HOST_ID, len + 7);
            if (ble_status != BLE_ERR_OK)
            {
                printf("setBLE_DataLength failed: %d.\n", ble_status);
                return;
            }
            // show result
            print_app_data_len_param();
        }
        break;


    case BLEACTION_SEC_REQ_SET:
        if (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED)
        {
            ble_err_t ble_status;

            action_status = true;
            ble_status = ble_cmd_security_request_set(APP_TRSP_C_HOST_ID);
            if (ble_status != BLE_ERR_OK)
            {
                printf("setBLE_SecurityRequest failed: %d.", ble_status);
                return;
            }
        }
        break;

    case BLEACTION_CH_CLASSIFICATION_SET:
    {
        ble_err_t ble_status;
        char tempHexArray[5];

        action_status = true;
        // convert channel map
        convert_hexStr_to_hexArry((char *)param, (5 * 2), tempHexArray);
        ble_status = ble_cmd_host_ch_classif_set((ble_gap_host_ch_classif_t *)&tempHexArray[0]);
        if (ble_status != BLE_ERR_OK)
        {
            printf("setBLE_Channel classification failed: %d.", ble_status);
            return;
        }
    }
    break;

    case BLEACTION_CH_MAP_READ:
    {
        ble_err_t ble_status;

        action_status = true;
        ble_status = ble_cmd_channel_map_read(APP_TRSP_C_HOST_ID);
        if (ble_status != BLE_ERR_OK)
        {
            printf("Read channel map failed: %d.", ble_status);
            return;
        }
    }
    break;

    default:
        break;
    }

    // check status
    if (action_status == false)
    {
        PRINT_CTRL_CMD_ERROR("error state.");
    }
}


/** handle data rate test commands. */
void handle_test_cmd(App_DataTestAction action, uint8_t *param)
{
    bool action_status = false;

    switch (action)
    {
    case TESTACTION_TOTAL_TEST_TX_LEN_SET:
        if (app_DR_test_info.test_state == DATARATE_STATE_IDLE)
        {
            action_status = true;
            convert_str_to_uint32((char *)param, &app_DR_test_info.total_Tx_test_len);
            print_app_test_total_len_param();
        }
        break;

    case TESTACTION_MODE_SET:
        if (app_DR_test_info.test_state == DATARATE_STATE_IDLE)
        {
            action_status = true;
            if (param[0] == '1')
            {
                app_DR_test_info.test_mode = DATARATE_MODE_TX;
            }
            else if (param[0] == '2')
            {
                app_DR_test_info.test_mode = DATARATE_MODE_RX;
            }
            else
            {
                PRINT_CTRL_CMD_ERROR("error parameter.");
                return;
            }
            print_app_test_mode_param();
        }
        break;

    case TESTACTION_TEST_START:
        if ((app_DR_test_info.test_state == DATARATE_STATE_IDLE) && (ble_app_link_info[APP_TRSP_C_HOST_ID].state == STATE_CONNECTED))
        {
            action_status = true;
            app_DR_test_info.test_state = DATARATE_STATE_RUN;

            if (app_request_set(APP_TRSP_C_HOST_ID, APP_DATA_RATE_TEST_RUN, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        break;

    case TESTACTION_TEST_STOP:
        if (app_DR_test_info.test_state != DATARATE_STATE_IDLE)
        {
            action_status = true;
            app_DR_test_info.test_state = DATARATE_STATE_IDLE;
            if (app_request_set(APP_TRSP_C_HOST_ID, APP_DATA_RATE_TEST_STOP_RUN, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        break;

    default:
        break;
    }

    // check status
    if (action_status == false)
    {
        PRINT_CTRL_CMD_ERROR("error state.");
    }
}


/**************************************************************************
* Public Functions
**************************************************************************/
/** handle all ctrl commands. */
void handle_ctrl_cmd(uint8_t *data, int length)
{
    printf("\n<Command Processing> %s\n", data);

    CTRL_CMD_EValue cmd = chekc_ctrl_cmd(&data, length); // data will trim CMD string

    switch (cmd)
    {
    case CMD_HELP:
        print_ctrl_cmd_help();
        break;

    case CMD_GET_PARAM:
        print_app_param();
        break;

    // BLE Address Command
    case CMD_LADDR:
        handle_localAddr_command(data);
        break;

    case CMD_TADDR:
        handle_targetAddr_command(data);
        break;

    // BLE Common Command
    case CMD_CREATE_CON:
        handle_BLE_cmd(BLEACTION_CONNECT, data);
        break;

    case CMD_CANCEL_CON:
        handle_BLE_cmd(BLEACTION_CONNECT_CANCEL, data);
        break;

    case CMD_DISCON:
        handle_BLE_cmd(BLEACTION_DISCONNECT, data);
        break;

    case CMD_RF_PHY:
        handle_BLE_cmd(BLEACTION_PHY_SET, data);
        break;

    case CMD_DATA_LENGTH:
        handle_BLE_cmd(BLEACTION_DATA_LENGTH_SET, data);
        break;

    case CMD_CONN_INTERVAL:
        handle_BLE_cmd(BLEACTION_CONNECT_INT_SET, data);
        break;

    case CMD_SEC_REQ:
        handle_BLE_cmd(BLEACTION_SEC_REQ_SET, data);
        break;

    // Data Rate Test Command
    case CMD_TEST_TOTAL_TX_LEN:
        handle_test_cmd(TESTACTION_TOTAL_TEST_TX_LEN_SET, data);
        break;

    case CMD_TEST_MODE_SET:
        handle_test_cmd(TESTACTION_MODE_SET, data);
        break;

    case CMD_TEST_START_SET:
        handle_test_cmd(TESTACTION_TEST_START, data);
        break;

    case CMD_TEST_STOP_SET:
        handle_test_cmd(TESTACTION_TEST_STOP, data);
        break;

    case CMD_CH_CLASSIF_SET:
        handle_BLE_cmd(BLEACTION_CH_CLASSIFICATION_SET, data);
        break;

    case CMD_CH_MAP_READ:
        handle_BLE_cmd(BLEACTION_CH_MAP_READ, data);
        break;

    default:
        PRINT_CTRL_CMD_ERROR("this is not a control command.");
        break;
    }
}

