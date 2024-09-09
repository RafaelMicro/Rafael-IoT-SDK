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


void S_NWK_01_ZED1_main();
void S_NWK_01_ZED1_zb_zdo_startup_complete(zb_uint8_t param);
void S_NWK_01_ZED2_main();
void S_NWK_01_ZED2_zb_zdo_startup_complete(zb_uint8_t param);

void NVRAM_ERASE_main();

#if defined ZB_TEST_GROUP_RAFAEL

static const zb_test_table_t s_tests_table[] = {
{ "S_NWK_01_ZED1", S_NWK_01_ZED1_main, S_NWK_01_ZED1_zb_zdo_startup_complete },
{ "S_NWK_01_ZED2", S_NWK_01_ZED2_main, S_NWK_01_ZED2_zb_zdo_startup_complete },

{ "NVRAM_ERASE", NVRAM_ERASE_main, NULL},
};

#endif /* else ZB_TEST_GROUP_NORDIC_R22_SPOTCHECK */

#endif /* ZED_TESTS_TABLE_H */
