#include "atcmd_command_list.h"

#define PRINT_INT(name,value) printf("%s = %d\n",name,value);
#define PRINT_STR(name,value) printf("%s = %s\n",name,value);
#define PRINT_ADDR(name,value,str) \
parse_addr_array_to_string(str, (value).addr); \
printf("%s = %s (type=%d)\n",name,str,(value).addr_type);

// PRIVATE FUNCTION DECLARE
static ble_err_t read_cmd(atcmd_item_t *item);
static void test_cmd(atcmd_item_t *item);
static void all_param_init(cmd_info_t *this);

// PUBLIC VARIABLE DECLARE
cmd_info_t all_param =
{
    .cmd_name = "+ALLPARAM",
    .description = "read the all param",
    .init = all_param_init
};

// PRIVATE FUNCTION IMPLEMENT
static void all_param_init(cmd_info_t *this)
{
    cmd_info_init(this);
    this->read_cmd = read_cmd;
    this->test_cmd = test_cmd;
}
static ble_err_t read_cmd(atcmd_item_t *item)
{
    char str[100];
    atcmd_ble_param_t *ble_param = item->ble_param;
    PRINT_ADDR("device_addr", ble_param->device_addr, str);

    printf("adv_param:\n");
    PRINT_INT("    adv_type", ble_param->adv_param.adv_type);
    PRINT_INT("    adv_interval_min", ble_param->adv_param.adv_interval_min);
    PRINT_INT("    adv_interval_max", ble_param->adv_param.adv_interval_max);
    PRINT_ADDR("    adv_peer_addr_param", ble_param->adv_param.adv_peer_addr_param, str);
    PRINT_INT("    adv_channel_map", ble_param->adv_param.adv_channel_map);
    PRINT_INT("    adv_filter_policy", ble_param->adv_param.adv_filter_policy);

    printf("scan_param:\n");
    PRINT_INT("    scan_type", ble_param->scan_param.scan_type);
    PRINT_INT("    scan_interval", ble_param->scan_param.scan_interval);
    PRINT_INT("    scan_window", ble_param->scan_param.scan_window);
    PRINT_INT("    scan_filter_policy", ble_param->scan_param.scan_filter_policy);

    parse_hex_array_to_string_with_colon(str, sizeof(str), ble_param->adv_data.data, ble_param->adv_data.length);
    PRINT_STR("adv_data", str);
    parse_hex_array_to_string_with_colon(str, sizeof(str), ble_param->scan_rsp.data, ble_param->scan_rsp.length);
    PRINT_STR("scan_rsp", str);

    printf("read_data_len:\n");
    for (int i = 0; i < LINK_NUM; i++)
    {
        printf("    read_data_len[%d] = %d\n", i, ble_param->read_data_len[i]);
    }

    printf("error_code:\n");
    for (int i = 0; i < LINK_NUM; i++)
    {
        printf("    error_code[%d] = %d\n", i, ble_param->error_code[i]);
    }

    printf("connection param:\n");
    for (int i = 0; i < LINK_NUM; i++)
    {
        printf("%d :\n", i);
        PRINT_INT("    min_conn_interval", ble_param->con_param[i].min_conn_interval);
        PRINT_INT("    max_conn_interval", ble_param->con_param[i].max_conn_interval);
        PRINT_INT("    periph_latency", ble_param->con_param[i].periph_latency);
        PRINT_INT("    supv_timeout", ble_param->con_param[i].supv_timeout);
    }

    printf("create connection param:\n");
    PRINT_INT("    scan_interval", ble_param->create_con_param.scan_interval);
    PRINT_INT("    scan_window", ble_param->create_con_param.scan_window);
    PRINT_INT("    init_filter_policy", ble_param->create_con_param.init_filter_policy);
    printf("    create connection param - conn_param:\n");
    PRINT_INT("        min_conn_interval", ble_param->create_con_param.conn_param.min_conn_interval);
    PRINT_INT("        max_conn_interval", ble_param->create_con_param.conn_param.max_conn_interval);
    PRINT_INT("        periph_latency", ble_param->create_con_param.conn_param.periph_latency);
    PRINT_INT("        supv_timeout", ble_param->create_con_param.conn_param.supv_timeout);


    PRINT_INT("preferred_tx_data_length", ble_param->preferred_tx_data_length);
    PRINT_INT("preferred_mtu_size", ble_param->preferred_mtu_size);


    item->status = AT_CMD_STATUS_OK;
    return BLE_ERR_OK;
}
static void test_cmd(atcmd_item_t *item)
{
    printf(
        "+ALLPARAM?\n"
        "  read the all param\n"
    );
}
