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
/* PURPOSE: common include for zdo_startup
*/

#ifndef ZDO_STARTUP_H
#define ZDO_STARTUP_H 1

#include "zb_types.h"

#define CHANNEL 21

/* IEEE address of ED */
#define TEST_ZE_ADDR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
/* IEEE address of ZR */
#define TEST_ZR_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/*
 * Number of packets that should be sent from ED to ZC.
 * Special value 0xffffffff means infinity.
 */
#ifndef PACKETS_FROM_ED_NR
#define PACKETS_FROM_ED_NR 0xffffffff
#endif

/*
 * Number of packets that should be sent from ZC to ED.
 * Special value 0xffffffff means infinity.
 */
#ifndef PACKETS_FROM_ZC_NR
#define PACKETS_FROM_ZC_NR 0xffffffff
#endif

#endif /* ZDO_STARTUP_H */
