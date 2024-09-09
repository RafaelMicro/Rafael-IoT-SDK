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
#ifndef __DN_KTU_TC_03_
#define __DN_KTU_TC_03_

zb_ieee_addr_t g_ieee_addr_thr1 = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_dut  = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

zb_uint8_t g_ic1[16+2] = {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                          0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                          /* CRC */ 0xC3, 0xB5};
/* Derived key is 66 b6 90 09 81 e1 ee 3c a4 20 6b 6b 86 1c 02 bb (normal). Add
 * it to Wireshark. */

#define TEST_BDB_PRIMARY_CHANNEL_SET ((1 << 11) | (1 << 13))
#define TEST_BDB_SECONDARY_CHANNEL_SET ((1 << 20) | (1 << 21) | (1 << 22))

#endif /* __DN_KTU_TC_03_ */
