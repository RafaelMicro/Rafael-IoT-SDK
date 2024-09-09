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
#ifndef __TEST_COMMON__H__
#define __TEST_COMMON__H_

#define ZB_EXIT( _p )

/* Predefined long IEEE addresse for testing devices */
#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R3 {0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R4 {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00}

/* Predefined short addresses for testing devices */
#ifdef ZB_PRO_ADDRESS_ASSIGNMENT_CB
static const zb_uint16_t    g_short_addr_r2 = 0x2222;
static const zb_uint16_t    g_short_addr_r3 = 0x3333;
static const zb_uint16_t    g_short_addr_r4 = 0x4444;
#else
#define SHORT_ADDR_ZR4            0xABCD
#define START_RANDOM              0x13579bdf
#endif

/* Set ZC timeouts */
#define TIME_ZC_CONNECTION          (25 * ZB_TIME_ONE_SECOND)
#define TIME_ZC_SEND_DATA           ((8 * ZB_TIME_ONE_SECOND) + TIME_ZC_CONNECTION)

/* Set ZR3 timeouts */
#define TIME_ZR3_SET_INVISIBLE_MODE (23 * ZB_TIME_ONE_SECOND)

/* Set ZR4 timeouts */
#define TIME_ZR4_CHANGE_SHORT_ADDR  (1  * ZB_TIME_ONE_SECOND)
#define TIME_ZR4_SEND_ROURE_REC     (3  * ZB_TIME_ONE_SECOND) 

/* Set test profile transmission settings */
#define TEST_BUFFER_LEN              0x0A
#define TEST_PACKET_COUNT            35
#define TEST_PACKET_COUNT_THRESHOLD  2
#define TEST_PACKET_DELAY            1
#define TEST_DST_ADDR                g_ieee_addr_r4
//#define TEST_CHANNEL                 (1l << 24)

/* Use data indication callback on ZR4 */
//#define ZR4_USE_DATA_INDICATION

/* Send device announce from ZR3 before enable invisible mode */
#define ZR3_SEND_DEV_ANNCE_BEFORE_FILTERING

#endif
