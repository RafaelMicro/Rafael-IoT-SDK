#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void adv_filter_policy_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t adv_filter_policy =
{
    .cmd_name = "+ADVFP",
    .description = "adv filter policy",
    .init = adv_filter_policy_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void adv_filter_policy_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    ble_adv_param_t adv_param = item->ble_param->adv_param;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        adv_param.adv_filter_policy = param[0].num;
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
    printf("%u\n", adv_param->adv_filter_policy);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ADVFP?\n"
        "  get the advertising filter policy\n"
        "+ADVFP = <num>\n"
        "  set the advertising filter policy\n"
        "    <num> : the advertising filter policy\n"
        "    notice:\n"
        "      not support now\n"
    );
}

