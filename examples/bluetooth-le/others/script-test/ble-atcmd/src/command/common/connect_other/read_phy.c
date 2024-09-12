#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void read_phy_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t read_phy =
{
    .cmd_name = "+READPHY",
    .description = "read phy",
    .init = read_phy_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void read_phy_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    uint8_t host_id = 0;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        host_id = param[0].num;
        ble_err_t status = ble_cmd_phy_read(host_id);
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+READPHY =  <num>\n"
        "  read PHY rate of specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-0\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_GAP_EVT_PHY_READ)
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)param;
        if (p_phy_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            item->status = AT_CMD_STATUS_OK;
        }
    }
}
