#include "atcmd_command_list.h"
#include "ble_service_gaps.h"

// PRIVATE FUNCTION DECLARE
static void notify_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t notify =
{
    .cmd_name = "+NFY",
    .description = "notify",
    .init = notify_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void notify_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        uint8_t host_id = param[0].num;
        CHECK_HOST_ID(host_id);

        uint16_t handle_num = ble_info_link[host_id].svcs_info_atcmd.info.handles.hdl_charac01;
        uint8_t data_len = param[1].num;

        ble_gatt_data_param_t data_param =
        {
            .host_id = host_id,
            .handle_num = handle_num,
            .p_data = send_data,
            .length = data_len
        };
        ble_err_t status = ble_cmd_gatt_notification(&data_param);

        item->status = AT_CMD_STATUS_OK;
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+NFY = <num1>, <num2>\n"
        "  send notification for specific host ID\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : the num of notification length\n"
        "      range : 0-244\n"
        "        notice: the notification value will be set by the num of notification length.\n"
        "          The notification value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423\n"
        "          ex. <num2> = 8\n"
        "            the notification value = 00234567\n"
        "    notice: the num of notification value length should lower than (mtu size - 3)\n"
    );
}
