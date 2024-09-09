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
#ifndef ___SEC____TEST_COMMON_25_
#define ___SEC____TEST_COMMON_25_

#define ZB_EXIT( _p )

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#if (defined ZB_NS_BUILD) | (defined ZB_NSNG)
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#else
#define IEEE_ADDR_R1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_R2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

/* NWK Key 0 */
static const zb_uint8_t g_key0[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};
/* NWK const Key 1 */
static const zb_uint8_t g_key1[16] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};

#define TEST_ENABLED

/* Security level configuration */
#define SECURITY_LEVEL 0x05

/* Timeout setup  */
#define TP_SEC_BV_25_I_STEP_1_DELAY_ZC   (120 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_25_I_STEP_1_TIME_ZC    (5 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_25_I_STEP_3_TIME_ZC    (5 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_25_I_STEP_5_TIME_ZC    (5 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_25_I_STEP_6_DELAY_ZED  (140 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_25_I_STEP_6_TIME_ZED   (5 * ZB_TIME_ONE_SECOND)

#define TIME_ZED_POLL_TIMEOUT_MS	   350000

/* insecure join configuration */
#define INSECURE_JOIN_ZC                ZB_FALSE
#define INSECURE_JOIN_ZR1               ZB_FALSE
#define INSECURE_JOIN_ZR2               ZB_FALSE
#define INSECURE_JOIN_ZED1              ZB_FALSE

/* #define CHANNEL 24 */

#endif
