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
/*  PURPOSE: TP/154/PHY24/TRANSMIT-01
*/
#ifndef TP_154_PHY24_TRANSMIT_01_COMMON_H
#define TP_154_PHY24_TRANSMIT_01_COMMON_H 1

#define TEST_MSDU_PAYLOAD \
  {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10}
#define TEST_MSDU_PAYLOAD_SIZE 16

#define TEST_DUT_MAC_ADDRESS  {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_SHORT_ADDRESS 0x3344
#define TEST_PAN_ID 0x1AAA

#define TEST_DST_ADDRESS 0xFFFF
#define TEST_PACKETS_PER_CHANNEL 100
#define TEST_PACKET_TX_PERIOD (ZB_MILLISECONDS_TO_BEACON_INTERVAL(100))
#define TEST_CHANNEL_MIN 11
#define TEST_CHANNEL_MAX 26

#endif /* TP_154_PHY24_TRANSMIT_01_COMMON_H */
