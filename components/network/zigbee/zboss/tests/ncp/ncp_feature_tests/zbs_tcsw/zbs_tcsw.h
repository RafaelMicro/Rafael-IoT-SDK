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

#ifndef ZBS_TCSW_H
#define ZBS_TCSW_H 1

#define SE_CRYPTOSUITE_2

#ifdef SE_CRYPTOSUITE_2
/** Public key of CERTICOM's Test Certifications Authority */

zb_uint8_t ca_public_key_cs2[37] = {0x02,0x07,0xa4,0x45,0x02,0x2d,0x9f,0x39,
                                    0xf4,0x9b,0xdc,0x38,0x38,0x00,0x26,0xa2,
                                    0x7a,0x9e,0x0a,0x17,0x99,0x31,0x3a,0xb2,
                                    0x8c,0x5c,0x1a,0x1c,0x6b,0x60,0x51,0x54,
                                    0xdb,0x1d,0xff,0x67,0x52};

/* ---------------------------- ESI device ----------------------------------- */

#define ESI_DEV_SWAPPED_ADDR   {0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a}

zb_uint8_t zbs_tcsw_zc_certificate_cs2[74] = {0x00,0x26,0x22,0xA5,0x05,0xE8,0x93,0x8F,
                                              0x27,0x0D,0x08,0x11,0x12,0x13,0x14,0x15,
                                              0x16,0x17,0x18,0x00,0x52,0x92,0xA3,0x5B,
                                              0xFF,0xFF,0xFF,0xFF,0x0A,0x0B,0x0C,0x0D,
                                              0x0E,0x0F,0x10,0x11,0x88,0x03,0x03,0xB4,
                                              0xE9,0xDC,0x54,0x3A,0x64,0x33,0x3C,0x98,
                                              0x23,0x08,0x02,0x2B,0x54,0xE6,0x7E,0x2F,
                                              0x15,0xF5,0x32,0x55,0x1B,0x0A,0x11,0xE2,
                                              0xE2,0xC1,0xC1,0xD3,0x09,0x7A,0x43,0x24,
                                              0xE7,0xED};

zb_uint8_t zbs_tcsw_zc_private_key_cs2[36] = {0x01,0x51,0xCD,0x0D,0xBC,0xB8,0x04,0x74,
                                              0xBF,0x7A,0xC9,0xFE,0xEB,0xE3,0x9C,0x7A,
                                              0x32,0xA6,0x35,0x18,0x93,0x8F,0xCA,0x97,
                                              0x54,0xAA,0xE1,0x32,0xBC,0x9C,0x73,0xBE,
                                              0x94,0xA7,0xE1,0xBE};

#endif

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

#endif /* ZBS_TCSW_H */
