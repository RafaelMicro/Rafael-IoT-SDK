#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void create_connect_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t create_connect =
{
    .cmd_name = "+CRCON",
    .description = "create connect",
    .init = create_connect_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void create_connect_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    ble_gap_peer_addr_t peer_addr = item->ble_param->peer_addr;

    switch (item->param_length)
    {
    case 2:
    {
        atcmd_param_type param_type_list[] = {INT, ADDR};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        peer_addr.addr_type = param[0].num;
        memcpy(peer_addr.addr, param[1].addr, sizeof(peer_addr.addr));
    }
    break;

    default:
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_gap_create_conn_param_t *create_con_param = &item->ble_param->create_con_param;
    ble_scan_param_t *scan_param = &item->ble_param->scan_param;
    create_con_param->host_id = 0;
    create_con_param->init_filter_policy = scan_param->scan_filter_policy;
    create_con_param->scan_interval = scan_param->scan_interval;
    create_con_param->scan_window = scan_param->scan_window;
    memcpy(&create_con_param->peer_addr, &peer_addr, sizeof(peer_addr));

    ble_err_t status = ble_cmd_conn_create(create_con_param);
    if (status == BLE_ERR_OK)
    {
        item->ble_param->peer_addr.addr_type =  peer_addr.addr_type;
        memcpy(peer_addr.addr, param[1].addr, sizeof(peer_addr.addr));
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CRCON = <num1>, <addr>\n"
        "  create connection\n"
        "    <num1> : address type\n"
        "    <addr> : the address of device\n"
        "      format : XX:XX:XX:XX:XX:XX\n"
        "  ex.AT+CRCON=1,11:12:13:BB:BB:C4, random address 11:12:13:BB:BB:C4\n"
    );
}
