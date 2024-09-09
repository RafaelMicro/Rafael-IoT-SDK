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
/* PURPOSE: Test emulator of SP descriptor
*/

#ifndef _SP_EMULATOR_H_
#define _SP_EMULATOR_H_ 1

#if defined SP_EMULATOR_ON

void sp_start_test(zb_uint8_t text_index);

#define SP_START_TEST(text_index)               sp_start_test((text_index))

#define SP_BUTTON_PIN                 0

#define SP_RELAY_ON() TRACE_MSG(TRACE_APP1, "RELAY ON", (FMT__0))
#define SP_RELAY_OFF() TRACE_MSG(TRACE_APP1, "RELAY OFF", (FMT__0))
#define SP_LED_ON() TRACE_MSG(TRACE_APP1, "LED ON", (FMT__0))
#define SP_LED_OFF() TRACE_MSG(TRACE_APP1, "LED OFF", (FMT__0))

void zb_reset(zb_uint8_t param);

#else /* SP_EMULATOR_ON */

#define SP_START_TEST(text_index) while(0)

#endif /* SP_EMULATOR_ON */

#endif /* _SP_EMULATOR_H_ */
