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
/*  PURPOSE: Test configuration.
*/
#ifndef TEST_CFG_H
#define TEST_CFG_H 1

/* SPI bitrate */
//#define TEST_BITRATE
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

/* length of every TEST_WRONG_LEN_PERIOD received packet
 * is changed to TEST_WRONG_LEN_VALUE, and then SPI reads
 * new count of bytes instead of the old length 
 */
//#define TEST_WRONG_LEN
#ifdef TEST_WRONG_LEN
  #define TEST_WRONG_LEN_PERIOD   7
  #define TEST_WRONG_LEN_VALUE    150
#endif

/* every TEST_MISS_HOST_INT_PERIOD host interrup
 * is missed, so packet will not be received
 */
/* or */
/* during every TEST_WRITE_WHEN_HOST_INT_PERIOD
 * receiving packet, host will send packet with 
 * TEST_WRITE_WHEN_HOST_INT_SIZE lenght simultaneously
 */
//#define TEST_MISS_HOST_INT
//#define TEST_WRITE_WHEN_HOST_INT
#ifdef TEST_MISS_HOST_INT
  #define TEST_MISS_HOST_INT_PERIOD         5
#elif defined(TEST_WRITE_WHEN_HOST_INT)
  #define TEST_WRITE_WHEN_HOST_INT_PERIOD   3
  #define TEST_WRITE_WHEN_HOST_INT_SIZE     110
#endif

/* enabling statistic module,
 * TEST_STATS_PERIOD_US - period of result printing, us  
 * TEST_STATS_CORRUPTED - check for corrupted packets
 * TEST_STATS_ODD_BYTE and TEST_STATS_EVEN_BYTE - 
 * expected data for checking
*/
//#define TEST_STATS
#define TEST_STATS_CORRUPTED
#define TEST_STATS_PERIOD_US   (1000000u)
#define TEST_STATS_ODD_BYTE     0xAA
#define TEST_STATS_EVEN_BYTE    0xCC

/* enabling sending of packets
 * TEST_SIMPLE_SEND_PACKS_NUM - number of packets
 * TEST_SIMPLE_SEND_PERIOD_US - period of packets, us
 * TEST_SEND_SIZE - packet size
 * TEST_SEND_INCREMENTAL_SIZE - if enabled, packet size
 * will grow from 0 to TEST_SEND_SIZE
 */  
#define TEST_SIMPLE_SEND
#ifdef TEST_SIMPLE_SEND
  #define TEST_SIMPLE_SEND_PACKS_NUM   (20000u)
  #define TEST_SIMPLE_SEND_PERIOD_US   (1000u)
  #define TEST_SEND_SIZE                3
  //#define TEST_SEND_INCREMENTAL_SIZE
#endif

#endif /* TEST_CFG_H */
