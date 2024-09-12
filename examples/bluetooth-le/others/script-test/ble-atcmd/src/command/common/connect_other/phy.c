#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void phy_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
static uint8_t g_host_id;
cmd_info_t phy =
{
    .cmd_name = "+PHY",
    .description = "phy",
    .init = phy_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void phy_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    ble_gap_phy_update_param_t phy;

    if (item->param_length == 4)
    {
        atcmd_param_type param_type_list[] = {INT, INT, INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        phy.host_id = param[0].num;
        phy.tx_phy = param[1].num;
        phy.rx_phy = param[2].num;
        phy.coded_phy_option = param[3].num;

        g_host_id = phy.host_id;

        ble_err_t status = ble_cmd_phy_update(&phy);
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_err_t status = ble_cmd_phy_read(0);
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PHY?\n"
        "  read PHY rate of host ID 0\n"
        "+PHY =  <num1>,<num2>,<num3>,<num4>\n"
        "  set PHY rate of specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : TX PHY\n"
        "      1:BLE_PHY_1M\n"
        "      2:BLE_PHY_2M\n"
        "      4:BLE_PHY_CODED\n"
        "    <num3> : RX PHY\n"
        "      1:BLE_PHY_1M\n"
        "      2:BLE_PHY_2M\n"
        "      4:BLE_PHY_CODED\n"
        "    <num4> : phy option when TX/RX choose BLE_PHY_CODED(4)\n"
        "      0:BLE_CODED_PHY_NO_PREFERRED\n"
        "      1:BLE_CODED_PHY_S2\n"
        "      2:BLE_CODED_PHY_S8\n"
        "    notice\n"
        "      TX PHY must equal to RX PHY\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_GAP_EVT_PHY_UPDATE && item->cmd_type == AT_CMD_TYPE_SET_COMMAND)
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)param;
        if (p_phy_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            item->status = AT_CMD_STATUS_OK;
        }
    }
    else if (event == BLE_GAP_EVT_PHY_READ && item->cmd_type == AT_CMD_TYPE_READ_COMMAND)
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)param;
        if (p_phy_param->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            item->status = AT_CMD_STATUS_OK;
        }
    }
    else if (event == BLE_GAP_EVT_DISCONN_COMPLETE)
    {
        ble_evt_gap_disconn_complete_t *p_disconn_param = (ble_evt_gap_disconn_complete_t *)param;
        if (p_disconn_param->host_id == g_host_id)
        {
            item->status = AT_CMD_STATUS_FAIL;
        }
    }
}
