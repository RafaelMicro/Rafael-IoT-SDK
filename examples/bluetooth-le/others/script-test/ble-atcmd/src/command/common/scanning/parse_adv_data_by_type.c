#include "atcmd_command_list.h"
#include "atcmd_scan.h"

// PRIVATE FUNCTION DECLARE
static void parse_adv_data_by_type_cmd_init(cmd_info_t *this);
static ble_err_t set_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);

// PUBLIC VARIABLE DECLARE
cmd_info_t parse_adv_data_by_type =
{
    .cmd_name = "+PARSADV",
    .description = "parsing adv data by type",
    .init = parse_adv_data_by_type_cmd_init
};

// PRIVATE FUNCTION IMPLEMENT
static void parse_adv_data_by_type_cmd_init(cmd_info_t *this)
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
        ble_gap_ad_type_t parsing_ad_type = param[0].num;
        ble_evt_scan_adv_report_t *adv_report = last_adv_data_get();

        uint8_t p_data[31];
        uint8_t p_data_length;
        ble_err_t status = ble_cmd_scan_report_adv_data_parsing(adv_report, parsing_ad_type, p_data, &p_data_length);
        if (status == BLE_ERR_OK)
        {
            char data_str[150] = "";
            parse_hex_array_to_string_with_colon(data_str, sizeof(data_str), p_data, p_data_length);
            printf("adv_ind_len=%d adv_ind=%s\n", p_data_length, data_str);
            item->status = AT_CMD_STATUS_OK;
        }
        return status;
    }
    return BLE_ERR_INVALID_PARAMETER;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+PARSADV = <num>\n"
        "get adv data by BLE_GAP_AD_TYPE\n"
        "  <num> =  BLE_GAP_AD_TYPE\n"
    );
}
