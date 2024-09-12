#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void write_cccd_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t write_cccd =
{
    .cmd_name = "+WRITECCCD",
    .description = "write CCCD",
    .init = write_cccd_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;

// PRIVATE FUNCTION IMPLEMENT
static void write_cccd_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
    this->check_service = check_service;
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
        ble_gatt_cccd_val_t cccd_value = param[1].num;
        CHECK_HOST_ID(host_id);
        uint16_t handle_num = ble_info_link[host_id].svcs_info_atcmd.info.handles.hdl_charac01_cccd;
        ble_err_t status = ble_svcs_cccd_set(host_id, handle_num, cccd_value);
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        " +WRITECCCD\n"
        "+WRITECCCD = <num1>, <num2>\n"
        "  write CCCD value for specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : the cccd value\n"
        "      0:disable notify & disable indicate\n"
        "      1:enable notify & disable indicate\n"
        "      2:disable notify & enable indicate\n"
        "      3:enable notify & enable indicate\n"
    );
}
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item)
{
    if (p_param->event == BLESERVICE_ATCMD_CHARAC01_CCCD_WRITE_RSP_EVENT && p_param->host_id == host_id)
    {
        item->status = AT_CMD_STATUS_OK;
    }
}
