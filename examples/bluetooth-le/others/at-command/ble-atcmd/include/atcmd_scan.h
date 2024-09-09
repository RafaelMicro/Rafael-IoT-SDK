/**************************************************************************//**
* @file       atcmd_scan.h
* @brief
*
*****************************************************************************/

#ifndef _ATCMD_SCAN_H_
#define _ATCMD_SCAN_H_

#include "atcmd.h"

void scan_result_clear(void);
ble_evt_scan_adv_report_t *last_adv_data_get(void);
ble_evt_scan_adv_report_t *last_scan_rsp_get(void);

#endif // _ATCMD_SCAN_H
