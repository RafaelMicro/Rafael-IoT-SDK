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
/* PURPOSE: Common samples header file.
*/

#ifndef SE_SAMPLE_COMMON_H
#define SE_SAMPLE_COMMON_H 1

#ifndef ZB_PRODUCTION_CONFIG
#define ENABLE_RUNTIME_APP_CONFIG
/* #define ENABLE_PRECOMMISSIONED_REJOIN */
#endif

#define SE_CRYPTOSUITE_1
//#define SE_CRYPTOSUITE_2

/*** Production config data ***/
typedef ZB_PACKED_PRE struct se_app_production_config_t
{
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
  zb_uint16_t manuf_code;
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
}
ZB_PACKED_STRUCT se_app_production_config_t;

enum se_app_prod_cfg_version_e
{
  SE_APP_PROD_CFG_VERISON_1_0 = 1,
};

#define ZB_SE_CBKE_ZC_DEV_IN_CLUSTER_NUM 3
#define ZB_SE_CBKE_ZC_DEV_OUT_CLUSTER_NUM 2

#define ZB_SE_DECLARE_CBKE_ZC_DEV_CLUSTER_LIST(cluster_list_name,                     \
                                                                basic_attr_list,      \
                                                                time_attr_list,       \
                                                                kec_attr_list)        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                                         \
  {                                                                                   \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                              \
      (basic_attr_list),                                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_TIME,                                                         \
      ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),                               \
      (time_attr_list),                                                               \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                                \
      (kec_attr_list),                                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                                \
      (kec_attr_list),                                                                \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      ),                                                                              \
    ZB_ZCL_CLUSTER_DESC(                                                              \
      ZB_ZCL_CLUSTER_ID_TIME,                                                         \
      0, NULL,                                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                                       \
      )                                                                               \
  }

#define ZB_SE_DEVICE_VER_CBKE_ZC 0

#define ZB_SE_CBKE_ZC_DEVICE_ID 0x0501

#define ZB_ZCL_DECLARE_CBKE_ZC_DEV_SIMPLE_DESC(ep_name,                        \
                                                ep_id,                         \
                                                in_clust_num,                  \
                                                out_clust_num)                 \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                            \
    ep_id,                                                                     \
    ZB_AF_SE_PROFILE_ID,                                                       \
    ZB_SE_CBKE_ZC_DEVICE_ID,                                                   \
    ZB_SE_DEVICE_VER_CBKE_ZC,                                                  \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_TIME,                                                  \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                     \
      ZB_ZCL_CLUSTER_ID_TIME,                                                  \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT                                      \
    }                                                                          \
  }

#define ZB_SE_DECLARE_CBKE_ZC_DEV_EP(ep_name, ep_id, cluster_list)                            \
  ZB_ZCL_DECLARE_CBKE_ZC_DEV_SIMPLE_DESC(ep_name,                                             \
                                          ep_id,                                              \
                                          ZB_SE_CBKE_ZC_DEV_IN_CLUSTER_NUM,                   \
                                          ZB_SE_CBKE_ZC_DEV_OUT_CLUSTER_NUM);                 \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                        \
                              ep_id,                                                          \
                              ZB_AF_SE_PROFILE_ID,                                            \
                              0,                                                              \
                              NULL,                                                           \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),         \
                              cluster_list,                                                   \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,               \
                              0, NULL,                                                        \
                              0, NULL)

#define ZB_SE_DECLARE_CBKE_ZC_DEV_CTX(device_ctx_name, ep_name)     \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx_name, ep_name)

#define SE_APP_PROD_CFG_CURRENT_VERSION SE_APP_PROD_CFG_VERISON_1_0


#define ZB_SE_CBKE_ZR_DEV_IN_CLUSTER_NUM  2

#define ZB_SE_CBKE_ZR_DEV_OUT_CLUSTER_NUM 2

#define ZB_SE_DECLARE_CBKE_ZR_DEV_CLUSTER_LIST(cluster_list_name,     \
                                                   basic_attr_list,   \
                                                   kec_attr_list)     \
  zb_zcl_cluster_desc_t cluster_list_name[] =                         \
  {                                                                   \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_BASIC,                                        \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),              \
      (basic_attr_list),                                              \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                \
      (kec_attr_list),                                                \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                            \
      ZB_ZCL_ARRAY_SIZE(kec_attr_list, zb_zcl_attr_t),                \
      (kec_attr_list),                                                \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      ),                                                              \
    ZB_ZCL_CLUSTER_DESC(                                              \
      ZB_ZCL_CLUSTER_ID_TIME,                                         \
      0, NULL,                                                        \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                     \
      ZB_ZCL_MANUF_CODE_INVALID                                       \
      )                                                               \
  }

#define ZB_SE_CBKE_ZR_DEVICE_ID 0x0502

#define ZB_SE_DEVICE_VER_CBKE_ZR 0

#define ZB_ZCL_DECLARE_CBKE_ZR_DEV_SIMPLE_DESC(ep_name,                        \
                                                   ep_id,                      \
                                                   in_clust_num,               \
                                                   out_clust_num)              \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
  {                                                                            \
    ep_id,                                                                     \
    ZB_AF_SE_PROFILE_ID,                                                       \
    ZB_SE_CBKE_ZR_DEVICE_ID,                                                   \
    ZB_SE_DEVICE_VER_CBKE_ZR,                                                  \
    0,                                                                         \
    in_clust_num,                                                              \
    out_clust_num,                                                             \
    {                                                                          \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                     \
      ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT,                                     \
      ZB_ZCL_CLUSTER_ID_TIME,                                                  \
    }                                                                          \
  }

#define ZB_SE_DECLARE_CBKE_ZR_DEV_EP(ep_name, ep_id, cluster_list)                      \
  ZB_ZCL_DECLARE_CBKE_ZR_DEV_SIMPLE_DESC(ep_name,                                       \
                                             ep_id,                                     \
                                             ZB_SE_CBKE_ZR_DEV_IN_CLUSTER_NUM,          \
                                             ZB_SE_CBKE_ZR_DEV_OUT_CLUSTER_NUM);        \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                  \
                              ep_id,                                                    \
                          ZB_AF_SE_PROFILE_ID,                                          \
                          0,                                                            \
                          NULL,                                                         \
                          ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),       \
                          cluster_list,                                                 \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
                          0, NULL,                                                      \
                          0, NULL)

#define ZB_SE_DECLARE_CBKE_ZR_DEV_CTX(device_ctx_name, ep_name)     \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx_name, ep_name)

#endif /* SE_SAMPLE_COMMON_H */
