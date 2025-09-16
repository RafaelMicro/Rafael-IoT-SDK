#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void help_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t help =
{
    .cmd_name = "+HELP",
    .description = "help",
    .init = help_init
};

// PRIVATE FUNCTION IMPLEMENT
static void help_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    cmd_list_print();
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
