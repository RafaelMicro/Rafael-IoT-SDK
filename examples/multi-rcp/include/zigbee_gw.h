/**
 * @file zigbee_gw.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ZIGBEE_GW_H
#define __ZIGBEE_GW_H

#include "stdint.h"

void zigbee_gw_init(void* cmd_queue);
void zigbee_gw_cmd_proc(uint8_t* pBuf, uint32_t len);
void zigbee_gw_cmd_send(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                        uint8_t src_endp, uint8_t* pParam, uint32_t len);
void zigbee_gw_cmd_act(uint32_t cmd_id, uint16_t addr, uint8_t addr_mode,
                       uint8_t src_endp, uint8_t* pParam, uint32_t len);
// void _zcl_report_attribute_cb(uint16_t cluster_id, uint16_t addr, uint8_t src_endp, uint8_t *pd, uint8_t pd_len);
// void _zcl_zone_status_change_notificatin_cb(uint16_t cluster_id, uint16_t addr, uint8_t src_endp, uint8_t *pd, uint8_t pd_len);

#endif // __ZIGBEE_GW_H