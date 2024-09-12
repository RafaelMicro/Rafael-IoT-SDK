#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void adv_channel_map_cmd_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t adv_channel_map =
{
    .cmd_name = "+ADVCHMAP",
    .description = "adv channel map",
    .init = adv_channel_map_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void adv_channel_map_cmd_init(cmd_info_t *this)
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
        adv_param.adv_channel_map = param[0].num;
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
    printf("%u\n", adv_param->adv_channel_map);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ADVCHMAP?\n"
        "  get the advertising channel\n"
        "+ADVCHMAP = <num>\n"
        "  set the advertising channel\n"
        "    <num> : the advertising channel\n"
        "      range : 0-7\n"
        "      bit1: channel 37\n"
        "      bit2: channel 38\n"
        "      bit3: channel 39\n"
    );
}

