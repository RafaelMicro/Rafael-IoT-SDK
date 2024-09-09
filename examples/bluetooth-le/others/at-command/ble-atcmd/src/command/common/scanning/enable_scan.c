#include "atcmd_command_list.h"
#include "ble_scan.h"

// PRIVATE FUNCTION DECLARE
static void enable_scan_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t enable_scan =
{
    .cmd_name = "+ENSCAN",
    .description = "enable scan",
    .init = enable_scan_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void enable_scan_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_gap_peer_addr_t *peer_addr = &item->ble_param->peer_addr;
    ble_gap_peer_addr_t *scan_addr = &item->ble_param->scan_addr;

    if (item->param_length == 0)
    {
        memcpy(scan_addr, peer_addr, sizeof(ble_gap_peer_addr_t));
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_scan_enable();
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ENSCAN\n"
        "  enable scan"
    );
}
