#include "atcmd_command_list.h"
#include "ble_profile.h"

// PRIVATE FUNCTION DECLARE
static void clear_accept_list_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t clear_accept_list =
{
    .cmd_name = "+CLEARFAL",
    .description = "clear filter accept list",
    .init = clear_accept_list_init
};

// PRIVATE FUNCTION IMPLEMENT
static void clear_accept_list_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    if (item->param_length == 0)
    {
        ble_err_t status = ble_cmd_clear_filter_accept_list();
        if (status == BLE_ERR_OK)
        {
            item->status = AT_CMD_STATUS_OK;
        }
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CLEARFAL\n"
        "  clear filter accept list.\n"
    );
}
