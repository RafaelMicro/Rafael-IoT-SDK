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
/* PURPOSE: On/off server device definition for test
*/

#ifndef ON_OFF_SERVER_H
#define ON_OFF_SERVER_H 1

/******************* Specific definitions for On/Off server device **************************/

#define DEVICE_ID_ON_OFF_SERVER   0xaa02
#define DEVICE_VER_ON_OFF_SERVER  0  /*!< device version */

#define ON_OFF_SERVER_IN_CLUSTER_NUM 3 /*!< IN (server) clusters number */
#define ON_OFF_SERVER_OUT_CLUSTER_NUM 1 /*!<  OUT (client) clusters number */

#define ON_OFF_SERVER_CLUSTER_NUM (ON_OFF_SERVER_IN_CLUSTER_NUM + \
                                   ON_OFF_SERVER_OUT_CLUSTER_NUM)

/*!
  @brief Declare cluster list for On/Off server
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param on_off_attr_list - attribute list for On/Off cluster
 */
#define DECLARE_ON_OFF_SERVER_CLUSTER_LIST(                 \
  cluster_list_name,                                        \
  basic_attr_list,                                          \
  identify_attr_list,                                       \
  on_off_attr_list)                                         \
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
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),   \
      (on_off_attr_list),                                   \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
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

#define DECLARE_SERVER_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =          \
  {                                                                                    \
    ep_id,                                                                             \
    ZB_AF_HA_PROFILE_ID,                                                               \
    DEVICE_ID_ON_OFF_SERVER,                                                           \
    DEVICE_VER_ON_OFF_SERVER,                                                          \
    0,                                                                                 \
    in_clust_num,                                                                      \
    out_clust_num,                                                                     \
    {                                                                                  \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                      \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                         \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY                                                       \
    }                                                                                  \
  }

/* Declare device ctx */
#define DECLARE_SERVER_EP(ep_name, ep_id, cluster_list)                                 \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,							\
                              ep_id, ZB_AF_HA_PROFILE_ID,				\
                              0,							\
                              NULL,							\
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),   \
                              cluster_list,						\
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,		\
                              0, NULL,							\
                              0, NULL)

/* Declare device ctx */
#define DECLARE_ON_OFF_SERVER_CTX(device_ctx_name, ep1_name, ep2_name)	\
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_list_##device_ctx_name)		\
    &ep1_name,                                                          \
    &ep2_name,								\
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST;					\
  ZBOSS_DECLARE_DEVICE_CTX(device_ctx_name, ep_list_##device_ctx_name,  \
                           ZB_ZCL_ARRAY_SIZE(ep_list_##device_ctx_name, zb_af_endpoint_desc_t*))

#endif /* ON_OFF_SERVER_H */
