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
#ifndef _TP_NWK_BV_05_TEST_COMMON
#define _TP_NWK_BV_05_TEST_COMMON

#define TEST_CHANNEL (1l << 16)
#define TEST_PAN_ID  0x1AAA
#define MGMT_LQI_REQUEST_DELAY_SECONDS 5

extern zb_ieee_addr_t g_ieee_addr_zc;
extern zb_ieee_addr_t g_ieee_addr_r1;
extern zb_ieee_addr_t g_ieee_addr_r2;
extern zb_ieee_addr_t g_ieee_addr_r3;
extern zb_ieee_addr_t g_ieee_addr_r4;
extern zb_ieee_addr_t g_ieee_addr_r5;
extern zb_ieee_addr_t g_ieee_addr_r6;
extern zb_ieee_addr_t g_ieee_addr_r7;
extern zb_ieee_addr_t g_ieee_addr_r8;
extern zb_ieee_addr_t g_ieee_addr_r9;
extern zb_ieee_addr_t g_ieee_addr_r10;
extern zb_ieee_addr_t g_ieee_addr_r11;
extern zb_ieee_addr_t g_ieee_addr_r12;
extern zb_ieee_addr_t g_ieee_addr_r13;
extern zb_ieee_addr_t g_ieee_addr_r14;
extern zb_ieee_addr_t g_ieee_addr_r15;
extern zb_ieee_addr_t g_ieee_addr_r16;
extern zb_ieee_addr_t g_ieee_addr_r17;

void test_after_startup_action(zb_uint8_t param);

/* #define ONLY_TWO_DEV_FOR_ROUTERS 1 */

#endif  /* _TP_NWK_BV_05_TEST_COMMON */
