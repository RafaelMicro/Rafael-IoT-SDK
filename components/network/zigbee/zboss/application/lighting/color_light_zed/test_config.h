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

#define DUT_ADDR 0
#define DUT_ENDPOINT 5
#define DUT_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT

#define TH_ENDPOINT 10
#define TH_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT

#define ED_IEEE_ADDR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define ZC_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

#define TEST_PAN_ID  0x1aaa

#endif /* TEST_CONFIG_H */
