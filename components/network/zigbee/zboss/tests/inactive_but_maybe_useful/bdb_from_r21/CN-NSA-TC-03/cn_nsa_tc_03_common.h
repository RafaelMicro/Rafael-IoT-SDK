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
/* PURPOSE: Common definitions for test.
*/
#ifndef __CN_NSA_TC_03_
#define __CN_NSA_TC_03_


struct ZB_PACKED_PRE thx_nvram_app_dataset_s
{
  zb_uint32_t current_test_step;
} ZB_PACKED_STRUCT;

typedef struct thx_nvram_app_dataset_s thx_nvram_app_dataset_t;


static zb_ieee_addr_t g_ieee_addr_dut  = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_thc1 = {0x01, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
static zb_ieee_addr_t g_ieee_addr_thc2 = {0x02, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
static zb_ieee_addr_t g_ieee_addr_thr1 = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_DUT_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_DUT_SECONDARY_CHANNEL_SET (1l<<13)

#define TEST_BDB_THC1_PRIMARY_CHANNEL (1l << 11)
#define TEST_BDB_THC2_PRIMARY_CHANNEL (1l << 13)
#define TEST_BDB_THR1_PRIMARY_CHANNEL (1l << 11)
#define TEST_BDB_THX_SECONDARY_CHANNEL_SET 0


#define THC1_START_TEST_TIMEOUT (6 * ZB_TIME_ONE_SECOND)

#endif /* __CN_NSA_TC_03_ */
