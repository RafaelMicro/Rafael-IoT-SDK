#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void scan_window_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t scan_window =
{
    .cmd_name = "+SCANWIN",
    .description = "scan window",
    .init = scan_window_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void scan_window_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    ble_scan_param_t scan_param = item->ble_param->scan_param;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        scan_param.scan_window = param[0].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_scan_param_set(&scan_param);
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
        item->ble_param->scan_param = scan_param;
    }
    return status;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_scan_param_t *scan_param = &item->ble_param->scan_param;
    printf("scan window = %d\n", scan_param->scan_window);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+SCANWIN?\n"
        "  get the scan window\n"
        "+SCANWIN = <num>\n"
        "  set scan window\n"
        "  <num> : the scan window\n"
        "    range : 4-16384\n"
        "    window = <num> * 0.625ms\n"
    );
}

