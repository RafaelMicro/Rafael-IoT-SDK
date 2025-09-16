#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void cancel_create_connect_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);


// PUBLIC VARIABLE DECLARE
cmd_info_t cancel_create_connect =
{
    .cmd_name = "+CCCON",
    .description = "cancel create connect",
    .init = cancel_create_connect_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void cancel_create_connect_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_conn_create_cancel();
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CCCON\n"
        "  cancel create connection\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_GAP_EVT_CONN_CANCEL)
    {
        item->status = AT_CMD_STATUS_OK;
    }
}
