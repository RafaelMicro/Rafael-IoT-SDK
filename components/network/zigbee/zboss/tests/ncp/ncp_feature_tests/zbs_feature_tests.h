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
/* PURPOSE: common include for zbs_features_tests
*/

#ifndef ZBS_FEATURE_TESTS_H
#define ZBS_FEATURE_TESTS_H 1

#include "zb_types.h"

#define CHANNEL         19
#define NEW_CHANNEL     23

#define TEST_PAN_ID     0x5043

#define TEST_ZC_ADDR    {0x00, 0x00, 0xef, 0xcd, 0xab, 0x50, 0x50, 0x50}
#define TEST_ZR_ADDR    {0x11, 0x11, 0xef, 0xcd, 0xab, 0x50, 0x50, 0x50}
#define TEST_ZE_ADDR    TEST_ZR_ADDR
#define TEST_NCP_ADDR   {0x44, 0x33, 0x22, 0x11, 0x00, 0x50, 0x50, 0x50}
#define TEST_NWK_KEY    {0x11, 0xaa, 0x22, 0xbb, 0x33, 0xcc, 0x44, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define TEST_IC         {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23, 0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05, 0xC3, 0xB5}

/* Maximum payload size requested by the customer for SNCP APS long fragmented packet. See ZBNCP_MAX_FRAGMENTED_PAYLOAD_SIZE */
#define ZB_TEST_LONG_APS_MESSAGE_DATA_SIZE 1550U

/*
 * Number of packets that should be sent from ED to ZC.
 * Special value 0xffffffff means infinity.
 */
#ifndef PACKETS_FROM_ED_NR
#define PACKETS_FROM_ED_NR 0xffffffff
#endif

/*
 * Number of packets that should be sent from ZC to ED.
 * Special value 0xffffffff means infinity.
 */
#ifndef PACKETS_FROM_ZC_NR
#define PACKETS_FROM_ZC_NR 0xffffffff
#endif

#endif /* ZBS_FEATURE_TESTS_H */
