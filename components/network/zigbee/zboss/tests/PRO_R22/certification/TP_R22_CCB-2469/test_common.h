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
#ifndef __TP_R22_CCB_2469_TEST_COMMON
#define __TP_R22_CCB_2469_TEST_COMMON

/* Security level configuration */
#define SECURITY_LEVEL 0x05

static const zb_uint16_t TEST_PAN_ID = 0xABCD;

#define IEEE_ADDR_gZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_DUT_ZR {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_DUT_ZED {0x01, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_gZED {0x02, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define GROUP_ADDR      0x0001
#define GROUP_EP        0xF0

#define TEST_GZC_NEXT_TS_DELAY	(5  * ZB_TIME_ONE_SECOND)
#define TEST_GZC_STARTUP_DELAY  (40 * ZB_TIME_ONE_SECOND)

#endif  /* __TP_R22_CCB_2469_TEST_COMMON */
