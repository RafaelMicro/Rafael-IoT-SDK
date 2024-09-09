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
#ifndef __DR_TAR_TC_03D_
#define __DR_TAR_TC_03D_


zb_ieee_addr_t g_ieee_addr_dut   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_thc1  = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
zb_ieee_addr_t g_ieee_addr_thr1  = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_thr2  = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


/* Dealy after DUT broadcasts Device Announce */
#define THC1_SEND_LEAVE_DELAY_DUT_ZED (10 * ZB_TIME_ONE_SECOND)
#define THC1_SEND_LEAVE_DELAY_DUT_ZR  (50 * ZB_TIME_ONE_SECOND)
#define THR1_SEND_LEAVE_DELAY_START   (15 * ZB_TIME_ONE_SECOND)
#define THR1_SEND_LEAVE_DELAY_NEXT    (5 * ZB_TIME_ONE_SECOND)
#define THR2_SEND_LEAVE_DELAY         (30 * ZB_TIME_ONE_SECOND)


#endif /* __DR_TAR_TC_03D_ */
