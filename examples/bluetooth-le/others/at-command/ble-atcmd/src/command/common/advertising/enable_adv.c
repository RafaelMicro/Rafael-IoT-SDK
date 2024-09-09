#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void enable_adv_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t enable_adv =
{
    .cmd_name = "+ENADV",
    .description = "enable adv",
    .init = enable_adv_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void enable_adv_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    uint8_t host_id;

    if (item->param_length == 0)
    {
        host_id = 1;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_adv_enable(host_id);
    item->status = AT_CMD_STATUS_OK;
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ENADV\n"
        "  enable advertising of host ID = 1 (Peripheral Role)\n"
    );
}
