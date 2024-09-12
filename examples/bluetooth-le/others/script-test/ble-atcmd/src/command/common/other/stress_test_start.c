#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void stress_test_start_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t stress_test_start =
{
    .cmd_name = "+STSTART",
    .description = "stress test start",
    .init = stress_test_start_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void stress_test_start_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->is_high_level_cmd = true;
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    stress_test_t *stress_test = &item->ble_param->stress_test;

    stress_test->start(stress_test);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+STSTART\n"
        "  stress test start\n"
    );
}
