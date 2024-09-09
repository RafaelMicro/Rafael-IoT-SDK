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
/* PURPOSE: Dimable Light HAL header file
*/
#ifndef BULB_HAL_H
#define BULB_HAL_H 1

/* Enable one of the macros below to use non-linear relationship
 * between ZCL level and PWM duty cycle (electrical current). If
 * none is enabled, use linear mapping */
#define xLED_BULB_PWM_USE_FAST_EXP_CURVE
#define LED_BULB_PWM_USE_EXP_CURVE
#define xLED_BULB_PWM_USE_CUBIC_CURVE
#define xLED_BULB_PWM_USE_QUAD_CURVE

#define BULB_BUTTON_2_IDX 1

/*  */
void bulb_hal_init();
void bulb_hal_set_level(zb_uint8_t level);
void bulb_hal_set_on_off(zb_bool_t on);
void bulb_hal_set_connect(zb_bool_t on);
zb_bool_t bulb_hal_is_button_pressed();


#endif /* BULB_HAL_H */
