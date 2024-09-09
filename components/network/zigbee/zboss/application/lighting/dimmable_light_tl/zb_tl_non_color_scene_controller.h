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
/* PURPOSE: Touchlink Non-color Scene controller device definition
*/

#ifndef ZB_TL_NON_COLOR_SCENE_CONTR_H
#define ZB_TL_NON_COLOR_SCENE_CONTR_H 1

#include "zboss_api_tl.h"

/** @cond touchlink */

/**
 *  @defgroup ZB_TL_NON_COLOR_SCENE_CONTR Non-color scene controller
 *  @ingroup ZB_TL_DEVICES
 *  @addtogroup ZB_TL_NON_COLOR_SCENE_CONTR
 *  Touchlink Non-color scene controller
 *  @{
    @details
    Non-color scene controller has 6 clusters (see spec 5.3.4): \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_IDENTIFY \n
        - @ref ZB_ZCL_GROUPS \n
        - @ref ZB_ZCL_SCENES \n
        - @ref ZB_ZCL_ON_OFF \n
        - @ref ZB_ZCL_LEVEL_CONTROL
*/

#define ZB_TL_DEVICE_VER_NON_COLOR_SCENE_CONTROLLER 0

/* Touchlink Non-color Scene controller device ID */
#define ZB_TL_NON_COLOR_SCENE_CONTROLLER_DEVICE_ID 0x0830

/**< Non-color scene controller input clusters number. */
#define ZB_TL_NON_COLOR_SCENE_CONTROLLER_IN_CLUSTER_NUM  7

/**< Non-color scene controller output clusters number. */
#define ZB_TL_NON_COLOR_SCENE_CONTROLLER_OUT_CLUSTER_NUM 1

/**
 *  @brief Declare cluster list for Non-color Scene Controller device.
 *  @param cluster_list_name [IN] - cluster list variable name.
 *  @param commissioning_cluster_role [IN] - determines Commissioning @ref zcl_cluster_role
 *  "cluster role."
 */
#define ZB_TL_DECLARE_NON_COLOR_SCENE_CONTROLLER_CLUSTER_LIST(                      \
  cluster_list_name, commissioning_cluster_role)                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                                   \
      {                                                                             \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_BASIC,          0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IDENTIFY,       0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_ON_OFF,         0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_GROUPS,         0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,  0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_SCENES,         0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,  0, NULL, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_MANUF_CODE_INVALID ),   \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,  0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),    \
      }

/**
 *  @brief Declare simple descriptor for Non-color Scene Controller device.
 *  @param ep_name - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param in_clust_num [IN] - number of supported input clusters.
 *  @param out_clust_num [IN] - number of supported output clusters.
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name.
 */
#define ZB_ZCL_DECLARE_NON_COLOR_SCENE_CONTROLLER_SIMPLE_DESC(                     \
  ep_name, ep_id, in_clust_num, out_clust_num)                                     \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
      {                                                                            \
        ep_id,                                                                     \
        ZB_AF_ZLL_PROFILE_ID,                                                      \
        ZB_TL_NON_COLOR_SCENE_CONTROLLER_DEVICE_ID,                                \
        ZB_TL_DEVICE_VER_NON_COLOR_SCENE_CONTROLLER,                               \
        0,                                                                         \
        in_clust_num,                                                              \
        out_clust_num,                                                             \
        {                                                                          \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                              \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                                \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                                \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                         \
          ZB_ZCL_CLUSTER_ID_SCENES,                                                \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                               \
          ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING                                \
        }                                                                          \
      }

/**
 *  @brief Declare endpoint for Non-color Scene Controller device.
 *  @param ep_name [IN] - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param cluster_list [IN] - endpoint cluster list.
 */
#define ZB_TL_DECLARE_NON_COLOR_SCENE_CONTROLLER_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_NON_COLOR_SCENE_CONTROLLER_SIMPLE_DESC(                \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_TL_NON_COLOR_SCENE_CONTROLLER_IN_CLUSTER_NUM,                    \
    ZB_TL_NON_COLOR_SCENE_CONTROLLER_OUT_CLUSTER_NUM);                  \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_AF_ZLL_PROFILE_ID,                                               \
    0,                                                                  \
    NULL,                                                               \
    ZB_ZCL_ARRAY_SIZE(                                                  \
      cluster_list,                                                     \
      zb_zcl_cluster_desc_t),                                           \
    cluster_list,                                                       \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                   \
    0, NULL, /* No reporting ctx */                                     \
    0, NULL) /* No CVC ctx */

#define ZB_TL_DECLARE_NON_COLOR_SCENE_CONTROLLER_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/**
 * @}
 */

/** @endcond */ /* touchlink */

#endif /* ZB_TL_NON_COLOR_SCENE_CONTR_H */
