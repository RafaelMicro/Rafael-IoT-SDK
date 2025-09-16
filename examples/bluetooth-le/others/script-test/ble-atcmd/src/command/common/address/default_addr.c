#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void default_addr_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t default_addr =
{
    .cmd_name = "+DADDR",
    .description = "default address",
    .init = default_addr_init
};

// PRIVATE FUNCTION IMPLEMENT
static void default_addr_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_gap_peer_addr_t *peer_addr = &item->ble_param->peer_addr;
    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {ADDR};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        memcpy(peer_addr->addr, item->param[0].addr, sizeof(peer_addr->addr));
    }
    else if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, ADDR};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        peer_addr->addr_type = item->param[0].num;
        memcpy(peer_addr->addr, item->param[1].addr, sizeof(peer_addr->addr));
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_gap_peer_addr_t *peer_addr = &item->ble_param->peer_addr;
    char str[ADDR_FORMAT_SIZE + 1];
    parse_addr_array_to_string(str, peer_addr->addr);
    printf("addr = %s, addr type = %u\n", str, peer_addr->addr_type);

    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DADDR?\n"
        "  get the default address for +CRCON, +ENSCAN\n"
        "+DADDR = <addr>\n"
        "    <addr> : the default address\n"
        "+DADDR = <num>,<addr>\n"
        "  set the default address for +CRCON, +ENSCAN\n"
        "    <num> : the type of default address\n"
        "    <addr> : the default address\n"
        "      format : XX:XX:XX:XX:XX:XX\n"
        "      ex. 01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01\n"
        "    notice\n"
        "      If BLE Address Type is set to ramdom address, addr[5] >= 0xC0 (the two most significant bits of the address shall be equal to 1).\n"
    );
}

