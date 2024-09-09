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
/*  PURPOSE: TP/154/PHY24/RECEIVE-01
*/
#ifndef TP_154_PHY24_RECEIVE_01_COMMON_H
#define TP_154_PHY24_RECEIVE_01_COMMON_H 1

#define TEST_MSDU_HEADER    {0x41, 0x88, 0x00, 0xAA, 0x1A, 0xFF, 0xFF, 0x44, 0x33}
#define TEST_MSDU_HEADER_SIZE 9

#define TEST_MSDU_PAYLOAD0  {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80}
#define TEST_MSDU_PAYLOAD1  {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x21}
#define TEST_MSDU_PAYLOAD2  {0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B}
#define TEST_MSDU_PAYLOAD3  {0x1C, 0x1D, 0x1E, 0x1F, 0x22, 0x32, 0x42, 0x52, 0x62}
#define TEST_MSDU_PAYLOAD4  {0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F}
#define TEST_MSDU_PAYLOAD5  {0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xA3, 0xB3}
#define TEST_MSDU_PAYLOAD6  {0x3C, 0x3D, 0x3E, 0x3F, 0x44, 0x54, 0x64, 0x74, 0x84}
#define TEST_MSDU_PAYLOAD7  {0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x55, 0x65}
#define TEST_MSDU_PAYLOAD8  {0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F}
#define TEST_MSDU_PAYLOAD9  {0x66, 0x76, 0x86, 0x96, 0xA6, 0xB6, 0xC6, 0xD6, 0xE6}
#define TEST_MSDU_PAYLOAD10 {0x6F, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7}
#define TEST_MSDU_PAYLOAD11 {0x7F, 0x88, 0x98, 0xA8, 0xB8, 0xC8, 0xD8, 0xE8, 0xF9}
#define TEST_MSDU_PAYLOAD12 {0x99, 0xA9, 0xB9, 0xC9, 0xD9, 0xE9, 0xFA, 0xAB, 0xAC}
#define TEST_MSDU_PAYLOAD13 {0xAD, 0xAE, 0xAF, 0xB1, 0xBB, 0xCB, 0xDB, 0xEB, 0xFC}
#define TEST_MSDU_PAYLOAD14 {0xCA, 0xCC, 0xDC, 0xEC, 0xFD, 0xDE, 0xDF, 0xEE, 0xFF}
#define TEST_MSDU_PAYLOAD15 {0xF3, 0xF6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TEST_MSDU_PAYLOAD_NUM  16
#define TEST_MSDU_PAYLOAD_SIZE 9

#define TEST_DUT_MAC_ADDRESS  {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_SHORT_ADDRESS 0x1122

#define TEST_TH_MAC_ADDRESS  {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_TH_SHORT_ADDRESS 0x3344

#define TEST_PAN_ID 0x1AAA

#define TEST_DST_ADDRESS 0xFFFF
#define TEST_PACKETS_PER_CHANNEL 1000
#define TEST_PACKET_TX_PERIOD (ZB_MILLISECONDS_TO_BEACON_INTERVAL(100))
#define TEST_PACKET_CHANNEL_PERIOD (30 * ZB_TIME_ONE_SECOND)

#define TEST_DUT_SWITCH_CHANNEL_TMO \
  ((TEST_PACKET_TX_PERIOD * TEST_PACKETS_PER_CHANNEL) + (TEST_PACKET_CHANNEL_PERIOD/2))

#define TEST_CHANNEL_MIN 11
#define TEST_CHANNEL_MAX 26

#ifndef PHY_TEST
#define TEST_DUMP_PACKETS
#define TEST_VERIFY_PACKETS
#endif

static const zb_uint8_t gs_msdu_payload[TEST_MSDU_PAYLOAD_NUM][TEST_MSDU_PAYLOAD_SIZE] =
{
  TEST_MSDU_PAYLOAD0,
  TEST_MSDU_PAYLOAD1,
  TEST_MSDU_PAYLOAD2,
  TEST_MSDU_PAYLOAD3,
  TEST_MSDU_PAYLOAD4,
  TEST_MSDU_PAYLOAD5,
  TEST_MSDU_PAYLOAD6,
  TEST_MSDU_PAYLOAD7,
  TEST_MSDU_PAYLOAD8,
  TEST_MSDU_PAYLOAD9,
  TEST_MSDU_PAYLOAD10,
  TEST_MSDU_PAYLOAD11,
  TEST_MSDU_PAYLOAD12,
  TEST_MSDU_PAYLOAD13,
  TEST_MSDU_PAYLOAD14,
  TEST_MSDU_PAYLOAD15
};

#endif /* TP_154_PHY24_RECEIVE_01_COMMON_H */
