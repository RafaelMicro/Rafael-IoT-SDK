/**************************************************************************//**
* @file       profile_init.h
* @brief
*
*****************************************************************************/

#ifndef _PROFILE_INIT_H_
#define _PROFILE_INIT_H_

#include "ble_profile.h"
#include "atcmd_helper.h"
#include "atcmd_setting.h"

ble_err_t profile_init(ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback);
ble_err_t slave_profile_init(int host_id, ble_info_link_t *ble_info_link, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback);
ble_err_t master_profile_init(int host_id, ble_info_link_t *ble_info_link, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback);

#endif // _PROFILE_INIT_H_
