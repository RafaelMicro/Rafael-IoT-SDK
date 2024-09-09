#include "atcmd_command_list.h"
#include "cm3_mcu.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void reset_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t reset =
{
    .cmd_name = "+RFRST",
    .description = "rf reset",
    .init = reset_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void reset_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->is_high_level_cmd = true;
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    // rf reset
    NVIC_SystemReset();
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+RFRST\n"
        "  reset SoC\n"
    );
}
