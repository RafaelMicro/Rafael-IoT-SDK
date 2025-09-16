#include "atcmd_init.h"

#define ADDRESS {0x12, 0x52, 0x53, 0x54, 0x55, 0xC0}
#define DEVICE_NAME  "AT_DEMO"

static uint8_t device_name_str[] = {DEVICE_NAME};


ble_err_t ble_addr_init(atcmd_t *this)
{
    ble_err_t status = BLE_ERR_OK;
    const ble_gap_addr_t device_addr =
    {
        .addr_type = PUBLIC_ADDR,
        .addr = ADDRESS
    };
    const ble_gap_peer_addr_t peer_addr =
    {
        .addr_type = PUBLIC_ADDR,
        .addr = ADDRESS
    };

    memcpy(&this->ble_param.device_addr, &device_addr, sizeof(device_addr));
    memcpy(&this->ble_param.peer_addr, &peer_addr, sizeof(peer_addr));
    memcpy(&this->ble_param.scan_addr, &peer_addr, sizeof(peer_addr));

    status = ble_cmd_device_addr_set(&this->ble_param.device_addr);
    if (status != BLE_ERR_OK)
    {
        return status;
    }
    status = ble_cmd_resolvable_address_init();

    return status;
}

ble_err_t ble_scan_param_init(atcmd_t *this)
{
    ble_gap_addr_t addr_param;
    ble_err_t status = BLE_ERR_OK;
    // get device address
    ble_cmd_device_addr_get(&addr_param);

    const ble_scan_param_t scan_param =
    {
        .own_addr_type = addr_param.addr_type,
        .scan_type = SCAN_TYPE_ACTIVE,
        .scan_interval = 160U, //160*0.625ms=100ms
        .scan_window = 160U,   //160*0.625ms=100ms
        .scan_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED,
    };
    memcpy(&this->ble_param.scan_param, &scan_param, sizeof(scan_param));

    status = ble_cmd_scan_param_set(&this->ble_param.scan_param);
    return status;
}

ble_err_t ble_adv_param_init(atcmd_t *this)
{
    ble_gap_addr_t addr_param;
    ble_err_t status = BLE_ERR_OK;

    // get device address
    ble_cmd_device_addr_get(&addr_param);

    const ble_adv_param_t adv_param =
    {
        .adv_type = ADV_TYPE_ADV_IND,
        .own_addr_type = addr_param.addr_type,
        .adv_interval_min = 160U, //160*0.625ms=100ms
        .adv_interval_max = 160U, //160*0.625ms=100ms
        .adv_peer_addr_param = {
            .addr_type = PUBLIC_ADDR,
            .addr = ADDRESS,
        },
        .adv_channel_map = ADV_CHANNEL_ALL,
        .adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL,
    };

    memcpy(&this->ble_param.adv_param, &adv_param, sizeof(adv_param));
    if (this->is_no_connect_mode)
    {
        this->ble_param.adv_param.adv_type = ADV_TYPE_SCAN_IND;
    }
    status = ble_cmd_adv_param_set(&this->ble_param.adv_param);
    return status;
}

ble_err_t ble_adv_data_init(atcmd_t *this)
{
    ble_err_t status = BLE_ERR_OK;
    const uint8_t adv_data[] =
    {
        2, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
        3, GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x12, 0x34
    };

    this->ble_param.adv_data.length = sizeof(adv_data);
    memcpy(this->ble_param.adv_data.data, adv_data, sizeof(adv_data));

    status = ble_cmd_adv_data_set(&this->ble_param.adv_data);
    return status;
}

ble_err_t ble_scan_rsp_init(atcmd_t *this)
{
    ble_err_t status = BLE_ERR_OK;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + strlen(device_name_str); //  1 byte data type

    //Scan response data
    uint8_t adv_scan_rsp_data[2 + strlen(device_name_str)];
    adv_scan_rsp_data[0] = SCANRSP_ADLENGTH;                      // AD length
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;       // AD data type
    memcpy(&adv_scan_rsp_data[2], device_name_str, strlen(device_name_str)); // Copy the name

    this->ble_param.scan_rsp.length = sizeof(adv_scan_rsp_data);
    memcpy(this->ble_param.scan_rsp.data, adv_scan_rsp_data, sizeof(adv_scan_rsp_data));

    status = ble_cmd_adv_scan_rsp_set(&this->ble_param.scan_rsp);
    return status;
}

ble_err_t ble_gaps_data_init(atcmd_t *this)
{
    ble_err_t status;
    status = ble_svcs_gaps_device_name_set(device_name_str, strlen(device_name_str));
    return status;
}

void ble_service_data_init(atcmd_t *this)
{
    for (int i = 0; i < LINK_NUM; i++)
    {
        this->ble_param.read_data_len[i] = 0;
        this->ble_param.error_code[i] = 0;
    }
}

void ble_con_param_init(atcmd_t *this)
{
    const ble_gap_conn_param_t con_param =
    {
        .min_conn_interval = 60, //60*1.25ms=75ms
        .max_conn_interval = 60, //60*1.25ms=75ms
        .periph_latency = 0,
        .supv_timeout = 1000,
    };
    for (int i = 0; i < LINK_NUM; i++)
    {
        memcpy(&this->ble_param.con_param[i], &con_param, sizeof(con_param));
    }
}
void ble_create_con_param_init(atcmd_t *this)
{
    ble_gap_addr_t addr_param;
    // get device address
    ble_cmd_device_addr_get(&addr_param);

    const ble_gap_create_conn_param_t create_con_param =
    {
        .conn_param = {
            .min_conn_interval = 60, //60*1.25ms=75ms
            .max_conn_interval = 60, //60*1.25ms=75ms
            .periph_latency = 0,
            .supv_timeout = 1000,
        },
        .init_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED,
        .scan_interval = 160U, //160*0.625ms=100ms
        .scan_window = 160U,   //160*0.625ms=100ms
        .peer_addr = ADDRESS,
        .own_addr_type = addr_param.addr_type,
    };
    memcpy(&this->ble_param.create_con_param, &create_con_param, sizeof(create_con_param));
}
