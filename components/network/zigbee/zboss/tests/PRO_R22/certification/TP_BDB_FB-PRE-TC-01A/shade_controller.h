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
/* PURPOSE: Shade controller device definition for test
*/

#ifndef SHADE_CONTROLLER_H
#define SHADE_CONTROLLER_H 1

/******************* Specific definitions for Shade controller device **************************/

#define DEVICE_ID_SHADE_CONTROLLER   0xffa0
#define DEVICE_VER_SHADE_CONTROLLER 0  /*!< Shade device version */

#define SHADE_CONTROLLER_IN_CLUSTER_NUM 2 /*!< Shade IN (server) clusters number */
#define SHADE_CONTROLLER_OUT_CLUSTER_NUM 3 /*!< Shade OUT (client) clusters number */

#define SHADE_CONTROLLER_CLUSTER_NUM (SHADE_CONTROLLER_IN_CLUSTER_NUM + \
                                      SHADE_CONTROLLER_OUT_CLUSTER_NUM)


/*!
  @brief Declare cluster list for Shade controller device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param on_off_attr_list - attribute list for On/Off cluster
  @param level_control_attr_list - attribute list for Level Control cluster
 */
#define DECLARE_SHADE_CONTROLLER_CLUSTER_LIST(              \
  cluster_list_name,                                        \
  basic_attr_list,                                          \
  identify_attr_list)                                       \
  zb_zcl_cluster_desc_t cluster_list_name[] =               \
  {                                                         \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t), \
      (identify_attr_list),                                 \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID,                            \
      ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_INIT           \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                              \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),    \
      (basic_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID,                            \
      NULL                                                  \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
      0,                                                    \
      NULL,                                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID,                            \
      NULL                                                  \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                      \
      0,                                                    \
      NULL,                                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID,                            \
      NULL                                                  \
    },                                                      \
    {                                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
      0,                                                    \
      NULL,                                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
      ZB_ZCL_MANUF_CODE_INVALID,                            \
      ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_INIT           \
    }                                                       \
  }


/*!
  @brief Declare simple descriptor for Shade controller device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define DECLARE_SHADE_CONTROLLER_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =         \
  {                                                                                   \
    ep_id,                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                              \
    DEVICE_ID_SHADE_CONTROLLER,                                                       \
    DEVICE_VER_SHADE_CONTROLLER,                                                      \
    0,                                                                                \
    in_clust_num,                                                                     \
    out_clust_num,                                                                    \
    {                                                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                       \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY                                                      \
    }                                                                                 \
  }

/*!
  @brief Declare endpoint for Shade controller device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */

#define DECLARE_SHADE_CONTROLLER_EP(ep_name, ep_id, cluster_list)                       \
  DECLARE_SHADE_CONTROLLER_SIMPLE_DESC(							\
    ep_name,                                                                            \
    ep_id,                                                                              \
    SHADE_CONTROLLER_IN_CLUSTER_NUM,                                                    \
    SHADE_CONTROLLER_OUT_CLUSTER_NUM);                                                  \
    ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                                ep_id, ZB_AF_HA_PROFILE_ID,                             \
                                0,                                                      \
                                NULL,                                                   \
                                ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                                cluster_list,                                           \
                                (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,       \
                                0, NULL,                                                \
                                0, NULL)


#define DECLARE_SHADE_CONTROLLER_CTX(device_ctx, ep_name)             \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

#endif /* SHADE_CONTROLLER_H */
