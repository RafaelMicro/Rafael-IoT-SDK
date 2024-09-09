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

#ifndef __RTP_BDB_17_
#define __RTP_BDB_17_

static zb_uint8_t g_ic1[16+2] = {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                                 0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                                 /* CRC */ 0xC3, 0xB5};

#define RTP_BDB_17_STEP_1_DELAY_ZC (5 * ZB_TIME_ONE_SECOND)
#define RTP_BDB_17_STEP_TIME_ZC (10 * ZB_TIME_ONE_SECOND)

#endif /* __RTP_BDB_17_ */
