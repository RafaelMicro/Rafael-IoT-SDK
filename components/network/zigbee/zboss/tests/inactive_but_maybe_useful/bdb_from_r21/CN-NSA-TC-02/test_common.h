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
#ifndef __CN_NSA_TC_02_
#define __CN_NSA_TC_02_


struct ZB_PACKED_PRE thcx_nvram_app_dataset_s
{
  zb_uint32_t current_test_step;
} ZB_PACKED_STRUCT;

typedef struct thcx_nvram_app_dataset_s thcx_nvram_app_dataset_t;


zb_ieee_addr_t g_ieee_addr_dut  = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_thc1 = {0x01, 0x01, 0x01, 0x01, 0xbb, 0xbb, 0xbb, 0xbb};
zb_ieee_addr_t g_ieee_addr_thc2 = {0x02, 0x02, 0x02, 0x02, 0xbb, 0xbb, 0xbb, 0xbb};
zb_ieee_addr_t g_ieee_addr_thc3 = {0x03, 0x03, 0x03, 0x03, 0xbb, 0xbb, 0xbb, 0xbb};

zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_DUT_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_DUT_SECONDARY_CHANNEL_SET (1l<<13)

#define TEST_BDB_THC1_PRIMARY_CHANNEL (1l << 11)
#define TEST_BDB_THC2_PRIMARY_CHANNEL (1l << 11)
#define TEST_BDB_THC3_PRIMARY_CHANNEL (1l << 13)
#define TEST_BDB_THCX_SECONDARY_CHANNEL_SET 0


#define TEST_THC1_INVALID_PROTOCOL_ID          0x01
#define TEST_THC1_INVALID_PROTOCOL_VER         0x08
#define TEST_TCH1_SHORT_BEACON_PL_LEN          2
#define TEST_TCH1_LONG_BEACON_PL_EXTRA_BYTES   5

#define TEST_THC2_INVALID_STACK_PROFILE 0x03
#define TEST_TCH2_SHORT_BEACON_PL_LEN   11

#endif /* __CN_NSA_TC_02_ */
