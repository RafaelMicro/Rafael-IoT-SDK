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
#ifndef __TP_BDB_FB_PIM_TC_01_
#define __TP_BDB_FB_PIM_TC_01_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define TEST_TRIGGER_FB_INITIATOR_DELAY (11 * ZB_TIME_ONE_SECOND)
#define TEST_TRIGGER_FB_TARGET_DELAY    (5 * ZB_TIME_ONE_SECOND)
#define TEST_FB_TARGET_DURATION         (25)

/* DUT is target, THr1 is initiator */
#define DUT_ENDPOINT 8
#define THR1_ENDPOINT 13
#define CMD_RESP_TIMEOUT (3 * ZB_TIME_ONE_SECOND)


#endif /* __TP_BDB_FB_PIM_TC_01_ */
