#include "atcmd_command_list.h"
#include "ble_scan.h"

// PRIVATE FUNCTION DECLARE
static void disable_scan_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t disable_scan =
{
    .cmd_name = "+DISSCAN",
    .description = "disable scan",
    .init = disable_scan_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void disable_scan_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_scan_disable();
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DISSCAN\n"
        "  disable scan\n"
    );
}
