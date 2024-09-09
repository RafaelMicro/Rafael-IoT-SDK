#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void adv_type_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t adv_type =
{
    .cmd_name = "+ADVTYPE",
    .description = "adv type",
    .init = adv_type_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void adv_type_cmd_init(cmd_info_t *this)
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
        adv_param.adv_type = param[0].num;
    }
    else if (item->param_length == 3)
    {
        atcmd_param_type param_type_list[] = {INT, INT, ADDR};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        adv_param.adv_type = param[0].num;
        adv_param.adv_peer_addr_param.addr_type = param[1].num;
        memcpy(adv_param.adv_peer_addr_param.addr, item->param[2].addr, sizeof(adv_param.adv_peer_addr_param.addr));
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
    printf("%u\n", adv_param->adv_type);
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ADVTYPE?\n"
        "  get advertising type\n"
        "+ADVTYPE = <num>\n"
        "  set advertising type\n"
        "    <num> : the advertising type\n"
        "       0: Connectable and scannable undirected advertising\n"
        "       1: Connectable directed advertising\n"
        "       2: Scanable undirected advertising\n"
        "       3: Non-Connectable undirected advertising\n"
        "+ADVTYPE = 1,<num>,<addr>\n"
        "  when set advertising type to 1(direct address mode), it has to provide an address as a target\n"
        "  <num> : the address type of device\n"
        "    0: public address\n"
        "    1: ramdom address\n"
        "  <addr> : the address of device\n"
        "    format : XX:XX:XX:XX:XX:XX\n"
        "      ex.01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01\n"
    );
}

