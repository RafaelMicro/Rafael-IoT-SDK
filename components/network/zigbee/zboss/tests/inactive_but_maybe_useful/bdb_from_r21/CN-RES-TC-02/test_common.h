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
#ifndef __CN_RES_TC_02_
#define __CN_RES_TC_02_

zb_ieee_addr_t g_ieee_addr_c    = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
zb_ieee_addr_t g_ieee_addr_r    = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_ed   = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
zb_ieee_addr_t g_ieee_addr_dut  = {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

#define TEST_START_TIMEOUT      (25 * ZB_TIME_ONE_SECOND)
#define TEST_MGMT_LQI_REQ_DELAY (2 * ZB_TIME_ONE_SECOND)
#define TEST_SCAN_DURATION      3

#endif  /* __CN_RES_TC_02_ */
