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
/* PURPOSE: SE KEC custom device definitions
*/

/**
 *  @defgroup ZB_SE_DEFINE_DEVICE_KEC_CUSTOM Costom device to serve key establishment in SE profile
 *  @ingroup ZB_SE_DEVICES
 *  @addtogroup ZB_SE_DEFINE_DEVICE_KEC_CUSTOM_H
 *  @{
    @details
        - @ref ZB_ZCL_KEY_ESTABLISHMENT_CLUSTER \n

*/

/******************* Specific definitions for kec server device **************************/

#define ZB_SE_DEVICE_KEC_VER 1

#define ZB_SE_KEC_IN_CLUSTER_NUM 1
#define ZB_SE_KEC_OUT_CLUSTER_NUM 0

#define ZB_SE_KEC_CLUSTER_NUM (ZB_SE_KEC_IN_CLUSTER_NUM + ZB_SE_KEC_OUT_CLUSTER_NUM)

/*! Number of attribute for reporting on KEC device */
#define ZB_SE_KEC_REPORT_ATTR_COUNT         1

#define ZB_SE_KEC_CVC_ATTR_COUNT 1

/*!
  @brief Declare cluster list for KEC device
  @param cluster_list_name - cluster list variable name
  @param kec_attr_list - attribute list for Key Establishment Cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param groups_attr_list - attribute list for Groups cluster
  @param scenes_attr_list - attribute list for Scenes cluster
  @param on_off_attr_list - attribute list for On/Off cluster
  @param level_control_attr_list - attribute list for Level Control cluster
  kec_attr_list)                                                 \
 */
#define ZB_SE_DECLARE_KEC_CLUSTER_LIST(                          \
  cluster_list_name)                                             \
  zb_zcl_cluster_desc_t cluster_list_name[] =                    \
  {                                                              \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                       \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),           \
      (kec_attr_list),                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    )                                                            \
  }

/*!
  @brief Declare simple descriptor for kec device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_SE_KEC_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                  \
  {                                                                                            \
    ep_id,                                                                                     \
    ZB_AF_SE_PROFILE_ID,                                                                       \
    ZB_SE_TEST_DEVICE_ID,                                                                      \
    ZB_SE_DEVICE_KEC_VER,                                                                      \
    0,                                                                                         \
    in_clust_num,                                                                              \
    out_clust_num,                                                                             \
    {                                                                                          \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT                                                      \
    }                                                                                          \
  }

/*!
  @brief Declare endpoint for kec device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */
#define ZB_SE_DECLARE_KEC_EP(ep_name, ep_id, cluster_list)                \
  ZB_ZCL_DECLARE_SE_KEC_SIMPLE_DESC(ep_name, ep_id,                       \
    ZB_SE_KEC_IN_CLUSTER_NUM, ZB_SE_KEC_OUT_CLUSTER_NUM);                 \
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_name)                              \
  ZB_AF_SET_ENDPOINT_DESC(ep_id, ZB_AF_SE_PROFILE_ID, 0, NULL,            \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list, \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name)                     \
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST


/*
  @brief Declare application's device context for Dimmable Light device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/

#define ZB_SE_DECLARE_KEC_CTX(device_ctx, ep_name)                               \
  ZBOSS_DECLARE_DEVICE_CTX(device_ctx, ep_name,                                  \
                           ZB_ZCL_ARRAY_SIZE(ep_name, zb_af_endpoint_desc_t),    \
                           0, NULL, /*No reporting ctx*/                         \
                           0, NULL) /*No CVC ctx*/


/*! @} */

