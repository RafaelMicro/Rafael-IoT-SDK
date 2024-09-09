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
/*  PURPOSE: TP/154/MAC/CHANNEL-ACCESS-04 test constants
*/
#ifndef TP_154_MAC_CHANNEL_ACCESS_04_COMMON_H
#define TP_154_MAC_CHANNEL_ACCESS_04_COMMON_H 1

#define TEST_PAN_ID                 	0x1AAA

#define TEST_DUT_FFD0_MAC_ADDRESS   	{0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xDE, 0xAC}
#define TEST_DUT_FFD0_SHORT_ADDRESS	    0x0000
#define TEST_DST_SHORT_ADDRESS      	0x1234
#define TEST_MAX_MSDU_LENGTH	    	116
#define TEST_MSG_LENGTH		    	    14
#define TEST_RX_ON_WHEN_IDLE        	1
#define TEST_LEAD_CH			        0x40
#define TEST_FILL_CH			        0x23

#define TEST_STATE_STARTOFTEST		    {'#','#','#','S','T','A','R','T','O','F','T','E','S','T'}
#define TEST_STATE_NORMAL		        {'#','#','#','N','O','R','M','A','L','#','#','#','#','#'}
#define TEST_STATE_LIMITED		        {'#','#','#','L','I','M','I','T','E','D','#','#','#','#'}
#define TEST_STATE_CRITICAL		        {'#','#','#','C','R','I','T','I','C','A','L','#','#','#'}
#define TEST_STATE_SUSPENDED		    {'#','#','#','S','U','S','P','E','N','D','E','D','#','#'}
#define TEST_STATE_ENDOFTEST		    {'#','#','#','E','N','D','O','F','T','E','S','T','#','#'}

#define TEST_TP1_TIMEOUT_SYMB		    200
#define TEST_TP1_MAX_TEST_SCEN		    7

#define TEST_TP2_GL_MAX_COUNT		    5

/** Test step enumeration for test procedure 2. */
enum test_step_e
{
  STARTOFTEST,
  STATE1_NORMAL_TO_LIMITED,
  STATE2_LIMITED_TO_NORMAL,
  STATE3_NORMAL_TO_CRITICAL,
  STATE4_CRITICAL_TO_LIMITED,
  STATE5_LIMITED_TO_SUSPENDED,
  STATE6_CRITICAL_3PACKETS,
  STATE7_WAIT_LIMITED,
  STATE7_LIMITED_3PACKETS,
  STATE8_WAIT_NORMAL,
  STATE8_NORMAL_3PACKETS,
  ENDOFTEST
};

#endif /* TP_154_MAC_CHANNEL_ACCESS_04_COMMON_H */
