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
/* PURPOSE: Definitions common to all R22 regression tests
*/

#include "../../multitest_common/zb_multitest.h"
#include "../../multitest_common/zb_test_step.h"

#ifndef _ZB_REG_TEST_GLOBALS
#define _ZB_REG_TEST_GLOBALS

typedef struct zb_r22_reg_test_ctx_s
{
  zb_uint8_t page;
  zb_uint8_t channel;
  zb_test_control_mode_t mode;
} zb_r22_reg_test_ctx_t;

extern zb_r22_reg_test_ctx_t g_reg_test_ctx;

#define ZB_REG_TEST_DEAULT_PAGE    0
#define ZB_REG_TEST_DEAULT_CHANNEL 11
#define ZB_REG_TEST_DEAULT_MODE ZB_TEST_CONTROL_ALARMS

#define ZB_REG_TEST_DEFAULT_NWK_KEY {0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define ZB_REG_TEST_DEFAULT_INSTALL_CODE {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,  \
                                          0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,  \
                                          /* CRC */ 0xC3, 0xB5};

#ifdef ZB_MULTI_TEST

#ifndef TEST_PAGE
#define TEST_PAGE        (zb_multitest_get_test_ctx()->page)
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     (zb_multitest_get_test_ctx()->channel)
#endif /* ifndef TEST_CHANNEL */

#ifndef TEST_MODE
#define TEST_MODE        (zb_multitest_get_test_ctx()->mode)
#endif /* ifndef TEST_MODE */

#else /* ifdef ZB_MULTI_TEST */

#ifndef TEST_PAGE
#define TEST_PAGE        ZB_REG_TEST_DEAULT_PAGE
#endif /* ifndef TEST_PAGE */

#ifndef TEST_CHANNEL
#define TEST_CHANNEL     ZB_REG_TEST_DEAULT_CHANNEL
#endif /* ifndef TEST_CHANNEL */

#ifndef TEST_MODE
#define TEST_MODE        ZB_REG_TEST_DEAULT_MODE
#endif /* ifndef TEST_MODE */

#endif /* ifdef ZB_MULTI_TEST */


void zb_reg_test_set_init_globals();
void zb_reg_test_set_common_channel_settings();
void zb_reg_test_set_zc_role();
void zb_reg_test_set_zr_role();
void zb_reg_test_set_zed_role();

/* Add stubs for some functions those are not available on NCP */
#ifdef NCP_MODE_HOST

#include "zb_aps.h"

void fill_nldereq(zb_uint8_t param, zb_uint16_t addr, zb_uint8_t secure);
void zb_nlde_data_request(zb_uint8_t param);
void zb_set_installcode_policy(zb_bool_t allow_ic_only);
void zb_nwk_change_me_addr(zb_uint8_t param);
void mac_add_invisible_short(zb_uint16_t addr);
void mac_remove_invisible_short(zb_uint16_t addr);
zb_bool_t zb_zdo_rejoin_backoff_is_running();
void zb_nwk_set_address_assignment_cb(zb_addr_assignment_cb_t cb);
void zb_zdo_rejoin_backoff_cancel();
zb_ret_t zb_zdo_rejoin_backoff_start(zb_bool_t insecure_rejoin);
void zb_nvram_register_app1_write_cb(zb_nvram_write_app_data_t wcb, zb_nvram_get_app_data_size_t gcb);
void zb_nvram_register_app1_read_cb(zb_nvram_read_app_data_t cb);
void zb_secur_apsme_request_key(zb_uint8_t param);
void zb_tp_buffer_test_request(zb_uint8_t param, zb_callback_t cb);
void zb_zdo_register_device_annce_cb(zb_device_annce_cb_t cb);
void aps_data_hdr_fill_datareq(zb_uint8_t fc, zb_apsde_data_req_t *req, zb_bufid_t param);
void zb_aib_tcpol_set_authenticate_always(zb_bool_t authenticate_always);

#ifdef ZB_TRANSCEIVER_SET_RX_ON_OFF
#undef ZB_TRANSCEIVER_SET_RX_ON_OFF
#define ZB_TRANSCEIVER_SET_RX_ON_OFF(param) ((void)param)
#endif

#endif /* NCP_MODE_HOST */

#endif /* _ZB_REG_TEST_GLOBALS */
