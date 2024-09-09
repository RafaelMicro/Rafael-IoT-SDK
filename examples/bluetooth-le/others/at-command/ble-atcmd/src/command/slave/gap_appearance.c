#include "atcmd_command_list.h"
#include "ble_service_gaps.h"

// PRIVATE FUNCTION DECLARE
static void gap_appearance_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t gap_appearance =
{
    .cmd_name = "+DEVAP",
    .description = "gap appearance",
    .init = gap_appearance_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void gap_appearance_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 1)
    {
        atcmd_param_type param_type_list[] = {INT};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        uint16_t appearance = param[0].num;
        ble_svcs_gaps_appearance_set(appearance);
        item->status = AT_CMD_STATUS_OK;
        return BLE_ERR_OK;
    }
    return BLE_ERR_INVALID_PARAMETER;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+DEVAP = <num>\n"
        "  set GAP appearance\n"
        "    <num> : appearance\n"
        "       range : 0-65535"
    );
}
