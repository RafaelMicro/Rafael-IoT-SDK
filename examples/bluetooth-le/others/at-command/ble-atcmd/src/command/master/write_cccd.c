#include "atcmd_command_list.h"
#include "project_config.h"

// PRIVATE FUNCTION DECLARE
static void write_cccd_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t write_cccd =
{
    .cmd_name = "+WRITECCCD",
    .description = "write CCCD",
    .init = write_cccd_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;
static uint16_t handle_num;

// PRIVATE FUNCTION IMPLEMENT
static void write_cccd_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    bool check = true;

    if (item->param_length == 3)
    {
        atcmd_param_type param_type_list[] = {INT, INT, INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        CHECK_HOST_ID(host_id);
        handle_num = param[1].num;
        ble_gatt_cccd_val_t cccd_value = param[2].num;
        ble_err_t status = ble_svcs_cccd_set(host_id, handle_num, cccd_value);
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
        " +WRITECCCD\n"
        "+WRITECCCD = <num1>, <num2>, <num3>\n"
        "  write CCCD value for specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : service handle value\n"
        "      range : 0-65535\n"
        "    <num3> : the cccd value\n"
        "      0:disable notify & disable indicate\n"
        "      1:enable notify & disable indicate\n"
        "      2:disable notify & enable indicate\n"
        "      3:enable notify & enable indicate\n"
    );
}
