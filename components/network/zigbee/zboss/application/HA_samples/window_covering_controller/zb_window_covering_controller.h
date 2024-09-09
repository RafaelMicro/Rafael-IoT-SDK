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
/* PURPOSE: ZB WINDOW_COVERING_CONTROLLER device
*/

#ifndef ZB_WINDOW_COVERING_CONTROLLER_H
#define ZB_WINDOW_COVERING_CONTROLLER_H 1

#define ZB_WINDOW_COVERING_CONTROLLER_DEVICE_ID 0x0FAE
#define ZB_WINDOW_COVERING_CONTROLLER_DEVICE_VER 1
#define ZB_WINDOW_COVERING_CONTROLLER_IN_CLUSTER_NUM_ZED 2
#define ZB_WINDOW_COVERING_CONTROLLER_OUT_CLUSTER_NUM_ZED 2

/*!
  @brief Declare cluster list for WINDOW_COVERING_CONTROLLER Device
  @param cluster_list_name - cluster list variable name
  @param identify_cli_attr_list - attribute list for Identify cluster
  @param window_covering_cli_attr_list - attribute list for Window Covering cluster
*/
#define ZB_HA_DECLARE_WINDOW_COVERING_CONTROLLER_CLUSTER_LIST_ZED(    \
  cluster_list_name,                                                  \
  identify_cli_attr_list,                                             \
  window_covering_cli_attr_list)                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =                         \
{                                                                     \
  ZB_ZCL_CLUSTER_DESC(                                                \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
    0,                                                                \
    NULL,                                                             \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
    ZB_ZCL_MANUF_CODE_INVALID                                         \
  ),                                                                  \
  ZB_ZCL_CLUSTER_DESC(                                                \
    ZB_ZCL_CLUSTER_ID_BASIC,                                          \
    0,                                                                \
    NULL,                                                             \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
    ZB_ZCL_MANUF_CODE_INVALID                                         \
  ),                                                                  \
  ZB_ZCL_CLUSTER_DESC(                                                \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
    ZB_ZCL_ARRAY_SIZE(identify_cli_attr_list, zb_zcl_attr_t),         \
    (identify_cli_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
    ZB_ZCL_MANUF_CODE_INVALID                                         \
  ),                                                                  \
  ZB_ZCL_CLUSTER_DESC(                                                \
    ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,                                \
    ZB_ZCL_ARRAY_SIZE(window_covering_cli_attr_list, zb_zcl_attr_t),  \
    (window_covering_cli_attr_list),                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
    ZB_ZCL_MANUF_CODE_INVALID                                         \
  )                                                                   \
}

/*!
  @brief Declare simple descriptor for WINDOW_COVERING_CONTROLLER Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_WINDOW_COVERING_CONTROLLER_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                                          \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                                   \
  {                                                                                                             \
    ep_id,                                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                                        \
    ZB_WINDOW_COVERING_CONTROLLER_DEVICE_ID,                                                                    \
    ZB_WINDOW_COVERING_CONTROLLER_DEVICE_VER,                                                                   \
    0,                                                                                                          \
    in_clust_num,                                                                                               \
    out_clust_num,                                                                                              \
    {                                                                                                           \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                               \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                                  \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                               \
      ZB_ZCL_CLUSTER_ID_WINDOW_COVERING                                                                         \
    }                                                                                                           \
  }
/*!
  @brief Declare endpoint for WINDOW_COVERING_CONTROLLER Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_WINDOW_COVERING_CONTROLLER_EP_ZED(ep_name, ep_id, cluster_list)                      \
  ZB_ZCL_DECLARE_WINDOW_COVERING_CONTROLLER_SIMPLE_DESC_ZED(ep_name, ep_id,                                \
    ZB_WINDOW_COVERING_CONTROLLER_IN_CLUSTER_NUM_ZED, ZB_WINDOW_COVERING_CONTROLLER_OUT_CLUSTER_NUM_ZED);  \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,                                     \
                                     0);                                                                   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                                         \
    0,                                                                                                     \
    NULL,                                                                                                  \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,                                  \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                                                      \
    0, reporting_info## device_ctx_name,                                                                   \
    0, NULL)

/*!
  @brief Declare application's device context for WINDOW_COVERING_CONTROLLER Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_WINDOW_COVERING_CONTROLLER_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

#endif /* ZB_WINDOW_COVERING_CONTROLLER_H */

