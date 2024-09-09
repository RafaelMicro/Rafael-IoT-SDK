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
/* PURPOSE: common definitions for test
*/
#ifndef __TP_BDB_FB_PRE_TC_01B_
#define __TP_BDB_FB_PRE_TC_01B_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_THR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THE1 {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define IEEE_ADDR_THC1 {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}

#define TEST_BDB_PRIMARY_CHANNEL_SET (1 << 11)
#define TEST_BDB_SECONDARY_CHANNEL_SET 0


#define THE1_FB_DELAY1                  (20 * ZB_TIME_ONE_SECOND)
#define THE1_FB_DELAY2                  (20 * ZB_TIME_ONE_SECOND)
#define DUT_FB_DELAY1                   (40 * ZB_TIME_ONE_SECOND)
#define DUT_FB_DELAY2                   (50 * ZB_TIME_ONE_SECOND)
#define FB_TARGET_DURATION1             (60 * 4 * ZB_TIME_ONE_SECOND)
#define FB_INITIATOR_DURATION           (15)

/* DUT is initiator, TH is target */
#define DUT_ENDPOINT 8
#define THR1_ENDPOINT 13
#define THE1_ENDPOINT 13


#endif /* __TP_BDB_FB_PRE_TC_01B_ */
