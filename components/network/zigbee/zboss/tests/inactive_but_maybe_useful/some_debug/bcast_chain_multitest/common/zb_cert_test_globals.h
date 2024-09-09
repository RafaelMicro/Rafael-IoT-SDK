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
/* PURPOSE: Definitions common to all R22 certification tests
*/

#ifndef _ZB_CERT_TEST_GLOBALS
#define _ZB_CERT_TEST_GLOBALS

typedef struct zb_r22_cert_test_ctx_s
{
  zb_uint8_t page;
  zb_uint8_t channel;
} zb_r22_cert_test_ctx_t;

extern zb_r22_cert_test_ctx_t g_cert_test_ctx;

#define ZB_CERT_TEST_DEAULT_PAGE    0
#define ZB_CERT_TEST_DEAULT_CHANNEL 24


#ifdef ZB_MULTI_TEST

#ifndef TEST_PAGE
#define TEST_PAGE        (g_cert_test_ctx.page)
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     (g_cert_test_ctx.channel)
#endif /* ifndef TEST_CHANNEL */

#else /* ifdef ZB_MULTI_TEST */

#ifndef TEST_PAGE
#define TEST_PAGE        ZB_CERT_TEST_DEAULT_PAGE
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     ZB_CERT_TEST_DEAULT_CHANNEL
#endif /* ifndef TEST_CHANNEL */

#endif /* ifdef ZB_MULTI_TEST */


void zb_cert_test_set_init_globals();
void zb_cert_test_set_common_channel_settings();
void zb_cert_test_set_zc_role();
void zb_cert_test_set_zr_role();
void zb_cert_test_set_zed_role();


#endif /* _ZB_CERT_TEST_GLOBALS */
