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
/* PURPOSE: ZC Thermostat sample for HA profile
*/

#ifndef _THERMOSTAT_ZC_H_
#define _THERMOSTAT_ZC_H_

#include "zboss_api.h"

/* Default channel */
#define ZB_THERMOSTAT_CHANNEL_MASK (1l << 21)

#define DST_ADDR_MODE ZB_APS_ADDR_MODE_16_ENDP_PRESENT
#define SRC_ENDPOINT 10
#define DISABLE_DEFAULT_RESPONSE_FLAG 0

/* Handler for specific ZCL commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);
void start_fb_initiator(zb_uint8_t param);
zb_bool_t finding_binding_cb(zb_int16_t status,
                             zb_ieee_addr_t addr,
                             zb_uint8_t ep,
                             zb_uint16_t cluster);

#define MINUTES_PER_DAY 24*60

#define THERMOSTAT_LOCAL_TEMPERATURE_RANGE \
  ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MAX_VALUE \
  - ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MIN_VALUE

#define GET_RANDOM_TEMPERATURE \
  ZB_RANDOM_VALUE(ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MIN_VALUE) \
  + THERMOSTAT_LOCAL_TEMPERATURE_RANGE

#define ZB_THERMOSTAT_IN_CLUSTER_NUM 2
#define ZB_THERMOSTAT_OUT_CLUSTER_NUM 2


/*!
  @brief Declare cluster list for Thermostat device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param thermostat_attr_list - attribute list for Thermostat cluster
 */
#define ZB_DECLARE_THERMOSTAT_CLUSTER_LIST(                             \
  cluster_list_name,                                                    \
  basic_attr_list,                                                      \
  identify_attr_list)                                                   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                           \
  {                                                                     \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
      (identify_attr_list),                                             \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
    ),                                                                  \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_BASIC,                                          \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
      (basic_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
    ),                                                                  \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,                                     \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
    ),                                                                  \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
      0,                                                                \
      NULL,                                                             \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
    )                                                                   \
  }


/*! @brief Declare simple descriptor for Thermostat device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
*/
#define ZB_DECLARE_THERMOSTAT_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)     \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                     \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =              \
  {                                                                                        \
    ep_id,                                                                                 \
    ZB_AF_HA_PROFILE_ID,                                                                   \
    ZB_HA_THERMOSTAT_DEVICE_ID,                                                            \
    ZB_HA_DEVICE_VER_THERMOSTAT,                                                           \
    0,                                                                                     \
    in_clust_num,                                                                          \
    out_clust_num,                                                                         \
    {                                                                                      \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                             \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
    }                                                                                      \
  }


/*! @brief Declare endpoint for Thermostat device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
*/
#define ZB_DECLARE_THERMOSTAT_EP(ep_name, ep_id, cluster_list)            \
  ZB_DECLARE_THERMOSTAT_SIMPLE_DESC(ep_name, ep_id,                       \
    ZB_THERMOSTAT_IN_CLUSTER_NUM, ZB_THERMOSTAT_OUT_CLUSTER_NUM);         \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,    \
                                     ZB_HA_THERMOSTAT_REPORT_ATTR_COUNT); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,        \
    0,                                                                    \
    NULL,                                                                 \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list, \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                     \
    ZB_HA_THERMOSTAT_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, \
    0, NULL)


/* Devices management routines */
void send_thermostat_cmd(zb_uint8_t param);
void send_thermostat_cmd_cb(zb_uint8_t param);

void send_setpoint_raise_lower_cmd(
  zb_zcl_thermostat_setpoint_raise_lower_req_t *setpoint_raise_lower_data);

void send_set_weekly_schedule_cmd(
  zb_zcl_thermostat_set_weekly_schedule_req_t *set_weekly_schedule_req,
  zb_zcl_thermostat_weekly_schedule_point_pair_t *weekly_schedule_point_pair);

void send_get_weekly_schedule_cmd(
  zb_zcl_thermostat_get_weekly_schedule_req_t *get_weekly_schedule_data);

void send_clear_weekly_schedule_cmd(void);

void send_get_relay_status_log_cmd(void);

#endif /* #ifndef _THERMOSTAT_ZC_H_ */
