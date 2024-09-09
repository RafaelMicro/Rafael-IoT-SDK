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
/* PURPOSE: Test config
*/

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H 1

#include "zgp/zb_zgp.h"

/* 04/09/2014 CR:CX9262:MAJOR
 *
 * General notes for all tests:
 *
 * Tests should be clear and do not include anything unrelated to this test.
 * For example, test "Battery value is 0" should send only battery notifications,
 * no door/window state attribute reports are needed. So function user_action,
 * define REPORT_COUNT are unneeded and should be deleted. Also, we don't
 * test security here, so we don't need TEST_SECURITY_LEVEL define and can use
 * ZB_ZGP_SEC_LEVEL_FULL_WITH_ENC as default. In some tests test_config.h can
 * be deleted completely.
 *
 * Deleting all unnecessary things would help developers maintaining these tests,
 * because their behaviour becomes more clear.
 *
 * Also, tests should be easily verifiable. I mean, we need to provide clear
 * pass verdict criteria for test executor. For example, for test "Battery
 * value is 0" I think it would better to send four battery measurements one 
 * after another with some timeout. Values for voltage are 0, 3, 0, 2 in volts.
 * So, test executor should verify that result of test is two measurements from
 * gateway to cloud with 3.0 and 2.0 voltage values. It is easy to verify that
 * measurements with 0 volts are really skipped in this case.
 *
 * Another point about verification. Some tests contain pass verdict message
 * "Test finished. Status OK", but not all of them. Since tests implement test
 * harness role, they should output pass verdict message. Tests should
 * output not only "Status OK", but "Status FAILED" messages also. It will
 * help significantly to the test executor.
 *
 * Copy/paste of the same coordinator to all tests seems wrong. It's better to
 * extract coordinator code to the separate folder and use it in all tests.
 *
 * Also, modify readme.txt in tests so they describe actual behaviour of the tests.
 * Add pass verdict criteria in readme wherever possible.
 */

/* Specifies number of attribute reporting commands from ZGPD */
#define TEST_REPORT_COUNT 15

#define TEST_REPORT_TIMEOUT 7 * ZB_TIME_ONE_SECOND

/* ZGPD Src ID */
#define  TEST_ZGPD_SRC_ID 0x12345678

#define TEST_SEC_KEY { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF }

#endif //TEST_CONFIG_H
