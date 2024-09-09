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

#define ZB_GENERAL_DEVICE_VER 1

#define ZB_DECLARE_GENERAL_SERVER_CLUSTER_LIST(                 \
  cluster_list_name,                                            \
  on_off_attr_list,                                             \
  basic_attr_list,                                              \
  identify_attr_list,                                           \
  groups_attr_list,                                             \
  scenes_attr_list,                                             \
  on_off_switch_configuration_attr_list,                        \
  level_control_attr_list,                                      \
  power_config_mains_attr_list,                                 \
  power_config_battery_attr_list,                               \
  alarms_attr_list,                                             \
  time_attr_list,                                               \
  binary_input_attr_list,                                       \
  diagnostics_attr_list,                                        \
  poll_control_attr_list,                                       \
  meter_identification_attr_list);                              \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t), \
          (identify_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_BASIC,                              \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),    \
          (basic_attr_list),                                    \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
          ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),   \
          (on_off_attr_list),                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_GROUPS,                             \
          ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),   \
          (groups_attr_list),                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_SCENES,                             \
          ZB_ZCL_ARRAY_SIZE(scenes_attr_list, zb_zcl_attr_t),   \
          (scenes_attr_list),                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,               \
          ZB_ZCL_ARRAY_SIZE(on_off_switch_configuration_attr_list, zb_zcl_attr_t),\
          (on_off_switch_configuration_attr_list),              \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                      \
          ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t),\
          (level_control_attr_list),                            \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                       \
          ZB_ZCL_ARRAY_SIZE(power_config_mains_attr_list, zb_zcl_attr_t),\
          (power_config_mains_attr_list),                       \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                       \
          ZB_ZCL_ARRAY_SIZE(power_config_battery_attr_list, zb_zcl_attr_t),\
          (power_config_battery_attr_list),                     \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ALARMS,                             \
          ZB_ZCL_ARRAY_SIZE(alarms_attr_list, zb_zcl_attr_t),   \
          (alarms_attr_list),                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
          ZB_ZCL_CLUSTER_DESC(                                  \
          ZB_ZCL_CLUSTER_ID_TIME,                               \
          ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),     \
          (time_attr_list),                                     \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
          ZB_ZCL_CLUSTER_DESC(                                  \
          ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                       \
          ZB_ZCL_ARRAY_SIZE(binary_input_attr_list, zb_zcl_attr_t), \
          (binary_input_attr_list),                             \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                        \
          ZB_ZCL_ARRAY_SIZE(diagnostics_attr_list, zb_zcl_attr_t),   \
          (diagnostics_attr_list),                              \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                       \
          ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t),   \
          (poll_control_attr_list),                             \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION,               \
          ZB_ZCL_ARRAY_SIZE(meter_identification_attr_list, zb_zcl_attr_t),\
          (meter_identification_attr_list),                     \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
      }

#define ZB_DECLARE_GENERAL_CLIENT_CLUSTER_LIST(                 \
  cluster_list_name)                                            \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_BASIC,                              \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_GROUPS,                             \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_SCENES,                             \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,               \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                      \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                       \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                       \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ) ,                                                     \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_ALARMS,                             \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_TIME,                               \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                       \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                        \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                       \
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
        )                                                       \
      }                                                         \

/*!
  @brief Declare simple descriptor for GENERAL_SERVER Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_GENERAL_SERVER_SIMPLE_DESC(                                                \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_GENERAL_DEVICE_VER,                                                                        \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                    \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                   \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                                                   \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                                   \
      ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                                                     \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                            \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                             \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                             \
      ZB_ZCL_CLUSTER_ID_ALARMS,                                                                   \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                     \
      ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                                                             \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                                                              \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                                             \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION                                                      \
    }                                                                                             \
  }

/*!
  @brief Declare simple descriptor for GENERAL_CLIENT Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_GENERAL_CLIENT_SIMPLE_DESC(                                                \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_GENERAL_DEVICE_VER,                                                                        \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                    \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                   \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                                                   \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                                   \
      ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG,                                                     \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                            \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                             \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                             \
      ZB_ZCL_CLUSTER_ID_ALARMS,                                                                   \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                     \
      ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                                                             \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                                                              \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                                             \
      ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION                                                      \
    }                                                                                             \
  }

/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_GENERAL_SERVER_IN_CLUSTER_NUM 15
#define ZB_GENERAL_SERVER_OUT_CLUSTER_NUM 0
#define ZB_DECLARE_GENERAL_SERVER_EP(ep_name, ep_id, cluster_list)             \
  ZB_ZCL_DECLARE_GENERAL_SERVER_SIMPLE_DESC(                                   \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_GENERAL_SERVER_IN_CLUSTER_NUM,                                          \
    ZB_GENERAL_SERVER_OUT_CLUSTER_NUM);                                        \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    0, NULL,                                                                   \
    0, NULL)

/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_GENERAL_CLIENT_IN_CLUSTER_NUM 0
#define ZB_GENERAL_CLIENT_OUT_CLUSTER_NUM 15
#define ZB_DECLARE_GENERAL_CLIENT_EP(ep_name, ep_id, cluster_list)             \
  ZB_ZCL_DECLARE_GENERAL_CLIENT_SIMPLE_DESC(                                   \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_GENERAL_CLIENT_IN_CLUSTER_NUM,                                          \
    ZB_GENERAL_CLIENT_OUT_CLUSTER_NUM);                                        \
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
#define ZB_DECLARE_GENERAL_CTX(device_ctx, ep1_name, ep2_name)  \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep1_name, ep2_name)


/* Missing Alarms Cluster on stack */
#define ZB_ZCL_DECLARE_ALARMS_ATTRIB_LIST(attr_list, alarm_count)            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ALARMS_ALARM_COUNT_ID, (alarm_count))     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ALARMS_ALARM_COUNT_ID(data_ptr)   \
{                                                                            \
  ZB_ZCL_ATTR_ALARMS_ALARM_COUNT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (void*) data_ptr                                                           \
}
