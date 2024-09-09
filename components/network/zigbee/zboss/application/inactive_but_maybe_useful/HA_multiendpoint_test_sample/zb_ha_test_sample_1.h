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
/* PURPOSE: ZB HA_TEST_SAMPLE_1 device
*/

#ifndef ZB_HA_TEST_SAMPLE_1_H
#define ZB_HA_TEST_SAMPLE_1_H 1

#define ZB_HA_TEST_SAMPLE_1_DEVICE_ID 0x0FD1
#define ZB_HA_TEST_SAMPLE_1_DEVICE_VER 1
#define ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_1_ZC 3
#define ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_2_ZC 4
#define ZB_HA_TEST_SAMPLE_1_OUT_CLUSTER_NUM_ZC 0

#define ZB_HA_TEST_SAMPLE_1_REPORT_ATTR_COUNT (  \
  ZB_ZCL_ALARMS_REPORT_ATTR_COUNT +              \
  ZB_ZCL_TIME_REPORT_ATTR_COUNT +                \
  ZB_ZCL_DEHUMIDIFICATION_CONTROL_REPORT_ATTR_COUNT)

/*!
  @brief Declare cluster list for HA_TEST_SAMPLE_1 Device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param diagnostics_attr_list - attribute list for Diagnostics cluster
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_CLUSTER_LIST_1_ZC(                  \
  cluster_list_name,                                                       \
  basic_attr_list,                                                         \
  identify_attr_list,                                                      \
  diagnostics_attr_list)                                                   \
  zb_zcl_cluster_desc_t cluster_list_name[] =                              \
{                                                                          \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_BASIC,                                               \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                     \
    (basic_attr_list),                                                     \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  ),                                                                       \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                            \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),                  \
    (identify_attr_list),                                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  ),                                                                       \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_DIAGNOSTICS,                                         \
    ZB_ZCL_ARRAY_SIZE(diagnostics_attr_list, zb_zcl_attr_t),               \
    (diagnostics_attr_list),                                               \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  )                                                                        \
}

/*!
  @brief Declare cluster list for HA_TEST_SAMPLE_1 Device
  @param cluster_list_name - cluster list variable name
  @param time_attr_list - attribute list for Time cluster
  @param dehumidification_control_attr_list - attribute list for Dehumidification Control cluster
  @param alarms_attr_list - attribute list for Alarms cluster
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_CLUSTER_LIST_2_ZC(                  \
  cluster_list_name,                                                       \
  time_attr_list,                                                          \
  dehumidification_control_attr_list,                                      \
  identify_attr_list)                                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =                              \
{                                                                          \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_TIME,                                                \
    ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),                      \
    (time_attr_list),                                                      \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  ),                                                                       \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                                     \
    ZB_ZCL_ARRAY_SIZE(dehumidification_control_attr_list, zb_zcl_attr_t),  \
    (dehumidification_control_attr_list),                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  ),                                                                       \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_ALARMS,                                              \
    0,                                                                     \
    NULL,                                                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  ),                                                                       \
  ZB_ZCL_CLUSTER_DESC(                                                     \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                            \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),                  \
    (identify_attr_list),                                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                            \
    ZB_ZCL_MANUF_CODE_INVALID                                              \
  )                                                                        \
}

/*!
  @brief Declare simple descriptor for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_1_ZC(                                         \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_TEST_SAMPLE_1_DEVICE_ID,                                                                \
    ZB_HA_TEST_SAMPLE_1_DEVICE_VER,                                                               \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                    \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                 \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS                                                               \
    }                                                                                             \
  }

/*!
  @brief Declare simple descriptor for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_2_ZC(                                         \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_TEST_SAMPLE_1_DEVICE_ID,                                                                \
    ZB_HA_TEST_SAMPLE_1_DEVICE_VER,                                                               \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                     \
      ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                                                          \
      ZB_ZCL_CLUSTER_ID_ALARMS,                                                                   \
      ZB_ZCL_CLUSTER_ID_IDENTIFY                                                                  \
    }                                                                                             \
  }

/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_EP_1_ZC(ep_name, ep_id, cluster_list)   \
  ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_1_ZC(                            \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_1_ZC,                                   \
    ZB_HA_TEST_SAMPLE_1_OUT_CLUSTER_NUM_ZC);                                   \
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
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_EP_2_ZC(ep_name, ep_id, cluster_list)   \
  ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_2_ZC(                            \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_2_ZC,                                   \
    ZB_HA_TEST_SAMPLE_1_OUT_CLUSTER_NUM_ZC);                                   \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,         \
                                     ZB_HA_TEST_SAMPLE_1_REPORT_ATTR_COUNT);   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    ZB_HA_TEST_SAMPLE_1_REPORT_ATTR_COUNT, reporting_info## device_ctx_name,   \
    0, NULL)


/*************************************************************************************************/


/*!
  @brief Declare application's device context for HA_TEST_SAMPLE_1 Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_ZED 0
#define ZB_HA_TEST_SAMPLE_1_OUT_CLUSTER_NUM_ZED 6

/*!
  @brief Declare cluster list for HA_TEST_SAMPLE_1 Device
  @param cluster_list_name - cluster list variable name
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_CLUSTER_LIST_ZED(  \
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
    ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                    \
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
  )                                                       \
}

/*!
  @brief Declare simple descriptor for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                         \
  {                                                                                                   \
    ep_id,                                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                                              \
    ZB_HA_TEST_SAMPLE_1_DEVICE_ID,                                                                    \
    ZB_HA_TEST_SAMPLE_1_DEVICE_VER,                                                                   \
    0,                                                                                                \
    in_clust_num,                                                                                     \
    out_clust_num,                                                                                    \
    {                                                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                     \
      ZB_ZCL_CLUSTER_ID_ALARMS,                                                                       \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                         \
      ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL,                                                              \
      ZB_ZCL_CLUSTER_ID_DIAGNOSTICS                                                                   \
    }                                                                                                 \
  }
/*!
  @brief Declare endpoint for HA_TEST_SAMPLE_1 Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_EP_ZED(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_HA_TEST_SAMPLE_1_SIMPLE_DESC_ZED(ep_name, ep_id,                      \
    ZB_HA_TEST_SAMPLE_1_IN_CLUSTER_NUM_ZED, ZB_HA_TEST_SAMPLE_1_OUT_CLUSTER_NUM_ZED);  \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                     \
    0,                                                                                 \
    NULL,                                                                              \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,              \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                                  \
    0, NULL,                                                                           \
    0, NULL)
/* ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name, 0);             \ */
/*!
  @brief Declare application's device context for HA_TEST_SAMPLE_1 Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/*!
  @brief Declare application's device context for HA_TEST_SAMPLE_1 Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_HA_TEST_SAMPLE_1_CTX2(device_ctx, ep_name1, ep_name2)      \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep_name1, ep_name2)

#endif /* ZB_HA_TEST_SAMPLE_1_H */

