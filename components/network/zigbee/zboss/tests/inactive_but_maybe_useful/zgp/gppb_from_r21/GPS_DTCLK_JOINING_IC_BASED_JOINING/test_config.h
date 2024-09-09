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
/* PURPOSE: Test configuration
*/

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H 1

#define NWK_KEY { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0 }

#define TH_ZC_IEEE_ADDR {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}
#define TH_GPP_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define DUT_GPS_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define SETUP_IC {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23, 0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05, /* CRC */ 0xC3, 0xB5}

#define TEST_PAN_ID  0x1aaa
#define DUT_ENDPOINT 10

/* ZGPD Src ID */
#define TEST_ZGPD_SRC_ID 0x12345678

#define TEST_SEC_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }
#define TEST_OOB_KEY { 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb }

/* DUT and TH Settings */
#define TEST_STEP1_DUT_DELAY (15) /* Used by DUT and TH to wait for step 1 completion */
#define TEST_STEP2_DUT_DELAY (15)
#define TEST_STEP_REST_DUT_DELAY (90) /* Delay for DUT before finish the test */

/* How many seconds DUT will stay on this test step before continuing to the next state */
#define TEST_STEP1_DUT_DELAY   (15)
#define TEST_STEP2A_DUT_DELAY  (20)
#define TEST_STEP2B_DUT_DELAY  (20)
#define TEST_STEP3A_DUT_DELAY  (20)
#define TEST_STEP3B_DUT_DELAY  (20)
#define TEST_STEP4_DUT_DELAY   (20)

/* TH Settings */
#define TEST_STEP1_TH_DELAY (30) /* Delay before Reading Sink Table on step 2 */
#define TEST_STEP2_TH_DELAY (10) /* Delay before Reading Sink Table on step 3 */


/* GPD defines */
#define TEST_GPD_DELAY_BEFORE_STEP3 18
#define TEST_GPD_DELAY_BEFORE_STEP4 15

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#ifndef ZB_NSNG

#ifndef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define USE_HW_DEFAULT_BUTTON_SEQUENCE
#endif

#endif

#endif /* TEST_CONFIG_H */
