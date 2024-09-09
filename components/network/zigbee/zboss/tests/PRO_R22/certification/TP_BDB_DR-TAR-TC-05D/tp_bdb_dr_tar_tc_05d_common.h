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
#ifndef __TP_BDB_DR_TAR_TC_05D_
#define __TP_BDB_DR_TAR_TC_05D_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THC1 {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define THC1_START_TEST_DELAY           (10 * ZB_TIME_ONE_SECOND)
#define THC1_SEND_LEAVE_DELAY           (10 * ZB_TIME_ONE_SECOND)
#define THR1_SEND_LEAVE_DELAY           (25 * ZB_TIME_ONE_SECOND)
#define THR1_GET_PEER_ADDR_REQ_DELAY	(3 * ZB_TIME_ONE_SECOND)

#endif /* __TP_BDB_DR_TAR_TC_05D_ */
