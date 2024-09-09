#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void create_connect_interval_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t create_connect_interval =
{
    .cmd_name = "+CCONINT",
    .description = "create connection interval",
    .init = create_connect_interval_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void create_connect_interval_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    ble_gap_create_conn_param_t *create_con_param = &item->ble_param->create_con_param;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        create_con_param->conn_param.min_conn_interval = param[0].num;
        create_con_param->conn_param.max_conn_interval = param[1].num;
        item->status = AT_CMD_STATUS_OK;
        return BLE_ERR_OK;
    }
    return BLE_ERR_INVALID_PARAMETER;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_gap_create_conn_param_t *create_con_param = &item->ble_param->create_con_param;
    printf("min:%u max:%u\n",
           create_con_param->conn_param.min_conn_interval,
           create_con_param->conn_param.max_conn_interval
          );
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CCONINT?\n"
        "  get the create connection interval\n"
        "+CCONINT = <num1>, <num2>\n"
        "  set the create connection interval\n"
        "  <num1> : the minimum create connection interval\n"
        "    range : 6-3200\n"
        "    interval = <num> * 1.25ms\n"
        "  <num2> : the maximum create connection interval\n"
        "    range : 6-3200\n"
        "    interval = <num> * 1.25ms\n"
        "  notice\n"
        "    <num1> must smaller <num2>\n"
        "    must match this formula : timeout * 4 > interval_max * (1+latency)\n"
    );
}
