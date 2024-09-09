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
/* PURPOSE: HA sample controller device definition
*/

#ifndef ZB_HA_SAMPLE_DEVICE_H
#define ZB_HA_SAMPLE_DEVICE_H 1

/**
 *  @defgroup ZB_HA_SAMPLE_DEVICE sample controller device.
 *  @ingroup ZB_ZCL_SAMPLE
 *  @addtogroup ZB_HA_SAMPLE_DEVICE
 *  HA Sample device.
 *  @{
    @details
    Diagnostics device has 2 clusters: \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_CLUSTER_ID_IDENTIFY

*/

#define ZB_HA_DEVICE_VER_SAMPLE        0       /**< Device version */
#define ZB_HA_SAMPLE_DEVICE_ID         0xfff0  /**< Device ID */

#define ZB_HA_SAMPLE_IN_CLUSTER_NUM    2 /**< Diagnostics test input clusters number. */
#define ZB_HA_SAMPLE_OUT_CLUSTER_NUM   2 /**< Diagnostics test output clusters number. */

/**
 *  @brief Declare cluster list for Diagnostics device.
 *  @param cluster_list_name [IN] - cluster list variable name.
 *  @param basic_attr_list [IN] - attribute list for Basic cluster.
 *  @param identify_attr_list [IN] - attribute list for Identify cluster.
 */
#define ZB_HA_DECLARE_SAMPLE_CLUSTER_LIST(                            \
  cluster_list_name,                                                  \
  basic_attr_list,                                                    \
  identify_attr_list)                                                 \
      zb_zcl_cluster_desc_t cluster_list_name[] =                     \
      {                                                               \
        {                                                             \
          ZB_ZCL_CLUSTER_ID_BASIC,                                    \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
          (basic_attr_list),                                          \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
        },                                                            \
        {                                                             \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                 \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),       \
          (identify_attr_list),                                       \
          ZB_ZCL_CLUSTER_SERVER_ROLE                                  \
        },                                                            \
        { ZB_ZCL_CLUSTER_ID_IAS_ZONE,  0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE },  \
        { ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,  0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE },  \
      }

/**
 *  @brief Declare simple descriptor for Diagnostics device.
 *  @param ep_name - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param in_clust_num [IN] - number of supported input clusters.
 *  @param out_clust_num [IN] - number of supported output clusters.
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name.
 */
#define ZB_ZCL_DECLARE_SAMPLE_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)   \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);    \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)     \
      simple_desc_##ep_name =                                 \
      {                                                       \
        ep_id,                                                \
        ZB_AF_HA_PROFILE_ID,                                  \
        ZB_HA_SAMPLE_DEVICE_ID,                               \
        ZB_HA_DEVICE_VER_SAMPLE,                              \
        0,                                                    \
        in_clust_num,                                         \
        out_clust_num,                                        \
        {                                                     \
          ZB_ZCL_CLUSTER_ID_BASIC,                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                         \
          ZB_ZCL_CLUSTER_ID_IAS_ZONE,                         \
          ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                 \
        }                                                     \
      }

/**
 *  @brief Declare endpoint for Diagnostics device.
 *  @param ep_name [IN] - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param cluster_list [IN] - endpoint cluster list.
 */
#define ZB_HA_DECLARE_SAMPLE_EP(ep_name, ep_id, cluster_list)            \
      ZB_ZCL_DECLARE_SAMPLE_SIMPLE_DESC(                                 \
          ep_name,                                                       \
          ep_id,                                                         \
          ZB_HA_SAMPLE_IN_CLUSTER_NUM,                                   \
          ZB_HA_SAMPLE_OUT_CLUSTER_NUM);                                 \
      ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_name)                         \
        ZB_AF_SET_ENDPOINT_DESC(                                         \
            ep_id,                                                       \
            ZB_AF_HA_PROFILE_ID,                                         \
            0,                                                           \
            NULL,                                                        \
            ZB_ZCL_ARRAY_SIZE(                                           \
                cluster_list,                                            \
                zb_zcl_cluster_desc_t),                                  \
            cluster_list,                                                \
            (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name)            \
      ZB_AF_FINISH_DECLARE_ENDPOINT_LIST

#define ZB_HA_DECLARE_SAMPLE_CTX(device_ctx, ep_name)                    \
  ZB_AF_DECLARE_DEVICE_CTX_NO_REP(device_ctx, ep_name,                   \
                     ZB_ZCL_ARRAY_SIZE(ep_name, zb_af_endpoint_desc_t))

/**
 *  @}
 */

#endif /* ZB_HA_SAMPLE_DEVICE_H */
