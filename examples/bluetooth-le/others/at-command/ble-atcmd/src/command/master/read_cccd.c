#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void read_cccd_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t read_cccd =
{
    .cmd_name = "+READCCCD",
    .description = "read CCCD",
    .init = read_cccd_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;
static uint16_t handle_num;

// PRIVATE FUNCTION IMPLEMENT
static void read_cccd_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        handle_num = param[1].num;
        CHECK_HOST_ID(host_id);
        ble_gatt_read_req_param_t read_req_param =
        {
            .host_id = host_id,
            .handle_num = handle_num
        };
        ble_err_t status = ble_cmd_gatt_read_req(&read_req_param);
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
        "+READCCCD = <num>, <num2>\n"
        "  read CCCD value for specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-1\n"
        "    notice :\n"
        "      0:disable notify & disable indicate\n"
        "      1:enable notify & disable indicate\n"
        "      2:disable notify & enable indicate\n"
        "      3:enable notify & enable indicate\n"
        "    <num2> : service handle value\n"
        "      range : 0-65535\n"
    );
}
