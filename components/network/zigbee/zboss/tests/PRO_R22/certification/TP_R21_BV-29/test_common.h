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
#ifndef __TP_R21_BV_29_TEST_COMMON
#define __TP_R21_BV_29_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05

//#define TEST_CHANNEL (1l << 24)

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZC {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

//zb_uint16_t g_dest_group_addr = 0x1234;

/* Key for DUT ZC */
static const zb_uint8_t g_nwk_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

/* Extended Pan Id */
/* Key 0 */
static const zb_uint8_t g_nwk_key0[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};
/* Key 1 */
static const zb_uint8_t g_nwk_key1[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                              0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
/* Extended Pan Id */
static const zb_ieee_addr_t g_ext_pan_id = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

/* Delay between gZR start and sending Reset Packet count message */
//define TEST_SEND_FIRST_CMD_DELAY (10 * ZB_TIME_ONE_SECOND)

#endif  /* __TP_R21_BV_29_TEST_COMMON */
