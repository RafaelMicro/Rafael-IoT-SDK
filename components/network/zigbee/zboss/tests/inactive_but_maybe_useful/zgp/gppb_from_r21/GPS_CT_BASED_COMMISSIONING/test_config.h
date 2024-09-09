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

#define DUT_GPS_ADDR 0
#define DUT_GPS_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT

#define NWK_KEY { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0 }

/* ZGPD Src ID */
#define TEST_ZGPD_SRC_ID 0x12345678
#define TEST_ZGPD2_SRC_ID 0x11223344

#define TEST_OOB_KEY { 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb }
#define TEST_GROUP_KEY { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf }

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#define DUT_GPS_COMMUNICATION_MODE ZGP_COMMUNICATION_MODE_LIGHTWEIGHT_UNICAST

#define DUT_GPS_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define TH_GPD_IEEE_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
#define TH_GPD_IEEE_ADDR1 {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}
#define TH_GPD_IEEE_ADDR2 {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11}
#define TH_TOOL_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TEST_PAN_ID  0x1aaa

#ifndef ZB_NSNG

#ifndef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define USE_HW_DEFAULT_BUTTON_SEQUENCE
#endif

#endif

#endif /* TEST_CONFIG_H */
