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

/** @cond touchlink */


#define BULB_LED_JOINED 0
#define BULB_LED_TOUCHLINK_IN_PROGRESS 1
#define BULB_LED_NEW_DEV_JOINED 2
#define BULB_LED4 2

/* idx in array of 4 buttons */
#define BULB_BUTTON_1_IDX ZB_BOARD_BUTTON_0
#define BULB_BUTTON_2_IDX ZB_BOARD_BUTTON_1
#define BULB_BUTTON_3_IDX ZB_BOARD_BUTTON_2
#define BULB_BUTTON_4_IDX 3

#define BULB_BUTTON_1_PIN ZB_BOARD_BUTTON_0
#define BULB_BUTTON_2_PIN ZB_BOARD_BUTTON_1
#define BULB_BUTTON_3_PIN ZB_BOARD_BUTTON_2
#define BULB_BUTTON_4_PIN 3

void light_control_hal_init();
zb_bool_t light_control_hal_is_button_pressed(zb_uint8_t button_no);
void light_control_led_on_off(zb_uint8_t led_idx, zb_uint8_t on_state);

/** @endcond */ /* touchlink */

#endif /* BULB_HAL_H */
