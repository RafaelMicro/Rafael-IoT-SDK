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
/* PURPOSE: TH device definition for test
*/

#ifndef DEVICE_TH_H
#define DEVICE_TH_H 1

/******************* Specific definitions for device **************************/

#define ZB_WWAH_DEVICE_ID 0x3131
#define ZB_WWAH_DEVICE_VER 1

#define ZB_WWAH_ZC_HA_EP_IN_CLUSTER_NUM 4
#define ZB_WWAH_ZC_HA_EP_OUT_CLUSTER_NUM 3

#define ZC_HA_EP 4

#define ZB_WWAH_REPORT_ATTR_COUNT (  \
  ZB_ZCL_WWAH_REPORT_ATTR_COUNT +    \
  ZB_ZCL_TIME_REPORT_ATTR_COUNT)

/*!
  @brief Declare HA cluster list for WWAH Device
  @param cluster_list_name - cluster list variable name
  @param identify_attr_list - attribute list for Identify cluster
  @param time_attr_list - attribute list for Time cluster
  @param keep_alive_attr_list - attribute list for Keep-Alive cluster
  @param pool_control_attr_list - attribute list for Poll Control cluster
  @param ota_upgrade_attr_list - attribute list for OTA Upgrade cluster
*/
#define ZB_HA_DECLARE_WWAH_CLUSTER_LIST_ZC(                   \
  cluster_list_name,                                          \
  identify_attr_list,                                         \
  time_attr_list,                                             \
  keep_alive_attr_list,                                       \
  poll_control_attr_list,                                     \
  ota_upgrade_attr_list,                                      \
  wwah_client_attr_list)                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =                 \
{                                                             \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_BASIC,                                  \
    0,                                                        \
    NULL,                                                     \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_WWAH,                                   \
    ZB_ZCL_ARRAY_SIZE(wwah_client_attr_list, zb_zcl_attr_t),  \
    (wwah_client_attr_list),                                  \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    WWAH_MANUFACTURER_CODE                                    \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                               \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),     \
    (identify_attr_list),                                     \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_TIME,                                   \
    ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),         \
    (time_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                             \
    ZB_ZCL_ARRAY_SIZE(keep_alive_attr_list, zb_zcl_attr_t),   \
    (keep_alive_attr_list),                                   \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                           \
    ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t), \
    (poll_control_attr_list),                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                            \
    ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),  \
    (ota_upgrade_attr_list),                                  \
     ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
     ZB_ZCL_MANUF_CODE_INVALID                                \
  )                                                           \
}

/*!
  @brief Declare HA simple descriptor for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZC(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                      \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =               \
  {                                                                                         \
    ep_id,                                                                                  \
    ZB_AF_HA_PROFILE_ID,                                                                    \
    ZB_WWAH_DEVICE_ID,                                                                      \
    ZB_WWAH_DEVICE_VER,                                                                     \
    0,                                                                                      \
    in_clust_num,                                                                           \
    out_clust_num,                                                                          \
    {                                                                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                           \
      ZB_ZCL_CLUSTER_ID_TIME,                                                               \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                                         \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                                                        \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
      ZB_ZCL_CLUSTER_ID_WWAH,                                                               \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL                                                        \
    }                                                                                       \
  }

/*!
  @brief Declare HA endpoint for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_WWAH_EP_ZC(ep_name, ep_id, cluster_list)             \
  ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZC(ep_name, ep_id,                    \
    ZB_WWAH_ZC_HA_EP_IN_CLUSTER_NUM, ZB_WWAH_ZC_HA_EP_OUT_CLUSTER_NUM);    \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,         \
    0,                                                                     \
    NULL,                                                                  \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,  \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                      \
    0, NULL,                                                               \
    0, NULL)

#endif /* DEVICE_TH_H */
