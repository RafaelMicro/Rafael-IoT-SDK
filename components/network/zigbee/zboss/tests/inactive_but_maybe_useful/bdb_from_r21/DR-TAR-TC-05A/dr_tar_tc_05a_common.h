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
#ifndef __DR_TAR_TC_05A_
#define __DR_TAR_TC_05A_


ZB_PACKED_PRE struct dut_nvram_app_dataset_s
{
  zb_uint16_t on_off_arr[2];
} ZB_PACKED_STRUCT;

typedef struct dut_nvram_app_dataset_s dut_nvram_app_dataset_t;


static zb_ieee_addr_t g_ieee_addr_dut    = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static zb_ieee_addr_t g_ieee_addr_thc1   = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};
static zb_ieee_addr_t g_ieee_addr_thr1   = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
static zb_ieee_addr_t g_ieee_addr_thr1c2 = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
static zb_ieee_addr_t g_ieee_addr_thr2   = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

static zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


/* thc1 and thr1 start f&b as targets after DUT joins (triggered by device_annce_cb) */
#define DUT_FB_START_DELAY  (10 * ZB_TIME_ONE_SECOND)
#define THC1_FB_START_DELAY (2 * ZB_TIME_ONE_SECOND)
#define THR1_FB_START_DELAY (7 * ZB_TIME_ONE_SECOND)
#define FB_DURATION         (20)

/* DUT is initiator, THr1 is target */
#define DUT_ENDPOINT1 1
#define DUT_ENDPOINT2 2

#define TH_ZCRX_ENDPOINT 177

#define DUT_CLOSE_NETWORK_DELAY       (5 * ZB_TIME_ONE_SECOND)
#define THR1_MUTE_THC1_PACKETS        (30 * ZB_TIME_ONE_SECOND)
#define THR2_MUTE_THR1C2_PACKETS      (10 * ZB_TIME_ONE_SECOND)
#define THR1C2_START_TEST_DELAY       (20 * ZB_TIME_ONE_SECOND)
#define THR1C2_SEND_MGMT_LEAVET_DELAY (35 * ZB_TIME_ONE_SECOND)
/* Delay after DUT joins for thc1 and thc1 */
#define TH_ZCRX_CLOSE_NETWORK_DELAY  (5 * ZB_TIME_ONE_SECOND)
#define THR2_REJOIN_TO_NETWORK_DELAY (10 * ZB_TIME_ONE_SECOND)
/* Delays after f&b procedure expiration */
#define THR1_REJOIN_TO_NETWORK_DELAY (30 * ZB_TIME_ONE_SECOND)
#define THC1_SEND_MGMT_LEAVE_DELAY   (40 * ZB_TIME_ONE_SECOND)
/* Delay after THC1_SEND_MGMT_LEAVE_DELAY */
#define THC1_RETRIGGER_STEERING      (60 * ZB_TIME_ONE_SECOND)
/* Delay after leaving network */
#define DUT_RETRIGGER_STEERING       (5 * ZB_TIME_ONE_SECOND)

#define TH_SHORT_DELAY (1 * ZB_TIME_ONE_SECOND)
/* Uncomment line below if nvram is needed in test */
#define USE_NVRAM_IN_TEST


#endif /* __DR_TAR_TC_05A_ */
