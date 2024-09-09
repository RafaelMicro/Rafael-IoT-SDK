#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void preferred_data_length_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t preferred_data_length =
{
    .cmd_name = "+PREPLDATALEN",
    .description = "preferred payload data length",
    .init = preferred_data_length_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void preferred_data_length_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    uint16_t tx_octets;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        tx_octets = param[0].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_suggest_data_len_set(tx_octets);
    if (status == BLE_ERR_OK)
    {
        item->ble_param->preferred_tx_data_length = tx_octets;
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    printf("%u\n", item->ble_param->preferred_tx_data_length);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PREPLDATALEN?\n"
        "  get the preferred tx payload octets\n"
        "+PREPLDATALEN = <num1>\n"
        "  set the preferred tx max payload octets\n"
        "    <num1> : the tx max payload octets\n"
        "      range : 27-251\n"
    );
}
