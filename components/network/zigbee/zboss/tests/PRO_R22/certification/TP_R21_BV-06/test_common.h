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
#ifndef __TP_R21_BV_06_TEST_COMMON
#define __TP_R21_BV_06_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05

#define IEEE_ADDR_gZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZR {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZR {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZED {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

static zb_ieee_addr_t g_ext_pan_id = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_addr_tc   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif  /* __TP_R21_BV_06_TEST_COMMON  */
