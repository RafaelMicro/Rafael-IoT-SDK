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
#ifndef __CS_NFS_TC_02_
#define __CS_NFS_TC_02_


struct ZB_PACKED_PRE th_nvram_app_dataset_s
{
  zb_uint32_t current_test_step;
} ZB_PACKED_STRUCT;

typedef struct th_nvram_app_dataset_s th_nvram_app_dataset_t;


zb_ieee_addr_t g_ieee_addr_dut   = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_thr1  = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_thr2  = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};


zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

zb_uint8_t g_link_key[16] = {0x66, 0xb6, 0x90, 0x09, 0x81, 0xe1, 0xee, 0x3c,
                             0xa4, 0x20, 0x6b, 0x6b, 0x86, 0x1c, 0x02, 0xbb};


#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


#define TH_REVERT_KEY_DELAY                   (1 * ZB_TIME_ONE_SECOND)
#define TH_WAIT_FOR_TCLK_UPDATE_DELAY         (7 * ZB_TIME_ONE_SECOND)
#define TH_WAIT_FOR_PERMIT_JOIN_EXPIRES_DELAY (181 * ZB_TIME_ONE_SECOND)
#define TH_RESEND_REQUEST_KEY_DELAY           (5 * ZB_TIME_ONE_SECOND)
#define DUT_RETRIGGER_NETWORK_STEERING_DELAY  (240 * ZB_TIME_ONE_SECOND)


#define USE_NVRAM_IN_TEST


#endif /* __CS_NFS_TC_02_ */
