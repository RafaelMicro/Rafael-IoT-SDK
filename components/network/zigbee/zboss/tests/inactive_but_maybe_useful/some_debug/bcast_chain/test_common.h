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
#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

#define TEST_PAGE    0
#define TEST_CHANNEL 11

static zb_ieee_addr_t g_ieee_addr_zc = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static zb_ieee_addr_t g_ieee_addr_ed1 = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_ed2 = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static zb_ieee_addr_t g_ieee_addr_r1 = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
static zb_ieee_addr_t g_ieee_addr_r2 = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
static zb_ieee_addr_t g_ieee_addr_r3 = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
static zb_ieee_addr_t g_ieee_addr_r4 = {0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44};
static zb_ieee_addr_t g_ieee_addr_r5 = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
static zb_ieee_addr_t g_ieee_addr_r6 = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
static zb_ieee_addr_t g_ieee_addr_r7 = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static zb_ieee_addr_t g_ieee_addr_r8 = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
static zb_ieee_addr_t g_ieee_addr_r9 = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};

#endif // #define _TEST_COMMON_H_
