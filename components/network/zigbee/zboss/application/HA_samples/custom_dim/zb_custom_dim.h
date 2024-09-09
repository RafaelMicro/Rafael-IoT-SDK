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
/* PURPOSE: ZB CUSTOM_DIMMABLE_LIGHT device
*/

#ifndef ZB_CUSTOM_DIMMABLE_LIGHT_H
#define ZB_CUSTOM_DIMMABLE_LIGHT_H 1

#define ZB_CUSTOM_DIMMABLE_LIGHT_DEVICE_ID 0
#define ZB_CUSTOM_DIMMABLE_LIGHT_DEVICE_VER 1
#define ZB_CUSTOM_DIMMABLE_LIGHT_IN_CLUSTER_NUM_ZED 0
#define ZB_CUSTOM_DIMMABLE_LIGHT_OUT_CLUSTER_NUM_ZED 7

/*!
  @brief Declare cluster list for CUSTOM_DIMMABLE_LIGHT Device
  @param cluster_list_name - cluster list variable name
*/
#define ZB_HA_DECLARE_CUSTOM_DIMMABLE_LIGHT_CLUSTER_LIST_ZED(  \
  cluster_list_name)                                           \
  zb_zcl_cluster_desc_t cluster_list_name[] =                  \
{                                                              \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_BASIC,                                   \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_SCENES,                                  \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_GROUPS,                                  \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_ON_OFF,                                  \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                           \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_COLOR_CONTROL,                           \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  )                                                            \
}

/*!
  @brief Declare simple descriptor for CUSTOM_DIMMABLE_LIGHT Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_CUSTOM_DIMMABLE_LIGHT_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                                     \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                              \
  {                                                                                                        \
    ep_id,                                                                                                 \
    ZB_AF_HA_PROFILE_ID,                                                                                   \
    ZB_CUSTOM_DIMMABLE_LIGHT_DEVICE_ID,                                                                    \
    ZB_CUSTOM_DIMMABLE_LIGHT_DEVICE_VER,                                                                   \
    0,                                                                                                     \
    in_clust_num,                                                                                          \
    out_clust_num,                                                                                         \
    {                                                                                                      \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                             \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                                            \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                                                            \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                            \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                                     \
      ZB_ZCL_CLUSTER_ID_COLOR_CONTROL                                                                      \
    }                                                                                                      \
  }
/*!
  @brief Declare endpoint for CUSTOM_DIMMABLE_LIGHT Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_CUSTOM_DIMMABLE_LIGHT_EP_ZED(ep_name, ep_id, cluster_list)                 \
  ZB_ZCL_DECLARE_CUSTOM_DIMMABLE_LIGHT_SIMPLE_DESC_ZED(ep_name, ep_id,                           \
    ZB_CUSTOM_DIMMABLE_LIGHT_IN_CLUSTER_NUM_ZED, ZB_CUSTOM_DIMMABLE_LIGHT_OUT_CLUSTER_NUM_ZED);  \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,                           \
                                     1);                                                         \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                               \
    0,                                                                                           \
    NULL,                                                                                        \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,                        \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                                            \
    0, reporting_info## device_ctx_name,                                                         \
    0, NULL)

/*!
  @brief Declare application's device context for CUSTOM_DIMMABLE_LIGHT Device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#define ZB_HA_DECLARE_CUSTOM_DIMMABLE_LIGHT_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/**
 *  @brief Declare attribute list for On/Off cluster (extended attribute set).
 *  @param attr_list [IN] - attribute list name being declared by this macro.
 *  @param on_off [IN] - pointer to a boolean variable storing on/off attribute value.
 *  @param global_scene_ctrl [IN] - pointer to a boolean variable storing global scene control attribute value.
 *  @param on_time [IN] - pointer to a unsigned 16-bit integer variable storing on time attribute value.
 *  @param off_wait_time [IN] - pointer to a unsigned 16-bit integer variable storing off wait time attribute value.
 *  @param start_up_on_off [IN] - pointer to a 8-bit enum variable storing start up on off attribute value.
 */
#define ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT_WITH_START_UP_ON_OFF(                             \
    attr_list, on_off, global_scene_ctrl, on_time, off_wait_time, start_up_on_off               \
    )                                                                                           \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, (on_off))                                \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL, (global_scene_ctrl))          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_TIME, (on_time))                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, (off_wait_time))                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, (start_up_on_off))                 \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#endif /* ZB_CUSTOM_DIMMABLE_LIGHT_H */

