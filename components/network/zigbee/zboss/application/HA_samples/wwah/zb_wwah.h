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
/* PURPOSE: ZB WWAH device
*/

#ifndef ZB_WWAH_H
#define ZB_WWAH_H 1
#include "se/zb_se_keep_alive.h"

#define DEV_CHANNEL_MASK (1L << 11)

char ed1_installcode[]= "966b9f3ef98ae605 9708";
char ed2_installcode[]= "966b9f3ef98ae605 9708";

#define ZB_WWAH_DEVICE_ID 0x3131
#define ZB_WWAH_DEVICE_VER 1

#define ZB_WWAH_ZC_HA_EP_IN_CLUSTER_NUM 4
#define ZB_WWAH_ZC_HA_EP_OUT_CLUSTER_NUM 3

#define ZC_HA_EP 4

zb_ieee_addr_t g_ed_addr = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
zb_ieee_addr_t g_ed2_addr = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
zb_ieee_addr_t g_zc_addr = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

#define ZB_WWAH_REPORT_ATTR_COUNT (  \
  ZB_ZCL_WWAH_REPORT_ATTR_COUNT +    \
  ZB_ZCL_TIME_REPORT_ATTR_COUNT)

/*!
  @brief Declare HA cluster list for WWAH Device
  @param cluster_list_name - cluster list variable name
  @param identify_attr_list - attribute list for Identify cluster
  @param time_attr_list - attribute list for Time cluster
  @param keep_alive_attr_list - attribute list for Keep-Alive cluster
  @param pool_control_attr_list - attribute list for Poll Control cluster
  @param ota_upgrade_attr_list - attribute list for OTA Upgrade cluster
*/
#define ZB_HA_DECLARE_WWAH_CLUSTER_LIST_ZC(                   \
  cluster_list_name,                                          \
  identify_attr_list,                                         \
  time_attr_list,                                             \
  keep_alive_attr_list,                                       \
  poll_control_attr_list,                                     \
  ota_upgrade_attr_list,                                      \
  wwah_client_attr_list)                                      \
  zb_zcl_cluster_desc_t cluster_list_name[] =                 \
{                                                             \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_BASIC,                                  \
    0,                                                        \
    NULL,                                                     \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_WWAH,                                   \
    ZB_ZCL_ARRAY_SIZE(wwah_client_attr_list, zb_zcl_attr_t),  \
    (wwah_client_attr_list),                                  \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    WWAH_MANUFACTURER_CODE                                    \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                               \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),     \
    (identify_attr_list),                                     \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_TIME,                                   \
    ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),         \
    (time_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                             \
    ZB_ZCL_ARRAY_SIZE(keep_alive_attr_list, zb_zcl_attr_t),   \
    (keep_alive_attr_list),                                   \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                           \
    ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t), \
    (poll_control_attr_list),                                 \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
    ZB_ZCL_MANUF_CODE_INVALID                                 \
  ),                                                          \
  ZB_ZCL_CLUSTER_DESC(                                        \
    ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                            \
    ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),  \
    (ota_upgrade_attr_list),                                  \
     ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
     ZB_ZCL_MANUF_CODE_INVALID                                \
  )                                                           \
}

/*!
  @brief Declare HA simple descriptor for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZC(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                      \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =               \
  {                                                                                         \
    ep_id,                                                                                  \
    ZB_AF_HA_PROFILE_ID,                                                                    \
    ZB_WWAH_DEVICE_ID,                                                                      \
    ZB_WWAH_DEVICE_VER,                                                                     \
    0,                                                                                      \
    in_clust_num,                                                                           \
    out_clust_num,                                                                          \
    {                                                                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                           \
      ZB_ZCL_CLUSTER_ID_TIME,                                                               \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                                                         \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                                                        \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                              \
      ZB_ZCL_CLUSTER_ID_WWAH,                                                               \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL                                                        \
    }                                                                                       \
  }

/*!
  @brief Declare HA endpoint for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_WWAH_EP_ZC(ep_name, ep_id, cluster_list)             \
  ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZC(ep_name, ep_id,                    \
    ZB_WWAH_ZC_HA_EP_IN_CLUSTER_NUM, ZB_WWAH_ZC_HA_EP_OUT_CLUSTER_NUM);    \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,         \
    0,                                                                     \
    NULL,                                                                  \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,  \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                      \
    0, NULL,                                                               \
    0, NULL)

#define ZB_WWAH_ZED_HA_EP_IN_CLUSTER_NUM 4
#define ZB_WWAH_ZED_HA_EP_OUT_CLUSTER_NUM 4


#define ZED_HA_EP 9
/*!
  @brief Declare HA cluster list for WWAH Device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param wwah_attr_list - attribute list for WWAH cluster
  @param pool_control_attr_list - attribute list for Poll Control cluster
  @param ota_upgrade_attr_list - attribute list for OTA Upgrade cluster
  @param identify_attr_list - attribute list for Identify cluster
*/
#define ZB_HA_DECLARE_WWAH_CLUSTER_LIST_ZED(                   \
  cluster_list_name,                                           \
  basic_attr_list,                                             \
  wwah_attr_list,                                              \
  poll_control_attr_list,                                      \
  ota_upgrade_attr_list,                                       \
  identify_attr_list                                           \
)                                                              \
  zb_zcl_cluster_desc_t cluster_list_name[] =                  \
{                                                              \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_BASIC,                                   \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),         \
    (basic_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_WWAH,                                    \
    ZB_ZCL_ARRAY_SIZE(wwah_attr_list, zb_zcl_attr_t),          \
    (wwah_attr_list),                                          \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    WWAH_MANUFACTURER_CODE                                     \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_TIME,                                    \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                            \
    ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t),  \
    (poll_control_attr_list),                                  \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                             \
    ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),   \
    (ota_upgrade_attr_list),                                   \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),      \
    (identify_attr_list),                                      \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_KEEP_ALIVE,                              \
    0, NULL,                                                   \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  )                                                            \
}

/*!
  @brief Declare simple descriptor for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/

#define ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZED(ep_name, ep_id, in_clust_num, out_clust_num)  \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                       \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                \
  {                                                                                          \
    ep_id,                                                                                   \
    ZB_AF_HA_PROFILE_ID,                                                                     \
    ZB_WWAH_DEVICE_ID,                                                                       \
    ZB_WWAH_DEVICE_VER,                                                                      \
    0,                                                                                       \
    in_clust_num,                                                                            \
    out_clust_num,                                                                           \
    {                                                                                        \
      ZB_ZCL_CLUSTER_ID_WWAH,                                                                \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                               \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                                        \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                            \
      ZB_ZCL_CLUSTER_ID_TIME,                                                                \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                            \
      ZB_ZCL_CLUSTER_ID_KEEP_ALIVE                                                           \
    }                                                                                        \
  }

/*!
  @brief Declare HA endpoint for WWAH Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_WWAH_ZED_EP(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_WWAH_HA_SIMPLE_DESC_ZED(ep_name, ep_id,                   \
    ZB_WWAH_ZED_HA_EP_IN_CLUSTER_NUM, ZB_WWAH_ZED_HA_EP_OUT_CLUSTER_NUM);  \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,         \
    0,                                                                     \
    NULL,                                                                  \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,  \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                      \
    0, NULL,                                                               \
    0, NULL)

#define ZB_WWAH_DOOR_LOCK_EP_IN_CLUSTER_NUM 4
#define ZB_WWAH_DOOR_LOCK_EP_OUT_CLUSTER_NUM 4

#define WWAH_DOOR_LOCK_EP 12
/*!
  @brief Declare HA cluster list for Door Lock Device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param wwah_attr_list - attribute list for WWAH cluster
  @param door_lock_attr_list - attribute list for Door Lock cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param ota_attr_list - attribute list for OTA cluster
*/

#define ZB_HA_DECLARE_WWAH_DOOR_LOCK_CLUSTER_LIST(             \
  cluster_list_name,                                           \
  basic_attr_list,                                             \
  wwah_attr_list,                                              \
  door_lock_attr_list,                                         \
  identify_attr_list,                                          \
  ota_attr_list                                                \
)                                                              \
  zb_zcl_cluster_desc_t cluster_list_name[] =                  \
{                                                              \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_BASIC,                                   \
    ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),         \
    (basic_attr_list),                                         \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_WWAH,                                    \
    ZB_ZCL_ARRAY_SIZE(wwah_attr_list, zb_zcl_attr_t),          \
    (wwah_attr_list),                                          \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    WWAH_MANUFACTURER_CODE                                     \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                               \
    ZB_ZCL_ARRAY_SIZE(door_lock_attr_list, zb_zcl_attr_t),     \
    (door_lock_attr_list),                                     \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),      \
    (identify_attr_list),                                      \
    ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_BASIC,                                   \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                             \
    ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),   \
    (ota_upgrade_attr_list),                                   \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  ),                                                           \
  ZB_ZCL_CLUSTER_DESC(                                         \
    ZB_ZCL_CLUSTER_ID_TIME,                                    \
    0,                                                         \
    NULL,                                                      \
    ZB_ZCL_CLUSTER_CLIENT_ROLE,                                \
    ZB_ZCL_MANUF_CODE_INVALID                                  \
  )                                                            \
}


/*!
  @brief Declare simple descriptor for Door Lock Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/

#define ZB_ZCL_DECLARE_WWAH_DOOR_LOCK_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                       \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =                \
  {                                                                                          \
    ep_id,                                                                                   \
    ZB_AF_HA_PROFILE_ID,                                                                     \
    ZB_WWAH_DEVICE_ID,                                                                       \
    ZB_WWAH_DEVICE_VER,                                                                      \
    0,                                                                                       \
    in_clust_num,                                                                            \
    out_clust_num,                                                                           \
    {                                                                                        \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                               \
      ZB_ZCL_CLUSTER_ID_WWAH,                                                                \
      ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                                                           \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                            \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                            \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                               \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                                                         \
      ZB_ZCL_CLUSTER_ID_TIME                                                                 \
    }                                                                                        \
  }

/*!
  @brief Declare HA endpoint for Door Lock Device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
*/
#define ZB_HA_DECLARE_WWAH_DOOR_LOCK_EP(ep_name, ep_id, cluster_list)          \
  ZB_ZCL_DECLARE_WWAH_DOOR_LOCK_SIMPLE_DESC(ep_name, ep_id,                    \
    ZB_WWAH_DOOR_LOCK_EP_IN_CLUSTER_NUM, ZB_WWAH_DOOR_LOCK_EP_OUT_CLUSTER_NUM);\
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    0, NULL,                                                                   \
    0, NULL)


#define SP_INIT_BASIC_HW_VERSION        2
#define SP_INIT_BASIC_MANUF_NAME	  "DSR"

#define SP_DONT_CLOSE_NETWORK_AT_REBOOT

/* Use SP_MAC_ADDR if it is needed to setup IEEE addr manually. */
#define SP_MAC_ADDR {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}
#define SP_OTA

#define SP_BUTTON_PIN                 0

#define SP_LED_ON() TRACE_MSG(TRACE_APP1, "LED ON", (FMT__0))
#define SP_LED_OFF() TRACE_MSG(TRACE_APP1, "LED OFF", (FMT__0))

#define SP_PAGE_SIZE     0x100
#define SP_OTA_FW_CTRL_ADDRESS ((zb_uint32_t)0)
#define OTA_ERASE_PORTION_SIZE ((zb_uint32_t)0)
#define SP_OTA_FW_DOWNLOADED   ((zb_uint32_t)0)
#define SP_OTA_FW_START_ADDRESS ((zb_uint32_t)0)
#define SP_OTA_FW_ERASE_ADDRESS (SP_OTA_FW_CTRL_ADDRESS + SP_OTA_FW_SECTOR_SIZE)
#define SP_OTA_FW_SECTOR_SIZE   ((zb_uint32_t)0)

zb_ret_t sp_hal_init();
zb_bool_t sp_get_button_state(zb_uint16_t button);
void sp_relay_on_off(zb_bool_t is_on);
void sp_update_button_state_ctx(zb_uint8_t button_state);


typedef enum sp_dev_state_e
{
  SP_DEV_INIT = 0,
  SP_DEV_IDLE,
  SP_DEV_COMMISSIONING,
  SP_DEV_NORMAL
}
sp_dev_state_t;

#define SP_OPERATIONAL_LED 1

/********************* Timeouts **************************/

/* Timeout after button pressed, before starting comissioning.
   Purpose: timeout is needed to support different  button - press sequences */
#define SP_BUTTON_START_JOIN_TIMEOUT  (2 * ZB_TIME_ONE_SECOND)
#define SP_BUTTON_START_JOIN_TIMEOUT_MAX  (3 * ZB_TIME_ONE_SECOND)

#define SP_DELAY_BETWEEN_BUTTON_PRESS    (1 * ZB_TIME_ONE_SECOND)

#define SP_BUTTON_BOUNCE_TIMEOUT ZB_MILLISECONDS_TO_BEACON_INTERVAL(10)

/* Timeout after button pressed, for start reset to factory default */
#define SP_RESET_TO_FACTORY_DEFAULT_TIMEOUT  (5 * ZB_TIME_ONE_SECOND)

/* Button not pressed/released after last command yet */
#define SP_BUTTON_NOT_PRESSED                            0

/* Number of button press-release to star forced rejoin */
#define SP_BUTTON_FORCED_REJOIN_PRESS_COUNT       5

#define SP_FIRST_JOIN_ATTEMPT       0
#define SP_RETRY_JOIN_ATTEMPT       1
#define SP_JOIN_LIMIT 10

#define SP_COMMISSIONING_LED_TIMEOUT  (10 * ZB_TIME_ONE_SECOND)

/*********** H/W specific ***************/
#define SP_BUTTON_PRESSED 1
#define SP_BUTTON_RELEASE 0


/*************************************************************************/
/* attributes of Basic cluster */
typedef struct sp_device_basic_attr_s
{
  zb_uint8_t zcl_version;
  zb_uint8_t app_version;
  zb_uint8_t stack_version;
  zb_uint8_t hw_version;
  zb_char_t mf_name[32];
  zb_char_t model_id[32];
  zb_char_t date_code[16];
  zb_uint8_t power_source;
  zb_char_t location_id[5];
  zb_uint8_t ph_env;
  zb_char_t sw_build_id[3];
}
sp_device_basic_attr_t;

/* attributes of Identify cluster */
typedef struct sp_device_identify_attr_s
{
  zb_uint16_t identify_time;
}
sp_device_identify_attr_t;


/* Groups cluster attributes data */
typedef struct sp_device_groups_attr_s
{
  zb_uint8_t name_support;
}
sp_device_groups_attr_t;

/* Door lock cluster attributes data */
typedef struct door_lock_attr_s
{
  zb_uint8_t lock_state;
  zb_uint8_t lock_type;
  zb_uint8_t actuator_enabled;
  zb_uint16_t rf_operation_event_mask;
} door_lock_attr_t;

/* OTA Upgrade client cluster attributes data */
typedef struct sp_device_ota_attr_s
{
  zb_ieee_addr_t upgrade_server;
  zb_uint32_t file_offset;
  zb_uint32_t file_version;
  zb_uint16_t stack_version;
  zb_uint32_t downloaded_file_ver;
  zb_uint16_t downloaded_stack_ver;
  zb_uint8_t image_status;
  zb_uint16_t manufacturer;
  zb_uint16_t image_type;
  zb_uint16_t min_block_reque;
  zb_uint16_t image_stamp;
  zb_uint16_t server_addr;
  zb_uint8_t server_ep;
}
sp_device_ota_attr_t;

#define SP_INIT_OTA_MIN_BLOCK_REQUE             10
#define SP_INIT_OTA_IMAGE_STAMP			ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE
#define SP_OTA_IMAGE_BLOCK_DATA_SIZE_MAX        32
#define SP_OTA_UPGRADE_SERVER                   { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }
#define SP_OTA_UPGRADE_QUERY_TIMER_COUNTER      (12*60)

#define SP_OTA_UPGARDE_HASH_LENGTH              16
#define SP_OTA_DEVICE_RESET_TIMEOUT             30*ZB_TIME_ONE_SECOND

#define SP_BUTTON_BOUNCE_OFF                    0
#define SP_BUTTON_BOUNCE_ON                     1
/*************************************************/
/* Smart Plug device context */

typedef struct sp_ota_upgrade_ctx_s
{
  zb_uint32_t total_image_size;
  zb_uint32_t addr_to_erase;
  zb_uint32_t address;          /* Supposed to be constant value, init
                                 * on OTA Upgrade start  */
  void       *flash_dev;
  zb_uint32_t fw_version;
  zb_uint8_t param;     // buffer, contain process command (if sheduling process)
  zb_bool_t is_started_manually;
#ifndef ZB_USE_OSIF_OTA_ROUTINES
  zb_uint8_t fw_image_portion[SP_OTA_IMAGE_BLOCK_DATA_SIZE_MAX * 2];
  zb_uint32_t fw_image_portion_size;
  zb_uint32_t file_length;        /*!< OTA file length got from next_image_resp  */

  zb_uint32_t hash_addr;
  zb_uint8_t hash[SP_OTA_UPGARDE_HASH_LENGTH];
  zb_bool_t hash16_calc_ongoing;
#endif
} sp_ota_upgrade_ctx_t;

typedef struct door_lock_ctx_s
{
  sp_dev_state_t dev_state;
  zb_time_t      button_press_time;    /* time last pressed button */
  zb_time_t      button_release_time;  /* time last released button */
  zb_uint8_t     button_bounce_flag;   /* button bounce flag*/
  zb_uint8_t     button_changed_count;  /* time circle pressed/released duration one series */

  /* Flag that is set if critical error appeared => deferred reset operation */
  zb_uint16_t door_lock_client_addr;
  zb_uint8_t door_lock_client_endpoint;
  zb_time_t last_obtained_time;
  zb_time_t obtained_at;
  zb_bool_t reset_device;
  sp_device_basic_attr_t basic_attr;
  sp_device_identify_attr_t identify_attr;
  sp_device_groups_attr_t groups_attr;
  sp_device_ota_attr_t ota_attr;
  sp_ota_upgrade_ctx_t ota_ctx;
  door_lock_attr_t door_lock_attr;
}
door_lock_ctx_t;

/*** App init/deinit ***/
void sp_clusters_attr_init(zb_uint8_t param);
void sp_device_app_init(zb_uint8_t param);
void sp_app_ctx_init();
void sp_critical_error();
zb_ret_t sp_hw_init(zb_uint8_t param);
void sp_start_device(zb_uint8_t);
void sp_start_join(zb_uint8_t param);
void sp_retry_join();

/*** General callbacks ***/
void sp_leave_indication(zb_uint8_t param);
void sp_basic_reset_to_defaults_cb(zb_uint8_t param);

/*** ZB network API ***/
void sp_leave_nwk(zb_uint8_t param);

/*** OTA API ***/
zb_uint8_t sp_ota_upgrade_init(zb_uint32_t image_size,
                               zb_uint32_t image_version);
zb_ret_t sp_ota_upgrade_write_next_portion(zb_uint8_t *ptr, zb_uint32_t off, zb_uint8_t len);
zb_uint8_t sp_ota_upgrade_check_fw(zb_uint8_t param);
void sp_ota_upgrade_mark_fw_ok();
void sp_ota_upgrade_abort();
void sp_ota_upgrade_server_not_found();

/*** H/W API ***/
void sp_com_button_changed(void);
void sp_device_interface_cb(zb_uint8_t param);

void sp_com_button_pressed_reset_fd(zb_uint8_t param);
void sp_button_debounce(zb_uint8_t param);
void sp_update_on_off_state(zb_uint8_t is_on);
/*****************************************************************************/

/*** internal part ***/

typedef struct sp_device_reporting_default_s
{
  zb_uint16_t cluster_id;
  zb_uint16_t attr_id;
  zb_uint8_t attr_type;
}
sp_device_reporting_default_t;

#define SP_REPORTING_MIN_INTERVAL       5
#define SP_REPORTING_MAX_INTERVAL       7

#ifdef ZB_USE_NVRAM
typedef ZB_PACKED_PRE struct wwah_door_lock_device_nvram_dataset_s
{
  zb_uint48_t curr_summ_delivered;
  zb_uint8_t sp_onoff_state;
  zb_uint8_t aligned[5];
} ZB_PACKED_STRUCT
sp_device_nvram_dataset_t;

/* Check dataset alignment for IAR compiler and ARM Cortex target platfrom */
ZB_ASSERT_IF_NOT_ALIGNED_TO_4(sp_device_nvram_dataset_t);

/*** Production config data ***/
typedef ZB_PACKED_PRE struct sp_production_config_t
{
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
  zb_uint16_t manuf_code;
  zb_uint16_t overcurrent_ma;
  zb_uint16_t overvoltage_dv;
}
ZB_PACKED_STRUCT sp_production_config_t;

zb_uint16_t sp_get_nvram_data_size();
void sp_nvram_read_app_data(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);
zb_ret_t sp_nvram_write_app_data(zb_uint8_t page, zb_uint32_t pos);

#endif /* ZB_USE_NVRAM */

zb_ret_t sp_read_config_data(zb_uint32_t address, zb_uint8_t *buf, zb_uint16_t len);
void sp_update_metering_data(zb_uint8_t param);

#ifndef ZB_USE_OSIF_OTA_ROUTINES
zb_uint8_t zb_erase_fw(zb_uint32_t address, zb_uint32_t pages_count);
zb_uint8_t zb_read_fw(zb_uint32_t address, zb_uint8_t *buf, zb_uint16_t len);
zb_uint8_t zb_write_fw(zb_uint32_t address, zb_uint8_t *buf, zb_uint16_t len);
#endif


#ifndef ZB_USE_NVRAM
#error "NVRAM support is needed - define ZB_USE_NVRAM"
#endif


/* OTA Manufacturer code */
#define SP_DEVICE_MANUFACTURER_CODE  0x1234   /* DSR SmartPlug manufacture code*/
/* ZBOSS SDK for Smart plug application - from zb_ver_sdk_type.h */
#define ZBOSS_SDK_SMART_PLUG_MAJOR 3

/* Basic cluster attributes data */
#define SP_INIT_BASIC_STACK_VERSION     ZBOSS_MAJOR

#define SP_INIT_BASIC_APP_VERSION       1

/* OTA Upgrade client cluster attributes data */
#define SP_INIT_OTA_FILE_VERSION	\
  ( ((zb_uint32_t)ZBOSS_MAJOR) | ((zb_uint32_t)ZBOSS_MINOR << 8) | ((zb_uint32_t)ZBOSS_SDK_SMART_PLUG_MAJOR << 16) | ((zb_uint32_t)SP_INIT_BASIC_APP_VERSION << 24) )
#define SP_INIT_OTA_HW_VERSION			SP_INIT_BASIC_HW_VERSION
#define SP_INIT_OTA_MANUFACTURER		SP_DEVICE_MANUFACTURER_CODE

#define SP_INIT_OTA_IMAGE_TYPE			0x0012

/* Note: instead of the 1st space string len will be set */
#define SP_INIT_BASIC_MODEL_ID     "SPv313p691"
#define SP_INIT_BASIC_DATE_CODE    "2017-02-01"
#define SP_INIT_BASIC_LOCATION_ID  "US"
#define SP_INIT_BASIC_PH_ENV                            0

#define SP_PERMIT_JOIN_DURATION 180 /* seconds */

#endif /* ZB_WWAH_H */
