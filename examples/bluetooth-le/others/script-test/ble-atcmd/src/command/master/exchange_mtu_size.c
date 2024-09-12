#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void exchange_mtu_size_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t exchange_mtu_size =
{
    .cmd_name = "+EXMTUS",
    .description = "exchange mtu size",
    .init = exchange_mtu_size_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void exchange_mtu_size_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    uint8_t host_id = 0;
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
    ble_err_t status = ble_cmd_mtu_size_update(host_id, mtu_size);
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+EXMTUS = <num1>, <num2>\n"
        "  exchange MTU size of specific host ID with server\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : Client rx mtu size\n"
        "      range : 23-247\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_ATT_GATT_EVT_MTU_EXCHANGE)
    {
        item->status = AT_CMD_STATUS_OK;
    }
}
