#include "atcmd_command_list.h"
#include "hosal_lpm.h"

// PRIVATE FUNCTION DECLARE
static void sleep_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t sleep =
{
    .cmd_name = "+SLEEP",
    .description = "sleep",
    .init = sleep_init
};

// PRIVATE FUNCTION IMPLEMENT
static void sleep_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    hosal_lpm_ioctrl(HOSAL_LPM_UNMASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+SLEEP\n"
        "  Set the device to go into sleep when it is idle.\n"
    );
}
