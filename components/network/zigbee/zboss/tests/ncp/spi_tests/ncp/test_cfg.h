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

#ifndef TEST_CFG_H
#define TEST_CFG_H 1

/* SPI bitrate */
#define TEST_BITRATE
#ifdef TEST_BITRATE
  #define TEST_BITRATE_VALUE    (976000u)
#endif

/* every TEST_CORRUPTED_PACKET_PERIOD transmitted packet
 * is filled with TEST_CORRUPTED_PACKET_ODD_BYTE and
 * TEST_CORRUPTED_PACKET_EVEN_BYTE bytes
 */
//#define TEST_CORRUPTED_PACKET
#ifdef TEST_CORRUPTED_PACKET
  #define TEST_CORRUPTED_PACKET_PERIOD    5
  #define TEST_CORRUPTED_PACKET_ODD_BYTE  0xAF
  #define TEST_CORRUPTED_PACKET_EVEN_BYTE 0xBE
#endif

/* enabling statistic module,
 * TEST_STATS_PERIOD_MS - period of result printing, ms   
 * TEST_STATS_CORRUPTED - check for corrupted packets
 * TEST_STATS_ODD_BYTE and TEST_STATS_EVEN_BYTE - 
 * expected data for checking
*/
#define TEST_STATS
#define TEST_STATS_CORRUPTED
#define TEST_STATS_PERIOD_MS   (1000u)
#define TEST_STATS_ODD_BYTE     0xAA
#define TEST_STATS_EVEN_BYTE    0xCC

/* enabling sending of packets
 * TEST_SIMPLE_SEND_PACKS_NUM - number of packets
 * TEST_SIMPLE_SEND_PERIOD_MS - period of packets, ms
 * TEST_SEND_SIZE - packet size
 * TEST_SEND_INCREMENTAL_SIZE - if enabled, packet size
 * will grow from 0 to TEST_SEND_SIZE
 */
#define TEST_SIMPLE_SEND
#ifdef TEST_SIMPLE_SEND
  #define TEST_SIMPLE_SEND_PACKS_NUM   (10000u)
  #define TEST_SIMPLE_SEND_PERIOD_MS   (10)
  #define TEST_SEND_SIZE                126
  #define TEST_SEND_INCREMENTAL_SIZE
#endif

#endif /* TEST_CFG_H */
