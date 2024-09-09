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
/*  PURPOSE: TP/154/MAC/CHANNEL-ACCESS-03 test constants
*/
#ifndef TP_154_MAC_CHANNEL_ACCESS_03_COMMON_H
#define TP_154_MAC_CHANNEL_ACCESS_03_COMMON_H 1

#define TEST_PAN_ID                 	0x1AAA

#ifdef TEST_TH_LARGE_TIMEOUT
#define TEST_TH_TIMEOUT		            (100 + ZB_RANDOM_JTR(50))
#else
#define TEST_TH_TIMEOUT		            (48 + ZB_RANDOM_JTR(96))
#endif

#define TEST_TH_FFD0_MAC_ADDRESS	    {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD1_MAC_ADDRESS   	{0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_TH_FFD0_SHORT_ADDRESS	    0x1122
#define TEST_DUT_FFD1_SHORT_ADDRESS	    0x3344
#define TEST_RX_ON_WHEN_IDLE        	1
#define TEST_MAX_MSDU_LENGTH		    116
#define TEST_TH_PACKET_LENGTH           139
#define TEST_MFR_LEN			        2
#define TEST_DUT_MSDU_COPY_LEN		    10
#define TEST_DUT_RESPONSE_START_SYMB	200
#define TEST_TH_OFFSET_SYMB		        {0, 100, 300, 650, 300, 100, 300, 300, 300, 300, 300, 300, 300, 100, 400}
#define TEST_MAX_SCEN			        15
#define TEST_TH_NOF_FRAGMENTS		    {0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4}
#define TEST_TH_SIZE_OF_LAST_FR_SYMB	{0, 0, 0, 0, 792, 440, 440, 576, 160, 776, 160, 1080, 880, 200, 696}
#define TEST_TH_SIZE_OF_FIRST_FR_SYMB	{0, 1000, 1000, 456, 1112, 1112, 1112, 1112, 1112, 1112, 1112, 1112, 1112, 1112, 1112}
#define TEST_TH_GAP_SYMB		        {0, 0, 0, 0, 100, 450, 450, 100, 450, 100, 450, 100, 200, 490, 490}
#define TEST_RQST_FLAG			        0x3F
#define TEST_RESP_FLAG			        0x40
#define TEST_INTERF_FLAG		        0x23
#define TEST_FILL_CH			        0x35

#endif /* TP_154_MAC_CHANNEL_ACCESS_03_COMMON_H */
