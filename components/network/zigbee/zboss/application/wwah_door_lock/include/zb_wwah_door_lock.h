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

#include "zboss_api.h"
#include "se/zb_se_keep_alive.h"
#include "zb_dl_device_basic.h"
#include "zb_dl_device_door_lock.h"
#include "zb_dl_device_nvram.h"
#include "zb_dl_device_ota.h"
#include "zb_dl_device_wwah.h"
#include "zb_dl_hal.h"

#define DL_DONT_CLOSE_NETWORK_AT_REBOOT

#define DEV_CHANNEL_MASK (1L << 18)

#define DL_RESET_TO_FACTORY_DEFAULT_TIMEOUT  (5 * ZB_TIME_ONE_SECOND)

#define ZB_WWAH_DEVICE_ID 0x3131
#define ZB_WWAH_DEVICE_VER 1

#define ZB_WWAH_DOOR_LOCK_EP_IN_CLUSTER_NUM 6
#define ZB_WWAH_DOOR_LOCK_EP_OUT_CLUSTER_NUM 4

/*! Number of attribute for reporting on WWAH Door Lock device */
#define ZB_WWAH_DOOR_LOCK_REPORT_ATTR_COUNT   \
  (ZB_ZCL_ALARMS_REPORT_ATTR_COUNT +          \
   ZB_ZCL_WWAH_REPORT_ATTR_COUNT +            \
   ZB_ZCL_DOOR_LOCK_REPORT_ATTR_COUNT +       \
   ZB_ZCL_POLL_CONTROL_REPORT_ATTR_COUNT)


#define WWAH_DOOR_LOCK_EP 1
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
  ota_attr_list,                                               \
  poll_control_attr_list                                       \
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
    ZB_ZCL_CLUSTER_ID_ALARMS,                                  \
    0,                                                         \
    NULL,                                                      \
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
    ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                            \
    ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t),  \
    (poll_control_attr_list),                                  \
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
      ZB_ZCL_CLUSTER_ID_ALARMS,                                                              \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                            \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                                        \
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
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,         \
                                     ZB_WWAH_DOOR_LOCK_REPORT_ATTR_COUNT);     \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,             \
    0,                                                                         \
    NULL,                                                                      \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,      \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                          \
    ZB_WWAH_DOOR_LOCK_REPORT_ATTR_COUNT, reporting_info## device_ctx_name,     \
    0, NULL)


typedef enum dl_dev_state_e
{
  DL_DEV_INIT = 0,
  DL_DEV_IDLE,
  DL_DEV_COMMISSIONING,
  DL_DEV_NORMAL
} dl_dev_state_t;

/* attributes of Identify cluster */
typedef struct dl_device_identify_attr_s
{
  zb_uint16_t identify_time;
} dl_device_identify_attr_t;

/* Groups cluster attributes data */
typedef struct dl_device_groups_attr_s
{
  zb_uint8_t name_support;
} dl_device_groups_attr_t;

typedef struct dl_device_reporting_default_s
{
  zb_uint16_t cluster_id;
  zb_uint16_t attr_id;
  zb_uint8_t attr_type;
} dl_device_reporting_default_t;


typedef struct dl_poll_control_attrs_s
{
  zb_uint32_t checkin_interval;
  zb_uint32_t long_poll_interval;
  zb_uint16_t short_poll_interval;
  zb_uint16_t fast_poll_timeout;
  zb_uint32_t checkin_interval_min;
  zb_uint32_t long_poll_interval_min;
  zb_uint16_t fast_poll_timeout_max;
} dl_poll_control_attrs_t;

typedef struct door_lock_ctx_s
{
  dl_dev_state_t dev_state;
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
  dl_device_basic_attr_t basic_attr;
  dl_device_identify_attr_t identify_attr;
  dl_device_groups_attr_t groups_attr;
  dl_device_ota_attr_t ota_attr;
  dl_ota_upgrade_ctx_t ota_ctx;
  door_lock_attr_t door_lock_attr;
  dl_wwah_app_event_retry_ctx_t app_event_retry_ctx;
  dl_poll_control_attrs_t poll_control_attrs;
} door_lock_ctx_t;

#define AER_CTX() g_dev_ctx.app_event_retry_ctx

extern door_lock_ctx_t g_dev_ctx;

/*** App init/deinit ***/
void dl_clusters_attr_init(zb_uint8_t param);
void dl_device_app_init(zb_uint8_t param);
void dl_app_ctx_init(void);
void dl_init_default_reporting(zb_uint8_t param);
void dl_critical_error(void);
zb_ret_t dl_hw_init(zb_uint8_t param);
void dl_start_device(zb_uint8_t);
void dl_start_join(zb_uint8_t param);
void dl_retry_join(void);

/*** General callbacks ***/
void dl_leave_indication(zb_uint8_t param);
void dl_basic_reset_to_defaults_cb(zb_uint8_t param);

/*** ZB network API ***/
void dl_leave_nwk(zb_uint8_t param);
/*****************************************************************************/

#endif /* ZB_WWAH_H */
