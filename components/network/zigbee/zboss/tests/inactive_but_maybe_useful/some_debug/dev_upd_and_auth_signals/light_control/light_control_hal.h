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
/* PURPOSE: Light control HAL header file
*/
#ifndef LIGHT_CONTROL_HAL_H
#define LIGHT_CONTROL_HAL_H 1

#define BULB_BUTTON_2_IDX 1

void light_control_hal_init();
zb_bool_t light_control_hal_is_button_pressed(zb_uint8_t button_no);

#endif /* BULB_HAL_H */
