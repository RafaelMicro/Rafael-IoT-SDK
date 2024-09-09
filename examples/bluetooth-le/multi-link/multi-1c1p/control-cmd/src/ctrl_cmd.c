#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ctrl_cmd.h"
#include "ble_profile.h"
#include "ble_app.h"

/** check AT command is valid */
bool check_AT_Command_Length(CTRL_CMD_EValue cmd, uint8_t *data, uint16_t length)
{
    switch (cmd)
    {
    case UART_CMD_DATA:
        if ((*(data + strlen(ctrl_cmd_table[cmd])) == CTRL_CMD_ASSIGN_PUNC) && (length >= strlen(ctrl_cmd_table[cmd]) + 4)) // 4: = + param (addr char 12) + , + \n
        {
            return true;
        }
        break;

    case UART_CMD_DISCONN:
        if ((*(data + strlen(ctrl_cmd_table[cmd])) == CTRL_CMD_ASSIGN_PUNC) && ((strlen(ctrl_cmd_table[cmd]) + 3) == length)) // 3: = + param + \n
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
                return UART_CMD_NONE;
            }
        }
    }

    return UART_CMD_NONE;
}

/**************************************************************************
* Public Functions
**************************************************************************/
/** handle all ctrl commands. */
void handle_ctrl_cmd(uint8_t *data, int length)
{
    static uint16_t data_len = 0;
    static uint8_t host_id;

    if (data_len == 0)
    {
        printf("\n<Command Processing> %s\n", data);

        CTRL_CMD_EValue cmd = check_ctrl_cmd(&data, length); // data will trim CMD string
        switch (cmd)
        {
        case UART_CMD_HELP:
            print_ctrl_cmd_help();
            break;

        case UART_CMD_DATA:
        {
            uint8_t i;
            char *p_str;
            uint32_t str_pointer_value[3];
            uint16_t str_len;

            char delim[] = {CTRL_CMD_PARAM_SEP_PUNC};
            p_str = strtok((char *)data, delim);
            i = 0;
            while (p_str != NULL)
            {
                str_pointer_value[i] = (uint32_t)p_str;
                i++;
                p_str = strtok((char *)NULL, delim);
            }

            sscanf((char *)str_pointer_value[0], "%hhd", &host_id);
            if (ble_app_link_info[host_id].state == STATE_CONNECTED)
            {
                sscanf((char *)str_pointer_value[1], "%hd", &data_len);
                str_len = strlen((char *)str_pointer_value[2]);
                if (data_len < str_len)
                {
                    str_len = data_len;
                }
                if ((str_len + tx_index[host_id]) <= 1024)
                {
                    memcpy(&tx_data_buff[host_id][tx_index[host_id]], (void *)str_pointer_value[2], str_len);
                    tx_index[host_id] += str_len;
                }
                else
                {
                    memcpy(&tx_data_buff[host_id][tx_index[host_id]], (void *)str_pointer_value[2], (str_len - (1024 - tx_index[host_id])));
                    str_len = str_len - (1024 - tx_index[host_id]);
                    tx_index[host_id] = 0;
                    memcpy(&tx_data_buff[host_id][tx_index[host_id]], (void *)(str_pointer_value[2] + (str_len - (1024 - tx_index[host_id]))), str_len);
                    tx_index[host_id] += str_len;
                }
                data_len -= str_len;
                if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
                {
                    // No application queue buffer. Error.
                }
            }
            else
            {
                printf("Invalid State\n");
            }
        }
        break;

        case UART_CMD_ADV_START:
            if (app_request_set(1, APP_REQUEST_ADV_START, false) == false)
            {
                // No application queue buffer. Error.
            }
            break;

        case UART_CMD_ADV_STOP:
            if (app_request_set(1, APP_REQUEST_ADV_STOP, false) == false)
            {
                // No application queue buffer. Error.
            }
            break;

        case UART_CMD_SCAN_START:
            if (app_request_set(0, APP_REQUEST_SCAN_START, false) == false)
            {
                // No application queue buffer. Error.
            }
            break;

        case UART_CMD_SCAN_STOP:
            if (app_request_set(0, APP_REQUEST_SCAN_STOP, false) == false)
            {
                // No application queue buffer. Error.
            }
            break;

        case UART_CMD_DISCONN:
        {
            sscanf((char *)data, "%hhd", &host_id);
            if (app_request_set(host_id, APP_REQUEST_DISCONNECT, false) == false)
            {
                // No application queue buffer. Error.
            }
        }
        break;

        default:
            PRINT_CTRL_CMD_ERROR("this is not a correct command.");
            break;
        }
    }
    else
    {
        uint16_t str_len;

        printf("\n<Data Processing> %s\n", data);

        str_len = strlen((char *)data);
        if (data_len < str_len)
        {
            str_len = data_len;
        }
        if ((str_len + tx_index[host_id]) <= 1024)
        {
            memcpy(&tx_data_buff[host_id][tx_index[host_id]], data, str_len);
            tx_index[host_id] += str_len;
        }
        else
        {
            memcpy(&tx_data_buff[host_id][tx_index[host_id]], data, (str_len - (1024 - tx_index[host_id])));
            str_len = str_len - (1024 - tx_index[host_id]);
            tx_index[host_id] = 0;
            memcpy(&tx_data_buff[host_id][tx_index[host_id]], (void *)(data + (str_len - (1024 - tx_index[host_id]))), str_len);
            tx_index[host_id] += str_len;
        }
        data_len -= str_len;
        if (app_request_set(host_id, APP_REQUEST_DATA_SEND, false) == false)
        {
            // No application queue buffer. Error.
        }
    }
}

