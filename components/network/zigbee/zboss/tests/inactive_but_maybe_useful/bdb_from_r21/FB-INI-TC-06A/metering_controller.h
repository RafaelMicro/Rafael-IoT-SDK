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
/* PURPOSE: Metering controller device definition for test
*/

#ifndef METERING_CONTROLLER_H
#define METERING_CONTROLLER_H 1

/******************* Specific definitions for Metering controller device **************************/

#define DEVICE_ID_METERING_CONTROLLER   0xfff0
#define DEVICE_VER_METERING_CONTROLLER 0  /*!< Shade device version */

#define METERING_CONTROLLER_IN_CLUSTER_NUM 2
#define METERING_CONTROLLER_OUT_CLUSTER_NUM 2

#define METERING_CONTROLLER_CLUSTER_NUM (METERING_CONTROLLER_IN_CLUSTER_NUM +    \
                                         METERING_CONTROLLER_OUT_CLUSTER_NUM +   \
                                         ZB_HA_OTA_UPGRADE_SERVER_IN_CLUSTER_NUM+ \
                                         ZB_HA_OTA_UPGRADE_SERVER_OUT_CLUSTER_NUM) \

/*!
  @brief Declare cluster list for Shade controller device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
 */
#define DECLARE_METERING_CONTROLLER_CLUSTER_LIST(           \
  cluster_list_name,                                        \
  basic_attr_list,                                          \
  identify_attr_list)                                       \
  zb_zcl_cluster_desc_t cluster_list_name[] =               \
  {                                                         \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t), \
      (identify_attr_list),                                 \
      ZB_ZCL_CLUSTER_MIXED_ROLE                             \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                              \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),    \
      (basic_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE                            \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_METERING,                           \
      0,                                                    \
      NULL,                                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE                            \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
      0,                                                    \
      NULL,                                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE                            \
    },                                                      \
    {                                                             \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                              \
      ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),    \
      (ota_upgrade_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
    }                                                             \
  }



/*!
  @brief Declare simple descriptor for Metering controller device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define DECLARE_METERING_CONTROLLER_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =         \
  {                                                                                   \
    ep_id,                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                              \
    DEVICE_ID_METERING_CONTROLLER,                                                    \
    DEVICE_VER_METERING_CONTROLLER,                                                   \
    0,                                                                                \
    in_clust_num,                                                                     \
    out_clust_num,                                                                    \
    {                                                                                 \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
      ZB_ZCL_CLUSTER_ID_METERING,                                                     \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                       \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE                                                   \
    }                                                                                 \
  }

#define _SUM(a,b)

/*!
  @brief Declare endpoint for Ьуеукштп controller device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */

#define DECLARE_METERING_CONTROLLER_EP(ep_name, ep_id, cluster_list)                   \
    DECLARE_METERING_CONTROLLER_SIMPLE_DESC(                                           \
    ep_name,                                                                           \
    ep_id,                                                                             \
    4,                                                                                 \
    2);                                                                                \
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_name)                                           \
  ZB_AF_SET_ENDPOINT_DESC(ep_id, ZB_AF_HA_PROFILE_ID,                                  \
    0,                                                                                 \
    NULL,                                                                              \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,              \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name)                                  \
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST

/*
-    (METERING_CONTROLLER_IN_CLUSTER_NUM + ZB_HA_OTA_UPGRADE_SERVER_IN_CLUSTER_NUM),    \
-    (METERING_CONTROLLER_OUT_CLUSTER_NUM + ZB_HA_OTA_UPGRADE_SERVER_OUT_CLUSTER_NUM)); \
*/


#define DECLARE_METERING_CONTROLLER_CTX(device_ctx, ep_name)             \
  ZB_AF_DECLARE_DEVICE_CTX_NO_REP(device_ctx, ep_name,                      \
    ZB_ZCL_ARRAY_SIZE(ep_name, zb_af_endpoint_desc_t))

/*! @} */

#endif /* ZB_HA_SHADE_CONTROLLER_H */
