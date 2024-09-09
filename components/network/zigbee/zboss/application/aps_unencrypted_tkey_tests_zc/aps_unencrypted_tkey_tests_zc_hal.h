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
/* PURPOSE: APS Unencrypted Transport Key feature coordinator HAL header file
*/
#ifndef LIGHT_ZC_HAL_H
#define LIGHT_ZC_HAL_H 1

#define ZDO_LEAVE_BUTTON 1
#define APS_TKEY_SECURITY_BUTTON 2
#define SEND_TO_FIRST_DEVICE 3
#define SEND_TO_SECOND_DEVICE 4

void aps_unencrypted_tkey_zc_hal_init();
zb_bool_t aps_unencrypted_tkey_zc_hal_is_button_pressed();
zb_uint8_t aps_unencrypted_tkey_zc_device_joined_indication();
void aps_unencrypted_tkey_zc_device_leaved_indication(zb_uint8_t led_idx);
void aps_unencrypted_tkey_zc_device_message_indication(zb_uint8_t led_idx);

#endif /* LIGHT_ZC_HAL_H */
