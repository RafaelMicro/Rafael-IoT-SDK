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
#ifndef __TP_LONG_RUNNING_01_COMMON_
#define __TP_LONG_RUNNING_01_COMMON_

#define TEST_CHANNEL 11

#define IEEE_ADDR_ZC {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define IEEE_ADDR_ZR {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define IEEE_ADDR_ZED {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TEST_DEFAULT_NWK_KEY {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, \
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/* ZC */
#define TP_LONG_RUNNING_01_ZC_FB_TARGET_START_DELAY (5 * ZB_TIME_ONE_SECOND)

#define TP_LONG_RUNNING_01_ZC_ACTIVE_EP_REQ_DELAY (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZC_ACTIVE_EP_REQ_INTERVAL (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZC_ACTIVE_EP_INDEPENDENT

/* ZR */
#define TP_LONG_RUNNING_01_ZR_FB_INITIATOR_START_DELAY (5 * ZB_TIME_ONE_SECOND)

#define TP_LONG_RUNNING_01_ZR_ZCL_REQ_DELAY (3 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZR_ZCL_REQ_INTERVAL (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZR_ZCL_REQ_INDEPENDENT

/* ZED */
#define TP_LONG_RUNNING_01_ZED_FB_INITIATOR_START_DELAY (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZED_ZDO_CLIENT_FINDING_DELAY (6 * ZB_TIME_ONE_SECOND)

#define TP_LONG_RUNNING_01_ZED_ZCL_REQ_DELAY (3 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZED_ZCL_REQ_INTERVAL (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZED_ZCL_REQ_INDEPENDENT

#define TP_LONG_RUNNING_01_ZED_ZDO_REQ_DELAY (3 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZED_ZDO_REQ_INTERVAL (5 * ZB_TIME_ONE_SECOND)
#define TP_LONG_RUNNING_01_ZED_ZDO_REQ_INDEPENDENT

#define TP_LONG_RUNNING_01_DEVICES_TRACE_MASK (TRACE_SUBSYSTEM_APP | TRACE_SUBSYSTEM_MAC | TRACE_SUBSYSTEM_APS | TRACE_SUBSYSTEM_NWK | TRACE_SUBSYSTEM_ZDO | TRACE_SUBSYSTEM_TRANSPORT)

/* Endpoints */
#define ZC_ENDPOINT                10
#define ZED_ENDPOINT               20
#define ZR_ENDPOINT                30

#endif /* __TP_LONG_RUNNING_01_COMMON_ */
