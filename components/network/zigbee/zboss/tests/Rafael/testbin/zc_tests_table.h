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

#ifndef ZC_TESTS_TABLE_H
#define ZC_TESTS_TABLE_H


void S_NWK_01_ZC_main();
void S_NWK_01_ZC_zb_zdo_startup_complete(zb_uint8_t param);
void S_NWK_01_ZR1_main();
void S_NWK_01_ZR1_zb_zdo_startup_complete(zb_uint8_t param);


void NVRAM_ERASE_main();

#ifdef ZB_TEST_GROUP_ZCP_R22_APS
#define ZB_TEST_GROUP_ZCP_R22_APS_DUT
#define ZB_TEST_GROUP_ZCP_R22_APS_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_BDB
#define ZB_TEST_GROUP_ZCP_R22_BDB_DUT
#define ZB_TEST_GROUP_ZCP_R22_BDB_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_NWK
#define ZB_TEST_GROUP_ZCP_R22_NWK_DUT
#define ZB_TEST_GROUP_ZCP_R22_NWK_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_PED
#define ZB_TEST_GROUP_ZCP_R22_PED_DUT
#define ZB_TEST_GROUP_ZCP_R22_PED_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_PRO
#define ZB_TEST_GROUP_ZCP_R22_PRO_DUT
#define ZB_TEST_GROUP_ZCP_R22_PRO_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_R20
#define ZB_TEST_GROUP_ZCP_R22_R20_DUT
#define ZB_TEST_GROUP_ZCP_R22_R20_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_R21
#define ZB_TEST_GROUP_ZCP_R22_R21_DUT
#define ZB_TEST_GROUP_ZCP_R22_R21_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_R22
#define ZB_TEST_GROUP_ZCP_R22_R22_DUT
#define ZB_TEST_GROUP_ZCP_R22_R22_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_SEC
#define ZB_TEST_GROUP_ZCP_R22_SEC_DUT
#define ZB_TEST_GROUP_ZCP_R22_SEC_TH
#endif

#ifdef ZB_TEST_GROUP_ZCP_R22_ZDO
#define ZB_TEST_GROUP_ZCP_R22_ZDO_DUT
#define ZB_TEST_GROUP_ZCP_R22_ZDO_TH
#endif

#if defined ZB_TEST_GROUP_RAFAEL

static const zb_test_table_t s_tests_table[] = {
{ "S_NWK_01_ZC", S_NWK_01_ZC_main, S_NWK_01_ZC_zb_zdo_startup_complete },
{ "S_NWK_01_ZR1", S_NWK_01_ZR1_main, S_NWK_01_ZR1_zb_zdo_startup_complete },

{ "NVRAM_ERASE", NVRAM_ERASE_main, NULL},
};

#endif

#endif /* ZC_TESTS_TABLE_H */
