/**************************************************************************//**
* @file       atcmd_init.h
* @brief
*
*****************************************************************************/

#ifndef _ATCMD_INIT_H_
#define _ATCMD_INIT_H_

#include "atcmd.h"

ble_err_t ble_addr_init(atcmd_t *this);
ble_err_t ble_scan_param_init(atcmd_t *this);
ble_err_t ble_adv_param_init(atcmd_t *this);
ble_err_t ble_adv_data_init(atcmd_t *this);
ble_err_t ble_scan_rsp_init(atcmd_t *this);
ble_err_t ble_gaps_data_init(atcmd_t *this);
void ble_service_data_init(atcmd_t *this);
void ble_con_param_init(atcmd_t *this);
void ble_create_con_param_init(atcmd_t *this);
ble_err_t ble_atcmd_gap_device_name_init(void);

#endif // _ATCMD_INIT_H_
