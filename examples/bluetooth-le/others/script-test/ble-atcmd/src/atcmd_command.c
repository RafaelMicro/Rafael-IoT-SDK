#include "atcmd_command.h"

// PUBLIC CLASS FUNCTION DECLARE
static void do_cmd(struct cmd_info_s *this, atcmd_item_t *item);
static ble_err_t not_support_cmd( atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item);
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item);

// PUBLIC FUNCTION IMPLEMENT
void cmd_info_init(cmd_info_t *this)
{
    this->is_high_level_cmd = false;
    this->do_cmd = do_cmd;
    this->set_cmd = not_support_cmd;
    this->read_cmd = not_support_cmd;
    this->test_cmd = test_cmd;
    this->check_event = check_event;
    this->check_service = check_service;
}

// PUBLIC CLASS IMPLEMENT
static void do_cmd(struct cmd_info_s *this, atcmd_item_t *item)
{
    ble_err_t status;
    if (item->cmd_type == AT_CMD_TYPE_SET_COMMAND)
    {
        status = this->set_cmd(item);
        if (status != BLE_ERR_OK)
        {
            item->err_status = status;
            item->status = AT_CMD_STATUS_FAIL;
            return;
        }
    }
    else if (item->cmd_type == AT_CMD_TYPE_READ_COMMAND)
    {
        status = this->read_cmd(item);
        if (status != BLE_ERR_OK)
        {
            item->err_status = status;
            item->status = AT_CMD_STATUS_FAIL;
            return;
        }
    }
    else if (item->cmd_type == AT_CMD_TYPE_TEST_COMMAND)
    {
        this->test_cmd(item);
        item->status = AT_CMD_STATUS_OK;
    }
    else
    {
        item->err_status = BLE_ERR_CMD_NOT_SUPPORTED;
        item->status = AT_CMD_STATUS_FAIL;
    }
}
static ble_err_t not_support_cmd( atcmd_item_t *item)
{
    return BLE_ERR_CMD_NOT_SUPPORTED;
}
static void test_cmd(atcmd_item_t *item)
{
    printf("this command haven't finished the help infomation\n");
}
static void check_event(ble_module_evt_t event, void *param, atcmd_item_t *item)
{
    // do nothing
}
static void check_service(ble_evt_att_param_t *p_param, atcmd_item_t *item)
{
    // do nothing
}
