#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ctrl_cmd.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_security_manager.h"
#include "ble_scan.h"

/**************************************************************************
* Private Functions
**************************************************************************/
/** hex string to hex byte array converter. */
void convert_hexStr_to_hexArry(char *hexStr, uint8_t strLen, char *hexArray)
{
    int count = 0;

    for (count = 0; count < strLen; count++)
    {
        sscanf(hexStr, "%02hx", (unsigned short *)&hexArray[count]);
        hexStr += 2;
    }
}

/** string to uint32 converter. */
void convert_str_to_uint32(char *hexStr, uint32_t *value)
{
    sscanf(hexStr, "%d", value);
}

/** check AT command is valid */
bool check_AT_Command_Length(CTRL_CMD_EValue cmd, uint8_t *data, uint16_t length)
{
    switch (cmd)
    {
    case AT_COMMAND_TARGET_ADDR_SET:
        if ((*(data + strlen(ctrl_cmd_table[cmd])) == AT_CMD_ASSIGN_PUNC) && (length == strlen(ctrl_cmd_table[cmd]) + 14)) // 14: = + param (addr char 12) + \n
        {
            return true;
        }
        break;

    case AT_COMMAND_ADV_INTERVAL:
    case AT_COMMAND_ADV_TYPE:
    case AT_COMMAND_CONN_INTERVAL:
    case AT_COMMAND_RF_PHY:
    case AT_COMMAND_SCAN_TYPE:
    case AT_COMMAND_SCAN_INT_WINDOW:
        if ((*(data + strlen(ctrl_cmd_table[cmd])) == AT_CMD_ASSIGN_PUNC) && ((strlen(ctrl_cmd_table[cmd]) + 3) == length)) // 3: = + param + \n
        {
            return true;
        }
        break;

    default:
        if (strlen(ctrl_cmd_table[cmd]) == (length - 1))
        {
            return true;
        }
        break;
    }
    return false;
}

/** Qruey ctrl command and retrun data with parameter only. */
CTRL_CMD_EValue check_ctrl_cmd(uint8_t **data, uint16_t length)
{
    uint8_t i = 0;
    uint8_t new_len = 0;
    int commandSize = sizeof(ctrl_cmd_table) / sizeof(ctrl_cmd_table[0]);


    for (i = 0; i < commandSize; i++)
    {
        if ((strncmp(ctrl_cmd_table[i], (char *)(*data), strlen(ctrl_cmd_table[i])) == 0) &&
                check_AT_Command_Length((CTRL_CMD_EValue)i, *data, length) == true)
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
                return AT_COMMAND_NONE;
            }
        }
    }
    return AT_COMMAND_NONE;
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

/** handle advertisement related AT commands. */
void handle_adv_command(CTRL_CMD_EValue index, uint8_t *param)
{
    // power measurement tool only can active 1 role.
    if (appParam.active_role == APP_ACTIVE_CENTRAL)
    {
        PRINT_CTRL_CMD_ERROR("already in central mode");
        return;
    }

    switch (index)
    {
    case AT_COMMAND_ADV_TYPE:
        if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
        {
            PRINT_CTRL_CMD_ERROR("invalid state");
            return;
        }

        switch (param[0])
        {
        case '1': // ADV_TYPE_ADV_IND
            appParam.app_param_p.adv_param.adv_type = ADV_TYPE_ADV_IND;
            break;
        case '2': // ADV_TYPE_ADV_NONCONN_IND
            appParam.app_param_p.adv_param.adv_type = ADV_TYPE_ADV_NONCONN_IND;
            break;
        default:
            PRINT_CTRL_CMD_ERROR("invalid parameter");
            return;
        }
        print_app_adv_param();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_ADV_INTERVAL:
        if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
        {
            PRINT_CTRL_CMD_ERROR("invalid state");
            return;
        }

        switch (param[0])
        {
        case '1': // 100ms
            appParam.app_param_p.adv_param.adv_interval_max = 160;
            appParam.app_param_p.adv_param.adv_interval_min = 160;
            break;
        case '2': // 500ms
            appParam.app_param_p.adv_param.adv_interval_max = 800;
            appParam.app_param_p.adv_param.adv_interval_min = 800;
            break;
        case '3': // 1000ms
            appParam.app_param_p.adv_param.adv_interval_max = 1600;
            appParam.app_param_p.adv_param.adv_interval_min = 1600;
            break;
        default:
            PRINT_CTRL_CMD_ERROR("invalid parameter");
            return;
        }
        print_app_adv_param();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_ADV_ENABLE:
        // power measurement tool only can active 1 role.
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            PRINT_CTRL_CMD_ERROR("already in central mode");
        }
        else
        {
            appParam.active_role = APP_ACTIVE_PERIPHERAL;
            if (app_request_set(APP_POWER_MEASUREMENT_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
            {
                // No application queue buffer. Error.
            }
            PRINT_AT_CMD_OK();
        }
        break;

    case AT_COMMAND_ADV_DISABLE:
        // power measurement tool only can active 1 role.
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            PRINT_CTRL_CMD_ERROR("already in central mode");
        }
        else
        {
            appParam.active_role = APP_ACTINE_NONE;
            if (app_request_set(APP_POWER_MEASUREMENT_P_HOST_ID, APP_REQUEST_ADV_STOP, false) == false)
            {
                // No application queue buffer. Error.
            }
            PRINT_AT_CMD_OK();
        }
        break;

    default:
        break;
    }
}

/** handle scan related AT commands. */
void handle_scan_command(CTRL_CMD_EValue index, uint8_t *param)
{
    // power measurement tool only can active 1 role.
    if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
    {
        PRINT_CTRL_CMD_ERROR("already in peripheral mode");
        return;
    }

    switch (index)
    {
    case AT_COMMAND_SCAN_TYPE:
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            PRINT_CTRL_CMD_ERROR("invalid state");
            return;
        }

        switch (param[0])
        {
        case '1': // SCAN_TYPE_PASSIVE
            appParam.app_param_c.scan_param.scan_type = SCAN_TYPE_PASSIVE;
            break;
        case '2': // SCAN_TYPE_ACTIVE
            appParam.app_param_c.scan_param.scan_type = SCAN_TYPE_ACTIVE;
            break;
        default:
            PRINT_CTRL_CMD_ERROR("invalid parameter");
            return;
        }
        print_app_scan_param();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_SCAN_INT_WINDOW:
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            PRINT_CTRL_CMD_ERROR("invalid state");
            return;
        }

        switch (param[0])
        {
        case '1': // interval = 20ms window = 20ms
            appParam.app_param_c.scan_param.scan_interval = 32;
            appParam.app_param_c.scan_param.scan_window = 32;
            break;
        case '2': // interval = 40ms window = 20ms
            appParam.app_param_c.scan_param.scan_interval = 64;
            appParam.app_param_c.scan_param.scan_window = 32;
            break;
        default:
            PRINT_CTRL_CMD_ERROR("invalid parameter");
            return;
        }
        print_app_scan_param();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_SCAN_ENABLE:
        appParam.active_role = APP_ACTIVE_CENTRAL;
        if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_SCAN_START, false) == false)
        {
            // No application queue buffer. Error.
        }
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_SCAN_DISABLE:
        appParam.active_role = APP_ACTINE_NONE;
        if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_SCAN_STOP, false) == false)
        {
            // No application queue buffer. Error.
        }
        PRINT_AT_CMD_OK();
        break;

    default:
        break;
    }
}

/** handle connection interval setting AT commands. */
void handle_conn_interval_command(uint8_t *param)
{
    switch (param[0])
    {
    case '1': // 7.5ms
        appParam.conn_param.ble_conn_param.min_conn_interval = 6;
        appParam.conn_param.ble_conn_param.max_conn_interval = 6;
        break;

    case '2': // 30ms
        appParam.conn_param.ble_conn_param.min_conn_interval = 24;
        appParam.conn_param.ble_conn_param.max_conn_interval = 24;
        break;

    case '3': // 1000ms
        appParam.conn_param.ble_conn_param.min_conn_interval = 800;
        appParam.conn_param.ble_conn_param.max_conn_interval = 800;
        break;

    default:
        PRINT_CTRL_CMD_ERROR("invalid parameter");
        return;
    }

    print_app_conn_param();

    if (appParam.active_role != APP_ACTINE_NONE)
    {
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_CONN_UPDATE_PARAM, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        else if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_P_HOST_ID, APP_REQUEST_CONN_UPDATE_PARAM, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
    }
}


/** handle phy setting AT commands. */
void handle_phy_command(uint8_t *param)
{
    // set param
    switch (param[0])
    {
    case '1': // 1M
        appParam.rf_phy = BLE_PHY_1M;
        appParam.phy_option = BLE_CODED_PHY_NO_PREFERRED;
        break;
    case '2': // 2M
        appParam.rf_phy = BLE_PHY_2M;
        appParam.phy_option = BLE_CODED_PHY_NO_PREFERRED;
        break;
    case '3': // CODED PHY S2
        appParam.rf_phy = BLE_PHY_CODED;
        appParam.phy_option = BLE_CODED_PHY_S2;
        break;
    case '4':
        appParam.rf_phy = BLE_PHY_CODED;
        appParam.phy_option = BLE_CODED_PHY_S8;
        break;

    default:
        PRINT_CTRL_CMD_ERROR("invalid parameter");
        return;
    }

    print_app_phy_param();

    if (appParam.active_role != APP_ACTINE_NONE)
    {
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_PHY_UPDATE, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        else if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_P_HOST_ID, APP_REQUEST_PHY_UPDATE, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
    }
}

/**************************************************************************
* Public Functions
**************************************************************************/
/** handle all ctrl commands. */
void handle_ctrl_cmd(uint8_t *data, int length)
{
    CTRL_CMD_EValue cmd = check_ctrl_cmd(&data, length); // data will trim CMD string

    switch (cmd)
    {
    case AT_COMMAND_TEST:
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_VERSION:
        print_app_tool_version();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_HELP:
        print_ctrl_cmd_help();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_TARGET_ADDR_SET:
        handle_targetAddr_command(data);
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_APPPARAM:
        print_app_param();
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_ADV_INTERVAL:
    case AT_COMMAND_ADV_TYPE:
    case AT_COMMAND_ADV_ENABLE:
    case AT_COMMAND_ADV_DISABLE:
        handle_adv_command(cmd, data);
        break;

    case AT_COMMAND_SCAN_TYPE:
    case AT_COMMAND_SCAN_INT_WINDOW:
    case AT_COMMAND_SCAN_ENABLE:
    case AT_COMMAND_SCAN_DISABLE:
        handle_scan_command(cmd, data);
        break;

    case AT_COMMAND_CREATE_CONN:
        appParam.active_role = APP_ACTIVE_CENTRAL;
        if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_CREATE_CONN, false) == false)
        {
            // No application queue buffer. Error.
        }
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_CANCEL_CONN:
        if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_CREATE_CONN_CANCEL, false) == false)
        {
            // No application queue buffer. Error.
        }
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_DISCONN:
        if (appParam.active_role == APP_ACTIVE_CENTRAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_C_HOST_ID, APP_REQUEST_DISCONNECT, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        else if (appParam.active_role == APP_ACTIVE_PERIPHERAL)
        {
            if (app_request_set(APP_POWER_MEASUREMENT_P_HOST_ID, APP_REQUEST_DISCONNECT, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_CONN_INTERVAL:
        handle_conn_interval_command(data);
        PRINT_AT_CMD_OK();
        break;

    case AT_COMMAND_RF_PHY:
        handle_phy_command(data);
        PRINT_AT_CMD_OK();
        break;

    default:
        PRINT_CTRL_CMD_ERROR("this is not an AT command");
        break;
    }
}

