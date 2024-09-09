#include "atcmd_command_list.h"
#include "ble_profile.h"

// PRIVATE FUNCTION DECLARE
static void role_init(cmd_info_t *this);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t role =
{
    .cmd_name = "+ROLE",
    .description = "role",
    .init = role_init
};

// PRIVATE FUNCTION IMPLEMENT
static void role_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    for (int i = 0; i < MAX_CONN_NO_APP; i++)
    {
        printf("Host Id:%d, ", i);
        PRINT_IF_ELSE(ble_app_link_info[i].gap_role == BLE_GAP_ROLE_CENTRAL, "Central", "Peripheral");
        printf("\n");
    }
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ROLE?\n"
        "  read the role(Central/Peripheral)\n"
    );
}
