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
#ifndef __CN_NSA_TC_01C_
#define __CN_NSA_TC_01C_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THE1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

static const zb_uint8_t g_nwk_key[16] = {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const zb_uint16_t TEST_PAN_ID = 0x1AAA;
static const zb_uint16_t TEST_DUT_ZR_SHORT_ADDR = 0x2bbb;

/* All delays is relative */
#define TEST_ZED1_MGMT_PERMIT_JOIN_180	   (30 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_00      (20 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_SHORT   (20 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_MGMT_PERMIT_JOIN_LONG    (30 * ZB_TIME_ONE_SECOND)
#define TEST_ZED1_CHECK_DUT_STEERING       (20 * ZB_TIME_ONE_SECOND)

#define TEST_THR1_TRIGGER_STEERING         (187 * ZB_TIME_ONE_SECOND)

#endif /* __CN_NSA_TC_01C_ */
