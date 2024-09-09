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
#ifndef __TP_PRO_BV_13__
#define __TP_PRO_BV_13__

#define ZB_EXIT( _p )

#if defined ZB_PRO_STACK && defined ZB_TEST_CUSTOM_LINK_STATUS && defined ZB_ROUTER_ROLE 
#define SEND_CONFLICT_LINK_STATUS
#endif

//#define TEST_CHANNEL (1l << 24)

/*
//Ember coordinator EM-ISA3-00
0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00
//Ember router EM-ISA3-01
0xf0, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00
*/

#ifdef ZB_EMBER_TESTS
//#define TWO_GOLDEN
#define G_ZR
//#define G_ZED
#endif

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#if defined G_ZED && !defined TWO_GOLDEN
#define IEEE_ADDR_ED {0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00}
#elif defined TWO_GOLDEN
#define IEEE_ADDR_ED {0xf0, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00}
#else
#define IEEE_ADDR_ED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

#if defined G_ZR
#define IEEE_ADDR_R {0xee, 0x23, 0x07, 0x00, 0x00, 0xed, 0x21, 0x00}
#else
#define IEEE_ADDR_R {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#endif

#ifndef ZB_PRO_ADDRESS_ASSIGNMENT_CB
#define START_RANDOM          0x1111
#define ZED_SHORT_ADDR_1      0xa276
#define ZED_SHORT_ADDR_2      0xb4e4
#undef SEND_CONFLICT_LINK_STATUS
#else
#define START_RANDOM          0x1111
#define ZR_SHORT_ADDR         0x1111
#define ZED_SHORT_ADDR_1      0x2222
#define ZED_SHORT_ADDR_2      0x5077
#endif

#endif
