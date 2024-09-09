#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void init_bonding_info_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t init_bonding_info =
{
    .cmd_name = "+IBOND",
    .description = "init bonding info",
    .init = init_bonding_info_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void init_bonding_info_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_bonding_space_init();
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+IBOND\n"
        "  initial BLE Bonding information\n"
    );
}
