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

#ifndef __TP_PED_13_TEST_COMMON
#define __TP_PED_13_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05
//#define TEST_CHANNEL (1l << 24)

#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_ZR {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}


static const zb_ieee_addr_t g_ext_pan_id = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const zb_uint16_t g_pan_id = 0x1aaa;

/* Key 0 */
static const zb_uint8_t g_nwk_key0[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};
/* Key 1 */
static const zb_uint8_t g_nwk_key1[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                                           0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

/* delay between ZC start and checking ZR2 neighbour table */
#define TEST_CHECK_DUTZC_NBT_DELAY 	(40 * ZB_TIME_ONE_SECOND)
#define TEST_SEND_BUFFER_TEST_REQ_DELAY	(60 * ZB_TIME_ONE_SECOND)
#define TEST_ZED_POLL_TIMEOUT_MS	(7500)

#define DUTZR_CLOSE_PERMIT_JOIN_DELAY 	(15 * ZB_TIME_ONE_SECOND)

#endif  /* __TP_PED_13_TEST_COMMON */
