#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void write_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t write =
{
    .cmd_name = "+WRITE",
    .description = "write",
    .init = write_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;
static uint16_t handle_num;

// PRIVATE FUNCTION IMPLEMENT
static void write_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    if (item->param_length == 4)
    {
        atcmd_param_type param_type_list[] = {INT, INT, INT, RAW};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        CHECK_HOST_ID(host_id);
        handle_num = param[1].num;
        int data_length = param[2].num;
        ble_gatt_data_param_t data_param =
        {
            .host_id = host_id,
            .handle_num = handle_num,
            .p_data = (uint8_t *) &param[3].raw,
            .length = data_length
        };
        ble_err_t status = ble_cmd_gatt_write_req(&data_param);
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
        "+WRITE = <num1>, <num2>, <num3>, <raw>\n"
        "  send write value for specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-1\n"
        "    <num2> : service handle value\n"
        "      range : 0-65535\n"
        "    <num3> : the num of write length\n"
        "      range : 0-244\n"
        "    <raw> : raw data\n"
        "    notice: the num of write value length should lower than (mtu size - 3)\n"
    );
}
