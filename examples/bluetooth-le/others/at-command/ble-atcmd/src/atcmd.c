#include "atcmd.h"
#include "atcmd_init.h"
#include "atcmd_command_list.h"
#include "atcmd_helper.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/

/**************************************************************************************************
 *    LOCAL FUNCTIONS DECLARE
 *************************************************************************************************/
static void atcmd_cmd_run(atcmd_t *this);
static void atcmd_err_str_print(ble_err_t status);
static bool atcmd_matched_cmd_assign(char *cmd_str, atcmd_item_t *running_at_item);

/**************************************************************************************************
 *    FUNCTIONS IMPLEMENT
 *************************************************************************************************/
void atcmd_init(atcmd_t *this)
{
    atcmd_queue_init(&this->at_queue);
    at_cmd_item_init(&this->running_at_item, &this->ble_param);
    cmd_list_init();
}

ble_err_t atcmd_ble_param_init(atcmd_t *this)
{
    ble_err_t  status;

    status = ble_cmd_phy_controller_init();
    CHECK(status);
    status = ble_addr_init(this);
    CHECK(status);
    status = ble_scan_param_init(this);
    CHECK(status);
    status = ble_adv_param_init(this);
    CHECK(status);
    status = ble_adv_data_init(this);
    CHECK(status);
    status = ble_scan_rsp_init(this);
    CHECK(status);
    status = ble_atcmd_gap_device_name_init();
    CHECK(status);

    this->ble_param.preferred_mtu_size[0] = BLE_GATT_ATT_MTU_MAX;
    this->ble_param.preferred_mtu_size[1] = BLE_GATT_ATT_MTU_MAX;
    status = ble_cmd_default_mtu_size_set(1, BLE_GATT_ATT_MTU_MAX);
    CHECK(status);

    ble_con_param_init(this);
    ble_create_con_param_init(this);

    this->ble_param.preferred_tx_data_length = 27;

    return BLE_ERR_OK;
}

void atcmd_main_handle(atcmd_t *this)
{
    atcmd_cmd_run(this);
}

void atcmd_uart_handle(atcmd_t *this, uint8_t *data, uint8_t length)
{
    // remove '\n' or '\r'
    data[length - 1] = 0;
    char *str = (char *)data;

    // check is AT cmd or not
    if (strlen(str) < 2 || (toupper(str[0]) != 'A') || (toupper(str[1]) != 'T'))
    {
        printf("ERROR : this is not at command [%s] \n", data);
        return;
    }

    // remove AT
    str = str + 2;

    //split command by ';'
    char *cmd = my_strtok(str, AT_CMD_SEP_PUNC);
    while (cmd != NULL)
    {
        atcmd_queue_t *at_queue = &this->at_queue;

        // do high level cmd
        if (high_level_cmd_check(cmd))
        {
            while (!at_queue->empty(at_queue))
            {
                printf("2 at_queue->pop(%s) \n", at_queue->front(at_queue)->str);
                at_queue->pop(at_queue);
            }
            this->running_at_item.status = AT_CMD_STATUS_QUEUE;
        }

        //put command into queue
        atcmd_string_t tmp;
        if (strlen(cmd) > (sizeof(tmp.str) - 1))
        {
            printf("ERROR : this at command length is too long\r\n");
            return;
        }
        strcpy(tmp.str, cmd);
        printf("at_queue->Push(%s) \n", tmp.str);
        at_queue->push(at_queue, &tmp);

        cmd = my_strtok(NULL, AT_CMD_SEP_PUNC);
    }
    bool check = jump_to_main();
    CHECK_BOOL(check);
}

static void atcmd_cmd_run(atcmd_t *this)
{
    atcmd_item_t *running_at_item = &this->running_at_item;

    switch (running_at_item->status)
    {
    case AT_CMD_STATUS_BUSY:
        return;

    case AT_CMD_STATUS_OK:
    {
        printf(OK_STR);
        running_at_item->status = AT_CMD_STATUS_QUEUE;
    }
    break;

    case AT_CMD_STATUS_FAIL:
    {
        atcmd_err_str_print(running_at_item->err_status);
        running_at_item->status = AT_CMD_STATUS_QUEUE;
    }
    break;

    case AT_CMD_STATUS_QUEUE:
    {
        atcmd_queue_t *at_queue = &this->at_queue;
        if (at_queue->empty(at_queue))
        {
            return;
        }

        //get cmd_str from queue
        char *cmd_str = at_queue->front(at_queue)->str;
        printf("1 at_queue->pop(%s) \n", at_queue->front(at_queue)->str);
        at_queue->pop(at_queue);

        // assign cmd
        bool check = atcmd_matched_cmd_assign(cmd_str, running_at_item);
        if (!check)
        {
            running_at_item->status = AT_CMD_STATUS_FAIL;
            running_at_item->err_status = BLE_ERR_CMD_NOT_SUPPORTED;
            break;
        }

        //do command
        running_at_item->status = AT_CMD_STATUS_BUSY;
        cmd_info_t *cmd = running_at_item->cmd_info;
        printf("do cmd : %s\n", cmd->cmd_name);
        cmd->do_cmd(cmd, running_at_item);
    }
    break;

    default:
        CHECKNR(0xAA);
        return;
    }

    bool check = jump_to_main();
    CHECK_BOOL(check);
}

static bool atcmd_matched_cmd_assign(char *cmd_str, atcmd_item_t *running_at_item)
{
    bool check = true;

    //turn cmd string to item
    check = parse_cmd_string_to_item(cmd_str, running_at_item);
    if (!check)
    {
        printf("command [%s] length is too long\n", cmd_str);
        return check;
    }

    //assign cmd
    check = cmd_assign(running_at_item);
    if (!check)
    {
        printf("command [%s] doesn't exist\n", running_at_item->cmd_str);
        return check;
    }

    return check;
}

static void atcmd_err_str_print(ble_err_t status)
{
    switch (status)
    {
    case BLE_BUSY:
        printf("ERROR BUSY\r\n");
        break;
    case BLE_ERR_INVALID_PARAMETER:
        printf("ERROR INVALID_PARAM\r\n");
        break;
    case BLE_ERR_INVALID_STATE:
        printf("ERROR INVALID_STATE\r\n");
        break;
    case BLE_ERR_INVALID_HOST_ID:
        printf("ERROR INVALID_HOST_ID\r\n");
        break;
    case BLE_ERR_INVALID_HANDLE:
        printf("ERROR INVALID_HANDLE\r\n");
        break;
    case BLE_ERR_CMD_NOT_SUPPORTED:
        printf("ERROR CMD_NOT_SUPPORTED\r\n");
        break;
    case BLE_ERR_DB_PARSING_IN_PROGRESS:
        printf("ERROR DB_PARSING_IN_PROGRESS\r\n");
        break;
    case BLE_ERR_SEQUENTIAL_PROTOCOL_VIOLATION:
        printf("ERROR SEQUENTIAL_PROTOCOL_VIOLATION\r\n");
        break;
    case BLE_ERR_WRONG_CONFIG:
        printf("ERROR WRONG_CONFIG\r\n");
        break;
    default:
        printf("ERROR %02d\r\n", status);
        break;
    }
}
