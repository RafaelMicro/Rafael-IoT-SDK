#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void preferred_mtu_size_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t preferred_mtu_size =
{
    .cmd_name = "+PFMTUS",
    .description = "preferred mtu size",
    .init = preferred_mtu_size_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void preferred_mtu_size_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    uint8_t host_id;
    uint16_t mtu_size;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        mtu_size = param[1].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_default_mtu_size_set(host_id, mtu_size);
    if (status == BLE_ERR_OK)
    {
        item->ble_param->preferred_mtu_size = mtu_size;
    }
    item->status = AT_CMD_STATUS_OK;
    return status;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    printf("%u\n", item->ble_param->preferred_mtu_size);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PFMTUS = <num1>, <num2>\n"
        "set preferred MTU size of specific host ID\n"
        "  <num1>: host ID\n"
        "    range : 0-0\n"
        "  <num2>: preferred rx mtu size\n"
        "    range : 23-247\n"
    );
}
