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
#ifndef __TP_R21_BV_25_TEST_COMMON
#define __TP_R21_BV_25_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05


static const zb_uint16_t TEST_PAN_ID = 0x1AAA;

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZR {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZR_RX g_ieee_addr_gzrx  = {0x00, 0x00, 0x00, 0x00, 0xe1, 0x00, 0x00, 0x00}

/* Key for DUT ZC */
static const zb_uint8_t g_nwk_key[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

/* Extended Pan Id */
static const zb_ieee_addr_t g_ext_pan_id = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Delay between gZR start and sending Reset Packet count message */
#define TEST_SEND_RESET_PCK_CNT_MSG_DELAY (10 * ZB_TIME_ONE_SECOND)
/* TC says 81, but 2 bytes are used for packet length in the Test profile */
#define TEST_PACKET_SIZE (81-2)


#endif  /* __TP_R21_BV_25_TEST_COMMON */
