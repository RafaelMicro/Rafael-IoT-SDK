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
/* PURPOSE: HAL stubs for SP
*/

#ifndef _SP_HAL_STUB_H_
#define _SP_HAL_STUB_H_ 1

#define SP_BUTTON_PIN                 0

#define SP_RELAY_ON() TRACE_MSG(TRACE_APP1, "RELAY ON", (FMT__0))
#define SP_RELAY_OFF() TRACE_MSG(TRACE_APP1, "RELAY OFF", (FMT__0))
#define SP_LED_ON() TRACE_MSG(TRACE_APP1, "LED ON", (FMT__0))
#define SP_LED_OFF() TRACE_MSG(TRACE_APP1, "LED OFF", (FMT__0))

#define SP_PAGE_SIZE     0x100
#define SP_OTA_FW_CTRL_ADDRESS ((zb_uint32_t)0)
#define OTA_ERASE_PORTION_SIZE ((zb_uint32_t)0)
#define SP_OTA_FW_DOWNLOADED   ((zb_uint32_t)0)
#define SP_OTA_FW_START_ADDRESS ((zb_uint32_t)0)
#define SP_OTA_FW_ERASE_ADDRESS (SP_OTA_FW_CTRL_ADDRESS + SP_OTA_FW_SECTOR_SIZE)
#define SP_OTA_FW_SECTOR_SIZE   ((zb_uint32_t)0)

#define SP_OVERVOLTAGE_THRESHOLD 1
#define SP_OVERCURRENT_THRESHOLD 1

zb_ret_t sp_hal_init();
zb_bool_t sp_get_button_state(zb_uint16_t button);
void sp_relay_on_off(zb_bool_t is_on);
void sp_update_button_state_ctx(zb_uint8_t button_state);

#endif /* _SP_HAL_STUB_H_ */
