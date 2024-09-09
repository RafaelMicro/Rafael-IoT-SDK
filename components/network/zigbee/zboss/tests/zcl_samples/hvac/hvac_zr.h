/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
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

#define ZB_HVAC_DEVICE_VER 1

#define ZB_DECLARE_HVAC_SERVER_CLUSTER_LIST(                    \
  cluster_list_name,                                            \
  thermostat_attr_list,                                         \
  fan_control_attr_list,                                        \
  dehumidification_control_attr_list,                           \
  thermostat_ui_config_attr_list);                              \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_THERMOSTAT,                         \
          ZB_ZCL_ARRAY_SIZE(thermostat_attr_list, zb_zcl_attr_t),\
          (thermostat_attr_list),                               \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                        \
          ZB_ZCL_ARRAY_SIZE(fan_control_attr_list, zb_zcl_attr_t),\
          (fan_control_attr_list),                              \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                    \
          ZB_ZCL_ARRAY_SIZE(dehumidification_control_attr_list, zb_zcl_attr_t),\
          (dehumidification_control_attr_list),                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG,               \
          ZB_ZCL_ARRAY_SIZE(thermostat_ui_config_attr_list, zb_zcl_attr_t),\
          (thermostat_ui_config_attr_list),                     \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        )                                                       \
      }

#define ZB_DECLARE_HVAC_CLIENT_CLUSTER_LIST(                    \
  cluster_list_name)                                            \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_THERMOSTAT,                         \
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
          ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                    \
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
        )                                                       \
      }

/*!
  @brief Declare simple descriptor for HVAC_SERVER Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HVAC_SERVER_SIMPLE_DESC(                                                   \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_HVAC_DEVICE_VER,                                                                           \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,                                                               \
      ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                                              \
      ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                                                          \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG                                                      \
   }                                                                                              \
  }

/*!
  @brief Declare simple descriptor for HVAC_CLIENT Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HVAC_CLIENT_SIMPLE_DESC(                                                   \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_HVAC_DEVICE_VER,                                                                           \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT,                                                               \
      ZB_ZCL_CLUSTER_ID_FAN_CONTROL,                                                              \
      ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                                                          \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG                                                      \
    }                                                                                             \
  }

/*!
  @brief Declare endpoint for HVAC Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HVAC_SERVER_IN_CLUSTER_NUM 4
#define ZB_HVAC_SERVER_OUT_CLUSTER_NUM 0
#define ZB_DECLARE_HVAC_SERVER_EP(ep_name, ep_id, cluster_list)                \
  ZB_ZCL_DECLARE_HVAC_SERVER_SIMPLE_DESC(                                      \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_HVAC_SERVER_IN_CLUSTER_NUM,                                             \
    ZB_HVAC_SERVER_OUT_CLUSTER_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    0, NULL,                                                                   \
    0, NULL)

/*!
  @brief Declare endpoint for HVAC Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HVAC_CLIENT_IN_CLUSTER_NUM 0
#define ZB_HVAC_CLIENT_OUT_CLUSTER_NUM 4
#define ZB_DECLARE_HVAC_CLIENT_EP(ep_name, ep_id, cluster_list)                \
  ZB_ZCL_DECLARE_HVAC_CLIENT_SIMPLE_DESC(                                      \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_HVAC_CLIENT_IN_CLUSTER_NUM,                                             \
    ZB_HVAC_CLIENT_OUT_CLUSTER_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    0, NULL,                                                                   \
    0, NULL)


/** @brief Declare Sample 1 Output device context.
    @param device_ctx - device context variable name.
    @param ep1_name - endpoint variable name.
    @param ep2_name - endpoint variable name.
*/
#define ZB_DECLARE_HVAC_CTX(device_ctx, ep1_name, ep2_name)  \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep1_name, ep2_name)
