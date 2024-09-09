#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void create_connect_timeout_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t create_connect_timeout =
{
    .cmd_name = "+CCONSUPTMO",
    .description = "create connection supervision timeout",
    .init = create_connect_timeout_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void create_connect_timeout_cmd_init(cmd_info_t *this)
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

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        create_con_param->conn_param.supv_timeout = param[0].num;
        item->status = AT_CMD_STATUS_OK;
        return BLE_ERR_OK;
    }
    return BLE_ERR_INVALID_PARAMETER;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_gap_create_conn_param_t *create_con_param = &item->ble_param->create_con_param;
    printf("%u\n", create_con_param->conn_param.supv_timeout);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CCONSUPTMO?\n"
        "  get the create connection supervision timeout\n"
        "+CCONSUPTMO = <num>\n"
        "  set the create connection supervision timeout\n"
        "  <num> : the create connection supervision timeout\n"
        "    range : 10-3200\n"
        "    supervision timeout = <num> * 10ms\n"
        "  notice\n"
        "    must match this formula : timeout * 4 > interval_max * (1+latency)\n"
    );
}
