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
#ifndef __CN_NSA_TC_01C_
#define __CN_NSA_TC_01C_

static zb_ieee_addr_t g_ieee_addr_dut  = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_thr1 = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_the1 = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET ((1 << 11) | (1l<<12))
#define TEST_BDB_SECONDARY_CHANNEL_SET (1l<<13)


#define TEST_ZED1_SEND_BEACON_REQ_DELAY    (2 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_GET_PEER_ADDR_REQ_DELAY  (15 * ZB_TIME_ONE_SECOND)
#define TEST_CLOSE_NETWORK_DELAY           (10 * ZB_TIME_ONE_SECOND)
#define TEST_SCAN_DURATION                 (3)
/* All delays is relative */
#define TEST_ZED1_STEERING_DELAY           (30 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_00      (15 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_SHORT   (11 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_CHECK_SHORT_DURATION     (15 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_LONG    (5 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_CHECK_LONG_DURATION      (181 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_CHECK_DUT_STEERING       (30 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_UNICAST_MGMT_TO_THR1_180 (10 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_UNICAST_MGMT_TO_DUT_180  (10 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_UNICAST_MGMT_TO_THR1_0   (10 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_UNICAST_MGMT_TO_DUT_0    (10 * ZB_TIME_ONE_SECOND)



#endif /* __CN_NSA_TC_01C_ */
