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
/* PURPOSE: Test configuration
*/

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H 1

#define NWK_KEY { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0, 0, 0, 0, 0, 0, 0, 0 }

#define TH_GPD_IEEE_ADDR {0xA1, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define DUT_GPP_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
#define TH_GPS_IEEE_ADDR {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}

#define TEST_PAN_ID  0x1aaa


#define TEST_GPS_GET_COMMISSIONING_WINDOW()  \
  ((ZGP_GPS_COMMISSIONING_EXIT_MODE & ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION)? \
    ZGP_GPS_COMMISSIONING_WINDOW: 0)

#define TH_SKIP_STEPS_1_2
#define TH_SKIP_STEP_3
#define TH_SKIP_STEP_4
#define TH_SKIP_STEP_5
#define TH_SKIP_STEP_7
#define TH_SKIP_STEP_8A
#define TH_SKIP_STEP_8B
#define TH_SKIP_STEP_8C
#define TH_SKIP_STEP_9A

/* TH-GPD definitions */
#define TH_GPD_SKIP_N_COMM_REPLYES_IN_2C (3)
#define TEST_ZGPD_SRC_ID 0x12345678
#define TEST_ZGPD_EP_X 3

/* TH-GPS definitions */
#define TH_GPS_TMP_MASTER_TX_CHANNEL_X (TEST_CHANNEL + 3)

/* TH-GPD timings */
#define TH_GPD_PAUSE_BEFORE_STEP_3    (5)
#define TH_GPD_PAUSE_BEFORE_STEP_4    (7)
#define TH_GPD_PAUSE_BEFORE_STEP_5    (6)
#define TH_GPD_PAUSE_BEFORE_STEP_7A   (4)
#define TH_GPD_PAUSE_BEFORE_CH_REQ_7A \
  (TH_GPD_PAUSE_BEFORE_STEP_7A + TH_GPS_PAUSE_BEFORE_CH_CFG1_7A + \
   TH_GPS_PAUSE_BEFORE_CH_CFG2_7A + 1)
#define TH_GPD_PAUSE_BEFORE_CH_REQ_7B \
  (TH_GPS_PAUSE_BEFORE_CH_CFG1_7B + TH_GPS_PAUSE_BEFORE_CH_CFG2_7B)
#define TH_GPD_PAUSE_BEFORE_COMM_7C \
  (TH_GPS_PAUSE_BEFORE_COMM_REPLY1_7C + TH_GPS_PAUSE_BEFORE_COMM_REPLY2_7C)
#define TH_GPD_PAUSE_BEFORE_STEP_8A   (TH_GPS_PAUSE_BEFORE_STEP_8A)
#define TH_GPD_PAUSE_BEFORE_STEP_8B   (TH_GPS_PAUSE_BEFORE_STEP_8B + 1)
#define TH_GPD_PAUSE_BEFORE_STEP_8C   (TH_GPS_PAUSE_BEFORE_STEP_8C + 1)
#define TH_GPD_PAUSE_BEFORE_STEP_9A   (TH_GPS_PAUSE_BEFORE_STEP_9A + 1)
#define TH_GPD_PAUSE_BEFORE_STEP_9B   (TH_GPS_PAUSE_BEFORE_STEP_9B + 1)
#define TH_GPD_SEND_COMM_DELAY_STEP_9 (1)


/* DUT-GPD timings */
#define DUT_GPP_WINDOW_1A (22)
#define DUT_GPP_WINDOW_3  (13)
#define DUT_GPP_WINDOW_4  (10)
#define DUT_GPP_WINDOW_5  (10)
#define DUT_GPP_WINDOW_7A (20)
#define DUT_GPP_WINDOW_8A (10)
#define DUT_GPP_WINDOW_8B (10)
#define DUT_GPP_WINDOW_8C (10)
#define DUT_GPP_WINDOW_9A (10)
#define DUT_GPP_WINDOW_9B (10)

/* TH-GPS timings */
#define TH_GPS_PAUSE_BEFORE_STEP_3         (5)
#define TH_GPS_PAUSE_BEFORE_STEP_4         (13)
#define TH_GPS_PAUSE_BEFORE_STEP_5         (10)
#define TH_GPS_PAUSE_BEFORE_STEP_7A        (10)
#define TH_GPS_PAUSE_BEFORE_CH_CFG1_7A     (1)
#define TH_GPS_PAUSE_BEFORE_CH_CFG2_7A     (1)
#define TH_GPS_PAUSE_BEFORE_CH_CFG1_7B     (3)
#define TH_GPS_PAUSE_BEFORE_CH_CFG2_7B     (1)
#define TH_GPS_PAUSE_BEFORE_COMM_REPLY1_7C (3)
#define TH_GPS_PAUSE_BEFORE_COMM_REPLY2_7C (1)
#define TH_GPS_PAUSE_BEFORE_STEP_8A        (12)
#define TH_GPS_PAUSE_BEFORE_CH_CFG_8A      (1)
#define TH_GPS_PAUSE_BEFORE_STEP_8B        (9)
#define TH_GPS_PAUSE_BEFORE_CH_CFG_8B      (1)
#define TH_GPS_PAUSE_BEFORE_STEP_8C        (9)
#define TH_GPS_PAUSE_BEFORE_CH_CFG_8C      (1)
#define TH_GPS_PAUSE_BEFORE_STEP_9A        (9)
#define TH_GPS_PAUSE_BEFORE_COMM_REPLY_9A  (1)
#define TH_GPS_PAUSE_BEFORE_STEP_9B        (9)
#define TH_GPS_PAUSE_BEFORE_COMM_REPLY_9B  (1)


#define TH_GPS_COMM_WINDOW_3   (8)
#define TH_GPS_COMM_WINDOW_4   (8)
#define TH_GPS_COMM_WINDOW_5   (8)
#define TH_GPS_COMM_WINDOW_7A  (18)
#define TH_GPS_COMM_WINDOW_8A  (8)
#define TH_GPS_COMM_WINDOW_8B  (8)
#define TH_GPS_COMM_WINDOW_8C  (8)
#define TH_GPS_COMM_WINDOW_9A  (8)
#define TH_GPS_COMM_WINDOW_9B  (8)


#define TEST_SEC_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }
#define TEST_OOB_KEY { 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xbb }

#ifndef TEST_CHANNEL
#define TEST_CHANNEL 11
#endif

#ifndef ZB_NSNG

#ifndef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define USE_HW_DEFAULT_BUTTON_SEQUENCE
#endif

#endif

#endif /* TEST_CONFIG_H */
