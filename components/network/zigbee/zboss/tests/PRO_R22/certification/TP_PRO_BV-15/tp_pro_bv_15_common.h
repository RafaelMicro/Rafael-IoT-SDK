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
/* PURPOSE:
*/
#ifndef __TP_PRO_BV_15__
#define __TP_PRO_BV_15__

#define TEST_ENABLED


/*
//Ember coordinator EM-ISA3-00
0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00
//Ember router EM-ISA3-01
0xf0, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00
*/

#ifdef ZB_EMBER_TESTS
#define G_ZR4
#endif

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#if (defined ZB_NS_BUILD) | (defined ZB_NSNG)
#define IEEE_ADDR_R1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R3 {0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R4 {0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00}
#else
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R3 {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}

#ifdef G_ZR4
#define IEEE_ADDR_R4 {0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00}
#else
#define IEEE_ADDR_R4 {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00}
#endif

#endif

#ifndef ZB_PRO_ADDRESS_ASSIGNMENT_CB
/* for ZR1 random inicialization */
#define START_RANDOM 0x11223344
/* for ZR2 random inicialization */
#define START_RANDOM2 0x12345678
/* ZR4 address equal value of ZR1 grant to ZR2 */
#define NEW_ZR4_ADDR 0xbc2d
#else
#define ZR1_SHORT_ADDR    0x1111
#define ZR2_SHORT_ADDR    0x2222
#define ZR3_SHORT_ADDR    0x3333
#define ZR4_SHORT_ADDR    0x4444
#define NEW_ZR4_ADDR ZR2_SHORT_ADDR
#endif /* ZB_PRO_ADDRESS_ASSIGNMENT_CB */

//#define TEST_CHANNEL (1l << 24)

#endif
