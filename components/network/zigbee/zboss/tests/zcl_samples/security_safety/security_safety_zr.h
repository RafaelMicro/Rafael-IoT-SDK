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

#define ZB_SECURITY_SAFETY_DEVICE_VER 1

#define ZB_DECLARE_SECURITY_SAFETY_SERVER_CLUSTER_LIST(         \
  cluster_list_name,                                            \
  ias_zone_attr_list,                                           \
  ias_ace_attr_list,                                            \
  ias_wd_attr_list);                                            \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_ZONE,                           \
          ZB_ZCL_ARRAY_SIZE(ias_zone_attr_list, zb_zcl_attr_t), \
          (ias_zone_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_ACE,                            \
          ZB_ZCL_ARRAY_SIZE(ias_ace_attr_list, zb_zcl_attr_t),  \
          (ias_ace_attr_list),                                  \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_WD,                             \
          ZB_ZCL_ARRAY_SIZE(ias_wd_attr_list, zb_zcl_attr_t),   \
          (ias_wd_attr_list),                                   \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        )                                                       \
      }

#define ZB_DECLARE_SECURITY_SAFETY_CLIENT_CLUSTER_LIST(         \
  cluster_list_name)                                            \
      zb_zcl_cluster_desc_t cluster_list_name[] =               \
      {                                                         \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_ZONE,                           \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_ACE,                            \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        ),                                                      \
        ZB_ZCL_CLUSTER_DESC(                                    \
          ZB_ZCL_CLUSTER_ID_IAS_WD,                             \
          0,                                                    \
          NULL,                                                 \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
          ZB_ZCL_MANUF_CODE_INVALID                             \
        )                                                       \
      }

/*!
  @brief Declare simple descriptor for SECURITY_SAFETY_SERVER Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_SECURITY_SAFETY_SERVER_SIMPLE_DESC(                                                \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_SECURITY_SAFETY_DEVICE_VER,                                                                \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE,                                                                 \
      ZB_ZCL_CLUSTER_ID_IAS_ACE,                                                                  \
      ZB_ZCL_CLUSTER_ID_IAS_WD                                                                    \
    }                                                                                             \
  }

/*!
  @brief Declare simple descriptor for SECURITY_SAFETY_CLIENT Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_SECURITY_SAFETY_CLIENT_SIMPLE_DESC(                                        \
  ep_name, ep_id, in_clust_num, out_clust_num)                                                    \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                     \
  {                                                                                               \
    ep_id,                                                                                        \
    ZB_AF_HA_PROFILE_ID,                                                                          \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                                  \
    ZB_SECURITY_SAFETY_DEVICE_VER,                                                                \
    0,                                                                                            \
    in_clust_num,                                                                                 \
    out_clust_num,                                                                                \
    {                                                                                             \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE,                                                                 \
      ZB_ZCL_CLUSTER_ID_IAS_ACE,                                                                  \
      ZB_ZCL_CLUSTER_ID_IAS_WD                                                                    \
    }                                                                                             \
  }

/*!
  @brief Declare endpoint for SECURITY_SAFETY Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_SECURITY_SAFETY_SERVER_IN_CLUSTER_NUM 3
#define ZB_SECURITY_SAFETY_SERVER_OUT_CLUSTER_NUM 0
#define ZB_DECLARE_SECURITY_SAFETY_SERVER_EP(ep_name, ep_id, cluster_list)     \
  ZB_ZCL_DECLARE_SECURITY_SAFETY_SERVER_SIMPLE_DESC(                           \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_SECURITY_SAFETY_SERVER_IN_CLUSTER_NUM,                                  \
    ZB_SECURITY_SAFETY_SERVER_OUT_CLUSTER_NUM);                                \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    0, NULL,                                                                   \
    0, NULL)

/*!
  @brief Declare endpoint for SECURITY_SAFETY Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_SECURITY_SAFETY_CLIENT_IN_CLUSTER_NUM 0
#define ZB_SECURITY_SAFETY_CLIENT_OUT_CLUSTER_NUM 3
#define ZB_DECLARE_SECURITY_SAFETY_CLIENT_EP(ep_name, ep_id, cluster_list)     \
  ZB_ZCL_DECLARE_SECURITY_SAFETY_CLIENT_SIMPLE_DESC(                           \
    ep_name,                                                                   \
    ep_id,                                                                     \
    ZB_SECURITY_SAFETY_CLIENT_IN_CLUSTER_NUM,                                  \
    ZB_SECURITY_SAFETY_CLIENT_OUT_CLUSTER_NUM);                                \
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
#define ZB_DECLARE_SECURITY_SAFETY_CTX(device_ctx, ep1_name, ep2_name)  \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep1_name, ep2_name)
