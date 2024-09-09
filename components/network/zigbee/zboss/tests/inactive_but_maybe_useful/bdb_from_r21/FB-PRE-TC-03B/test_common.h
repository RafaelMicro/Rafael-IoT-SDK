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
#ifndef __FB_PRE_TC_03B_
#define __FB_PRE_TC_03B_


zb_ieee_addr_t g_ieee_addr_dut   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_thr1  = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_the1  = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


#define DUT_FB_FIRST_START_DELAY  (14  * ZB_TIME_ONE_SECOND)
#define DUT_FB_START_DELAY        (10  * ZB_TIME_ONE_SECOND)
#define THR1_FB_START_DELAY       (10  * ZB_TIME_ONE_SECOND)
#define THE1_FB_FIRST_START_DELAY (245  * ZB_TIME_ONE_SECOND)
#define THE1_FB_START_DELAY       (10  * ZB_TIME_ONE_SECOND)
#define FB_DURATION               (10)

/* DUT is initiator, THr1 and THe1 are targets */
#define DUT_ENDPOINT1 8

#define THR1_ENDPOINT 13
#define THE1_ENDPOINT 124

#define THR1_DELAY (16 * ZB_TIME_ONE_SECOND)
#define THR1_SHORT_DELAY (2 * ZB_TIME_ONE_SECOND)


#endif /* __FB_PRE_TC_03B_ */
