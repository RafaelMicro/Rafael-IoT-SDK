#include "atcmd_scan.h"

#define INVALID_LEN 255

typedef struct scan_result_s
{
    ble_adv_type_t adv_type;   /**< BLE advertising type */
    ble_gap_peer_addr_t peer_addr; /**< Remote device address type and address. */
    uint8_t adv_data_len;      /**< Received advertising data length. */
    uint8_t adv_data[31];      /**< Received advertising data. */
    bool is_scan_rsp_exist;    /**< Have Received scan response data. */
    uint8_t scan_rsp_len;      /**< Received scan response data length. */
    uint8_t scan_rsp[31];      /**< Received scan response data. */
    int8_t rssi;               /**< Received Signal Strength Indication in dBm. */
} scan_result_t;

// PRIVATE VARIABLE DECLARE
static scan_result_t scan_result;
static int only_get_adv_data_count = 0;
static ble_evt_scan_adv_report_t last_adv_data;
static ble_evt_scan_adv_report_t last_scan_rsp;

// PRIVATE FUNCTION DECLARE
static void scan_print_once(atcmd_t *at_cmd, ble_evt_scan_adv_report_t *rpt);
static bool is_match_scan_filter(atcmd_t *at_cmd, ble_evt_scan_adv_report_t *rpt);
static void scan_result_update(scan_result_t *scan_res, ble_evt_scan_adv_report_t *rpt);
static void scan_result_print(const scan_result_t *scan_res);

// PUBLIC FUNCTION IMPLEMENT
void scan_report_event_handle(ble_evt_scan_adv_report_t *rpt, atcmd_t *at_cmd)
{
    switch (at_cmd->ble_param.scan_mode)
    {
    case SCANMODE_NO:
        break;
    case SCANMODE_PRINT_ONCE:
        scan_print_once(at_cmd, rpt);
        break;
    default:
        break;
    }
}
void scan_result_clear(void)
{
    memset(&scan_result, 0, sizeof(scan_result));
    only_get_adv_data_count = 0;
    scan_result.adv_data_len = INVALID_LEN;
    scan_result.scan_rsp_len = INVALID_LEN;
}
ble_evt_scan_adv_report_t *last_adv_data_get(void)
{
    return &last_adv_data;
}
ble_evt_scan_adv_report_t *last_scan_rsp_get(void)
{
    return &last_scan_rsp;
}

// PRIVATE FUNCTION IMPLEMENT
static void scan_print_once(atcmd_t *at_cmd, ble_evt_scan_adv_report_t *rpt)
{
    if (is_match_scan_filter(at_cmd, rpt))
    {
        scan_result_update(&scan_result, rpt);
        if (scan_result.adv_data_len != INVALID_LEN && scan_result.scan_rsp_len != INVALID_LEN)
        {
            scan_result_print(&scan_result);
            at_cmd->ble_param.scan_mode = SCANMODE_NO;
            only_get_adv_data_count = 0;
        }
        else
        {
            only_get_adv_data_count++;
            if (only_get_adv_data_count >= 5)
            {
                scan_result_print(&scan_result);
                at_cmd->ble_param.scan_mode = SCANMODE_NO;
                only_get_adv_data_count = 0;
            }
        }
    }
}
static bool is_match_scan_filter(atcmd_t *at_cmd, ble_evt_scan_adv_report_t *rpt)
{
    bool is_match = (memcmp(rpt->peer_addr.addr, at_cmd->ble_param.scan_addr.addr, sizeof(rpt->peer_addr.addr)) == 0);
    return is_match;
};
static void scan_result_update(scan_result_t *scan_res, ble_evt_scan_adv_report_t *rpt)
{
    scan_res->peer_addr = rpt->peer_addr;
    scan_res->rssi = rpt->rssi;
    if (rpt->adv_type == 4) //scan response
    {
        scan_res->is_scan_rsp_exist = true;
        scan_res->scan_rsp_len = rpt->length;
        memcpy(scan_res->scan_rsp, rpt->data, rpt->length);
        last_scan_rsp = *rpt;
    }
    else // adv data
    {
        scan_res->adv_type = rpt->adv_type;
        scan_res->adv_data_len = rpt->length;
        memcpy(scan_res->adv_data, rpt->data, rpt->length);
        last_adv_data = *rpt;
    }
}
static void scan_result_print(const scan_result_t *scan_res)
{
    static char addr_str[BLE_ADDR_LEN * 3] = "";
    static char data_str[150] = "";

    //get addr str
    parse_addr_array_to_string(addr_str, scan_res->peer_addr.addr);
    printf("adv_type:%d, addr_type:%d, addr:%s, RSSI:%d\n",
           scan_res->adv_type, scan_res->peer_addr.addr_type, addr_str, scan_res->rssi);

    //get adv_data str
    printf("adv_data_len=%d\n", scan_res->adv_data_len);
    parse_hex_array_to_string_with_colon(data_str, sizeof(data_str), scan_res->adv_data, scan_res->adv_data_len);
    printf("adv_data=%s\n", data_str);

    if (scan_res->is_scan_rsp_exist)
    {
        //get scan_rsp str
        printf("scan_rsp_len=%d\n", scan_res->scan_rsp_len);
        parse_hex_array_to_string_with_colon(data_str, sizeof(data_str), scan_res->scan_rsp, scan_res->scan_rsp_len);
        printf("scan_rsp=%s\n", data_str);
    }
    else
    {
        printf("there is no scan_rsp\n");
    }
}
