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
#ifndef __RTP_BDB_09_COMMON_
#define __RTP_BDB_09_COMMON_

#define IEEE_ADDR_DUT {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_TH_ZR1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

#define DUT_FB_INITIATOR_DELAY_1          (7 * ZB_TIME_ONE_SECOND)
#define DUT_FB_INITIATOR_CANCEL_DELAY_1   (2 * ZB_TIME_ONE_SECOND)


#define THR_FB_DELAY                      (7 * ZB_TIME_ONE_SECOND)
#define THR_FB_DURATION                   (30)

#define RTP_BDB_09_STEP_1_DELAY_ZC        (7 * ZB_TIME_ONE_SECOND)
#define RTP_BDB_09_STEP_1_TIME_ZC         (1 * ZB_TIME_ONE_SECOND)

#define RTP_BDB_09_STEP_1_DELAY_ZR        (3 * ZB_TIME_ONE_SECOND)
#define RTP_BDB_09_STEP_1_TIME_ZR         (12 * ZB_TIME_ONE_SECOND)
#define RTP_BDB_09_STEP_2_TIME_ZR         (1 * ZB_TIME_ONE_SECOND)


#define DUT_BIND_CLUSTERS_NUM             32

/* TH is target, DUT is initiator */
#define DUT_ENDPOINT                      143
#define THR_ENDPOINT_1                    10
#define THR_ENDPOINT_2                    11

#endif /* __RTP_BDB_09_COMMON_ */
