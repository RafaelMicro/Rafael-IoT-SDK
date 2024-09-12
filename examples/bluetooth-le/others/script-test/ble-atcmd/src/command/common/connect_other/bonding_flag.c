#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void bonding_flag_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t bonding_flag =
{
    .cmd_name = "+SBOND",
    .description = "set bonding flag",
    .init = bonding_flag_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void bonding_flag_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        ble_sm_bonding_flag_param_t bond_flag;
        bond_flag.bonding_flag = param[0].num;
        ble_err_t status = ble_cmd_bonding_flag_set(&bond_flag);
        item->status = AT_CMD_STATUS_OK;
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+SBOND\n"
        "  set BLE Bonding Flag\n"
        "+SBOND = <num>\n"
        "  <num>: flag\n"
    );
}
