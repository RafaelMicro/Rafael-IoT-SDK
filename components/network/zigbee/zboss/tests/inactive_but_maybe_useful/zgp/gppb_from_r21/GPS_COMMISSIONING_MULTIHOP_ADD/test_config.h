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

#define TH_GPD_IEEE_ADDR {0xA1, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define DUT_GPS_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define TH_GPP_IEEE_ADDR {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}

#define TEST_PAN_ID  0x1aaa

/* ZGPD Src ID */
#define TEST_ZGPD_SRC_ID 0x12345678
#define TEST_ZGPD_EP_X 3

#define TEST_GPS_GET_COMMISSIONING_WINDOW()  \
  ((ZGP_GPS_COMMISSIONING_EXIT_MODE & ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION)? \
    ZGP_GPS_COMMISSIONING_WINDOW: 0)

#define DUT_ACTIONS_WINDOW_0   (5)
#define DUT_ACTIONS_WINDOW_1   (6)
#define DUT_ACTIONS_WINDOW_2A  (16)
#define DUT_ACTIONS_WINDOW_3   (51)
#define DUT_ACTIONS_WINDOW_3J  (5)
#define DUT_ACTIONS_WINDOW_3L  (26)
#define DUT_ACTIONS_WINDOW_456 (8)

#define DUT_COMM_WINDOW_0      (4)
#define DUT_COMM_WINDOW_1      (5)
#define DUT_COMM_WINDOW_3      (50)
#define DUT_COMM_WINDOW_3J     (4)
#define DUT_COMM_WINDOW_3L     (25)
#define DUT_COMM_WINDOW_456    (7)


#define TEST_SEC_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }
#define TEST_OOB_KEY { 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb }

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#ifndef ZB_NSNG

#ifndef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define USE_HW_DEFAULT_BUTTON_SEQUENCE
#endif

#endif

#endif /* TEST_CONFIG_H */
