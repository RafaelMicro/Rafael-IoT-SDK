#include "atcmd_command_list.h"
#include "ble_profile.h"

// PRIVATE FUNCTION DECLARE
static void accept_list_size_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t accept_list_size =
{
    .cmd_name = "+READFALS",
    .description = "read filter accept list size",
    .init = accept_list_size_init
};

// PRIVATE FUNCTION IMPLEMENT
static void accept_list_size_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    if (item->param_length == 0)
    {
        ble_err_t status = ble_cmd_read_filter_accept_list_size();
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
        "+READFALS\n"
        "  read filter accept list size.\n"
    );
}
