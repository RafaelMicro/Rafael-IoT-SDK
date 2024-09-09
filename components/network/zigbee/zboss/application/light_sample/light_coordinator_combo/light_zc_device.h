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
/* PURPOSE: Light ZC device definition
*/
#ifndef LIGHT_ZC_DEVICE_H
#define LIGHT_ZC_DEVICE_H 1

#define ZB_HA_DEVICE_VER_LIGHT_ZC 0  /*!< Light ZC device version */
#define ZB_HA_LIGHT_ZC_DEVICE_ID 0

#define ZB_HA_LIGHT_ZC_IN_CLUSTER_NUM 2  /*!< @internal Light ZC IN clusters number */
#define ZB_HA_LIGHT_ZC_OUT_CLUSTER_NUM 5 /*!< @internal Light ZC OUT clusters number */

#define ZB_HA_LIGHT_ZC_CLUSTER_NUM                                      \
  (ZB_HA_LIGHT_ZC_IN_CLUSTER_NUM + ZB_HA_LIGHT_ZC_OUT_CLUSTER_NUM)

/*! @internal Number of attribute for reporting on Light ZC device */
#define ZB_HA_LIGHT_ZC_REPORT_ATTR_COUNT 0

/** @brief Declare cluster list for Light ZC device
    @param cluster_list_name - cluster list variable name
    @param basic_attr_list - attribute list for Basic cluster
    @param identify_attr_list - attribute list for Identify cluster
 */
#define ZB_HA_DECLARE_LIGHT_ZC_CLUSTER_LIST(                                \
      cluster_list_name,                                                    \
      basic_attr_list,                                                      \
      identify_attr_list)                                                   \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),             \
          (identify_attr_list),                                             \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                \
          (basic_attr_list),                                                \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                  \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_SCENES,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                       \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        )                                                                   \
    }


/** @internal @brief Declare simple descriptor for Light ZC device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param in_clust_num - number of supported input clusters
    @param out_clust_num - number of supported output clusters
    @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
    definitions, because these values are used to form simple descriptor type name
*/
#define ZB_ZCL_DECLARE_LIGHT_ZC_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)      \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                        \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                 \
  {                                                                                           \
    ep_id,                                                                                    \
    ZB_AF_HA_PROFILE_ID,                                                                      \
    ZB_HA_LIGHT_ZC_DEVICE_ID,                                                                 \
    ZB_HA_DEVICE_VER_LIGHT_ZC,                                                                \
    0,                                                                                        \
    in_clust_num,                                                                             \
    out_clust_num,                                                                            \
    {                                                                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                             \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                               \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                        \
      ZB_ZCL_CLUSTER_ID_SCENES,                                                               \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                                               \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                             \
    }                                                                                         \
  }

/** @brief Declare endpoint for Light ZC device
    @param ep_name - endpoint variable name
    @param ep_id - endpoint ID
    @param cluster_list - endpoint cluster list
 */
#define ZB_HA_DECLARE_LIGHT_ZC_EP(ep_name, ep_id, cluster_list)                       \
  ZB_ZCL_DECLARE_LIGHT_ZC_SIMPLE_DESC(                                                \
      ep_name,                                                                        \
      ep_id,                                                                          \
      ZB_HA_LIGHT_ZC_IN_CLUSTER_NUM,                                                  \
      ZB_HA_LIGHT_ZC_OUT_CLUSTER_NUM);                                                \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
      ZB_AF_HA_PROFILE_ID,                                                            \
      0,                                                                              \
      NULL,                                                                           \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                         \
      cluster_list,                                                                   \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,       \
                              0, NULL, /* No reporting ctx */                         \
                              0, NULL)

/** @brief Declare Light ZC device context.
    @param device_ctx - device context variable name.
    @param ep_name - endpoint variable name.
*/
#define ZB_HA_DECLARE_LIGHT_ZC_CTX(device_ctx, ep_name)    \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)
 /* No CVC ctx */

/*! @} */

#endif /* LIGHT_ZC_DEVICE_H */
