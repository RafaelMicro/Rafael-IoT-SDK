#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void write_enc_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t write_enc =
{
    .cmd_name = "+WRITEENC",
    .description = "write encryption",
    .init = write_enc_cmd_init
};

// PRIVATE VARIABLE DECLARE
static uint8_t host_id;

// PRIVATE FUNCTION IMPLEMENT
static void write_enc_cmd_init(cmd_info_t *this)
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
        int data_length = param[1].num;
        CHECK_HOST_ID(host_id);
        uint16_t handle_num = ble_info_link[host_id].svcs_info_atcmd.info.handles.hdl_charac02;
        ble_gatt_data_param_t data_param =
        {
            .host_id = host_id,
            .handle_num = handle_num,
            .p_data = send_data,
            .length = data_length
        };
        ble_err_t status = ble_cmd_gatt_write_req(&data_param);
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+WRITEENC = <num1>, <num2>\n"
        "  send write value for specific host ID with encryption\n"
        "    <num1> : host ID\n"
        "      range : 0-0\n"
        "    <num2> : the num of write length\n"
        "      range : 0-244\n"
        "        notice: the write value will be set by the num of write length.\n"
        "          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423\n"
        "          ex. <num2> = 8\n"
        "            the write value = 00234567\n"
        "    notice: the num of write value length should lower than (mtu size - 3)\n"
    );
}
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item)
{
    if (p_param->event == BLESERVICE_ATCMD_CHARAC02_WRITE_RSP_EVENT && p_param->host_id == host_id)
    {
        item->status = AT_CMD_STATUS_OK;
    }
    else if (p_param->event == BLESERVICE_ATCMD_CHARAC02_ERROR_RSP_EVENT && p_param->host_id == host_id)
    {
        item->status = AT_CMD_STATUS_OK;
    }
}
