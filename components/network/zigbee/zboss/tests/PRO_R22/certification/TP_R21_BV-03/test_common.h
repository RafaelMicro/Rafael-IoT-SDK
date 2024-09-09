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
#ifndef __TP_R21_BV_03_TEST_COMMON
#define __TP_R21_BV_03_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05

#define IEEE_ADDR_gZC1 {0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZC2 {0x22, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZC3 {0x33, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZR {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}


static const zb_uint16_t g_dutzr_pan_id = 0x1234;


/* Key for gZR and DUT ZC */
static const zb_uint8_t g_nwk_key_zr[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const zb_uint8_t g_nwk_key_zc[16] = { 0x12, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};

/* Delay between DUT ZR start's and opens it's network for joining */
#define TP_R21_BV_03_STEP_4_DELAY_DUTZR (5 * ZB_TIME_ONE_SECOND)
#define TP_R21_BV_03_STEP_4_TIME_DUTZR  (5 * ZB_TIME_ONE_SECOND)

#define SEND_DATA_ITERATION_DELAY 2 /* Assume 2 Beacon intervals == 32 ms */
#define STOP_SENDING_DATA_PACKETS_DELAY (300 * ZB_TIME_ONE_SECOND)

#define ZC1_CHANNEL  11
#define ZC2_CHANNEL  20
#define ZC3_CHANNEL  25
//#define TEST_CHANNEL 19
//#define DUT_CHANNEL_MASK ( (1l << ZC1_CHANNEL) | (1l << ZC2_CHANNEL) | (1l << ZC3_CHANNEL) | (1l << TEST_CHANNEL) )
#define DUT_CHANNEL_MASK ( (1l << ZC1_CHANNEL) | (1l << ZC2_CHANNEL) | (1l << ZC3_CHANNEL) | (1l << 15) )
//#define DUT_CHANNEL_MASK ZB_TRANSCEIVER_ALL_CHANNELS_MASK

#endif  /* __TP_R21_BV_03_TEST_COMMON  */
