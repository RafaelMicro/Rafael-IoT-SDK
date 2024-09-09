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
/* PURPOSE: common definitions for this test.
*/
#ifndef __CS_ICK_TC_02_
#define __CS_ICK_TC_02_

static zb_ieee_addr_t g_ieee_addr_dut1  = {0x01, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_dut2  = {0x02, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_thr1  = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_thr2  = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_thrx  = {0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00};


static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static zb_uint8_t g_ic1[16+2] = {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                          0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                          /* CRC */ 0xC3, 0xB5};
/* Derived key is 66 b6 90 09 81 e1 ee 3c a4 20 6b 6b 86 1c 02 bb (normal). Add
 * it to Wireshark. */

static zb_uint8_t g_ic2[16+2] = {0xE8, 0x77, 0xD4, 0x40, 0x7A, 0x93, 0x97, 0x23,
                          0xA5, 0x22, 0x39, 0xA0, 0xA1, 0xB6, 0xD5, 0x33,
                          /* CRC */ 0xCA, 0x9E};
/* Derived key is ca.7f.28.42.eb.93.35.f5.27.60.f2.49.c0.72.c4.62 (normal). Add
 * it to Wireshark. */

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


/* Delay for zc to enter invalid install code: from dut start */
#define INSTALL_INVALID_IC_CODE_DELAY (15 * ZB_TIME_ONE_SECOND)
/* Delay for zc to enter valid install code:
   time passed after INSTALL_INVALID_IC_CODE_DELAY seconds */
#define INSTALL_VALID_IC_CODE_DELAY (5 * ZB_TIME_ONE_SECOND)


#endif /* __CS_ICK_TC_02_ */
