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
/* PURPOSE: common definitions for this test.
*/
#ifndef __TP_BDB_CS_NFS_TC_04_
#define __TP_BDB_CS_NFS_TC_04_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THR2 {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THE1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define THED_SEND_LEAVE_DELAY		(7 * ZB_TIME_ONE_SECOND)
#define THED_JOIN_THR1_DELAY		((10 * ZB_TIME_ONE_SECOND) + THED_SEND_LEAVE_DELAY)
#define MIN_COMMISSIONINIG_TIME_DELAY 	(180 * ZB_TIME_ONE_SECOND)

/* Change if needed */
#define DO_RETRY_JOIN_DELAY 		(ZB_MILLISECONDS_TO_BEACON_INTERVAL(10000))

#endif /* __TP_BDB_CS_NFS_TC_04_ */
