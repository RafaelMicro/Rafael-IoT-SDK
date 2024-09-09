#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void dev_addr_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t dev_addr =
{
    .cmd_name = "+DEVADDR",
    .description = "device address",
    .init = dev_addr_init
};

// PRIVATE FUNCTION IMPLEMENT
static void dev_addr_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    ble_gap_addr_t *device_addr = &item->ble_param->device_addr;
    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {ADDR};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        memcpy(device_addr->addr, item->param[0].addr, sizeof(device_addr->addr));
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    ble_err_t status = ble_cmd_device_addr_set(device_addr);
    item->status = AT_CMD_STATUS_OK;
    return status;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_gap_addr_t p_addr;
    ble_err_t status = ble_cmd_device_addr_get(&p_addr);
    char addr_str[ADDR_FORMAT_SIZE + 1];
    parse_addr_array_to_string(addr_str, p_addr.addr);
    printf("addr = %s\n", addr_str);
    item->status = AT_CMD_STATUS_OK;
    return status;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DEVADDR?\n"
        "  get the address of device\n"
        "+DEVADDR = <addr>\n"
        "  set the address of device\n"
        "  <addr> : the address of device\n"
        "    format : XX:XX:XX:XX:XX:XX\n"
        "      ex.01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01\n"
        "  notice\n"
        "    If BLE Address Type is set to ramdom address,\n"
        "    addr[5] >= 0xC0(the two most significant bits\n"
        "    of the address shall be equal to 1).\n"
    );
}

