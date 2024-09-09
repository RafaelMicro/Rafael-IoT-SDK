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
#ifndef __FB_CDP_TC_01_
#define __FB_CDP_TC_01_

static zb_ieee_addr_t g_ieee_addr_dut   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_other = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
static zb_ieee_addr_t g_ieee_addr_thr1  = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_thr2  = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


#define TEST_THR1_START_TEST_DELAY          (30 * ZB_TIME_ONE_SECOND)
#define TEST_DUT_START_COMMUNICATION_DELAY  (70 * ZB_TIME_ONE_SECOND)
#define TEST_THR1_WAITS_DUT_DELAY           (60 * ZB_TIME_ONE_SECOND)
#define TEST_STARTUP_DELAY                  (10 * ZB_TIME_ONE_SECOND)
#define TEST_SEND_CMD_SKEW                  (4 * ZB_TIME_ONE_SECOND)
#define TEST_FB_TARGET_DURATION             (15)

/* DUT is initiator, THr2 is target */
#define DUT_ENDPOINT1 8
#define DUT_ENDPOINT2 9
#define DUT_MATCHING_CLUSTER ZB_ZCL_CLUSTER_ID_ON_OFF
#define THR2_ENDPOINT1 13
#define THR2_ENDPOINT2 14
#define GROUP_ADDRESS 0x1234
/* thr1 reads binding table of the dut at test start and stores it to
   it's internal array. Then unbind default values. */
#define MAX_THR1_BIND_TABLE_CACHE_SIZE 16


#define TEST_WRONG_SRC_EP DUT_ENDPOINT2
#define TEST_WRONG_DEST_EP 7
#define TEST_WRONG_CLUSTER ZB_ZCL_CLUSTER_ID_BASIC
#define TEST_UNSUP_CLUSTER ZB_ZCL_CLUSTER_ID_COLOR_CONTROL
#define TEST_INACTIVE_DUT_EP 97


#endif /* __FB_CDP_TC_01_ */
