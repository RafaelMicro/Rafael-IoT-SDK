#include "atcmd_command_list.h"
#include "ble_service_gaps.h"

// PRIVATE FUNCTION DECLARE
static void gap_device_name_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t gap_device_name =
{
    .cmd_name = "+DEVNAME",
    .description = "gap device name",
    .init = gap_device_name_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void gap_device_name_cmd_init(cmd_info_t *this)
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
        uint8_t *p_device_name = (uint8_t *)param[0].str;
        ble_err_t status = ble_svcs_gaps_device_name_set(p_device_name, strlen((char *)p_device_name));
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
        "+DEVNAME = <string>\n"
        "  set GAP device name\n"
        "    <string> : the GAP device name data\n"
        "       length : 1-30"
    );
}
