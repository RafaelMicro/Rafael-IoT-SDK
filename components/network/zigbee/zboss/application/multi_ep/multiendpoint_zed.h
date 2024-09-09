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
/* PURPOSE: Header file for Multiendpoint ZED application for HA
*/

#ifndef MULTI_EP_ZED_H
#define MULTI_EP_ZED_H 1

#include "zboss_api.h"

/* Bulb IEEE address */
#define MULTI_EP_ZED_IEEE_ADDRESS {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/* Default channel */
#define MULTI_EP_ZED_DEFAULT_APS_CHANNEL_MASK (1l<<21)
/* Used endpoint number */
#define HA_DIMMABLE_LIGHT_ENDPOINT 10

/* Handler for specific ZCL commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

/* Light Bulb initialization */
void multi_ep_zed_device_app_init(zb_uint8_t param);
/* Initialization of global device context */
void multi_ep_zed_app_ctx_init();
/* Initialization of HA attributes */
void multi_ep_zed_clusters_attr_init(zb_uint8_t param);
/* Device user application callback */
void test_device_cb(zb_uint8_t param);


/* Cluster Definition Macros */


#define MULTI_EP_ZED_DEVICE_VER 1

/* EP1 */
#define MULTI_EP_ZED_ENDPOINT_EP1 21
#define EP1_IN_CLUSTERS_NUM 3
#define EP1_OUT_CLUSTERS_NUM 0
#define EP1_REPORT_ATTR_COUNT (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT + \
                               ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT)
#define EP1_CVC_ATTR_COUNT 1

#define MULTI_EP_ZED_DECLARE_CLUSTER_LIST_EP1(                    \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  on_off_attr_list,                                               \
  level_control_attr_list)                                        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                   \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),         \
      (on_off_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                            \
      ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t),  \
      (level_control_attr_list),                                  \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                             \
  }

#define MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP1(ep_name, ep_id, in_clust_num, out_clust_num)   \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =               \
  {                                                                                         \
    ep_id,                                                                                  \
    ZB_AF_HA_PROFILE_ID,                                                                    \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                         \
    MULTI_EP_ZED_DEVICE_VER,                                                                \
    0,                                                                                      \
    in_clust_num,                                                                           \
    out_clust_num,                                                                          \
    {                                                                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                             \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL                                                       \
    }                                                                                       \
  }

#define MULTI_EP_ZED_DECLARE_EP1(ep_name, ep_id, cluster_list)                                     \
  MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP1(ep_name, ep_id, EP1_IN_CLUSTERS_NUM, EP1_OUT_CLUSTERS_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(ep1_reporting_info## device_ctx_name, EP1_REPORT_ATTR_COUNT); \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(ep1_cvc_alarm_info## device_ctx_name,  \
                                         EP1_CVC_ATTR_COUNT);                   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,              \
    0,                                                                          \
    NULL,                                                                       \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
    cluster_list,                                                               \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
    EP1_REPORT_ATTR_COUNT,                                                      \
    ep1_reporting_info## device_ctx_name,                                       \
    EP1_CVC_ATTR_COUNT,                                                         \
    ep1_cvc_alarm_info## device_ctx_name                                        \
  )

/*End EP1 */

/* EP2 */

#define MULTI_EP_ZED_ENDPOINT_EP2 22
#define EP2_IN_CLUSTERS_NUM 3
#define EP2_OUT_CLUSTERS_NUM 0
#define EP2_REPORT_ATTR_COUNT (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT + \
                               ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT)
#define EP2_CVC_ATTR_COUNT 1

#define MULTI_EP_ZED_DECLARE_CLUSTER_LIST_EP2(                    \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  on_off_attr_list,                                               \
  level_control_attr_list)                                        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                   \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),         \
      (on_off_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                            \
      ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t),  \
      (level_control_attr_list),                                  \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                             \
  }

#define MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP2(ep_name, ep_id, in_clust_num, out_clust_num)   \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =               \
  {                                                                                         \
    ep_id,                                                                                  \
    ZB_AF_HA_PROFILE_ID,                                                                    \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                         \
    MULTI_EP_ZED_DEVICE_VER,                                                                \
    0,                                                                                      \
    in_clust_num,                                                                           \
    out_clust_num,                                                                          \
    {                                                                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                             \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL                                                       \
    }                                                                                       \
  }

#define MULTI_EP_ZED_DECLARE_EP2(ep_name, ep_id, cluster_list)                                     \
  MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP2(ep_name, ep_id, EP2_IN_CLUSTERS_NUM, EP2_OUT_CLUSTERS_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(ep2_reporting_info## device_ctx_name, EP2_REPORT_ATTR_COUNT); \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(ep2_cvc_alarm_info## device_ctx_name,  \
                                         EP2_CVC_ATTR_COUNT);                   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,              \
    0,                                                                          \
    NULL,                                                                       \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
    cluster_list,                                                               \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
    EP2_REPORT_ATTR_COUNT,                                                      \
    ep2_reporting_info## device_ctx_name,                                       \
    EP2_CVC_ATTR_COUNT,                                                         \
    ep2_cvc_alarm_info## device_ctx_name                                        \
  )

/*End EP2 */

/* EP3 */

#define MULTI_EP_ZED_ENDPOINT_EP3 23
#define EP3_IN_CLUSTERS_NUM 4
#define EP3_OUT_CLUSTERS_NUM 0
#define EP3_REPORT_ATTR_COUNT (ZB_ZCL_TIME_REPORT_ATTR_COUNT + \
                               ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT)


#define MULTI_EP_ZED_DECLARE_CLUSTER_LIST_EP3(                    \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  identify_attr_list,                                             \
  time_attr_list,                                                 \
  on_off_attr_list)                                               \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                 \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),       \
      (identify_attr_list),                                       \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_TIME,                                     \
      ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),           \
      (time_attr_list),                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                   \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),           \
      (on_off_attr_list),                                           \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                              \
  }

#define MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP3(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =          \
  {                                                                                    \
    ep_id,                                                                             \
    ZB_AF_HA_PROFILE_ID,                                                               \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                    \
    MULTI_EP_ZED_DEVICE_VER,                                                           \
    0,                                                                                 \
    in_clust_num,                                                                      \
    out_clust_num,                                                                     \
    {                                                                                  \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                      \
      ZB_ZCL_CLUSTER_ID_TIME,                                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF                                                         \
    }                                                                                  \
  }

#define MULTI_EP_ZED_DECLARE_EP3(ep_name, ep_id, cluster_list)                                     \
  MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP3(ep_name, ep_id, EP3_IN_CLUSTERS_NUM, EP3_OUT_CLUSTERS_NUM);        \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(ep3_reporting_info## device_ctx_name, EP3_REPORT_ATTR_COUNT); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                      \
    0,                                                                                  \
    NULL,                                                                               \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                             \
    cluster_list,                                                                       \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
                          EP3_REPORT_ATTR_COUNT, ep3_reporting_info## device_ctx_name, 0, NULL)

/* EP4 */

#define ZB_ZCL_DECLARE_ALARMS_ATTRIB_LIST(attr_list, alarm_count) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list) \
  ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(ZB_ZCL_ATTR_ALARMS_ALARM_COUNT_ID, ZB_ZCL_ATTR_TYPE_U16, \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE, (alarm_count)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


#define MULTI_EP_ZED_ENDPOINT_EP4 24
#define EP4_IN_CLUSTERS_NUM 3
#define EP4_OUT_CLUSTERS_NUM 0
#define EP4_REPORT_ATTR_COUNT (ZB_ZCL_ALARMS_REPORT_ATTR_COUNT + \
                               ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT)

#define MULTI_EP_ZED_DECLARE_CLUSTER_LIST_EP4(                    \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  identify_attr_list,                                             \
  alarms_attr_list)                                               \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                 \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),       \
      (identify_attr_list),                                       \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_ALARMS,                                   \
      ZB_ZCL_ARRAY_SIZE(alarms_attr_list, zb_zcl_attr_t),         \
      (alarms_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                             \
  }

#define MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP4(ep_name, ep_id, in_clust_num, out_clust_num)\
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =            \
  {                                                                                      \
    ep_id,                                                                               \
    ZB_AF_HA_PROFILE_ID,                                                                 \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                      \
    MULTI_EP_ZED_DEVICE_VER,                                                             \
    0,                                                                                   \
    in_clust_num,                                                                        \
    out_clust_num,                                                                       \
    {                                                                                    \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                           \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                        \
      ZB_ZCL_CLUSTER_ID_ALARMS                                                           \
    }                                                                                    \
  }

#define MULTI_EP_ZED_DECLARE_EP4(ep_name, ep_id, cluster_list)                                     \
  MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP4(ep_name, ep_id, EP4_IN_CLUSTERS_NUM, EP4_OUT_CLUSTERS_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(ep4_reporting_info## device_ctx_name, EP4_REPORT_ATTR_COUNT); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                      \
    0,                                                                                  \
    NULL,                                                                               \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                             \
    cluster_list,                                                                       \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
                          EP4_REPORT_ATTR_COUNT, ep4_reporting_info## device_ctx_name, 0, NULL)



/* EP5 */
#define MULTI_EP_ZED_ENDPOINT_EP5 25
#define EP5_IN_CLUSTERS_NUM 3
#define EP5_OUT_CLUSTERS_NUM 0
#define EP5_REPORT_ATTR_COUNT (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT + \
                               ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT)
#define EP5_CVC_ATTR_COUNT 1

#define MULTI_EP_ZED_DECLARE_CLUSTER_LIST_EP5(                    \
  cluster_list_name,                                              \
  basic_attr_list,                                                \
  on_off_attr_list,                                               \
  level_control_attr_list)                                        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                     \
  {                                                               \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                    \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),          \
      (basic_attr_list),                                          \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                   \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),         \
      (on_off_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    ),                                                            \
    ZB_ZCL_CLUSTER_DESC(                                          \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                            \
      ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t),  \
      (level_control_attr_list),                                  \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                 \
      ZB_ZCL_MANUF_CODE_INVALID                                   \
    )                                                             \
  }

#define MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP5(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =               \
  {                                                                                         \
    ep_id,                                                                                  \
    ZB_AF_HA_PROFILE_ID,                                                                    \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                         \
    MULTI_EP_ZED_DEVICE_VER,                                                                \
    0,                                                                                      \
    in_clust_num,                                                                           \
    out_clust_num,                                                                          \
    {                                                                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                             \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL                                                       \
    }                                                                                       \
  }

#define MULTI_EP_ZED_DECLARE_EP5(ep_name, ep_id, cluster_list)                                     \
  MULTI_EP_ZED_DECLARE_SIMPLE_DESC_EP5(ep_name, ep_id, EP5_IN_CLUSTERS_NUM, EP5_OUT_CLUSTERS_NUM); \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(ep5_reporting_info## device_ctx_name, EP5_REPORT_ATTR_COUNT); \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(ep5_cvc_alarm_info## device_ctx_name,  \
                                         EP5_CVC_ATTR_COUNT);                   \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,              \
    0,                                                                          \
    NULL,                                                                       \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
    cluster_list,                                                               \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
    EP1_REPORT_ATTR_COUNT,                                                      \
    ep5_reporting_info## device_ctx_name,                                       \
    EP5_CVC_ATTR_COUNT,                                                         \
    ep5_cvc_alarm_info## device_ctx_name                                        \
  )

/*End EP5 */

/*
  @brief Declare application's device context for Dimmable Light device
*/
#define ZBOSS_DECLARE_DEVICE_CTX_5_EP(device_ctx_name, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name) \
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_list_##device_ctx_name)          \
    &ep1_name,                                                          \
    &ep2_name,                                                          \
    &ep3_name,                                                          \
    &ep4_name,                                                          \
    &ep5_name, \
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST;                                   \
  ZBOSS_DECLARE_DEVICE_CTX(device_ctx_name, ep_list_##device_ctx_name,  \
                           (ZB_ZCL_ARRAY_SIZE(ep_list_##device_ctx_name, zb_af_endpoint_desc_t*)))


#define MULTI_EP_ZED_DECLARE_CTX(device_ctx, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name)               \
  ZBOSS_DECLARE_DEVICE_CTX_5_EP(device_ctx, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name)

/*! @} */



#endif /* MULTI_EP_ZED_H */
