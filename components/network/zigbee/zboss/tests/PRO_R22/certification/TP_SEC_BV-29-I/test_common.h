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
#ifndef ___SEC____TEST_COMMON_29_
#define ___SEC____TEST_COMMON_29_

#define ZB_EXIT( _p )

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#if (defined ZB_NS_BUILD) | (defined ZB_NSNG)
#define IEEE_ADDR_R {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED2 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#else
#define IEEE_ADDR_R {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ED2 {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#endif

/* NWK Key 0 */
static const zb_uint8_t g_key0[16] = { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0};

#define TEST_ENABLED

/* Security level configuration */
#define SECURITY_LEVEL 0x05

/* Timeout setup  */
#define TP_SEC_BV_29_I_STEP_1_DELAY_ZED1  (20 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_29_I_STEP_1_TIME_ZED1   (5 * ZB_TIME_ONE_SECOND)

#define TP_SEC_BV_29_I_STEP_2_DELAY_ZC  (120 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_29_I_STEP_2_TIME_ZC   (5 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_29_I_STEP_5_TIME_ZC   (10 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_29_I_STEP_8_TIME_ZC   (5 * ZB_TIME_ONE_SECOND)
#define TP_SEC_BV_29_I_STEP_9_TIME_ZC   (5 * ZB_TIME_ONE_SECOND)

#define TIME_ZED_STOP_POLL	       	(5 * ZB_TIME_ONE_SECOND)

#define ZED_LONG_POLL_TO_MS	   	350000
#define ZED1_POLL_TO_3SEC_MS		3000

/* insecure join configuration */
#define INSECURE_JOIN_ZC               ZB_FALSE
#define INSECURE_JOIN_ZR               ZB_FALSE
#define INSECURE_JOIN_ZED1             ZB_FALSE
#define INSECURE_JOIN_ZED2             ZB_FALSE

/* #define CHANNEL 24 */

#endif
