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
/* PURPOSE: Light Coordinator HAL header file
*/
#ifndef LIGHT_ZC_HAL_H
#define LIGHT_ZC_HAL_H 1

#include "zb_led_button.h"

#define LIGHT_ZC_BUTTON_2_IDX ZB_BOARD_BUTTON_1

void light_zc_hal_init(void);
zb_bool_t light_zc_hal_is_button_pressed(zb_uint8_t button_no);

#endif /* LIGHT_ZC_HAL_H */
