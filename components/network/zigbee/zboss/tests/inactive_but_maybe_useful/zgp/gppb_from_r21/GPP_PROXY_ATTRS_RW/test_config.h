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

#define DUT_GPPB_ADDR 0
#define DUT_GPPB_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT

#define TH_TOOL_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define DUT_GPPB_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define TEST_PAN_ID  0x1aaa

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

/* ZGPD Src ID */
#define TEST_ZGPD_SRC_ID 0x12345678
#define TEST_ZGPD_SRC_ID2 0x11223344
#define TEST_ZGPD_SRC_ID3 0x12345679
#define TEST_ZGPD_IEEE_ADDR {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}

#define ZGPD_INITIAL_FRAME_COUNTER 0x81

#endif /* TEST_CONFIG_H */
