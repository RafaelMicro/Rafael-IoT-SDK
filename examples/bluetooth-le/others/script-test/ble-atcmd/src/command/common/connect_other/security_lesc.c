#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void lesc_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t lesc_enable =
{
    .cmd_name = "+LESC",
    .description = "le secure connections",
    .init = lesc_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void lesc_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    if (item->param_length == 0)
    {
        ble_err_t status = ble_cmd_lesc_init();
        item->status = AT_CMD_STATUS_OK;
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+LESC\n"
        "request lesc function enable\n"
        "+LESC\n"
    );
}
