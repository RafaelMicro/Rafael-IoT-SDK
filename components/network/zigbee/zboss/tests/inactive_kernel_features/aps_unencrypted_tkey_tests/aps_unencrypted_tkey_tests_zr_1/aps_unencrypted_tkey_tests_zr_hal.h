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
/* PURPOSE: APS Unencrypted Transport Key feature router HAL header file
*/
#ifndef TEST_APS_UNENCRYPTED_TKEY_ZR_HAL_H
#define TEST_APS_UNENCRYPTED_TKEY_ZR_HAL_H 1

#define LEAVE_NET_BUTTON 1

zb_bool_t aps_unencrypted_tkey_zc_hal_is_button_pressed();
void tests_aps_unencrypted_tkey_zr_button_pressed(zb_uint8_t button_no);
void aps_unencrypted_tkey_zr_hal_init();

#endif /* TEST_APS_UNENCRYPTED_TKEY_ZR_HAL_H */
