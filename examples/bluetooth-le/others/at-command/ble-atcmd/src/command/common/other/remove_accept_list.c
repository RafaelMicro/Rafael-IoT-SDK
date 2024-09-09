#include "atcmd_command_list.h"
#include "ble_profile.h"

// PRIVATE FUNCTION DECLARE
static void remove_accept_list_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t remove_accept_list =
{
    .cmd_name = "+REMOVEFAL",
    .description = "remove filter accept list",
    .init = remove_accept_list_init
};

// PRIVATE FUNCTION IMPLEMENT
static void remove_accept_list_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    ble_gap_peer_addr_t list_addr = item->ble_param->peer_addr;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, ADDR};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        list_addr.addr_type = param[0].num;
        memcpy(list_addr.addr, param[1].addr, sizeof(list_addr.addr));

        ble_err_t status = ble_cmd_remove_device_from_accept_list((ble_filter_accept_list_t *)&list_addr);
        if (status == BLE_ERR_OK)
        {
            item->status = AT_CMD_STATUS_OK;
        }
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+REMOVEFAL = <num>,<addr>\n"
        "  remove filter accept list.\n"
        "    <num> : the type of address\n"
        "    <addr> : the filter accept list address\n"
        "      format : XX:XX:XX:XX:XX:XX\n"
        "      ex. 01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01\n"
        "    notice\n"
        "      If BLE Address Type is set to ramdom address,"
        "      addr[5] >= 0xC0 (the two most significant bits"
        "      of the address shall be equal to 1).\n"
    );
}
