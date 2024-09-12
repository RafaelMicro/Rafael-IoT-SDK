#include "ble_profile_init.h"

ble_err_t profile_init(ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback)
{
    ble_err_t  status = BLE_ERR_OK;
    int i;

    for (i = 0; i < LINK_NUM; i++)
    {
        if (link_info[i].role == BLE_GATT_ROLE_SERVER)
        {
            status = slave_profile_init(i, &ble_info_link[i], gap_callback, atcmd_callback);
            CHECK(status);
        }
        else //link_info[0].role==BLE_GATT_ROLE_CLIENT
        {
            status = master_profile_init(i, &ble_info_link[i], gap_callback, atcmd_callback);
            CHECK(status);
        }
    }
    return status;
}

ble_err_t slave_profile_init(int host_id, ble_info_link_t *ble_info_link, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback)
{
    ble_err_t status;
    ble_info_link->host_id = host_id;
    ble_info_link->state = STATE_STANDBY;
    status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_SERVER, &(ble_info_link->svcs_info_gaps), gap_callback);
    CHECK(status);
    status = ble_svcs_atcmd_init(host_id, BLE_GATT_ROLE_SERVER, &(ble_info_link->svcs_info_atcmd), atcmd_callback);
    CHECK(status);
    return status;
}

ble_err_t master_profile_init(int host_id, ble_info_link_t *ble_info_link, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback)
{
    ble_err_t status;
    ble_info_link->host_id = host_id;
    ble_info_link->state = STATE_STANDBY;
    status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_CLIENT, &(ble_info_link->svcs_info_gaps), gap_callback);
    CHECK(status);
    status = ble_svcs_atcmd_init(host_id, BLE_GATT_ROLE_CLIENT, &(ble_info_link->svcs_info_atcmd), atcmd_callback);
    CHECK(status);
    return BLE_ERR_OK;
}
