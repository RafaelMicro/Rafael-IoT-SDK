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
/*  PURPOSE:
 Auto-generated file! Do not edit!
*/

#ifndef MAC_TESTS_TABLE_H
#define MAC_TESTS_TABLE_H

#ifdef MAC_ACK_FRAME_DELIVERY_TESTS
#define MAC_ACK_FRAME_DELIVERY_01
#endif /* MAC_ACK_FRAME_DELIVERY_TESTS */

#ifdef MAC_ASSOCIATION_TESTS
#define MAC_ASSOCIATION_01
#define MAC_ASSOCIATION_02
#define MAC_ASSOCIATION_03
#define MAC_ASSOCIATION_04
#endif /* MAC_ASSOCIATION_TESTS */

#ifdef MAC_BEACON_MANAGEMENT_TESTS
#define MAC_BEACON_MANAGEMENT_01
#define MAC_BEACON_MANAGEMENT_02
#define MAC_BEACON_MANAGEMENT_03
//#define MAC_BEACON_MANAGEMENT_04
//#define MAC_BEACON_MANAGEMENT_05
//#define MAC_BEACON_MANAGEMENT_06
#endif /* MAC_BEACON_MANAGEMENT_TESTS */

#ifdef MAC_CHANNEL_ACCESS_TESTS
#define MAC_CHANNEL_ACCESS_01
#define MAC_CHANNEL_ACCESS_02
//#define MAC_CHANNEL_ACCESS_03
//#define MAC_CHANNEL_ACCESS_04
#endif /* MAC_CHANNEL_ACCESS_TESTS */

#ifdef MAC_DATA_TESTS
#define MAC_DATA_01
#define MAC_DATA_02
#define MAC_DATA_03
#define MAC_DATA_04_FFD
#define MAC_DATA_04_RFD
#endif /* MAC_DATA_TESTS */

#ifdef MAC_FRAME_VALIDATION_TESTS
#define MAC_FRAME_VALIDATION_01
#define MAC_FRAME_VALIDATION_02
#define MAC_FRAME_VALIDATION_03
#define MAC_FRAME_VALIDATION_04
#define MAC_FRAME_VALIDATION_05
#define MAC_FRAME_VALIDATION_06
#define MAC_FRAME_VALIDATION_07
#endif /* MAC_FRAME_VALIDATION_TESTS */

#ifdef MAC_RETRIES_TESTS
#define MAC_RETRIES_01
#endif /* MAC_RETRIES_TESTS */

#ifdef MAC_SCANNING_TESTS
#define MAC_SCANNING_01
#define MAC_SCANNING_02
#define MAC_SCANNING_03
#define MAC_SCANNING_04
#define MAC_SCANNING_05
#define MAC_SCANNING_06
#define MAC_SCANNING_07
#define MAC_SCANNING_08
#endif /* MAC_SCANNING_TESTS */

#ifdef MAC_WARM_START_TESTS
#define MAC_WARM_START_01
#define MAC_WARM_START_02
#endif /* MAC_WARM_START_TESTS */

#ifdef PHY24_RECEIVE_TESTS
#define PHY24_RECEIVE_01_02_03_04
#define PHY24_RECEIVER_05
#define PHY24_RECEIVER_06
#define PHY24_RECEIVER_07
#endif /* PHY24_RECEIVE_TESTS */

#ifdef PHY24_TRANSMIT_TESTS
#define PHY24_TRANSMIT_01
#define PHY24_TRANSMIT_02
#define PHY24_TRANSMIT_03_04_05
#endif /* PHY24_TRANSMIT_TESTS */

#ifdef PHY24_TURNAROUND_TIME_TESTS
#define PHY24_TURNAROUND_TIME_01
#define PHY24_TURNAROUND_TIME_02
#endif /* PHY24_TURNAROUND_TIME_TESTS */

#ifdef MAC_ACK_FRAME_DELIVERY_01
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_main();
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_main();
void TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mcps_data_indication(zb_uint8_t param);
#endif /* MAC_ACK_FRAME_DELIVERY_01 */

#ifdef MAC_ASSOCIATION_01
void TP_154_MAC_ASSOCIATION_01_DUT_FFD0_main();
void TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_01_TH_RFD1_main();
void TP_154_MAC_ASSOCIATION_01_TH_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_01_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_ASSOCIATION_01 */

#ifdef MAC_ASSOCIATION_02
void TP_154_MAC_ASSOCIATION_02_DUT_FFD0_main();
void TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_02_TH_RFD1_main();
void TP_154_MAC_ASSOCIATION_02_TH_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_02_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_ASSOCIATION_02 */

#ifdef MAC_ASSOCIATION_03
void TP_154_MAC_ASSOCIATION_03_TH_FFD0_main();
void TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_03_DUT_RFD1_main();
void TP_154_MAC_ASSOCIATION_03_DUT_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_03_DUT_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_ASSOCIATION_03 */

#ifdef MAC_ASSOCIATION_04
void TP_154_MAC_ASSOCIATION_04_DUT_FFD0_main();
void TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_04_TH_RFD1_main();
void TP_154_MAC_ASSOCIATION_04_TH_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_ASSOCIATION_04_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_ASSOCIATION_04 */

#ifdef MAC_BEACON_MANAGEMENT_01
void TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_BEACON_MANAGEMENT_01 */

#ifdef MAC_BEACON_MANAGEMENT_02
void TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_BEACON_MANAGEMENT_02 */

#ifdef MAC_BEACON_MANAGEMENT_03
void TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_BEACON_MANAGEMENT_03 */

#if defined MAC_BEACON_MANAGEMENT_04 || defined DeviceFamily_CC13X2
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
#endif /* defined MAC_BEACON_MANAGEMENT_04 || defined DeviceFamily_CC13X2 */

#if defined MAC_BEACON_MANAGEMENT_05 || defined DeviceFamily_CC13X2
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* defined MAC_BEACON_MANAGEMENT_05 || defined DeviceFamily_CC13X2 */

#if defined MAC_BEACON_MANAGEMENT_06 || defined DeviceFamily_CC13X2
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_main();
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_main();
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
#endif /* defined MAC_BEACON_MANAGEMENT_06 || defined DeviceFamily_CC13X2 */

#ifdef MAC_CHANNEL_ACCESS_01
void TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_main();
void TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
//void TP_154_MAC_CHANNEL_ACCESS_01_TH_main();
//void TP_154_MAC_CHANNEL_ACCESS_01_TH_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_CHANNEL_ACCESS_01 */

#ifdef MAC_CHANNEL_ACCESS_02
void TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_main();
void TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_CHANNEL_ACCESS_02 */

#if defined MAC_CHANNEL_ACCESS_03 || defined DeviceFamily_CC13X2
void TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_main();
void TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_main();
void TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param); //DEBUG
void TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_zb_mcps_data_indication(zb_uint8_t param); //DEBUG
#endif /* defined MAC_CHANNEL_ACCESS_03 || defined DeviceFamily_CC13X2 */

#if defined MAC_CHANNEL_ACCESS_04 || defined DeviceFamily_CC13X2
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_main();
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_main();
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_duty_cycle_mode_indication(zb_uint8_t param);
#endif /* defined MAC_CHANNEL_ACCESS_04 || defined DeviceFamily_CC13X2 */

#ifdef MAC_DATA_01
void TP_154_MAC_DATA_01_TH_FFD0_main();
void TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_TH_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_01_TH_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_DUT_RFD1_main();
void TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_poll_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_01_DUT_RFD1_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_01_DUT_RFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_DATA_01 */

#ifdef MAC_DATA_02
void TP_154_MAC_DATA_02_DUT_FFD0_main();
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_purge_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_02_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_TH_RFD1_main();
void TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_poll_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_02_TH_RFD1_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_02_TH_RFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_DATA_02 */

#ifdef MAC_DATA_03
void TP_154_MAC_DATA_03_TH_FFD0_main();
void TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_03_TH_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_03_TH_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_03_DUT_FFD1_main();
void TP_154_MAC_DATA_03_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_03_DUT_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_03_DUT_FFD1_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_DATA_03_DUT_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_DATA_03 */

#ifdef MAC_DATA_04_FFD
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_main();
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_TH_RFD1_main();
void TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_poll_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mcps_data_indication(zb_uint8_t param);
#endif /* MAC_DATA_04_FFD */

#ifdef MAC_DATA_04_RFD
void TP_154_MAC_DATA_04_RFD_TH_FFD0_main();
void TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_DUT_RFD1_main();
void TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_poll_confirm(zb_uint8_t param);
void TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mcps_data_indication(zb_uint8_t param);
#endif /* MAC_DATA_04_RFD */

#ifdef MAC_FRAME_VALIDATION_01
void TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_01 */

#ifdef MAC_FRAME_VALIDATION_02
void TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_02 */

#ifdef MAC_FRAME_VALIDATION_03
void TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_03 */

#ifdef MAC_FRAME_VALIDATION_04
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_04 */

#ifdef MAC_FRAME_VALIDATION_05
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_05 */

#ifdef MAC_FRAME_VALIDATION_06
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_06 */

#ifdef MAC_FRAME_VALIDATION_07
void TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_main();
void TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_associate_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_main();
void TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mlme_associate_confirm(zb_uint8_t param);
void TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* MAC_FRAME_VALIDATION_07 */

#ifdef MAC_RETRIES_01
void TP_154_MAC_RETRIES_01_DUT_FFD0_main();
void TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_RETRIES_01 */

#ifdef MAC_SCANNING_01
void TP_154_MAC_SCANNING_01_DUT_FFD1_main();
void TP_154_MAC_SCANNING_01_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_01_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_01 */

#ifdef MAC_SCANNING_02
void TP_154_MAC_SCANNING_02_DUT_FFD1_main();
void TP_154_MAC_SCANNING_02_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_02_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_02 */

#ifdef MAC_SCANNING_03
void TP_154_MAC_SCANNING_03_TH_FFD0_main();
void TP_154_MAC_SCANNING_03_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_03_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_03_DUT_FFD1_main();
void TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_03 */

#ifdef MAC_SCANNING_04
void TP_154_MAC_SCANNING_04_TH_FFD0_main();
void TP_154_MAC_SCANNING_04_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_04_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_04_DUT_FFD1_main();
void TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_04 */

#ifdef MAC_SCANNING_05
void TP_154_MAC_SCANNING_05_TH_FFD0_main();
void TP_154_MAC_SCANNING_05_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_05_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_05_DUT_FFD1_main();
void TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_05 */

#ifdef MAC_SCANNING_06
void TP_154_MAC_SCANNING_06_DUT_FFD0_main();
void TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD1_main();
void TP_154_MAC_SCANNING_06_TH_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD2_main();
void TP_154_MAC_SCANNING_06_TH_FFD2_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD2_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD3_main();
void TP_154_MAC_SCANNING_06_TH_FFD3_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD3_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD4_main();
void TP_154_MAC_SCANNING_06_TH_FFD4_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD4_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD5_main();
void TP_154_MAC_SCANNING_06_TH_FFD5_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_06_TH_FFD5_zb_mlme_start_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_06 */

#ifdef MAC_SCANNING_07
void TP_154_MAC_SCANNING_07_TH_FFD0_main();
void TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_orphan_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_07_DUT_FFD1_main();
void TP_154_MAC_SCANNING_07_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_07_DUT_FFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_07 */

#ifdef MAC_SCANNING_08
void TP_154_MAC_SCANNING_08_DUT_FFD0_main();
void TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_orphan_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_comm_status_indication(zb_uint8_t param);
void TP_154_MAC_SCANNING_08_TH_RFD1_main();
void TP_154_MAC_SCANNING_08_TH_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_SCANNING_08_TH_RFD1_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* MAC_SCANNING_08 */

#ifdef MAC_WARM_START_01
void TP_154_MAC_WARM_START_01_TH_FFD0_main();
void TP_154_MAC_WARM_START_01_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_01_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_01_DUT_RFD1_main();
void TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_scan_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_WARM_START_01 */

#ifdef MAC_WARM_START_02
void TP_154_MAC_WARM_START_02_TH_FFD0_main();
void TP_154_MAC_WARM_START_02_TH_FFD0_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_02_TH_FFD0_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_02_DUT_FFD1_main();
void TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_beacon_notify_indication(zb_uint8_t param);
void TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_start_confirm(zb_uint8_t param);
void TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* MAC_WARM_START_02 */

#ifdef PHY24_RECEIVE_01_02_03_04
void TP_154_PHY24_RECEIVE_01_02_03_04_TH_main();
void TP_154_PHY24_RECEIVE_01_02_03_04_TH_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVE_01_02_03_04_TH_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVE_01_02_03_04_DUT_main();
void TP_154_PHY24_RECEIVE_01_02_03_04_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVE_01_02_03_04_DUT_zb_mcps_data_indication(zb_uint8_t param);
#endif /* PHY24_RECEIVE_01_02_03_04 */

#ifdef PHY24_RECEIVER_05
void TP_154_PHY24_RECEIVER_05_DUT_main();
void TP_154_PHY24_RECEIVER_05_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVER_05_DUT_zb_mlme_scan_confirm(zb_uint8_t param);
#endif /* PHY24_RECEIVER_05 */

#ifdef PHY24_RECEIVER_06
void TP_154_PHY24_RECEIVER_06_TH_main();
void TP_154_PHY24_RECEIVER_06_TH_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVER_06_TH_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVER_06_DUT_main();
void TP_154_PHY24_RECEIVER_06_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_RECEIVER_06_DUT_zb_mcps_data_indication(zb_uint8_t param);
#endif /* PHY24_RECEIVER_06 */

#ifdef PHY24_RECEIVER_07
void TP_154_PHY24_RECEIVER_07_DUT_main();
void TP_154_PHY24_RECEIVER_07_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
#endif /* PHY24_RECEIVER_07 */

#ifdef PHY24_TRANSMIT_01
void TP_154_PHY24_TRANSMIT_01_DUT_main();
void TP_154_PHY24_TRANSMIT_01_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TRANSMIT_01_DUT_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* PHY24_TRANSMIT_01 */

#ifdef PHY24_TRANSMIT_02
void TP_154_PHY24_TRANSMIT_02_DUT_main();
void TP_154_PHY24_TRANSMIT_02_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TRANSMIT_02_DUT_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* PHY24_TRANSMIT_02 */

#ifdef PHY24_TRANSMIT_03_04_05
void TP_154_PHY24_TRANSMIT_03_04_05_DUT_main();
void TP_154_PHY24_TRANSMIT_03_04_05_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TRANSMIT_03_04_05_DUT_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* PHY24_TRANSMIT_03_04_05 */

#ifdef PHY24_TURNAROUND_TIME_01
void TP_154_PHY24_TURNAROUND_TIME_01_TH_main();
void TP_154_PHY24_TURNAROUND_TIME_01_TH_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_01_TH_zb_mcps_data_confirm(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_01_DUT_main();
void TP_154_PHY24_TURNAROUND_TIME_01_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_01_DUT_zb_mcps_data_indication(zb_uint8_t param);
#endif /* PHY24_TURNAROUND_TIME_01 */

#ifdef PHY24_TURNAROUND_TIME_02
void TP_154_PHY24_TURNAROUND_TIME_02_TH_main();
void TP_154_PHY24_TURNAROUND_TIME_02_TH_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_02_TH_zb_mcps_data_indication(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_02_DUT_main();
void TP_154_PHY24_TURNAROUND_TIME_02_DUT_zb_mlme_reset_confirm(zb_uint8_t param);
void TP_154_PHY24_TURNAROUND_TIME_02_DUT_zb_mcps_data_confirm(zb_uint8_t param);
#endif /* PHY24_TURNAROUND_TIME_02 */

static const zb_mac_test_table_t s_mac_tests_table[] =
{
#ifdef MAC_ACK_FRAME_DELIVERY_01
  {
    "TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0",
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_main,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1",
    TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_main,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ACK_FRAME_DELIVERY_01_TH_FFD1_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* MAC_ACK_FRAME_DELIVERY_01 */

#ifdef MAC_ASSOCIATION_01
  {
    "TP_154_MAC_ASSOCIATION_01_DUT_FFD0",
    TP_154_MAC_ASSOCIATION_01_DUT_FFD0_main,
    TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_ASSOCIATION_01_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_ASSOCIATION_01_TH_RFD1",
    TP_154_MAC_ASSOCIATION_01_TH_RFD1_main,
    NULL,
    TP_154_MAC_ASSOCIATION_01_TH_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_01_TH_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_ASSOCIATION_01 */

#ifdef MAC_ASSOCIATION_02
  {
    "TP_154_MAC_ASSOCIATION_02_DUT_FFD0",
    TP_154_MAC_ASSOCIATION_02_DUT_FFD0_main,
    TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_ASSOCIATION_02_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_ASSOCIATION_02_TH_RFD1",
    TP_154_MAC_ASSOCIATION_02_TH_RFD1_main,
    NULL,
    TP_154_MAC_ASSOCIATION_02_TH_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_02_TH_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_ASSOCIATION_02 */

#ifdef MAC_ASSOCIATION_03
  {
    "TP_154_MAC_ASSOCIATION_03_TH_FFD0",
    TP_154_MAC_ASSOCIATION_03_TH_FFD0_main,
    TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_ASSOCIATION_03_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_ASSOCIATION_03_DUT_RFD1",
    TP_154_MAC_ASSOCIATION_03_DUT_RFD1_main,
    NULL,
    TP_154_MAC_ASSOCIATION_03_DUT_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_03_DUT_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_ASSOCIATION_03 */

#ifdef MAC_ASSOCIATION_04
  {
    "TP_154_MAC_ASSOCIATION_04_DUT_FFD0",
    TP_154_MAC_ASSOCIATION_04_DUT_FFD0_main,
    TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_ASSOCIATION_04_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_ASSOCIATION_04_TH_RFD1",
    TP_154_MAC_ASSOCIATION_04_TH_RFD1_main,
    NULL,
    TP_154_MAC_ASSOCIATION_04_TH_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_ASSOCIATION_04_TH_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_ASSOCIATION_04 */

#ifdef MAC_BEACON_MANAGEMENT_01
  {
    "TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_01_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_01_TH_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_BEACON_MANAGEMENT_01 */

#ifdef MAC_BEACON_MANAGEMENT_02
  {
    "TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_02_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_02_TH_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_BEACON_MANAGEMENT_02 */

#ifdef MAC_BEACON_MANAGEMENT_03
  {
    "TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_03_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_03_TH_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_BEACON_MANAGEMENT_03 */

#if defined MAC_BEACON_MANAGEMENT_04 || defined DeviceFamily_CC13X2
  {
    "TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_main,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_associate_indication,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_associate_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_beacon_notify_indication,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT2_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_main,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_associate_indication,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_associate_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_beacon_notify_indication,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_04_DUT1_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* defined MAC_BEACON_MANAGEMENT_04 || defined DeviceFamily_CC13X2 */

#if defined MAC_BEACON_MANAGEMENT_05 || defined DeviceFamily_CC13X2
  {
    "TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_main,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mcps_data_indication,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT2_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mcps_data_indication,
    TP_154_MAC_BEACON_MANAGEMENT_05_DUT1_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* defined MAC_BEACON_MANAGEMENT_05 || defined DeviceFamily_CC13X2 */

#if defined MAC_BEACON_MANAGEMENT_06 || defined DeviceFamily_CC13X2
  {
    "TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0",
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_main,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_associate_indication,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_associate_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_beacon_notify_indication,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT2_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1",
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_main,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_associate_indication,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_associate_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_beacon_notify_indication,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_scan_confirm,
    TP_154_MAC_BEACON_MANAGEMENT_06_DUT1_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* defined MAC_BEACON_MANAGEMENT_06 || defined DeviceFamily_CC13X2 */

#ifdef MAC_CHANNEL_ACCESS_01
  {
    "TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0",
    TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_01_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
	/*
  {
    "TP_154_MAC_CHANNEL_ACCESS_01_TH",
    TP_154_MAC_CHANNEL_ACCESS_01_TH_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_01_TH_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
	*/
#endif /* MAC_CHANNEL_ACCESS_01 */

#ifdef MAC_CHANNEL_ACCESS_02
  {
    "TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0",
    TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_02_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_CHANNEL_ACCESS_02 */

#if defined MAC_CHANNEL_ACCESS_03 || defined DeviceFamily_CC13X2
  {
    "TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0",
    TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_03_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1",
    TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_03_DUT_FFD1_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* defined MAC_CHANNEL_ACCESS_03 || defined DeviceFamily_CC13X2 */

#if defined MAC_CHANNEL_ACCESS_04 || defined DeviceFamily_CC13X2
  {
    "TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1",
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP1_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2",
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mcps_data_confirm,
    TP_154_MAC_CHANNEL_ACCESS_04_DUT_FFD0_TP2_zb_mlme_duty_cycle_mode_indication
  },
#endif /* defined MAC_CHANNEL_ACCESS_04 || defined DeviceFamily_CC13X2 */

#ifdef MAC_DATA_01
  {
    "TP_154_MAC_DATA_01_TH_FFD0",
    TP_154_MAC_DATA_01_TH_FFD0_main,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mcps_data_indication,
    TP_154_MAC_DATA_01_TH_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_DATA_01_DUT_RFD1",
    TP_154_MAC_DATA_01_DUT_RFD1_main,
    NULL,
    TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_01_DUT_RFD1_zb_mlme_poll_confirm,
    NULL,
    TP_154_MAC_DATA_01_DUT_RFD1_zb_mcps_data_indication,
    TP_154_MAC_DATA_01_DUT_RFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_DATA_01 */

#ifdef MAC_DATA_02
  {
    "TP_154_MAC_DATA_02_DUT_FFD0",
    TP_154_MAC_DATA_02_DUT_FFD0_main,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mlme_purge_confirm,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mcps_data_indication,
    TP_154_MAC_DATA_02_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_DATA_02_TH_RFD1",
    TP_154_MAC_DATA_02_TH_RFD1_main,
    NULL,
    TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_02_TH_RFD1_zb_mlme_poll_confirm,
    NULL,
    TP_154_MAC_DATA_02_TH_RFD1_zb_mcps_data_indication,
    TP_154_MAC_DATA_02_TH_RFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_DATA_02 */

#ifdef MAC_DATA_03
  {
    "TP_154_MAC_DATA_03_TH_FFD0",
    TP_154_MAC_DATA_03_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_03_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_03_TH_FFD0_zb_mcps_data_indication,
    TP_154_MAC_DATA_03_TH_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_DATA_03_DUT_FFD1",
    TP_154_MAC_DATA_03_DUT_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_03_DUT_FFD1_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_03_DUT_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_03_DUT_FFD1_zb_mcps_data_indication,
    TP_154_MAC_DATA_03_DUT_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_DATA_03 */

#ifdef MAC_DATA_04_FFD
  {
    "TP_154_MAC_DATA_04_FFD_DUT_FFD0",
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_main,
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_FFD_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_DATA_04_FFD_TH_RFD1",
    TP_154_MAC_DATA_04_FFD_TH_RFD1_main,
    NULL,
    TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mlme_poll_confirm,
    NULL,
    TP_154_MAC_DATA_04_FFD_TH_RFD1_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* MAC_DATA_04_FFD */

#ifdef MAC_DATA_04_RFD
  {
    "TP_154_MAC_DATA_04_RFD_TH_FFD0",
    TP_154_MAC_DATA_04_RFD_TH_FFD0_main,
    TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_RFD_TH_FFD0_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_MAC_DATA_04_RFD_DUT_RFD1",
    TP_154_MAC_DATA_04_RFD_DUT_RFD1_main,
    NULL,
    TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mlme_poll_confirm,
    NULL,
    TP_154_MAC_DATA_04_RFD_DUT_RFD1_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* MAC_DATA_04_RFD */

#ifdef MAC_FRAME_VALIDATION_01
  {
    "TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_01_DUT_FFD0_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_01_TH_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_01 */

#ifdef MAC_FRAME_VALIDATION_02
  {
    "TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_02_DUT_FFD0_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_02_TH_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_02 */

#ifdef MAC_FRAME_VALIDATION_03
  {
    "TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_03_DUT_FFD0_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_03_TH_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_03 */

#ifdef MAC_FRAME_VALIDATION_04
  {
    "TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_04_DUT_FFD0_zb_mcps_data_indication,  
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_04_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_04 */

#ifdef MAC_FRAME_VALIDATION_05
  {
    "TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
		NULL,
    TP_154_MAC_FRAME_VALIDATION_05_DUT_FFD0_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_05_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_05 */

#ifdef MAC_FRAME_VALIDATION_06
  {
    "TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0",
    TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_06_DUT_FFD0_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1",
    TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_06_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_06 */

#ifdef MAC_FRAME_VALIDATION_07
  {
    "TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0",
    TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_main,
    TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_associate_indication,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_comm_status_indication,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_07_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1",
    TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_main,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mlme_associate_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
		TP_154_MAC_FRAME_VALIDATION_07_DUT_FFD1_zb_mcps_data_indication,
		NULL,
    NULL
  },
#endif /* MAC_FRAME_VALIDATION_07 */

#ifdef MAC_RETRIES_01
  {
    "TP_154_MAC_RETRIES_01_DUT_FFD0",
    TP_154_MAC_RETRIES_01_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_RETRIES_01_DUT_FFD0_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_RETRIES_01 */

#ifdef MAC_SCANNING_01
  {
    "TP_154_MAC_SCANNING_01_DUT_FFD1",
    TP_154_MAC_SCANNING_01_DUT_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_01_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_01_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_01 */

#ifdef MAC_SCANNING_02
  {
    "TP_154_MAC_SCANNING_02_DUT_FFD1",
    TP_154_MAC_SCANNING_02_DUT_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_02_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_02_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_02 */

#ifdef MAC_SCANNING_03
  {
    "TP_154_MAC_SCANNING_03_TH_FFD0",
    TP_154_MAC_SCANNING_03_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_03_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_03_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_03_DUT_FFD1",
    TP_154_MAC_SCANNING_03_DUT_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_03_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_03 */

#ifdef MAC_SCANNING_04
  {
    "TP_154_MAC_SCANNING_04_TH_FFD0",
    TP_154_MAC_SCANNING_04_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_04_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_04_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_04_DUT_FFD1",
    TP_154_MAC_SCANNING_04_DUT_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_04_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_04 */

#ifdef MAC_SCANNING_05
  {
    "TP_154_MAC_SCANNING_05_TH_FFD0",
    TP_154_MAC_SCANNING_05_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_05_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_05_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_05_DUT_FFD1",
    TP_154_MAC_SCANNING_05_DUT_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_05_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_05 */

#ifdef MAC_SCANNING_06
  {
    "TP_154_MAC_SCANNING_06_DUT_FFD0",
    TP_154_MAC_SCANNING_06_DUT_FFD0_main,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_06_DUT_FFD0_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_06_TH_FFD1",
    TP_154_MAC_SCANNING_06_TH_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD1_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_06_TH_FFD2",
    TP_154_MAC_SCANNING_06_TH_FFD2_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD2_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD2_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_06_TH_FFD3",
    TP_154_MAC_SCANNING_06_TH_FFD3_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD3_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD3_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_06_TH_FFD4",
    TP_154_MAC_SCANNING_06_TH_FFD4_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD4_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD4_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_06_TH_FFD5",
    TP_154_MAC_SCANNING_06_TH_FFD5_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD5_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_06_TH_FFD5_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_06 */

#ifdef MAC_SCANNING_07
  {
    "TP_154_MAC_SCANNING_07_TH_FFD0",
    TP_154_MAC_SCANNING_07_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_orphan_indication,
    TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_07_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_07_DUT_FFD1",
    TP_154_MAC_SCANNING_07_DUT_FFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_07_DUT_FFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_07_DUT_FFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_07 */

#ifdef MAC_SCANNING_08
  {
    "TP_154_MAC_SCANNING_08_DUT_FFD0",
    TP_154_MAC_SCANNING_08_DUT_FFD0_main,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_comm_status_indication,
    TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_orphan_indication,
    TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_SCANNING_08_DUT_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_SCANNING_08_TH_RFD1",
    TP_154_MAC_SCANNING_08_TH_RFD1_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_SCANNING_08_TH_RFD1_zb_mlme_reset_confirm,
    TP_154_MAC_SCANNING_08_TH_RFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* MAC_SCANNING_08 */

#ifdef MAC_WARM_START_01
  {
    "TP_154_MAC_WARM_START_01_TH_FFD0",
    TP_154_MAC_WARM_START_01_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_01_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_WARM_START_01_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_WARM_START_01_DUT_RFD1",
    TP_154_MAC_WARM_START_01_DUT_RFD1_main,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_reset_confirm,
    TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_01_DUT_RFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_WARM_START_01 */

#ifdef MAC_WARM_START_02
  {
    "TP_154_MAC_WARM_START_02_TH_FFD0",
    TP_154_MAC_WARM_START_02_TH_FFD0_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_02_TH_FFD0_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_WARM_START_02_TH_FFD0_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
  {
    "TP_154_MAC_WARM_START_02_DUT_FFD1",
    TP_154_MAC_WARM_START_02_DUT_FFD1_main,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_beacon_notify_indication,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_reset_confirm,
    NULL,
    TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mlme_start_confirm,
    NULL,
    NULL,
    NULL,
    TP_154_MAC_WARM_START_02_DUT_FFD1_zb_mcps_data_confirm,
    NULL
  },
#endif /* MAC_WARM_START_02 */

#ifdef PHY24_RECEIVE_01_02_03_04
  {
    "TP_154_PHY24_RECEIVE_01_02_03_04_TH",
    TP_154_PHY24_RECEIVE_01_02_03_04_TH_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVE_01_02_03_04_TH_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVE_01_02_03_04_TH_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_PHY24_RECEIVE_01_02_03_04_DUT",
    TP_154_PHY24_RECEIVE_01_02_03_04_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVE_01_02_03_04_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVE_01_02_03_04_DUT_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* PHY24_RECEIVE_01_02_03_04 */

#ifdef PHY24_RECEIVER_05
  {
    "TP_154_PHY24_RECEIVER_05_DUT",
    TP_154_PHY24_RECEIVER_05_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_05_DUT_zb_mlme_reset_confirm,
    TP_154_PHY24_RECEIVER_05_DUT_zb_mlme_scan_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* PHY24_RECEIVER_05 */

#ifdef PHY24_RECEIVER_06
  {
    "TP_154_PHY24_RECEIVER_06_TH",
    TP_154_PHY24_RECEIVER_06_TH_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_06_TH_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_06_TH_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_PHY24_RECEIVER_06_DUT",
    TP_154_PHY24_RECEIVER_06_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_06_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_06_DUT_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* PHY24_RECEIVER_06 */

#ifdef PHY24_RECEIVER_07
  {
    "TP_154_PHY24_RECEIVER_07_DUT",
    TP_154_PHY24_RECEIVER_07_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_RECEIVER_07_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
#endif /* PHY24_RECEIVER_07 */

#ifdef PHY24_TRANSMIT_01
  {
    "TP_154_PHY24_TRANSMIT_01_DUT",
    TP_154_PHY24_TRANSMIT_01_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_01_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_01_DUT_zb_mcps_data_confirm,
    NULL
  },
#endif /* PHY24_TRANSMIT_01 */

#ifdef PHY24_TRANSMIT_02
  {
    "TP_154_PHY24_TRANSMIT_02_DUT",
    TP_154_PHY24_TRANSMIT_02_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_02_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_02_DUT_zb_mcps_data_confirm,
    NULL
  },
#endif /* PHY24_TRANSMIT_02 */

#ifdef PHY24_TRANSMIT_03_04_05
  {
    "TP_154_PHY24_TRANSMIT_03_04_05_DUT",
    TP_154_PHY24_TRANSMIT_03_04_05_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_03_04_05_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TRANSMIT_03_04_05_DUT_zb_mcps_data_confirm,
    NULL
  },
#endif /* PHY24_TRANSMIT_03_04_05  */

#ifdef PHY24_TURNAROUND_TIME_01
  {
    "TP_154_PHY24_TURNAROUND_TIME_01_TH",
    TP_154_PHY24_TURNAROUND_TIME_01_TH_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_01_TH_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_01_TH_zb_mcps_data_confirm,
    NULL
  },
  {
    "TP_154_PHY24_TURNAROUND_TIME_01_DUT",
    TP_154_PHY24_TURNAROUND_TIME_01_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_01_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_01_DUT_zb_mcps_data_indication,
    NULL,
    NULL
  },
#endif /* PHY24_TURNAROUND_TIME_01 */

#ifdef PHY24_TURNAROUND_TIME_02
  {
    "TP_154_PHY24_TURNAROUND_TIME_02_TH",
    TP_154_PHY24_TURNAROUND_TIME_02_TH_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_02_TH_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_02_TH_zb_mcps_data_indication,
    NULL,
    NULL
  },
  {
    "TP_154_PHY24_TURNAROUND_TIME_02_DUT",
    TP_154_PHY24_TURNAROUND_TIME_02_DUT_main,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_02_DUT_zb_mlme_reset_confirm,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TP_154_PHY24_TURNAROUND_TIME_02_DUT_zb_mcps_data_confirm,
    NULL
  },
#endif /* PHY24_TURNAROUND_TIME_02 */
};

#endif /* MAC_TESTS_TABLE_H */
