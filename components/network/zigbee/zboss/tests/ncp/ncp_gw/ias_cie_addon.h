/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE: Simple GW application IAS CIE additions
*/
#ifndef IAS_CIE_ADDON_H
#define IAS_CIE_ADDON_H 1

void find_ias_zone_device_tmo(zb_uint8_t dev_idx);
void find_ias_zone_device_delayed(zb_uint8_t idx);
void find_ias_zone_device(zb_uint8_t param, zb_uint16_t short_addr);
void find_ias_zone_device_cb(zb_uint8_t param);
void write_cie_addr(zb_uint8_t param, zb_uint16_t dev_idx);

#endif /* IAS_CIE_ADDON_H */
