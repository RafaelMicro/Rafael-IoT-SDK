#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);
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
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    uint8_t host_id;
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 0)
    {
        host_id = 0;
    }
    else if (item->param_length == 1 && parse_param_to_int(&(param[0])))
    {
        host_id = param[0].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_adv_enable(host_id);
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ENADV = <num>\n"
        "  enable advertising of specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-0\n"
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
