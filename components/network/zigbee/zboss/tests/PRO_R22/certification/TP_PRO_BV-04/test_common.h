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
/* PURPOSE: test_common.h
*/

//#define TEST_CHANNEL (1l << 24)

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}

#ifndef ZB_PRO_STACK
#error This test is not applicable for 2007 stack. ZB_PRO_STACK should be defined to enable PRO.
#endif

#ifndef ZB_CERTIFICATION_HACKS
#error Define ZB_CERTIFICATION_HACKS
#endif
