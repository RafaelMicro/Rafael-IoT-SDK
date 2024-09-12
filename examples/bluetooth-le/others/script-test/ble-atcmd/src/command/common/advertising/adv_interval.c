#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void adv_interval_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t adv_interval =
{
    .cmd_name = "+ADVINT",
    .description = "adv interval",
    .init = adv_interval_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void adv_interval_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_gap_addr_t addr_param;
    atcmd_param_block_t *param = item->param;
    ble_adv_param_t adv_param = item->ble_param->adv_param;

    ble_cmd_device_addr_get(&addr_param);

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        adv_param.adv_interval_min = param[0].num;
        adv_param.adv_interval_max = param[1].num;
        adv_param.own_addr_type = addr_param.addr_type;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_adv_param_set(&adv_param);
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
        item->ble_param->adv_param = adv_param;
    }
    return status;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_adv_param_t *adv_param = &item->ble_param->adv_param;
    printf("min:%u max:%u\n", adv_param->adv_interval_min, adv_param->adv_interval_max);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ADVINT?\n"
        "  get the advertising interval\n"
        "+ADVINT = <num1>, <num2>\n"
        "  set the advertising interval\n"
        "    <num1> : the minimum advertising interval\n"
        "      range : 32-16384\n"
        "      interval = <num> * 0.625ms\n"
        "    <num2> : the maximum advertising interval\n"
        "      range : 32-16384\n"
        "      interval = <num> * 0.625ms\n"
        "    notice\n"
        "       <num1> must smaller <num2>\n"
    );
}

