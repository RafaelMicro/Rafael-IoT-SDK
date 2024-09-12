#ifndef _BLE_PARAM_H_
#define _BLE_PARAM_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "ble_gap.h"
#include "ble_advertising.h"
#include "ble_scan.h"


typedef struct atcmd_ble_param_t
{
    ble_gap_addr_t device_addr;
    ble_gap_peer_addr_t peer_addr;
    ble_gap_peer_addr_t scan_addr;
    ble_adv_param_t adv_param;
    ble_scan_param_t scan_param;
    ble_adv_data_param_t adv_data;
    ble_adv_data_param_t scan_rsp;
    uint8_t read_data_len[BLE_SUPPORT_NUM_CONN_MAX];
    uint8_t error_code[BLE_SUPPORT_NUM_CONN_MAX];
    ble_gap_conn_param_t con_param[BLE_SUPPORT_NUM_CONN_MAX];
    uint16_t preferred_tx_data_length;
    uint16_t preferred_mtu_size[BLE_SUPPORT_NUM_CONN_MAX];
    ble_gap_create_conn_param_t create_con_param;
} atcmd_ble_param_t;

#endif //_BLE_PARAM_H_
