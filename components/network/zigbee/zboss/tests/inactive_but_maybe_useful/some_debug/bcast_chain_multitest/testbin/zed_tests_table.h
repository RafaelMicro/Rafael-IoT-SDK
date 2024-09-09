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
 Auto-generated file! Do not edit!
*/

#ifndef ZED_TESTS_TABLE_H
#define ZED_TESTS_TABLE_H

void TP_BCAST_CHAIN_BCAST_CHAIN_ZED1_main();
void TP_BCAST_CHAIN_BCAST_CHAIN_ZED1_zb_zdo_startup_complete(zb_uint8_t param);
void TP_BCAST_CHAIN_BCAST_CHAIN_ZED2_main();
void TP_BCAST_CHAIN_BCAST_CHAIN_ZED2_zb_zdo_startup_complete(zb_uint8_t param);
void NVRAM_ERASE_main();

static const zb_test_table_t s_tests_table[] = {
#if defined ZB_TEST_GROUP_ZCP_R22_BCAST
{ "TP_BCAST_CHAIN_BCAST_CHAIN_ZED1", TP_BCAST_CHAIN_BCAST_CHAIN_ZED1_main, TP_BCAST_CHAIN_BCAST_CHAIN_ZED1_zb_zdo_startup_complete },
{ "TP_BCAST_CHAIN_BCAST_CHAIN_ZED2", TP_BCAST_CHAIN_BCAST_CHAIN_ZED2_main, TP_BCAST_CHAIN_BCAST_CHAIN_ZED2_zb_zdo_startup_complete },
#endif 
{ "NVRAM_ERASE", NVRAM_ERASE_main, NULL},
};

#endif /* ZED_TESTS_TABLE_H */
