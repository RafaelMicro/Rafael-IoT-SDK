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
#ifndef __TP_PED_7_TEST_COMMON
#define __TP_PED_7_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05
//#define TEST_CHANNEL (1l << 24)

static const zb_uint16_t TEST_PAN_ID = 0x1AAA;

#define IEEE_ADDR_DUT_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_gZED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZED2 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/* Key 0 */
static const zb_uint8_t g_nwk_key0[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* Key 1 */
static const zb_uint8_t g_nwk_key1[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                              0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
/* Extended Pan Id */
static const zb_ieee_addr_t g_ext_pan_id = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define TIME_ZC_LQI_REQ1		        (20 * ZB_TIME_ONE_SECOND)
#define TIME_ZC_LQI_REQ2		        (80 * ZB_TIME_ONE_SECOND)
#define TIME_ZC_LQI_REQ3		        (220 * ZB_TIME_ONE_SECOND)

#endif  /* __TP_PED_7_TEST_COMMON */
