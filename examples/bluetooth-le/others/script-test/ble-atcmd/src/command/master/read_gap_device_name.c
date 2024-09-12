#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void read_gap_device_name_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t read_gap_device_name =
{
    .cmd_name = "+READDEVNAME",
    .description = "read gap device name",
    .init = read_gap_device_name_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;

// PRIVATE FUNCTION IMPLEMENT
static void read_gap_device_name_cmd_init(cmd_info_t *this)
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

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        CHECK_HOST_ID(host_id);
        uint16_t handle_num;
        if (link_info[host_id].role == BLE_GATT_ROLE_SERVER)
        {
            handle_num = ble_info_link[host_id].svcs_info_gaps.server_info.handles.hdl_device_name;
        }
        else
        {
            handle_num = ble_info_link[host_id].svcs_info_gaps.client_info.handles.hdl_device_name;
        }
        ble_gatt_read_req_param_t read_req_param =
        {
            .host_id = host_id,
            .handle_num = handle_num
        };
        ble_err_t status = ble_cmd_gatt_read_req(&read_req_param);
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+READDEVNAME = <num>\n"
        "  read GAP device name for specific host ID\n"
        "    <num> : host ID\n"
        "      range : 0-0\n"
    );
}
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item)
{
    if (p_param->event == BLESERVICE_GAPS_DEVICE_NAME_READ_RSP_EVENT && p_param->host_id == host_id)
    {
        item->status = AT_CMD_STATUS_OK;
    }
}
