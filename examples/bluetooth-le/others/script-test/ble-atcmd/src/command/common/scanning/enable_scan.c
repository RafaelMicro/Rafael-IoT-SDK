#include "atcmd_command_list.h"
#include "ble_scan.h"

// PRIVATE FUNCTION DECLARE
static void enable_scan_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);

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
    this->check_event = check_event;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    ble_gap_peer_addr_t *peer_addr = &item->ble_param->peer_addr;
    ble_gap_peer_addr_t *scan_addr = &item->ble_param->scan_addr;
    bool check = true;

    if (item->param_length == 0)
    {
        memcpy(scan_addr, peer_addr, sizeof(ble_gap_peer_addr_t));
    }
    else if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {ADDR};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        memcpy(scan_addr->addr, param[0].addr, sizeof(scan_addr->addr));
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_scan_enable();
    if (status == BLE_ERR_OK)
    {
        item->ble_param->scan_mode = SCANMODE_PRINT_ONCE;
    }
    return status;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ENSCAN = <addr>\n"
        "  enable scan. get one scan response or get three adv data without"
        "  any scan response, then print\n"
        "  <addr> : the address of device which be scanned\n"
        "    format : XX:XX:XX:XX:XX:XX\n"
    );
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    if (event == BLE_SCAN_EVT_SET_ENABLE)
    {
        ble_evt_scan_set_scan_enable_t *p_scan_enable = (ble_evt_scan_set_scan_enable_t *)param;
        if (p_scan_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            item->status = AT_CMD_STATUS_OK;
        }
    }
}
