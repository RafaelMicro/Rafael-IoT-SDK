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
#ifndef __TP_R21_BV_26_TEST_COMMON
#define __TP_R21_BV_26_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05
//#define TEST_CHANNEL (1l << 24)

static const zb_uint16_t TEST_PAN_ID = 0x1AAA;

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZC {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}
#define IEEE_ADDR_gZR1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZR2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZR3 {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}


/* delay for gzr1 to retrieve DUT's neighbour table */
#define TEST_RETRIEVE_DUT_NBT_DELAY 145 * ZB_TIME_ONE_SECOND

#endif  /* __TP_R21_BV_26_TEST_COMMON */
