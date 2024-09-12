#include "atcmd_command_list.h"
#include "ble_service_gaps.h"

// PRIVATE FUNCTION DECLARE
static void gap_pre_con_param_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t gap_pre_con_param =
{
    .cmd_name = "+PCONPAR",
    .description = "gap prefer connection param",
    .init = gap_pre_con_param_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void gap_pre_con_param_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;


    if (item->param_length == 4)
    {
        atcmd_param_type param_type_list[] = {INT, INT, INT, INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        ble_gap_conn_param_t con_param =
        {
            .min_conn_interval = param[0].num,
            .max_conn_interval = param[1].num,
            .periph_latency = param[2].num,
            .supv_timeout  = param[3].num,
        };
        ble_err_t status = ble_svcs_gaps_peripheral_conn_param_set(&con_param);

        item->status = AT_CMD_STATUS_OK;
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PCONPAR = <num1>, <num2>, <num3>, <num4>\n"
        "  set GAP perfer connection parameter\n"
        "    <num1> : connIntervalMin\n"
        "       range : 6-3200"
        "    <num2> : connIntervalMax\n"
        "       range : 6-3200"
        "    <num3> : connLatency\n"
        "       range : 0-499"
        "    <num4> : connSupervisionTimeout\n"
        "       range : 10-3200"
    );
}
