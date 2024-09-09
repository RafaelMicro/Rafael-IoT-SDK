#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void data_length_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t data_length =
{
    .cmd_name = "+PLDATALEN",
    .description = "payload data length",
    .init = data_length_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void data_length_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    uint8_t host_id;
    uint16_t tx_octets;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        tx_octets = param[1].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_err_t status = ble_cmd_data_len_update(host_id, tx_octets);
    if (status == BLE_ERR_OK)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PLDATALEN = <num1>,<num2>\n"
        "  set the tx max payload octets of specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-1\n"
        "    <num2> : the tx max payload octets\n"
        "      range : 27-251\n"
    );
}
