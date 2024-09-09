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
#ifndef __DN_KTU_TC_01_
#define __DN_KTU_TC_01_

static zb_ieee_addr_t g_ieee_addr_thr1 = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_dut_zed  = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_dut_zr  = {0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET ((1 << 11) | (1 << 13))
#define TEST_BDB_SECONDARY_CHANNEL_SET ((1 << 20) | (1 << 21) | (1 << 22))

#endif /* __DN_DNS_TC_02B_ */
