#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void scan_rsp_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t scan_rsp =
{
    .cmd_name = "+SCANRSP",
    .description = "scan response",
    .init = scan_rsp_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void scan_rsp_cmd_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->set_cmd = set_cmd;
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}

static ble_err_t set_cmd(atcmd_item_t *item)
{
    atcmd_param_block_t *param = item->param;

    if (item->param_length == 1)
    {
        int scan_rsp_len = strlen(param[0].str) / 3 + 1;

        atcmd_param_type param_type_list[] = {HEX};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        ble_adv_data_param_t scan_rsp_param;
        memcpy(scan_rsp_param.data, param[0].hex, sizeof(scan_rsp_param.data));
        scan_rsp_param.length = scan_rsp_len;

        ble_err_t status = ble_cmd_adv_scan_rsp_set(&scan_rsp_param);
        if (status == BLE_ERR_OK)
        {
            memcpy(&item->ble_param->scan_rsp, &scan_rsp_param, sizeof(ble_adv_data_param_t));
            item->status = AT_CMD_STATUS_OK;
        }
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}

static ble_err_t read_cmd(atcmd_item_t *item)
{
    uint8_t *scan_rsp = item->ble_param->scan_rsp.data;
    int scan_rsp_len = item->ble_param->scan_rsp.length;

    char scan_rsp_str[MAX_ADV_DATA_SIZE * 3];
    parse_hex_array_to_string_with_colon(scan_rsp_str, sizeof(scan_rsp_str), scan_rsp, scan_rsp_len);
    printf("%s\n", scan_rsp_str);

    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}

static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+SCANRSP?\n"
        "  get the scan response data\n"
        "+SCANRSP = <string>\n"
        "  set the scan response data\n"
        "    <string> : the scan response data\n"
        "      format : XX:XX:XX:XX:XX:XX\n"
        "      notice\n"
        "        XX : hex\n"
        "        It should follow adv_data format\n"
        "        The first XX means the length of value, first XX : 00-1F\n"
        "      ex. 09:08:09:61:62:63:64:65:66:67\n"
        "        It has 9 length of value\n"
        "        It has one AD structures\n"
        "          the first AD structure has 8 length\n"
        "            local_name_complete(09) which value is 61:62:63:64:65:66:67 = \"abcdefg\"\n"
    );
}
