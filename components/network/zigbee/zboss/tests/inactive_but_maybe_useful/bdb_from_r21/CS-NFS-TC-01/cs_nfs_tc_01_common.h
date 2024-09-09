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
#ifndef __CS_NFS_TC_01_
#define __CS_NFS_TC_01_


struct ZB_PACKED_PRE thc_nvram_app_dataset_s
{
  zb_uint32_t current_test_step;
} ZB_PACKED_STRUCT;

typedef struct thc_nvram_app_dataset_s thc_nvram_app_dataset_t;


static zb_ieee_addr_t g_ieee_addr_thc1  = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
static zb_ieee_addr_t g_ieee_addr_dut   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_KEY_CHANGE_ATTEMPT_MAX 3
#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


#define THC1_REVERT_KEY_DELAY (1 * ZB_TIME_ONE_SECOND)
/* After successful Transport Key with TC LK to DUT THC1 start wait some time,
 * then checks that DUT has virified it's new TC LK. After this delay
 * THC1 can send ant APS/ZDO/ZCL commands to DUT.
 */
#define THC1_WAIT_FOR_TCLK_UPDATE_DELAY (7 * ZB_TIME_ONE_SECOND)


#define USE_NVRAM_IN_TEST


#endif /* __CS_NFS_TC_01_ */
