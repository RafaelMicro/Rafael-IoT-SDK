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

#define TH_GPT_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define TH_GPP_IEEE_ADDR {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define DUT_ZR_IEEE_ADDR {0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TEST_PAN_ID  0x1aaa

/* ZGPD Src ID */
#define  TEST_ZGPD_SRC_ID 0x12345678

#define TEST_SEC_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#endif /* TEST_CONFIG_H */
