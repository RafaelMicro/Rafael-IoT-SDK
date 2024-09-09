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
/* PURPOSE: led indication of various ongoing processes and states
*/


#ifndef ZB_SE_INDICATION_H
#define ZB_SE_INDICATION_H 1

#include "zb_types.h"

#define LED_GREEN 1
#define LED_RED 0

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1

void zb_se_start_nvram_erase_indication(void);
void zb_se_stop_nvram_erase_indication(void);
void zb_se_indicate_service_discovery_started(void);
void zb_se_indicate_default_start(void);
void zb_se_indicate_commissioning_fail(void);
void zb_se_indicate_commissioning_started(void);
void zb_se_indicate_commissioning_stopped(void);
void zb_se_indicate_permit_joining_on(zb_int_t seconds);

#endif
