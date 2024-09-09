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
/* PURPOSE: ZB DOOR_LOCK device
*/

#ifndef ZB_DOOR_LOCK_H
#define ZB_DOOR_LOCK_H 1

#define ZB_DOOR_LOCK_DEVICE_ID 0x0FD0
#define ZB_DOOR_LOCK_DEVICE_VER 1
#define ZB_DOOR_LOCK_IN_CLUSTER_NUM_ZED 0
#define ZB_DOOR_LOCK_OUT_CLUSTER_NUM_ZED 5

/*!
  @brief Declare cluster list for DOOR_LOCK Device
  @param cluster_list_name - cluster list variable name
*/
#define ZB_HA_DECLARE_DOOR_LOCK_CLUSTER_LIST_ZED(  \
  cluster_list_name)                               \
  zb_zcl_cluster_desc_t cluster_list_name[] =      \
{                                                  \
  ZB_ZCL_CLUSTER_DESC(                             \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                    \
    0,                                             \
    NULL,                                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                    \
    ZB_ZCL_MANUF_CODE_INVALID                      \
  ),                                               \
  ZB_ZCL_CLUSTER_DESC(                             \
    ZB_ZCL_CLUSTER_ID_BASIC,                       \
    0,                                             \
    NULL,                                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                    \
    ZB_ZCL_MANUF_CODE_INVALID                      \
  ),                                               \
  ZB_ZCL_CLUSTER_DESC(                             \
    ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                   \
    0,                                             \
    NULL,                                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                    \
    ZB_ZCL_MANUF_CODE_INVALID                      \
  ),                                               \
  ZB_ZCL_CLUSTER_DESC(                             \
    ZB_ZCL_CLUSTER_ID_SCENES,                      \
    0,                                             \
    NULL,                                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                    \
    ZB_ZCL_MANUF_CODE_INVALID                      \
  ),                                               \
  ZB_ZCL_CLUSTER_DESC(                             \
    ZB_ZCL_CLUSTER_ID_GROUPS,                      \
    0,                                             \
    NULL,                                          \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                    \
    ZB_ZCL_MANUF_CODE_INVALID                      \
  )                                                \
}

/*!
  @brief Declare simple descriptor for DOOR_LOCK Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_DOOR_LOCK_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                  \
  {                                                                                            \
    ep_id,                                                                                     \
    ZB_AF_HA_PROFILE_ID,                                                                       \
    ZB_DOOR_LOCK_DEVICE_ID,                                                                    \
    ZB_DOOR_LOCK_DEVICE_VER,                                                                   \
    0,                                                                                         \
    in_clust_num,                                                                              \
    out_clust_num,                                                                             \
    {                                                                                          \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                 \
      ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                                                             \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                                \
      ZB_ZCL_CLUSTER_ID_GROUPS                                                                 \
    }                                                                                          \
  }
/*!
  @brief Declare endpoint for DOOR_LOCK Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_DOOR_LOCK_EP_ZED(ep_name, ep_id, cluster_list)       \
  ZB_ZCL_DECLARE_DOOR_LOCK_SIMPLE_DESC_ZED(ep_name, ep_id,                 \
    ZB_DOOR_LOCK_IN_CLUSTER_NUM_ZED, ZB_DOOR_LOCK_OUT_CLUSTER_NUM_ZED);    \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,     \
                                     0);                                   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,         \
    0,                                                                     \
    NULL,                                                                  \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,  \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                      \
    0, reporting_info## device_ctx_name,                                   \
    0, NULL)

/*!
  @brief Declare application's device context for DOOR_LOCK Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_DOOR_LOCK_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

#endif /* ZB_DOOR_LOCK_H */

