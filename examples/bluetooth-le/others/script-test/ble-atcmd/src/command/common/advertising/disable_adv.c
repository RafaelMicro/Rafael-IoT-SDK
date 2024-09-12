#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);
static void disable_adv_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t disable_adv =
{
    .cmd_name = "+DISADV",
    .description = "disable adv",
    .init = disable_adv_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void disable_adv_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_adv_disable();
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DISADV\n"
        "  disable advertising\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_ADV_EVT_SET_ENABLE)
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = (ble_evt_adv_set_adv_enable_t *)param;
        if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            item->status = AT_CMD_STATUS_OK;
        }
    }
}
