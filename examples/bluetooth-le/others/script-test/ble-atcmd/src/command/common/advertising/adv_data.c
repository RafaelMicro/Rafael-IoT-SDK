#include "atcmd_command_list.h"

// PRIVATE FUNCTION DECLARE
static void adv_data_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t adv_data =
{
    .cmd_name = "+ADVDATA",
    .description = "adv data",
    .init = adv_data_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void adv_data_cmd_init(cmd_info_t *this)
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
        int adv_data_len = strlen(param[0].str) / 3 + 1;

        atcmd_param_type param_type_list[] = {HEX};
        bool check = parse_param_type(item, param_type_list, SIZE_ARR(param_type_list));
        CHECK_PARAM(check);

        ble_adv_data_param_t adv_data_param;
        memcpy(adv_data_param.data, param[0].hex, sizeof(adv_data_param.data));
        adv_data_param.length = adv_data_len;

        ble_err_t status = ble_cmd_adv_data_set(&adv_data_param);
        if (status == BLE_ERR_OK)
        {
            memcpy(&item->ble_param->adv_data, &adv_data_param, sizeof(ble_adv_data_param_t));
            item->status = AT_CMD_STATUS_OK;
        }
        return status;
    }

    return BLE_ERR_INVALID_PARAMETER;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    uint8_t *adv_data = item->ble_param->adv_data.data;
    int adv_data_len = item->ble_param->adv_data.length;

    char adv_data_str[MAX_ADV_DATA_SIZE * 3];
    parse_hex_array_to_string_with_colon(adv_data_str, sizeof(adv_data_str), adv_data, adv_data_len);
    printf("%s\n", adv_data_str);

    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ADVDATA?\n"
        "  get the advertising data\n"
        "+ADVDATA = <string>\n"
        "  set the advertising data\n"
        "    <string> : the advertising data\n"
        "      format : XX:XX:XX:XX:XX:XX\n"
        "      notice\n"
        "        XX : hex\n"
        "        It should follow adv_data format\n"
        "        The first XX means the length of value,\n"
        "        first XX : 00-1F\n"
        "      ex. 07:02:01:05:03:FF:12:34\n"
        "        It has 7 length of value\n"
        "        It has two AD structures\n"
        "          the first AD structure has 2 length\n"
        "            type_flages(01) which value is 05\n"
        "          the second AD structure has 3 length\n"
        "            manufacturer_specific(FF) which value is 3412\n"
    );
}

