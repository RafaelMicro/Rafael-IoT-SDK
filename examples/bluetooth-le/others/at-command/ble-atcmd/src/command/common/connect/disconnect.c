#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void disconnect_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);


// PUBLIC VARIABLE DECLARE
cmd_info_t disconnect =
{
    .cmd_name = "+DISCON",
    .description = "disconnect",
    .init = disconnect_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void disconnect_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    uint8_t host_id;
    atcmd_param_block_t *param = item->param;
    bool check = true;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_conn_terminate(host_id);
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DISCON = <num>\n"
        "  disable connection of specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-1\n"
    );
}
