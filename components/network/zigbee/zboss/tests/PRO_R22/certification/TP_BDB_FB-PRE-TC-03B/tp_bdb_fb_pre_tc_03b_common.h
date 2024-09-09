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
#ifndef __TP_BDB_FB_PRE_TC_03B_
#define __TP_BDB_FB_PRE_TC_03B_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THE1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define DUT_ZC_FB_FIRST_START_DELAY		(27 * ZB_TIME_ONE_SECOND)
#define DUT_ZR_ZED_FB_FIRST_START_DELAY		(17 * ZB_TIME_ONE_SECOND)
#define DUT_FB_START_DELAY                      (15 * ZB_TIME_ONE_SECOND)

#define THR1_FB_FIRST_START_DELAY		(15 * ZB_TIME_ONE_SECOND)
#define THR1_FB_START_DELAY                     (15 * ZB_TIME_ONE_SECOND)
#define THR1_MGMT_BIND_REQ_DELAY_SHORT		(1 * ZB_TIME_ONE_SECOND)
#define THR1_MGMT_BIND_REQ_DELAY_LONG		(5 * ZB_TIME_ONE_SECOND)
#define THR1_RESP_DELAY                         (4 * ZB_TIME_ONE_SECOND)
#define THR1_INCR_TS_LONG_DELAY			(25 * ZB_TIME_ONE_SECOND)
#define THR1_INCR_TS_SHORT_DELAY                (24 * ZB_TIME_ONE_SECOND)

#define THE1_FB_FIRST_START_DELAY		(105 * ZB_TIME_ONE_SECOND)
#define THE1_FB_START_DELAY                     (15 * ZB_TIME_ONE_SECOND)

#define FB_DURATION                             (10)

/* DUT is initiator, THr1 and THe1 are targets */
#define DUT_ENDPOINT1 8

#define THR1_ENDPOINT 13
#define THE1_ENDPOINT 124

#endif /* __TP_BDB_FB_PRE_TC_03B_ */
