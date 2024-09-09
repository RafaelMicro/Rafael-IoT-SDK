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

#define DUT_GPPB_ADDR 0
#define DUT_GPPB_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT
#define DUT_GPPB_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define TH_GPS1_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define TH_GPS2_IEEE_ADDR {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define TH_GPD_IEEE_ADDR {0xba, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define TEST_PAN_ID  0x1aaa

#define TEST_ZGPD_SRC_ID 0x12345678
#define TEST_ZGPD2_SRC_ID 0x12345679
#define TEST_MAC_DSN_VALUE 0x91
//#define TEST_DEFAULT_COMM_WINDOW 20
#define TEST_DEFAULT_COMM_WINDOW 180

#define OOB_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#ifndef ZB_NSNG

#ifndef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define USE_HW_DEFAULT_BUTTON_SEQUENCE
#endif

#endif

#endif /* TEST_CONFIG_H */
