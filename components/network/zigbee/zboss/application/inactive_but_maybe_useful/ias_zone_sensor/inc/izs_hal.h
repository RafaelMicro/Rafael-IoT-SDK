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
/* PURPOSE: IAS zone sensor hardware description stub
*/

#ifndef IZS_HAL_STUB_H
#define IZS_HAL_STUB_H 1

#define IZS_GET_LOADED_BATTERY_VOLT(vol) ((*(zb_uint16_t*)(vol) = 2890), ZB_TRUE)
#define IZS_GET_UNLOADED_BATTERY_VOLT(vol) ((*(zb_uint16_t*)(vol) = 2900), ZB_TRUE)
#define IZS_CONVERT_BATTERY_VOLT(vol) (vol)

void izs_hal_hw_init(void);
void izs_hal_hw_enable(void);
void izs_hal_hw_disable(void);

#endif /* IZS_HAL_STUB_H */
