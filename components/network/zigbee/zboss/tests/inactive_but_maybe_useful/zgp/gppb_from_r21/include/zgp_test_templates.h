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
/* PURPOSE: ZGP test devices templates definition
*/

#ifndef ZGP_TEMPLATES_H
#define ZGP_TEMPLATES_H 1

#include "zgp_test_common.h"

#ifdef ZB_MULTI_TEST
#define TEST_DEVICE_CTX TN_CAT(ZB_TEST_NAME,_test_device_ctx)
#else
#define TEST_DEVICE_CTX g_test_device_ctx
#endif

/**
 *  @brief Declare ZGPD test device context.
 *  @param test_device_ctx_name [IN] - context variable name.
 */
#define ZB_ZGPD_DECLARE_TEST_DEVICE_CTX(test_device_ctx_name)          \
  zgpd_test_device_ctx_t test_device_ctx_name = {0};

/**
 *  @brief Declare ZGPC test device context.
 *  @param test_device_ctx_name [IN] - context variable name.
 */
#define ZB_ZGPC_DECLARE_TEST_DEVICE_CTX(test_device_ctx_name)          \
  zgpc_test_device_ctx_t test_device_ctx_name = {0};

#if defined USE_HW_DEFAULT_BUTTON_SEQUENCE && defined ZB_USE_BUTTONS
#define PERFORM_NEXT_STATE perform_next_state_hw
#define CMD_NEXT_STATE() perform_next_state_hw(0)
#define DECLARE_PERFORM_NEXT_STATE_HW                                  \
  static void perform_next_state_hw(zb_uint8_t param)                  \
  {                                                                    \
    if (TEST_DEVICE_CTX.skip_next_state == NEXT_STATE_PASSED ||        \
        (TEST_DEVICE_CTX.test_state + 1) == TEST_STATE_FINISHED)       \
    {                                                                  \
      TEST_DEVICE_CTX.skip_next_state = NEXT_STATE_SKIP;               \
	  TEST_DEVICE_CTX.pause = 0;                                       \
      ZB_ASSERT(TEST_DEVICE_CTX.perform_next_state_cb);                \
      TEST_DEVICE_CTX.perform_next_state_cb(param);                    \
    }                                                                  \
    else                                                               \
	if (TEST_DEVICE_CTX.skip_next_state == NEXT_STATES_SEQUENCE_PASSED)\
    {                                                                  \
      ZB_ASSERT(TEST_DEVICE_CTX.perform_next_state_cb);                \
      TEST_DEVICE_CTX.perform_next_state_cb(param);                    \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TEST_DEVICE_CTX.skip_next_state++;                               \
      return;                                                          \
    }                                                                  \
  }
#define DECLARE_LEFT_BUTTON_HANDLER                                    \
  static void left_btn_hndlr(zb_uint8_t param)                         \
  {                                                                    \
    ZVUNUSED(param);                                                   \
    if (TEST_DEVICE_CTX.left_button_handler)                           \
    {                                                                  \
      TEST_DEVICE_CTX.left_button_handler(0);                          \
    }                                                                  \
    else                                                               \
    {                                                                  \
      PERFORM_NEXT_STATE(0);                                           \
    }                                                                  \
  }
#define DECLARE_UP_BUTTON_HANDLER                                      \
  static void up_btn_hndlr(zb_uint8_t param)                           \
  {                                                                    \
    ZVUNUSED(param);                                                   \
    if (TEST_DEVICE_CTX.up_button_handler)                             \
    {                                                                  \
      TEST_DEVICE_CTX.up_button_handler(0);                            \
    }                                                                  \
    else                                                               \
    {                                                                  \
      PERFORM_NEXT_STATE(0);                                           \
    }                                                                  \
  }
#define DECLARE_DOWN_BUTTON_HANDLER                                    \
  static void down_btn_hndlr(zb_uint8_t param)                         \
  {                                                                    \
    ZVUNUSED(param);                                                   \
    if (TEST_DEVICE_CTX.down_button_handler)                           \
    {                                                                  \
      TEST_DEVICE_CTX.down_button_handler(0);                          \
    }                                                                  \
    else                                                               \
    {                                                                  \
      PERFORM_NEXT_STATE(0);                                           \
    }                                                                  \
  }
#define DECLARE_RIGHT_BUTTON_HANDLER                                   \
  static void right_btn_hndlr(zb_uint8_t param)                        \
  {                                                                    \
    ZVUNUSED(param);                                                   \
    if (TEST_DEVICE_CTX.right_button_handler)                          \
    {                                                                  \
      TEST_DEVICE_CTX.right_button_handler(0);                         \
    }                                                                  \
    else                                                               \
    {                                                                  \
      PERFORM_NEXT_STATE(0);                                           \
    }                                                                  \
  }
#define DECLARE_HW                                                     \
  DECLARE_PERFORM_NEXT_STATE_HW                                        \
  DECLARE_LEFT_BUTTON_HANDLER                                          \
  DECLARE_UP_BUTTON_HANDLER                                            \
  DECLARE_DOWN_BUTTON_HANDLER                                          \
  DECLARE_RIGHT_BUTTON_HANDLER

#define ZB_ZGP_SET_PASSED_STATE_SEQUENCE()\
  TEST_DEVICE_CTX.skip_next_state = NEXT_STATES_SEQUENCE_PASSED

#define ZB_ZGP_RESET_PASSED_STATE_SEQUENCE()\
  TEST_DEVICE_CTX.skip_next_state = 0
#else
#define PERFORM_NEXT_STATE perform_next_state
#define CMD_NEXT_STATE() \
  ZB_ASSERT(TEST_DEVICE_CTX.perform_next_state_cb); \
  TEST_DEVICE_CTX.perform_next_state_cb(0)
#define DECLARE_HW
#define ZB_ZGP_SET_PASSED_STATE_SEQUENCE()
#define ZB_ZGP_RESET_PASSED_STATE_SEQUENCE()
#endif

/**
 *  @brief Declare ZGPD send commands function.
 */
#define ZB_ZGPD_DECLARE_SEND_COMMAND()                                 \
  static void test_send_command(zb_uint8_t buf_ref)                    \
  {                                                                    \
    if (TEST_DEVICE_CTX.pause)                                         \
    {                                                                  \
      if (buf_ref)                                                     \
      {                                                                \
        zb_free_buf(ZB_BUF_FROM_REF(buf_ref));                         \
      }                                                                \
      ZB_SCHEDULE_ALARM(test_send_command, 0,                          \
                        ZB_TIME_ONE_SECOND*TEST_DEVICE_CTX.pause);     \
      TEST_DEVICE_CTX.pause = 0;                                       \
      return;                                                          \
    }                                                                  \
    if (buf_ref == 0)                                                  \
    {                                                                  \
      ZB_GET_OUT_BUF_DELAYED(test_send_command);                       \
      return;                                                          \
    }                                                                  \
    TRACE_MSG(TRACE_APP1, ">> test_send_command %hd",                  \
      (FMT__H, buf_ref));                                              \
    {                                                                  \
      zb_buf_t   *buf = ZB_BUF_FROM_REF(buf_ref);                      \
      zb_uint8_t *ptr;                                                 \
                                                                       \
      ptr = ZB_START_GPDF_PACKET(buf);                                 \
      ZB_ASSERT(TEST_DEVICE_CTX.make_gpdf_cb);                         \
      {                                                                \
        zb_uint8_t *cmd_ptr = ptr;                                     \
        TEST_DEVICE_CTX.make_gpdf_cb(buf, &ptr);                       \
        ZGPD->tx_cmd = *cmd_ptr;                                       \
      }                                                                \
      ZB_FINISH_GPDF_PACKET(buf, ptr);                                 \
                                                                       \
      ZB_SEND_DATA_GPDF_CMD(buf_ref);                                  \
                                                                       \
      CMD_NEXT_STATE();                                                \
    }                                                                  \
    TRACE_MSG(TRACE_APP1, "<< test_send_cmd", (FMT__0));               \
  }

/**
 *  @brief Declare ZGPC send commands function.
 */
#define ZB_ZGPC_DECLARE_SEND_COMMAND()                                 \
  static void test_send_command(zb_uint8_t buf_ref)                    \
  {                                                                    \
    if (TEST_DEVICE_CTX.pause)                                         \
    {                                                                  \
      if (buf_ref)                                                     \
      {                                                                \
        zb_free_buf(ZB_BUF_FROM_REF(buf_ref));                         \
      }                                                                \
      ZB_SCHEDULE_ALARM(test_send_command, 0,                          \
                        ZB_TIME_ONE_SECOND*TEST_DEVICE_CTX.pause);     \
      TEST_DEVICE_CTX.pause = 0;                                       \
      return;                                                          \
    }                                                                  \
    if (buf_ref == 0)                                                  \
    {                                                                  \
      ZB_GET_OUT_BUF_DELAYED(test_send_command);                       \
      return;                                                          \
    }                                                                  \
    TRACE_MSG(TRACE_APP1, ">> test_send_command %hd",                  \
      (FMT__H, buf_ref));                                              \
    {                                                                  \
      ZB_ASSERT(TEST_DEVICE_CTX.send_zcl_cb);                          \
      TEST_DEVICE_CTX.send_zcl_cb(buf_ref, TEST_DEVICE_CTX.next_zcl_cb);\
                                                                       \
      if (TEST_DEVICE_CTX.next_zcl_cb)                                 \
      {                                                                \
        TEST_DEVICE_CTX.next_zcl_cb = NULL;                            \
      }                                                                \
      else                                                             \
      {                                                                \
        if (TEST_DEVICE_CTX.skip_next_state == NEXT_STATE_SKIP_AFTER_CMD)\
        {                                                              \
          TEST_DEVICE_CTX.skip_next_state = 0;                         \
        }                                                              \
        else                                                           \
        {                                                              \
          CMD_NEXT_STATE();                                            \
        }                                                              \
      }                                                                \
    }                                                                  \
    TRACE_MSG(TRACE_APP1, "<< test_send_cmd", (FMT__0));               \
  }

#define ZB_ZGPD_DECLARE_COMMISSIONING_CALLBACK()                       \
  static void comm_cb(zb_uint8_t param)                                \
  {                                                                    \
    zb_buf_t   *buf = ZB_BUF_FROM_REF(param);                          \
    zb_uint8_t  comm_result = buf->u.hdr.status;                       \
                                                                       \
    zb_free_buf(buf);                                                  \
                                                                       \
    if (comm_result == ZB_ZGPD_COMM_SUCCESS)                           \
    {                                                                  \
      if (TEST_DEVICE_CTX.pause)                                       \
      {                                                                \
        ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE, 0,                       \
                          ZB_TIME_ONE_SECOND*TEST_DEVICE_CTX.pause);   \
        TEST_DEVICE_CTX.pause = 0;                                     \
      }                                                                \
      else                                                             \
      {                                                                \
        PERFORM_NEXT_STATE(0);                                         \
      }                                                                \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TRACE_MSG(TRACE_APP1, "commissioning error: %hd",                \
        (FMT__H, comm_result));                                        \
    }                                                                  \
  }

/* example custom_comm_cb:
static zb_bool_t custom_comm_cb(zb_zgpd_id_t         *zgpd_id,
                                zb_zgp_comm_status_t  result)
{
#ifdef ZB_USE_BUTTONS
  zb_led_blink_off(ZB_LED_ARG_CREATE(3, ZB_LED_BLINK_HALF_SEC));
#endif
}
*/
#define ZB_ZGPC_DECLARE_COMMISSIONING_CALLBACK()                       \
  static void comm_cb(zb_zgpd_id_t         *zgpd_id,                   \
                      zb_zgp_comm_status_t  result)                    \
  {                                                                    \
    TRACE_MSG(TRACE_APP1, "commissioning stopped", (FMT__0));          \
                                                                       \
    if (TEST_DEVICE_CTX.custom_comm_cb &&                              \
        TEST_DEVICE_CTX.custom_comm_cb(zgpd_id, result) == ZB_TRUE)    \
    {                                                                  \
      if (TEST_DEVICE_CTX.pause)                                       \
      {                                                                \
        ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE, 0,                       \
                          ZB_TIME_ONE_SECOND*TEST_DEVICE_CTX.pause);   \
        TEST_DEVICE_CTX.pause = 0;                                     \
      }                                                                \
      else                                                             \
      {                                                                \
        ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE, 0,                       \
                          (zb_time_t)(ZB_TIME_ONE_SECOND*0.1f));       \
      }                                                                \
    }                                                                  \
  }

#define ZB_ZGPD_DECLARE_SIMPLE_TEST_TEMPLATE(device_ctx_name,          \
                                             start_timeout)            \
  static void make_gpdf(zb_buf_t *buf, zb_uint8_t **ptr);              \
  static void perform_next_state(zb_uint8_t param);                    \
  ZB_ZGPD_DECLARE_TEST_DEVICE_CTX(device_ctx_name)                     \
  DECLARE_HW                                                           \
  ZB_ZGPD_DECLARE_SEND_COMMAND()                                       \
  static void prepare_test(zb_uint8_t param)                           \
  {                                                                    \
    if (param)                                                         \
    {                                                                  \
      zb_free_buf(ZB_BUF_FROM_REF(param));                             \
    }                                                                  \
    TEST_DEVICE_CTX.make_gpdf_cb = make_gpdf;                          \
    TEST_DEVICE_CTX.perform_next_state_cb = perform_next_state;        \
    ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE,                              \
                      0,                                               \
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(start_timeout));\
  }

#define DECLARE_ZGP_CLUSTER_SPECIFIC_CMD_HANDLER                       \
  static zb_uint8_t zgp_specific_cluster_cmd_handler(zb_uint8_t param) \
  {                                                                    \
    zb_buf_t *zcl_cmd_buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);        \
    zb_zcl_parsed_hdr_t *cmd_info = ZB_GET_BUF_PARAM(zcl_cmd_buf,      \
                                                     zb_zcl_parsed_hdr_t);\
                                                                       \
    TRACE_MSG(TRACE_ZCL1, "> zgp_specific_cluster_cmd_handler %hd",    \
              (FMT__H, param));                                        \
                                                                       \
    if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_GREEN_POWER)         \
    {                                                                  \
      if (cmd_info->is_common_command)                                 \
      {                                                                \
        switch (cmd_info->cmd_id)                                      \
        {                                                              \
          case ZB_ZCL_CMD_READ_ATTRIB_RESP:                            \
          {                                                            \
            zb_zcl_read_attr_res_t *read_attr_resp;                    \
                                                                       \
            ZB_ZCL_GENERAL_GET_NEXT_READ_ATTR_RES(zcl_cmd_buf,         \
                                                  read_attr_resp);     \
            if (cmd_info->cmd_direction ==                             \
                ZB_ZCL_FRAME_DIRECTION_TO_CLI &&                       \
                TEST_DEVICE_CTX.cli_r_attr_hndlr_cb)                   \
            {                                                          \
              TEST_DEVICE_CTX.cli_r_attr_hndlr_cb(param,               \
                                                  read_attr_resp);     \
            }                                                          \
            else                                                       \
            if (cmd_info->cmd_direction ==                             \
                ZB_ZCL_FRAME_DIRECTION_TO_SRV &&                       \
                TEST_DEVICE_CTX.srv_r_attr_hndlr_cb)                   \
            {                                                          \
              TEST_DEVICE_CTX.srv_r_attr_hndlr_cb(param,               \
                                                  read_attr_resp);     \
            }                                                          \
            break;                                                     \
          }                                                            \
          case ZB_ZCL_CMD_WRITE_ATTRIB_RESP:                           \
          {                                                            \
            zb_zcl_write_attr_res_t *write_attr_resp;                  \
                                                                       \
            ZB_ZCL_GET_NEXT_WRITE_ATTR_RES(zcl_cmd_buf,                \
                                           write_attr_resp);           \
            if (cmd_info->cmd_direction ==                             \
                ZB_ZCL_FRAME_DIRECTION_TO_CLI &&                       \
                TEST_DEVICE_CTX.cli_w_attr_hndlr_cb)                   \
            {                                                          \
              TEST_DEVICE_CTX.cli_w_attr_hndlr_cb(param,               \
                                                  write_attr_resp);    \
            }                                                          \
            else                                                       \
            if (cmd_info->cmd_direction ==                             \
                ZB_ZCL_FRAME_DIRECTION_TO_SRV &&                       \
                TEST_DEVICE_CTX.srv_w_attr_hndlr_cb)                   \
            {                                                          \
              TEST_DEVICE_CTX.srv_w_attr_hndlr_cb(param,               \
                                                  write_attr_resp);    \
            }                                                          \
            break;                                                     \
          }                                                            \
        }                                                              \
      }                                                                \
      else                                                             \
      {                                                                \
        switch (cmd_info->cmd_id)                                      \
        {                                                              \
          case ZGP_CLIENT_CMD_GP_PAIRING:                              \
            if (TEST_DEVICE_CTX.gp_pairing_hndlr_cb)                   \
            {                                                          \
              TEST_DEVICE_CTX.gp_pairing_hndlr_cb(param);              \
            }                                                          \
          break;                                                       \
          case ZGP_CLIENT_CMD_GP_PROXY_COMMISSIONING_MODE:             \
            if (TEST_DEVICE_CTX.gp_prx_comm_mode_hndlr_cb)             \
            {                                                          \
              TEST_DEVICE_CTX.gp_prx_comm_mode_hndlr_cb(param);        \
            }                                                          \
          break;                                                       \
          case ZGP_SERVER_CMD_GP_SINK_TABLE_REQUEST:                   \
            if (TEST_DEVICE_CTX.gp_sink_tbl_req_cb)                    \
            {                                                          \
              TEST_DEVICE_CTX.gp_sink_tbl_req_cb(param);               \
            }                                                          \
          break;                                                       \
        };                                                             \
      }                                                                \
    }                                                                  \
                                                                       \
    TRACE_MSG(TRACE_ZCL1, "< zgp_specific_cluster_cmd_handler", (FMT__0));\
    return ZB_FALSE;                                                   \
  }

#define ZB_ZGPC_DECLARE_SIMPLE_TEST_TEMPLATE(device_ctx_name,          \
                                             start_timeout)            \
  static void send_zcl(zb_uint8_t buf_ref, zb_callback_t cb);          \
  static void perform_next_state(zb_uint8_t param);                    \
  ZB_ZGPC_DECLARE_TEST_DEVICE_CTX(device_ctx_name)                     \
  DECLARE_ZGP_CLUSTER_SPECIFIC_CMD_HANDLER                             \
  DECLARE_HW                                                           \
  ZB_ZGPC_DECLARE_SEND_COMMAND()                                       \
  static void prepare_test(zb_uint8_t param)                           \
  {                                                                    \
    if (param)                                                         \
    {                                                                  \
      zb_free_buf(ZB_BUF_FROM_REF(param));                             \
    }                                                                  \
    zgp_cluster_set_app_zcl_cmd_handler(zgp_specific_cluster_cmd_handler);\
    TEST_DEVICE_CTX.send_zcl_cb = send_zcl;                            \
    TEST_DEVICE_CTX.perform_next_state_cb = perform_next_state;        \
    ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE,                              \
                      0,                                               \
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(start_timeout));\
  }

#ifdef ZB_USE_BUTTONS
#define ZB_ZGPC_DECLARE_ZCL_ON_OFF_TOGGLE_HANDLERS               \
  static int state = 0;                                          \
  switch (cmd_info->cmd_id)                                      \
  {                                                              \
    case ZB_ZCL_CMD_ON_OFF_ON_ID:                                \
      TRACE_MSG(TRACE_ZCL1, "ON", (FMT__0));                     \
      processed = ZB_TRUE;                                       \
      zb_osif_led_on(0);                                         \
      state = 1;                                                 \
      break;                                                     \
    case ZB_ZCL_CMD_ON_OFF_OFF_ID:                               \
      TRACE_MSG(TRACE_ZCL1, "OFF", (FMT__0));                    \
      processed = ZB_TRUE;                                       \
      zb_osif_led_off(0);                                        \
      state = 0;                                                 \
      break;                                                     \
    case ZB_ZCL_CMD_ON_OFF_TOGGLE_ID:                            \
      TRACE_MSG(TRACE_ZCL1, "TOGGLE", (FMT__0));                 \
      processed = ZB_TRUE;                                       \
      if (!state)                                                \
      {                                                          \
        zb_osif_led_on(0);                                       \
      }                                                          \
      else                                                       \
      {                                                          \
        zb_osif_led_off(0);                                      \
      }                                                          \
      state = !state;                                            \
      break;                                                     \
  };
#else
#define ZB_ZGPC_DECLARE_ZCL_ON_OFF_TOGGLE_HANDLERS               \
  switch (cmd_info->cmd_id)                                      \
  {                                                              \
    case ZB_ZCL_CMD_ON_OFF_ON_ID:                                \
      TRACE_MSG(TRACE_ZCL1, "ON", (FMT__0));                     \
      processed = ZB_TRUE;                                       \
      break;                                                     \
    case ZB_ZCL_CMD_ON_OFF_OFF_ID:                               \
      TRACE_MSG(TRACE_ZCL1, "OFF", (FMT__0));                    \
      processed = ZB_TRUE;                                       \
      break;                                                     \
    case ZB_ZCL_CMD_ON_OFF_TOGGLE_ID:                            \
      TRACE_MSG(TRACE_ZCL1, "TOGGLE", (FMT__0));                 \
      processed = ZB_TRUE;                                       \
      break;                                                     \
  };
#endif

#define ZB_ZGPC_DECLARE_ZCL_ON_OFF_TOGGLE_TEST_TEMPLATE(               \
  device_ctx_name, endpoint_num, start_timeout)                        \
  ZB_ZGPC_DECLARE_TEST_DEVICE_CTX(device_ctx_name)                     \
                                                                       \
  static zb_uint8_t zcl_cluster_cmd_handler(zb_uint8_t param)          \
  {                                                                    \
    zb_buf_t *zcl_cmd_buf = (zb_buf_t *)ZB_BUF_FROM_REF(param);        \
    zb_zcl_parsed_hdr_t *cmd_info = ZB_GET_BUF_PARAM(zcl_cmd_buf,      \
                                                     zb_zcl_parsed_hdr_t);\
    zb_bool_t processed = ZB_FALSE;                                    \
                                                                       \
    TRACE_MSG(TRACE_ZCL1, "zcl_cluster_cmd_handler %i", (FMT__H, param));\
                                                                       \
    if (cmd_info->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)              \
    {                                                                  \
      if (cmd_info->cmd_direction == ZB_ZCL_FRAME_DIRECTION_TO_SRV)    \
      {                                                                \
        ZB_ZGPC_DECLARE_ZCL_ON_OFF_TOGGLE_HANDLERS                     \
      }                                                                \
    }                                                                  \
                                                                       \
    if (processed)                                                     \
    {                                                                  \
      zb_free_buf(zcl_cmd_buf);                                        \
    }                                                                  \
    return processed;                                                  \
  }                                                                    \
                                                                       \
                                                                       \
  static zb_uint8_t g_attr_zcl_version  = ZB_ZCL_VERSION;              \
  static zb_uint8_t g_attr_power_source =                              \
                          ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN;           \
  ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(BASIC_ATTR_LIST,                    \
                                   &g_attr_zcl_version,                \
                                   &g_attr_power_source);              \
  static zb_uint16_t g_attr_identify_time =                            \
                          ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE; \
  ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(IDENTIFY_ATTR_LIST,              \
                                      &g_attr_identify_time);          \
  ZB_HA_DECLARE_SAMPLE_CLUSTER_LIST(SAMPLE_CLUSTERS,                   \
                                    BASIC_ATTR_LIST,                   \
                                    IDENTIFY_ATTR_LIST);               \
  ZB_HA_DECLARE_SAMPLE_EP(SAMPLE_EP,                                   \
                          endpoint_num, SAMPLE_CLUSTERS, SDESC_EP);    \
  ZB_HA_DECLARE_SAMPLE_CTX(SAMPLE_CTX, SAMPLE_EP);                     \
                                                                       \
  static void send_zcl(zb_uint8_t buf_ref, zb_callback_t cb);          \
  static void perform_next_state(zb_uint8_t param);                    \
  DECLARE_ZGP_CLUSTER_SPECIFIC_CMD_HANDLER                             \
  DECLARE_HW                                                           \
  ZB_ZGPC_DECLARE_SEND_COMMAND()                                       \
  ZB_ZGPC_DECLARE_COMMISSIONING_CALLBACK()                             \
  static void prepare_test(zb_uint8_t param)                           \
  {                                                                    \
    ZB_AF_REGISTER_DEVICE_CTX(&SAMPLE_CTX);                            \
    ZB_AF_SET_ENDPOINT_HANDLER(endpoint_num, zcl_cluster_cmd_handler); \
    /*    ZB_ZGP_SET_MATCH_INFO(&g_zgps_match_info);*/                  \
    zgp_cluster_set_app_zcl_cmd_handler(zgp_specific_cluster_cmd_handler);\
    if (param)                                                         \
    {                                                                  \
      zb_free_buf(ZB_BUF_FROM_REF(param));                             \
    }                                                                  \
    TEST_DEVICE_CTX.perform_next_state_cb = perform_next_state;        \
    TEST_DEVICE_CTX.send_zcl_cb = send_zcl;                            \
    ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE,                              \
                      0,                                               \
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(start_timeout));\
  }

#define ZB_ZGPD_DECLARE_COMMISSIONING_TEST_TEMPLATE(device_ctx_name,   \
                                                    start_timeout)     \
  static void perform_next_state(zb_uint8_t param);                    \
  ZB_ZGPD_DECLARE_TEST_DEVICE_CTX(device_ctx_name)                     \
  DECLARE_HW                                                           \
  static void prepare_test(zb_uint8_t param)                           \
  {                                                                    \
    if (param) zb_free_buf(ZB_BUF_FROM_REF(param));                    \
    TEST_DEVICE_CTX.perform_next_state_cb = perform_next_state;        \
    ZB_SCHEDULE_ALARM(PERFORM_NEXT_STATE,                              \
                      0,                                               \
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(start_timeout));\
  }

#ifdef ZB_USE_BUTTONS
#ifdef USE_HW_DEFAULT_BUTTON_SEQUENCE
#define HW_INIT()                                                      \
  zb_osif_led_button_init();                                           \
  zb_button_register_handler(0, 0, left_btn_hndlr);                    \
  zb_button_register_handler(2, 0, up_btn_hndlr);                      \
  zb_button_register_handler(3, 0, down_btn_hndlr);                    \
  zb_button_register_handler(1, 0, right_btn_hndlr);
#else
#define HW_INIT() zb_osif_led_button_init()
#endif
#define HW_DEV_START_INDICATION(b) zb_osif_led_on((b))
#else
#define HW_INIT()
#define HW_DEV_START_INDICATION(b)
#endif

#define ZB_ZGPC_DECLARE_STARTUP_PROCESS()                              \
  ZB_ZDO_STARTUP_COMPLETE(zb_uint8_t param)                            \
  {                                                                    \
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);                            \
                                                                       \
    TRACE_MSG(TRACE_APP2, "> zb_zdo_startup_complete status %d",       \
              (FMT__D, (int)buf->u.hdr.status));                       \
                                                                       \
    if (buf->u.hdr.status == RET_OK)                                   \
    {                                                                  \
      zb_zdo_app_event_t *ev_p = NULL;                                 \
      zb_zdo_app_signal_t sig = zb_get_app_event(param, &ev_p);        \
                                                                       \
      switch (sig)                                                     \
      {                                                                \
        case ZB_ZDO_SIGNAL_DEFAULT_START:                              \
        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:                         \
        {                                                              \
          TRACE_MSG(TRACE_APP2, "Device STARTED OK", (FMT__0));        \
          HW_DEV_START_INDICATION(2);                                  \
          prepare_test(param);                                         \
          break;                                                       \
        }                                                              \
        case ZB_ZGP_SIGNAL_COMMISSIONING:                              \
        {                                                              \
          zb_zgp_signal_commissioning_params_t *comm_params =          \
            ZB_ZDO_SIGNAL_GET_PARAMS(                                  \
              sig,                                                     \
              zb_zgp_signal_commissioning_params_t);                   \
                                                                       \
              comm_cb(comm_params->zgpd_id, comm_params->result);      \
          }                                                            \
          break;                                                       \
        }                                                              \
        default:                                                       \
        {                                                              \
          TRACE_MSG(TRACE_ERROR, "Unhandled signal %d", (FMT__D, sig));\
          zb_free_buf(buf);                                            \
        }                                                              \
      }                                                                \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "Device start FAILED", (FMT__0));         \
      zb_free_buf(buf);                                                \
    }                                                                  \
    TRACE_MSG(TRACE_ZCL1, "< zb_zdo_startup_complete", (FMT__0));      \
  }                                                                    \
                                                                       \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    zgpc_custom_startup();                                             \
    HW_INIT();                                                         \
                                                                       \
    if (zboss_start() != RET_OK)                                       \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "Device start FAILED", (FMT__0));         \
    }                                                                  \
    else                                                               \
    {                                                                  \
      zcl_main_loop();                                                 \
    }                                                                  \
                                                                       \
    TRACE_DEINIT();                                                    \
                                                                       \
    MAIN_RETURN(0);                                                    \
  }

#ifdef ZB_CERTIFICATION_HACKS
#define ZB_ZGPC_TH_DECLARE_STARTUP_PROCESS()                           \
  ZB_ZDO_STARTUP_COMPLETE(zb_uint8_t param)                            \
  {                                                                    \
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);                            \
                                                                       \
    TRACE_MSG(TRACE_APP2, "> zb_zdo_startup_complete status %d",       \
              (FMT__D, (int)buf->u.hdr.status));                       \
                                                                       \
    if (buf->u.hdr.status == RET_OK)                                   \
    {                                                                  \
      zb_zdo_app_event_t *ev_p = NULL;                                 \
      zb_zdo_app_signal_t sig = zb_get_app_event(param, &ev_p);        \
                                                                       \
      switch (sig)                                                     \
      {                                                                \
        case ZB_ZDO_SIGNAL_DEFAULT_START:                              \
        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:                         \
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:                              \
        {                                                              \
          TRACE_MSG(TRACE_APP2, "Device STARTED OK", (FMT__0));        \
          HW_DEV_START_INDICATION(2);                                  \
          prepare_test(param);                                         \
          break;                                                       \
        }                                                              \
        case ZB_BDB_SIGNAL_STEERING:                                   \
        {                                                              \
          if (TEST_DEVICE_CTX.steering_hndlr_cb)                       \
          {                                                            \
            TEST_DEVICE_CTX.steering_hndlr_cb(param);                  \
          }                                                            \
          else                                                         \
          {                                                            \
            zb_free_buf(buf);                                          \
          }                                                            \
          break;                                                       \
        }                                                              \
        case ZB_ZGP_SIGNAL_COMMISSIONING:                              \
        {                                                              \
          zb_zgp_signal_commissioning_params_t *comm_params =          \
            ZB_ZDO_SIGNAL_GET_PARAMS(                                  \
              sig,                                                     \
              zb_zgp_signal_commissioning_params_t);                   \
                                                                       \
            comm_cb(comm_params->zgpd_id, comm_params->result);        \
          break;                                                       \
        }                                                              \
        default:                                                       \
        {                                                              \
          TRACE_MSG(TRACE_ERROR, "Unhandled signal %d", (FMT__D, sig));\
          zb_free_buf(buf);                                            \
        }                                                              \
      }                                                                \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "Device start FAILED", (FMT__0));         \
      zb_free_buf(buf);                                                \
    }                                                                  \
    TRACE_MSG(TRACE_ZCL1, "< zb_zdo_startup_complete", (FMT__0));      \
  }                                                                    \
                                                                       \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    zgpc_custom_startup();                                             \
    HW_INIT();                                                         \
                                                                       \
    if (zboss_start() != RET_OK)                                       \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "Device start FAILED", (FMT__0));         \
    }                                                                  \
    else                                                               \
    {                                                                  \
      zcl_main_loop();                                                 \
    }                                                                  \
                                                                       \
    TRACE_DEINIT();                                                    \
                                                                       \
    MAIN_RETURN(0);                                                    \
  }
#else // defined ZB_CERTIFICATION_HACKS
#define ZB_ZGPC_TH_DECLARE_STARTUP_PROCESS()                           \
  ZB_ZDO_STARTUP_COMPLETE(zb_uint8_t param)                            \
  {                                                                    \
    ZVUNUSED(param);                                                   \
  }                                                                    \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    printf("ZB_CERTIFICATION_HACKS should be defined");\
                                                                       \
    MAIN_RETURN(1);                                                    \
  }
#endif //defined ZB_CERTIFICATION_HACKS

#if defined ZB_ZGPD_ROLE
#define ZB_ZGPD_DECLARE_STARTUP_PROCESS()                              \
  static void zgpd_startup_complete(zb_uint8_t param)                  \
  {                                                                    \
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);                            \
                                                                       \
    TRACE_MSG(TRACE_APP2, ">> zgpd_startup_complete status %d",        \
              (FMT__D, (int)buf->u.hdr.status));                       \
                                                                       \
    if (buf->u.hdr.status == RET_OK)                                   \
    {                                                                  \
      TRACE_MSG(TRACE_APP2, "ZGPD Device STARTED OK", (FMT__0));       \
      HW_DEV_START_INDICATION(2);                                      \
      zgpd_set_channel_and_call(param, prepare_test);                  \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "ZGPD Device start FAILED", (FMT__0));    \
      zb_free_buf(buf);                                                \
    }                                                                  \
  }                                                                    \
                                                                       \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    zgp_custom_startup();                                              \
    HW_INIT();                                                         \
                                                                       \
    if (zb_zgpd_dev_start(zgpd_startup_complete) != RET_OK)            \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "ZGPD Device start FAILED", (FMT__0));    \
    }                                                                  \
    else                                                               \
    {                                                                  \
      zgpd_main_loop();                                                \
    }                                                                  \
                                                                       \
    TRACE_DEINIT();                                                    \
                                                                       \
    MAIN_RETURN(0);                                                    \
  }
#else //defined ZB_ZGPD_ROLE
#define ZB_ZGPD_DECLARE_STARTUP_PROCESS()                              \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    printf("ZB_ZGPD_ROLE should be defined in zb_config.h");           \
                                                                       \
    MAIN_RETURN(1);                                                    \
  }
#endif //defined ZB_ZGPD_ROLE

#if defined ZB_ZGPD_ROLE && defined ZB_CERTIFICATION_HACKS
#define ZB_ZGPD_TH_DECLARE_STARTUP_PROCESS()                           \
  static void set_ieee(zb_uint8_t param)                               \
  {                                                                    \
    zgpd_set_ieee_and_call(param, prepare_test);                       \
  }                                                                    \
  static void zgpd_startup_complete(zb_uint8_t param)                  \
  {                                                                    \
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);                            \
                                                                       \
    TRACE_MSG(TRACE_APP2, ">> zgpd_startup_complete status %d",        \
              (FMT__D, (int)buf->u.hdr.status));                       \
                                                                       \
    if (buf->u.hdr.status == RET_OK)                                   \
    {                                                                  \
      TRACE_MSG(TRACE_APP2, "ZGPD Device STARTED OK", (FMT__0));       \
      HW_DEV_START_INDICATION(2);                                      \
      if (ZGPD->id.app_id == ZB_ZGP_APP_ID_0010)                       \
      {                                                                \
        zgpd_set_channel_and_call(param, set_ieee);                    \
      }                                                                \
      else                                                             \
      {                                                                \
        zgpd_set_channel_and_call(param, prepare_test);                \
      }                                                                \
    }                                                                  \
    else                                                               \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "ZGPD Device start FAILED", (FMT__0));    \
      zb_free_buf(buf);                                                \
    }                                                                  \
  }                                                                    \
                                                                       \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    zgp_custom_startup();                                              \
    HW_INIT();                                                         \
                                                                       \
    if (zb_zgpd_dev_start(zgpd_startup_complete) != RET_OK)            \
    {                                                                  \
      TRACE_MSG(TRACE_ERROR, "ZGPD Device start FAILED", (FMT__0));    \
    }                                                                  \
    else                                                               \
    {                                                                  \
      zgpd_main_loop();                                                \
    }                                                                  \
                                                                       \
    TRACE_DEINIT();                                                    \
                                                                       \
    MAIN_RETURN(0);                                                    \
  }
#else //defined ZB_ZGPD_ROLE && defined ZB_CERTIFICATION_HACKS
#define ZB_ZGPD_TH_DECLARE_STARTUP_PROCESS()                           \
  MAIN()                                                               \
  {                                                                    \
    ARGV_UNUSED;                                                       \
                                                                       \
    printf("ZB_ZGPD_ROLE or ZB_CERTIFICATION_HACKS should be defined in zb_config.h");\
                                                                       \
    MAIN_RETURN(1);                                                    \
  }
#endif

#endif /* ZGP_TEMPLATES_H */
