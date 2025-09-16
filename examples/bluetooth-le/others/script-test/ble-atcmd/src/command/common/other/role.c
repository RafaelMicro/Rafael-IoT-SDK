#include "atcmd_command_list.h"

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
    if (LINK_NUM == 0)
    {
        printf("there is no link.");
    }
    for (int i = 0; i < LINK_NUM; i++)
    {
        printf("Host Id:%d, ", i);
        PRINT_IF_ELSE(link_info[i].role == BLE_GATT_ROLE_CLIENT, "Master", "Slave");
        printf("\n");
    }
    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ROLE?\n"
        "  read the role(master/slave)\n"
    );
}
