#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void disable_adv_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t disable_adv =
{
    .cmd_name = "+DISADV",
    .description = "disable adv",
    .init = disable_adv_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void disable_adv_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_adv_disable();
    item->status = AT_CMD_STATUS_OK;
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DISADV\n"
        "  disable advertising\n"
    );
}
