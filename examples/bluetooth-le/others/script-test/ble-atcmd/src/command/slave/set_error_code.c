#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void set_error_code_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t set_error_code =
{
    .cmd_name = "+SETERRORCODE",
    .description = "set error code",
    .init = set_error_code_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void set_error_code_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        uint8_t host_id = param[0].num;
        CHECK_HOST_ID(host_id);
        item->ble_param->error_code[host_id] = param[1].num;
        item->status = AT_CMD_STATUS_OK;
        return BLE_ERR_OK;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+SETERRORCODE = <num1>, <num2>\n"
        "  set error code for specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : the error code\n"
        "      range : 0-255\n"
    );
}
