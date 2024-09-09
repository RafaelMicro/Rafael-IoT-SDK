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
#ifndef __BV_42_COMMON__H
#define __BV_42_COMMON__H

#define IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_R {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}

//#define TEST_CHANNEL          (1l << 24)

#define TIME_ZC_CONNECT       ( 15 * ZB_TIME_ONE_SECOND)                       // 15 sec
#define TIME_ZC_LEAVE_REQ     ((15 * ZB_TIME_ONE_SECOND) + TIME_ZC_CONNECT)    // 30 sec

#define TIME_ZR_LEAVE         ( 1  * ZB_TIME_ONE_SECOND)                       //  1 sec
#define TIME_ZR_DATA1         ((5  * ZB_TIME_ONE_SECOND) + TIME_ZR_LEAVE)      //  6 sec
#define TIME_ZR_JOIN1         ((5  * ZB_TIME_ONE_SECOND) + TIME_ZR_DATA1)      // 11 sec

#if 0
#define TIME_ZR_JOIN2         ((20 * ZB_TIME_ONE_SECOND) + TIME_ZR_JOIN1)      // 31 sec
#endif

#define TIME_ZR_DATA2         ((20 * ZB_TIME_ONE_SECOND) + TIME_ZR_JOIN1)      // 51 sec

#endif
