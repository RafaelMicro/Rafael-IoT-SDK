#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void read_mtu_size_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t read_mtu_size =
{
    .cmd_name = "+RMTUS",
    .description = "read mtu size",
    .init = read_mtu_size_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void read_mtu_size_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    uint8_t host_id;
    uint16_t mtu_size;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_mtu_size_get(host_id, &mtu_size);
    if (status == BLE_ERR_OK)
    {
        printf("mtu size = %d\n", mtu_size);
        item->status = AT_CMD_STATUS_OK;
    }

    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+RMTUS = <num>\n"
        "  read MTU size of specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-0\n"
    );
}
