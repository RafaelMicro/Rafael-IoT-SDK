#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void con_latency_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t con_latency =
{
    .cmd_name = "+CONLAT",
    .description = "connection latency",
    .init = con_latency_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void con_latency_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;
    ble_gap_conn_param_t con_param;
    uint8_t host_id = 0;

    if (item->param_length == 2)
    {
        atcmd_param_type param_type_list[] = {INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);
        host_id = param[0].num;
        con_param = item->ble_param->con_param[host_id];
        con_param.periph_latency = param[1].num;
    }
    else
    {
        return BLE_ERR_INVALID_PARAMETER;
    }
    ble_gap_conn_param_update_param_t gap_con_param =
    {
        .host_id = host_id,
        .ble_conn_param = con_param
    };
    ble_err_t status = ble_cmd_conn_param_update(&gap_con_param);
    if (status == BLE_ERR_OK)
    {
        item->ble_param->con_param[host_id].periph_latency = con_param.periph_latency;
        item->status = AT_CMD_STATUS_OK;
    }
    return status;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    ble_gap_conn_param_t *con_param = item->ble_param->con_param;

    for (uint8_t i = 0; i < MAX_CONN_NO_APP; i++)
    {
        printf("id=%d Latency=%u\n", i, con_param[i].periph_latency);
    }
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+CONLAT?\n"
        "  get the connection latency\n"
        "+CONLAT = <num1>, <num2>\n"
        "  set the connection latency\n"
        "    <num1> : host ID\n"
        "      range : 0-1\n"
        "    <num2> : the connection latency\n"
        "      range : 0-499\n"
        "      interval = <num2> * 1.25ms\n"
        "    notice\n"
        "       must match this formula : timeout * 4 > interval_max * (1+latency)\n"
    );
}
