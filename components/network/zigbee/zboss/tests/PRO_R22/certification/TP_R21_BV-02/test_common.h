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
#ifndef __TP_R21_BV_02_TEST_COMMON
#define __TP_R21_BV_02_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05

//#define TEST_CHANNEL (1l << 24)

static const zb_uint16_t TEST_PAN_ID = 0x1AAA;

#define IEEE_ADDR_gZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
#define IEEE_ADDR_gZR {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
#define IEEE_ADDR_gZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define IEEE_ADDR_DUT_ZR {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

static const zb_ieee_addr_t g_addr_tc         = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const zb_ieee_addr_t g_ext_pan_id      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Time delay between gZC start and sending mgmt_leave requests  */
#define TEST_SEND_MGMT_LEAVE1_TO_ZED   (75 * ZB_TIME_ONE_SECOND)
#define TEST_SEND_MGMT_LEAVE2_TO_ZED   (95 * ZB_TIME_ONE_SECOND)
#define TEST_SEND_MGMT_LEAVE1_TO_ZR    (115 * ZB_TIME_ONE_SECOND)
#define TEST_SEND_MGMT_LEAVE2_TO_ZR    (135 * ZB_TIME_ONE_SECOND)


#endif  /* __TP_R21_BV_02_TEST_COMMON  */
