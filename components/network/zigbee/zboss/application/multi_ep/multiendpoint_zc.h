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
/* PURPOSE: Header file for Simple GW
*/

#ifndef MULTI_EP_ZC_H
#define MULTI_EP_ZC_H 1

#include "zboss_api.h"

#define MULTI_EP_ZC_IEEE_ADDR {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}
/* Default channel */
#define MULTI_EP_ZC_CHANNEL_MASK (1l<<21)

/* Device settings definitions */
#define MULTI_EP_ZC_INVALID_DEV_INDEX 0xff
#define MULTI_EP_ZC_INVALID_EP_INDEX  0xff
#define MULTI_EP_ZC_DEV_NUMBER 5
#define MULTI_EP_ZC_MAX_EP_PER_DEV 5
#define MULTI_EP_ZC_MAX_CLUSTERS_PER_EP 20
#define MULTI_EP_ZC_MAX_ATTRS_PER_CLUSTER 20
#define MULTI_EP_ZC_TOGGLE_ITER_TIMEOUT (5*ZB_TIME_ONE_SECOND)
#define MULTI_EP_ZC_RANDOM_TIMEOUT_VAL (15)
#define MULTI_EP_ZC_COMMUNICATION_PROBLEMS_TIMEOUT (30) /* 300 sec */
#define MULTI_EP_ZC_TOGGLE_TIMEOUT (ZB_RANDOM_VALUE(MULTI_EP_ZC_RANDOM_TIMEOUT_VAL)) /* * ZB_TIME_ONE_SECOND */

/* Reporting settings */
#define MULTI_EP_ZC_REPORTING_MIN_INTERVAL 30
#define MULTI_EP_ZC_REPORTING_MAX_INTERVAL(dev_idx) ((90 * ((dev_idx) + 1)))

/* Device states enumeration */
enum multi_ep_zc_device_state_e
{
  NO_DEVICE,
  MATCH_DESC_DISCOVERY,
  SEARCH_ACTIVE_EP,
  IEEE_ADDR_DISCOVERY,
  CONFIGURE_BINDING,
  CONFIGURE_REPORTING,
  COMPLETED,
  COMPLETED_NO_TOGGLE,
#if defined IAS_CIE_ENABLED
  WRITE_CIE_ADDR,
#endif
};

/* Cluster state */
enum multi_ep_zc_cluster_state_e
{
  NO_CLUSTER_INFO,
  REQUESTED_CLUSTER_INFO,
  KNOWN_CLUSTER,
};


/* Attrs state */
enum multi_ep_zc_attrs_state_e
{
  NO_ATTRS_INFO,
  REQUESTED_ATTRS_INFO,
  KNOWN_ATTRS,
};

typedef ZB_PACKED_PRE struct multi_ep_zc_device_cluster_attr_s {
  zb_uint16_t attr_id;
  zb_uint8_t attr_type;

} multi_ep_zc_device_cluster_attr_t;

/* Attributes of a Cluster of a joined device */
typedef ZB_PACKED_PRE struct multi_ep_zc_device_cluster_s {
  zb_uint16_t cluster_id;
  zb_uint8_t attrs_state;
  zb_uint8_t num_attrs;
  multi_ep_zc_device_cluster_attr_t attr[MULTI_EP_ZC_MAX_ATTRS_PER_CLUSTER];

} multi_ep_zc_device_cluster_t;

/* Description of a EP of a joined device */
typedef ZB_PACKED_PRE struct multi_ep_zc_device_ep_s {
  zb_uint16_t ep_id;
  zb_uint16_t profile_id;
  zb_uint8_t clusters_state;
  zb_uint8_t num_in_clusters;
  zb_uint8_t num_out_clusters;
  multi_ep_zc_device_cluster_t ep_cluster[MULTI_EP_ZC_MAX_CLUSTERS_PER_EP];

} multi_ep_zc_device_ep_t;


/* Joined devices information context */
typedef ZB_PACKED_PRE struct multi_ep_zc_device_params_s
{
  zb_uint8_t dev_state;
  zb_uint8_t num_ep;
  multi_ep_zc_device_ep_t endpoints[MULTI_EP_ZC_MAX_EP_PER_DEV];
  zb_uint16_t short_addr;
  zb_ieee_addr_t ieee_addr;
  zb_uint8_t pending_toggle;
} multi_ep_zc_device_params_t;

/* Global device context */
typedef ZB_PACKED_PRE struct multi_ep_zc_device_ctx_s
{
  multi_ep_zc_device_params_t devices[MULTI_EP_ZC_DEV_NUMBER];
} ZB_PACKED_STRUCT multi_ep_zc_device_ctx_t;

/* Startup sequence routines */
zb_uint16_t multi_ep_zc_associate_cb(zb_uint16_t short_addr);
void multi_ep_zc_dev_annce_cb(zb_uint16_t short_addr);
void multi_ep_zc_leave_indication(zb_ieee_addr_t dev_addr);

void device_ieee_addr_req(zb_uint8_t param, zb_uint16_t dev_idx);
void device_ieee_addr_req_cb(zb_uint8_t param);

/* Devices management routines */
zb_uint8_t multi_ep_zc_get_dev_index_by_state(zb_uint8_t dev_state);
zb_uint8_t multi_ep_zc_get_dev_index_by_short_addr(zb_uint16_t short_addr);
zb_uint8_t multi_ep_zc_get_dev_index_by_ieee_addr(zb_ieee_addr_t ieee_addr);
zb_uint8_t multi_ep_zc_get_ep_idx_by_short_addr_and_ep_id(zb_uint16_t short_addr, zb_uint8_t ep_id);

void multi_ep_zc_remove_and_rejoin_device_delayed(zb_uint8_t idx);
void multi_ep_zc_remove_device_delayed(zb_uint8_t idx);

/* Handler for specific zcl commands */
zb_uint8_t zcl_specific_cluster_cmd_handler(zb_uint8_t param);

/* Macros for cluster definition */


#define ZB_HA_DEVICE_VER_MULTI_EP_ZC 0  /*!< MultiEP ZC device version */
#define ZB_HA_MULTI_EP_ZC_DEVICE_ID 0

#define ZB_HA_MULTI_EP_ZC_CLUSTER_NUM                                      \
  (ZB_HA_MULTI_EP_ZC_IN_CLUSTER_NUM + ZB_HA_MULTI_EP_ZC_OUT_CLUSTER_NUM)

/* EP1: Basic, On/Off, Level Control */
#define MULTI_EP_ZC_ENDPOINT_EP1 21
#define EP1_IN_CLUSTERS_NUM 0
#define EP1_OUT_CLUSTERS_NUM 3

#define ZB_HA_DECLARE_MULTI_EP_ZC_CLUSTER_LIST_EP1(                         \
      cluster_list_name)                                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
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
        )                                                                   \
    }


#define ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP1(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                   \
  {                                                                                             \
    ep_id,                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                        \
    ZB_HA_MULTI_EP_ZC_DEVICE_ID,                                                                \
    ZB_HA_DEVICE_VER_MULTI_EP_ZC,                                                               \
    0,                                                                                          \
    in_clust_num,                                                                               \
    out_clust_num,                                                                              \
    {                                                                                           \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                  \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                 \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                          \
    }                                                                                           \
  }


#define ZB_HA_DECLARE_MULTI_EP_ZC_EP1(ep_name, ep_id, cluster_list)    \
  ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP1(                          \
      ep_name,                                                         \
      ep_id,                                                           \
      EP1_IN_CLUSTERS_NUM,                                             \
      EP1_OUT_CLUSTERS_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                 \
                              ep_id,                                   \
      ZB_AF_HA_PROFILE_ID,                                             \
      0,                                                               \
      NULL,                                                            \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
      cluster_list,                                                    \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
      0, NULL, /* No reporting ctx */                                  \
      0, NULL)

/* EP2: Basic, On/Off, Level Control */
#define MULTI_EP_ZC_ENDPOINT_EP2 22
#define EP2_IN_CLUSTERS_NUM 0
#define EP2_OUT_CLUSTERS_NUM 3


#define ZB_HA_DECLARE_MULTI_EP_ZC_CLUSTER_LIST_EP2(                         \
      cluster_list_name)                                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
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
        )                                                                   \
    }


#define ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP2(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                   \
  {                                                                                             \
    ep_id,                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                        \
    ZB_HA_MULTI_EP_ZC_DEVICE_ID,                                                                \
    ZB_HA_DEVICE_VER_MULTI_EP_ZC,                                                               \
    0,                                                                                          \
    in_clust_num,                                                                               \
    out_clust_num,                                                                              \
    {                                                                                           \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                  \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                 \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                          \
    }                                                                                           \
  }


#define ZB_HA_DECLARE_MULTI_EP_ZC_EP2(ep_name, ep_id, cluster_list)    \
  ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP2(                          \
      ep_name,                                                         \
      ep_id,                                                           \
      EP2_IN_CLUSTERS_NUM,                                             \
      EP2_OUT_CLUSTERS_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                 \
                              ep_id,                                   \
      ZB_AF_HA_PROFILE_ID,                                             \
      0,                                                               \
      NULL,                                                            \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
      cluster_list,                                                    \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
      0, NULL, /* No reporting ctx */                                  \
      0, NULL)


/* EP3: Basic, Identify, Time  */
#define MULTI_EP_ZC_ENDPOINT_EP3 23
#define EP3_IN_CLUSTERS_NUM 0
#define EP3_OUT_CLUSTERS_NUM 4


#define ZB_HA_DECLARE_MULTI_EP_ZC_CLUSTER_LIST_EP3(                         \
      cluster_list_name)                                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
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
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_TIME,                                           \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        )                                                                   \
    }


#define ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP3(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                   \
  {                                                                                             \
    ep_id,                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                        \
    ZB_HA_MULTI_EP_ZC_DEVICE_ID,                                                                \
    ZB_HA_DEVICE_VER_MULTI_EP_ZC,                                                               \
    0,                                                                                          \
    in_clust_num,                                                                               \
    out_clust_num,                                                                              \
    {                                                                                           \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                  \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                               \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                   \
      ZB_ZCL_CLUSTER_ID_ON_OFF                                                                  \
    }                                                                                           \
  }


#define ZB_HA_DECLARE_MULTI_EP_ZC_EP3(ep_name, ep_id, cluster_list)    \
  ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP3(                          \
      ep_name,                                                         \
      ep_id,                                                           \
      EP3_IN_CLUSTERS_NUM,                                             \
      EP3_OUT_CLUSTERS_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                 \
                              ep_id,                                   \
      ZB_AF_HA_PROFILE_ID,                                             \
      0,                                                               \
      NULL,                                                            \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
      cluster_list,                                                    \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
      0, NULL, /* No reporting ctx */                                  \
      0, NULL)


/* EP4 */
#define MULTI_EP_ZC_ENDPOINT_EP4 24
#define EP4_IN_CLUSTERS_NUM 0
#define EP4_OUT_CLUSTERS_NUM 3

#define ZB_HA_DECLARE_MULTI_EP_ZC_CLUSTER_LIST_EP4(                         \
      cluster_list_name)                                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
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
        ),                                                                  \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_ALARMS,                                         \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
          ZB_ZCL_MANUF_CODE_INVALID                                         \
        )                                                                   \
    }


#define ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP4(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                   \
  {                                                                                             \
    ep_id,                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                        \
    ZB_HA_MULTI_EP_ZC_DEVICE_ID,                                                                \
    ZB_HA_DEVICE_VER_MULTI_EP_ZC,                                                               \
    0,                                                                                          \
    in_clust_num,                                                                               \
    out_clust_num,                                                                              \
    {                                                                                           \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                  \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                               \
      ZB_ZCL_CLUSTER_ID_ALARMS                                                                  \
    }                                                                                           \
  }


#define ZB_HA_DECLARE_MULTI_EP_ZC_EP4(ep_name, ep_id, cluster_list)    \
  ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP4(                          \
      ep_name,                                                         \
      ep_id,                                                           \
      EP4_IN_CLUSTERS_NUM,                                             \
      EP4_OUT_CLUSTERS_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                 \
                              ep_id,                                   \
      ZB_AF_HA_PROFILE_ID,                                             \
      0,                                                               \
      NULL,                                                            \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
      cluster_list,                                                    \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
      0, NULL, /* No reporting ctx */                                  \
      0, NULL)


/* EP5: Basic, On/Off, Level Control */
#define MULTI_EP_ZC_ENDPOINT_EP5 25
#define EP5_IN_CLUSTERS_NUM 0
#define EP5_OUT_CLUSTERS_NUM 3

#define ZB_HA_DECLARE_MULTI_EP_ZC_CLUSTER_LIST_EP5(                         \
      cluster_list_name)                                                    \
      zb_zcl_cluster_desc_t cluster_list_name[] =                           \
      {                                                                     \
        ZB_ZCL_CLUSTER_DESC(                                                \
          ZB_ZCL_CLUSTER_ID_BASIC,                                          \
          0,                                                                \
          NULL,                                                             \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
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
        )                                                                   \
    }


#define ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP5(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                   \
  {                                                                                             \
    ep_id,                                                                                      \
    ZB_AF_HA_PROFILE_ID,                                                                        \
    ZB_HA_MULTI_EP_ZC_DEVICE_ID,                                                                \
    ZB_HA_DEVICE_VER_MULTI_EP_ZC,                                                               \
    0,                                                                                          \
    in_clust_num,                                                                               \
    out_clust_num,                                                                              \
    {                                                                                           \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                                  \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                                 \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                          \
    }                                                                                           \
  }


#define ZB_HA_DECLARE_MULTI_EP_ZC_EP5(ep_name, ep_id, cluster_list)    \
  ZB_ZCL_DECLARE_MULTI_EP_ZC_SIMPLE_DESC_EP5(                          \
      ep_name,                                                         \
      ep_id,                                                           \
      EP1_IN_CLUSTERS_NUM,                                             \
      EP1_OUT_CLUSTERS_NUM);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                 \
                              ep_id,                                   \
      ZB_AF_HA_PROFILE_ID,                                             \
      0,                                                               \
      NULL,                                                            \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
      cluster_list,                                                    \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
      0, NULL, /* No reporting ctx */                                  \
      0, NULL)


#define ZBOSS_DECLARE_DEVICE_CTX_5_EP(device_ctx_name, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name) \
  ZB_AF_START_DECLARE_ENDPOINT_LIST(ep_list_##device_ctx_name)                                           \
    &ep1_name,                                                                                           \
    &ep2_name,                                                                                           \
    &ep3_name,                                                                                           \
    &ep4_name,                                                                                           \
    &ep5_name,                                                                                           \
  ZB_AF_FINISH_DECLARE_ENDPOINT_LIST;                                                                    \
  ZBOSS_DECLARE_DEVICE_CTX(device_ctx_name, ep_list_##device_ctx_name,                                   \
                           (ZB_ZCL_ARRAY_SIZE(ep_list_##device_ctx_name, zb_af_endpoint_desc_t*)))       \


#define ZB_HA_DECLARE_MULTI_EP_ZC_CTX(device_ctx, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name)      \
  ZBOSS_DECLARE_DEVICE_CTX_5_EP(device_ctx, ep1_name, ep2_name, ep3_name, ep4_name, ep5_name)


/* Macros used in test loop */

#define TEST_WRITE_ATTR(param, dev_idx, dst_ep, src_ep, cluster_id, attr_id, attr_type, attr_value)    \
{                                                                                                      \
  zb_uint8_t *cmd_ptr;                                                                                 \
  ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ(param, cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);                  \
  ZB_ZCL_GENERAL_ADD_VALUE_WRITE_ATTR_REQ(cmd_ptr, attr_id, attr_type, attr_value);                    \
  ZB_ZCL_GENERAL_SEND_WRITE_ATTR_REQ(param, cmd_ptr, g_device_ctx.devices[dev_idx].short_addr,         \
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                                                                  \
    dst_ep, src_ep,                                                                                    \
    get_profile_id_by_endpoint(src_ep),                                                                \
    cluster_id,                                                                                        \
    NULL);                                                                                             \
}

#define TEST_READ_ATTR(param, dev_idx, dst_ep, src_ep, cluster_id, attr_id)                            \
{                                                                                                      \
  zb_uint8_t *cmd_ptr;                                                                                 \
  ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ((param), cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);                 \
  ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, attr_id);                                               \
  ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ((param), cmd_ptr, g_device_ctx.devices[dev_idx].short_addr,        \
                                ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                                      \
                                dst_ep, src_ep, get_profile_id_by_endpoint(src_ep), cluster_id, NULL); \
}

#define  TEST_CONFIGURE_REPORTING_SRV(param, dev_idx, dst_ep, src_ep, cluster_id, attr_id, attr_type,  \
                                      min_interval, max_interval, report_value_change)                 \
{                                                                                                      \
  zb_uint8_t *cmd_ptr;                                                                                 \
  ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_SRV_REQ(param,                                               \
    cmd_ptr,                                                                                           \
    ZB_ZCL_ENABLE_DEFAULT_RESPONSE);                                                                   \
    ZB_ZCL_GENERAL_ADD_SEND_REPORT_CONFIGURE_REPORTING_REQ(                                            \
      cmd_ptr, attr_id, attr_type, min_interval,                                                       \
      max_interval, report_value_change);                                                              \
    ZB_ZCL_GENERAL_SEND_CONFIGURE_REPORTING_REQ(param,                                                 \
    cmd_ptr,                                                                                           \
    g_device_ctx.devices[dev_idx].short_addr,                                                          \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT, dst_ep, src_ep,                                                \
      get_profile_id_by_endpoint(src_ep), cluster_id, NULL);                                           \
}


#define TEST_CONFIGURE_BINDING(param, dev_idx, dst_ep, src_ep, bind_cluster_id) \
{                                                                               \
  zb_zdo_bind_req_param_t *req;                                                 \
  req = ZB_BUF_GET_PARAM(param, zb_zdo_bind_req_param_t);                       \
  ZB_MEMCPY(&req->src_address,                                                  \
    g_device_ctx.devices[dev_idx].ieee_addr,                                    \
    sizeof(zb_ieee_addr_t));                                                    \
  req->src_endp = src_ep;                                                       \
  req->cluster_id = bind_cluster_id;                                            \
  req->dst_addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;                        \
  zb_get_long_address(req->dst_address.addr_long);                              \
  req->dst_endp = dst_ep;                                                       \
  req->req_dst_addr = g_device_ctx.devices[dev_idx].short_addr;                 \
  zb_zdo_bind_req(param, NULL);                                                 \
};

#endif /* MULTI_EP_ZC_H */
