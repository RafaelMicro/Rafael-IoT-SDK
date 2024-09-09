/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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
/* PURPOSE: Common definitions for leds and buttons functionality
*/

#ifndef ZB_TEST_NRF52_LED_BUTTON_H
#define ZB_TEST_NRF52_LED_BUTTON_H

void zb_osif_led_button_init(zb_callback_t callback);
static zb_ret_t led_on_off(zb_uint8_t led_no, zb_uint8_t on);
void zb_osif_led_on(zb_uint8_t led_no);
void zb_osif_led_off(zb_uint8_t led_no);

#endif /* ZB_TEST_NRF52_LED_BUTTON_H */
