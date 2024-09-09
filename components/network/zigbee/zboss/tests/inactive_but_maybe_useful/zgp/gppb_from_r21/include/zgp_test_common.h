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
/* PURPOSE: ZGP test common definition
*/

#ifndef ZGP_TEST_COMMON_H
#define ZGP_TEST_COMMON_H 1

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"
#include "zb_zcl.h"
#include "zb_secur_api.h"
#include "zgp/zgp_internal.h"
#include "zgpd/zb_zgpd.h"
#include "zb_ha.h"
#include "simple_combo_match_info.h"
#include "simple_combo_controller.h"

#ifndef ZB_NSNG
#include "zb_led_button.h"
#endif

typedef void (*make_gpdf_t)(zb_buf_t *buf, zb_uint8_t **ptr);
typedef void (*send_zcl_t)(zb_uint8_t bef_ref, zb_callback_t cb);
typedef void (*perform_next_state_t)(zb_uint8_t param);

typedef zb_bool_t (*custom_comm_cb_t)(zb_zgpd_id_t *zgpd_id, zb_zgp_comm_status_t  result);
typedef void (*zgp_srv_clst_read_attr_handler_cb_t)
                            (zb_uint8_t              param,
                             zb_zcl_read_attr_res_t *read_attr_resp);

typedef void (*zgp_cli_clst_read_attr_handler_cb_t)
                            (zb_uint8_t              param,
                             zb_zcl_read_attr_res_t *read_attr_resp);
typedef void (*zgp_srv_clst_write_attr_handler_cb_t)
                            (zb_uint8_t               param,
                             zb_zcl_write_attr_res_t *write_attr_resp);

typedef void (*zgp_cli_clst_write_attr_handler_cb_t)
                            (zb_uint8_t               param,
                             zb_zcl_write_attr_res_t *write_attr_resp);
typedef void (*zgp_clst_gp_pairing_handler_cb_t)(zb_uint8_t param);
typedef void (*zgp_clst_gp_prx_comm_mode_handler_cb_t)(zb_uint8_t param);
typedef void (*zgp_clst_gp_sink_tbl_req_cb_t)(zb_uint8_t param);

#ifdef ZB_USE_BUTTONS
typedef void (*zgp_left_button_handler_t)(zb_uint8_t param);
typedef void (*zgp_up_button_handler_t)(zb_uint8_t param);
typedef void (*zgp_down_button_handler_t)(zb_uint8_t param);
typedef void (*zgp_right_button_handler_t)(zb_uint8_t param);
#endif

void zgpd_set_dsn_and_call(zb_uint8_t param, zb_callback_t func);
void zgpd_set_channel_and_call(zb_uint8_t param, zb_callback_t func);
void zgpd_set_ieee_and_call(zb_uint8_t param, zb_callback_t func);

enum cur_btn_state
{
  NEXT_STATE_SKIP,
  NEXT_STATE_PASSED,
  NEXT_STATE_SKIP_AFTER_CMD,
  NEXT_STATES_SEQUENCE_PASSED,
};

typedef struct zgpd_test_device_ctx_s
{
  zb_uint8_t test_state;
  zb_uint8_t err_cnt;
  zb_uint8_t pause;
#ifdef USE_HW_DEFAULT_BUTTON_SEQUENCE
  zb_uint8_t skip_next_state;
#endif
  make_gpdf_t make_gpdf_cb;
  perform_next_state_t perform_next_state_cb;

#ifdef ZB_USE_BUTTONS
  zgp_left_button_handler_t left_button_handler;
  zgp_up_button_handler_t up_button_handler;
  zgp_down_button_handler_t down_button_handler;
  zgp_right_button_handler_t right_button_handler;
#endif
} zgpd_test_device_ctx_t;

typedef struct zgpc_test_device_ctx_s
{
  zb_uint8_t test_state;
  zb_uint8_t err_cnt;
  zb_uint8_t pause;
  zb_uint8_t skip_next_state;
  send_zcl_t send_zcl_cb;
  zb_callback_t next_zcl_cb;
  zb_callback_t steering_hndlr_cb;
  perform_next_state_t perform_next_state_cb;
  custom_comm_cb_t custom_comm_cb;
  zgp_srv_clst_read_attr_handler_cb_t srv_r_attr_hndlr_cb;
  zgp_cli_clst_read_attr_handler_cb_t cli_r_attr_hndlr_cb;
  zgp_srv_clst_write_attr_handler_cb_t srv_w_attr_hndlr_cb;
  zgp_cli_clst_write_attr_handler_cb_t cli_w_attr_hndlr_cb;
  zgp_clst_gp_pairing_handler_cb_t gp_pairing_hndlr_cb;
  zgp_clst_gp_prx_comm_mode_handler_cb_t gp_prx_comm_mode_hndlr_cb;
  zgp_clst_gp_sink_tbl_req_cb_t gp_sink_tbl_req_cb;

#ifdef ZB_USE_BUTTONS
  zgp_left_button_handler_t left_button_handler;
  zgp_up_button_handler_t up_button_handler;
  zgp_down_button_handler_t down_button_handler;
  zgp_right_button_handler_t right_button_handler;
#endif
} zgpc_test_device_ctx_t;

#define ZB_ZGPD_SET_PAUSE(p)\
  TEST_DEVICE_CTX.pause = p;

#define ZB_ZGPC_SET_PAUSE(p)\
  TEST_DEVICE_CTX.pause = p;

#endif /* ZGP_TEST_COMMON_H */
