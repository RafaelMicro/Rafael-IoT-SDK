#include "atcmd_init.h"
#include "ble_service_gaps.h"

// Device BLE Address
static const ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0xD1, 0xD2, 0xC3, 0xC4, 0xE5, 0xC6 }
                                           };

#define DEVICE_NAME 'A', 'T', '_', 'C', 'o', 'm', 'm', 'a', 'n', 'd'

static const uint8_t device_name_str[] = {DEVICE_NAME};


ble_err_t ble_addr_init(atcmd_t *this)
{
    ble_err_t status = BLE_ERR_OK;
    ble_unique_code_format_t unique_code_param;

    do
    {
        status = ble_cmd_read_unique_code(&unique_code_param);
        if (status == BLE_ERR_OK)
        {
            this->ble_param.device_addr.addr_type = unique_code_param.addr_type;
            memcpy(&this->ble_param.device_addr.addr, &unique_code_param.ble_addr, 6);
            status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            memcpy(&this->ble_param.device_addr, &DEVICE_ADDR, sizeof(DEVICE_ADDR));
        }

    } while (0);

    status = ble_cmd_device_addr_set(&this->ble_param.device_addr);
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    status = ble_cmd_resolvable_address_init();
    if (status != BLE_ERR_OK)
    {
        return status;
    }

    return status;
}

ble_err_t ble_scan_param_init(atcmd_t *this)
{
    ble_err_t status = BLE_ERR_OK;
    ble_gap_addr_t addr_param;

    ble_cmd_device_addr_get(&addr_param);
    const ble_scan_param_t scan_param =
    {
        .scan_type = SCAN_TYPE_ACTIVE,
        .own_addr_type = addr_param.addr_type,
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
    ble_err_t status = BLE_ERR_OK;
    ble_gap_addr_t addr_param;

    ble_cmd_device_addr_get(&addr_param);
    const ble_adv_param_t adv_param =
    {
        .adv_type = ADV_TYPE_ADV_IND,
        .own_addr_type = addr_param.addr_type,
        .adv_interval_min = 160U, //160*0.625ms=100ms
        .adv_interval_max = 160U, //160*0.625ms=100ms
        .adv_channel_map = ADV_CHANNEL_ALL,
        .adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL,
    };

    memcpy(&this->ble_param.adv_param, &adv_param, sizeof(adv_param));
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
    uint8_t scan_rsp_len  = (1) + sizeof(device_name_str); //  1 byte data type

    // scan response data
    uint8_t adv_scan_rsp_data[] =
    {
        scan_rsp_len,                       // AD length
        GAP_AD_TYPE_LOCAL_NAME_COMPLETE,    // AD data type
        DEVICE_NAME,                        // the name is shown on scan list
    };
    this->ble_param.scan_rsp.length = sizeof(adv_scan_rsp_data);
    memcpy(this->ble_param.scan_rsp.data, adv_scan_rsp_data, sizeof(adv_scan_rsp_data));
    status = ble_cmd_adv_scan_rsp_set(&this->ble_param.scan_rsp);
    return status;
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
    for (int i = 0; i < BLE_SUPPORT_NUM_CONN_MAX; i++)
    {
        memcpy(&this->ble_param.con_param[i], &con_param, sizeof(con_param));
    }
}

void ble_create_con_param_init(atcmd_t *this)
{
    ble_gap_addr_t addr_param;

    ble_cmd_device_addr_get(&addr_param);
    const ble_gap_create_conn_param_t create_con_param =
    {
        .own_addr_type = addr_param.addr_type,
        .conn_param = {
            .min_conn_interval = 60, //60*1.25ms=75ms
            .max_conn_interval = 60, //60*1.25ms=75ms
            .periph_latency = 0,
            .supv_timeout = 1000,
        },
        .init_filter_policy = SCAN_FILTER_POLICY_BASIC_UNFILTERED,
        .scan_interval = 160U, //160*0.625ms=100ms
        .scan_window = 160U,   //160*0.625ms=100ms
    };
    memcpy(&this->ble_param.create_con_param, &create_con_param, sizeof(create_con_param));
}

ble_err_t ble_atcmd_gap_device_name_init(void)
{
    ble_err_t status;
    // set GAP device name
    status = ble_svcs_gaps_device_name_set((uint8_t *)device_name_str, sizeof(device_name_str));
    return status;
}
