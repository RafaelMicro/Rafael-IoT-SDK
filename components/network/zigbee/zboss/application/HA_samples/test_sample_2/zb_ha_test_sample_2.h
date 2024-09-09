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
/* PURPOSE: ZB HA_TEST_SAMPLE_2 device
*/

#ifndef ZB_HA_TEST_SAMPLE_2_H
#define ZB_HA_TEST_SAMPLE_2_H 1

#define ZB_HA_TEST_SAMPLE_2_DEVICE_ID 0x0FD2
#define ZB_HA_TEST_SAMPLE_2_DEVICE_VER 2
#define ZB_HA_TEST_SAMPLE_2_IN_CLUSTER_NUM 6
#define ZB_HA_TEST_SAMPLE_2_OUT_CLUSTER_NUM 0

#define ZB_HA_TEST_SAMPLE_2_REPORT_ATTR_COUNT (    \
  ZB_ZCL_METER_IDENTIFICATION_REPORT_ATTR_COUNT +  \
  ZB_ZCL_FAN_CONTROL_REPORT_ATTR_COUNT +           \
  ZB_ZCL_THERMOSTAT_UI_CONFIG_REPORT_ATTR_COUNT)

/*!
  @brief Declare cluster list for HA_TEST_SAMPLE_2 Device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param meter_identification_attr_list - attribute list for Meter Identification cluster
  @param fan_control_attr_list - attribute list for Fan Control cluster
  @param thermostat_ui_config_attr_list - attribute list for Thermostat User Interface Configuration cluster
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_2_CLUSTER_LIST(                   \
  cluster_list_name,                                                   \
  basic_attr_list,                                                     \
  identify_attr_list,                                                  \
  meter_identification_attr_list,                                      \
  fan_control_attr_list,                                               \
  thermostat_ui_config_attr_list)                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =                          \
{                                                                      \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_BASIC,                                           \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                 \
    (basic_attr_list),                                                 \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  ),                                                                   \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                        \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),              \
    (identify_attr_list),                                              \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  ),                                                                   \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,                            \
    ZB_ZCL_ARRAY_SIZE(meter_identification_attr_list, zb_zcl_attr_t),  \
    (meter_identification_attr_list),                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  ),                                                                   \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                     \
    ZB_ZCL_ARRAY_SIZE(fan_control_attr_list, zb_zcl_attr_t),           \
    (fan_control_attr_list),                                           \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  ),                                                                   \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG,                            \
    ZB_ZCL_ARRAY_SIZE(thermostat_ui_config_attr_list, zb_zcl_attr_t),  \
    (thermostat_ui_config_attr_list),                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  ),                                                                   \
  ZB_ZCL_CLUSTER_DESC(                                                 \
    ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS,                     \
    0,                                                                 \
    NULL,                                                              \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
    ZB_ZCL_MANUF_CODE_INVALID                                          \
  )                                                                    \
}

/*!
  @brief Declare simple descriptor for HA_TEST_SAMPLE_2 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_TEST_SAMPLE_2_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_TEST_SAMPLE_2_DEVICE_ID,                                                                \
    ZB_HA_TEST_SAMPLE_2_DEVICE_VER,                                                               \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                    \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                 \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,                                                     \
      ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                                              \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG,                                                     \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS                                               \
    }                                                                                             \
  }
/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_2 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_2_EP(ep_name, ep_id, cluster_list)        \
  ZB_ZCL_DECLARE_HA_TEST_SAMPLE_2_SIMPLE_DESC(ep_name, ep_id,                  \
    ZB_HA_TEST_SAMPLE_2_IN_CLUSTER_NUM, ZB_HA_TEST_SAMPLE_2_OUT_CLUSTER_NUM);  \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,         \
                                     ZB_HA_TEST_SAMPLE_2_REPORT_ATTR_COUNT);   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    ZB_HA_TEST_SAMPLE_2_REPORT_ATTR_COUNT, reporting_info## device_ctx_name,   \
    0, NULL)

/*!
  @brief Declare application's device context for HA_TEST_SAMPLE_2 Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_TEST_SAMPLE_2_IN_CLUSTER_NUM_ZED 0
#define ZB_HA_TEST_SAMPLE_2_OUT_CLUSTER_NUM_ZED 6

/*!
  @brief Declare cluster list for HA_TEST_SAMPLE_2 Device
  @param cluster_list_name - cluster list variable name
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_2_CLUSTER_LIST_ZED(  \
  cluster_list_name)                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =             \
{                                                         \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_BASIC,                              \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,               \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                        \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG,               \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  ),                                                      \
  ZB_ZCL_CLUSTER_DESC(                                    \
    ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS,        \
    0,                                                    \
    NULL,                                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
    ZB_ZCL_MANUF_CODE_INVALID                             \
  )                                                       \
}

/*!
  @brief Declare simple descriptor for HA_TEST_SAMPLE_2 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_TEST_SAMPLE_2_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                         \
  {                                                                                                   \
    ep_id,                                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                                              \
    ZB_HA_TEST_SAMPLE_2_DEVICE_ID,                                                                    \
    ZB_HA_TEST_SAMPLE_2_DEVICE_VER,                                                                   \
    0,                                                                                                \
    in_clust_num,                                                                                     \
    out_clust_num,                                                                                    \
    {                                                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                     \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,                                                         \
      ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                                                  \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG,                                                         \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS                                                   \
    }                                                                                                 \
  }
/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_2 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_2_EP_ZED(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_HA_TEST_SAMPLE_2_SIMPLE_DESC_ZED(ep_name, ep_id,                      \
    ZB_HA_TEST_SAMPLE_2_IN_CLUSTER_NUM_ZED, ZB_HA_TEST_SAMPLE_2_OUT_CLUSTER_NUM_ZED);  \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,                 \
                                     0);                                               \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                     \
    0,                                                                                 \
    NULL,                                                                              \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,              \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                                  \
    0, reporting_info## device_ctx_name,                                               \
    0, NULL)

/*!
  @brief Declare application's device context for HA_TEST_SAMPLE_2 Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_2_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

#endif /* ZB_HA_TEST_SAMPLE_2_H */

