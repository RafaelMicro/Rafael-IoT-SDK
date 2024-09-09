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
/* PURPOSE: Smart plug device declaration
*/

#ifndef SP_DEVICE_H
#define SP_DEVICE_H 1

#include "zboss_api.h"
#include "sp_config_stub.h"
#include "sp_hal_stub.h"
#include "zb_version.h"

/* Smart plug device states enumeration*/
typedef enum sp_dev_state_e
{
  SP_DEV_INIT = 0,
  SP_DEV_IDLE,
  SP_DEV_COMMISSIONING,
  SP_DEV_NORMAL
}
sp_dev_state_t;

/* Current Led states enumeration */
typedef enum sp_led_state_e
{
  SP_LED_SUCCESS,       /* Commissioning successful: light on the LED for 3 second then off */
  SP_LED_FAILED,        /* Operation failed: 5 short blinks (300 ms on, 300 ms off) */
  SP_LED_PROGRESS,      /* Commissioning in progress: blink periodically 1 second on, 1 second off */
  SP_LED_IDLE_MODE,     /* Idle mode: 3 short blinks (300 ms on, 300 ms off), 3 sec off */
  SP_LED_NORMAL,        /* Normal operation mode: 300 ms on, 3 sec off */
  SP_LED_IDENTIFY,      /* Identify blinking: 300 ms on, 200 ms off – repeat in a loop */
  SP_LED_RESET_FD,      /* Reset to Factory default blinking: 200 ms on, 200 ms off – 10 times */
  SP_LED_CRITICAL_ERROR,/* Critical error blinking: repeat 3 times(5 short blinks (300 ms on, 300 ms off), pause: 600 ms off) */
}
sp_led_state_t;

#define SP_OPERATIONAL_LED 1

/********************* Timeouts **************************/

/* Timeout after button pressed, before starting comissioning.
   Purpose: timeout is needed to support different  button - press sequences */
#define SP_BUTTON_START_JOIN_TIMEOUT  (2 * ZB_TIME_ONE_SECOND)
#define SP_BUTTON_START_JOIN_TIMEOUT_MAX  (3 * ZB_TIME_ONE_SECOND)

#ifdef SP_ENABLE_NWK_STEERING_BUTTON
#define SP_BUTTON_NWK_STEERING_TIMEOUT (5 * ZB_TIME_ONE_SECOND)
#define SP_BUTTON_NWK_STEERING_TIMEOUT_MAX  (8 * ZB_TIME_ONE_SECOND)
#endif

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

/************************* LED ***********************************/
#define SP_LED_LONG_TIMEOUT            (3 * ZB_TIME_ONE_SECOND)
#define SP_LED_SHORT_TIMEOUT           ZB_MILLISECONDS_TO_BEACON_INTERVAL(300)
#define SP_LED_PROGRESS_TIMEOUT        (1 * ZB_TIME_ONE_SECOND)
#define SP_LED_SMALL_TIMEOUT           ZB_MILLISECONDS_TO_BEACON_INTERVAL(200)
#define SP_LED_RESET_FD_TIMEOUT        (2 * ZB_TIME_ONE_SECOND)

#define SP_LED_FALED_CONUT              5
#define SP_LED_IDLE_MODE_COUNT          3
#define SP_LED_RESTRICTED               2
#define SP_LED_RESET_FD_COUNT           1
#define SP_LED_CRITICAL_ERROR_COUNT         10
#define SP_LED_CRITICAL_ERROR_LONG_SERIES   10


/******************* Specific definitions for window covering device **************************/

#define SP_DEVICE_ID            ZB_HA_SMART_PLUG_DEVICE_ID
#define SP_DEVICE_VER           1

#ifdef SP_WWAH_COMPATIBLE
#define SP_IN_CLUSTER_NUM       6
#else
#define SP_IN_CLUSTER_NUM       5
#endif

#ifdef SP_OTA
#define SP_OUT_CLUSTER_NUM      3
#else
#define SP_OUT_CLUSTER_NUM      2
#endif

#define SP_CLUSTER_NUM          (SP_IN_CLUSTER_NUM + SP_OUT_CLUSTER_NUM)

/*! Number of attribute for reporting on Smart Plug device */
#define SP_REPORT_ATTR_COUNT (ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT + ZB_ZCL_METERING_REPORT_ATTR_COUNT)

#ifdef SP_OTA
#define SP_OTA_CLUSTER_DESC(ota_upgrade_attr_list)             \
ZB_ZCL_CLUSTER_DESC(                                           \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                           \
      ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t), \
      (ota_upgrade_attr_list),                                 \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),

#define SP_OTA_CLUSTER_ID ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
#else
#define SP_OTA_CLUSTER_DESC(ota_upgrade_attr_list)
#define SP_OTA_CLUSTER_ID
#endif  /* SP_OTA */

#ifdef SP_WWAH_COMPATIBLE
#define SP_WWAH_CLUSTER_DESC(wwah_attr_list)                   \
ZB_ZCL_CLUSTER_DESC(                                           \
      ZB_ZCL_CLUSTER_ID_WWAH,                                  \
      ZB_ZCL_ARRAY_SIZE(wwah_attr_list, zb_zcl_attr_t),        \
      (wwah_attr_list),                                        \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      WWAH_MANUFACTURER_CODE                                   \
    ),

#define SP_WWAH_CLUSTER_ID ZB_ZCL_CLUSTER_ID_WWAH,
#else
#define SP_WWAH_CLUSTER_DESC(wwah_attr_list)
#define SP_WWAH_CLUSTER_ID
#endif  /* SP_WWAH_COMPATIBLE */

/*!
  @brief Declare cluster list for Smart Plug device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param metering_attr_list - attribute list for Metering cluster
  @param on_off_attr_list - attribute list for On/Off cluster
  @param ota_upgrade_attr_list - attribute list for OTA Upgrade cluster
 */
#define SP_DECLARE_CLUSTER_LIST(                               \
  cluster_list_name,                                           \
  basic_attr_list,                                             \
  identify_attr_list,                                          \
  metering_attr_list,                                          \
  on_off_attr_list,                                            \
  groups_attr_list,                                            \
  wwah_attr_list,                                              \
  ota_upgrade_attr_list)                                       \
  zb_zcl_cluster_desc_t cluster_list_name[] =                  \
  {                                                            \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                                 \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),       \
      (basic_attr_list),                                       \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                              \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),    \
      (identify_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_METERING,                              \
      ZB_ZCL_ARRAY_SIZE(metering_attr_list, zb_zcl_attr_t),    \
      (metering_attr_list),                                    \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),      \
      (on_off_attr_list),                                      \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                \
      ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),      \
      (groups_attr_list),                                      \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    SP_WWAH_CLUSTER_DESC(wwah_attr_list)                       \
    SP_OTA_CLUSTER_DESC(ota_upgrade_attr_list)                 \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                              \
      0,                                                       \
      NULL,                                                    \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_BASIC,                                 \
      0,                                                       \
      NULL,                                                    \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    )                                                          \
  }

/*!
  @brief Declare simple descriptor for Smart Plug device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define SP_DECLARE_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)            \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                 \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =          \
  {                                                                                    \
    ep_id,                                                                             \
    ZB_AF_HA_PROFILE_ID,                                                               \
    SP_DEVICE_ID,                                                                      \
    SP_DEVICE_VER,                                                                     \
    0,                                                                                 \
    in_clust_num,                                                                      \
    out_clust_num,                                                                     \
    {                                                                                  \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                      \
      ZB_ZCL_CLUSTER_ID_METERING,                                                      \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                        \
      ZB_ZCL_CLUSTER_ID_GROUPS,                                                        \
      SP_WWAH_CLUSTER_ID                                                               \
      SP_OTA_CLUSTER_ID                                                                \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                      \
      ZB_ZCL_CLUSTER_ID_BASIC                                                          \
    }                                                                                  \
  }

/*!
  @brief Declare endpoint for Smart Plug device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */
#define SP_DECLARE_EP(ep_name, ep_id, cluster_list)                                     \
  SP_DECLARE_SIMPLE_DESC(ep_name, ep_id, SP_IN_CLUSTER_NUM, SP_OUT_CLUSTER_NUM);        \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name, SP_REPORT_ATTR_COUNT); \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID,                      \
    0,                                                                                  \
    NULL,                                                                               \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                             \
    cluster_list,                                                                       \
                          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
                          SP_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, 0, NULL)


/*
  @brief Declare application's device context for Smart Plug device
  @param device_ctx - device context variable
  @param ep_name - endpoint variable name
*/
#ifdef SP_CONTROL4_COMPATIBLE
#define SP_DECLARE_CTX(device_ctx, ep1_name, ep2_name)           \
  ZBOSS_DECLARE_DEVICE_CTX_2_EP(device_ctx, ep1_name, ep2_name)
#else
#define SP_DECLARE_CTX(device_ctx, ep_name)                                \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)
#endif /* SP_CONTROL4_COMPATIBLE */

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

/* attributes of Metering cluster */
typedef struct sp_device_metering_attr_s
{
  zb_uint48_t curr_summ_delivered;
  zb_uint8_t status;
  zb_uint8_t unit_of_measure;
  zb_uint8_t summation_formatting;
  zb_uint8_t metering_device_type;
  zb_int24_t instantaneous_demand;
  zb_uint8_t demand_formatting;
  zb_uint8_t historical_consumption_formatting;
  zb_uint24_t multiplier;
  zb_uint24_t divisor;

}
sp_device_metering_attr_t;

#define SP_INIT_METERING_STATUS                         ZB_ZCL_METERING_STATUS_DEFAULT_VALUE
#define SP_INIT_METERING_UNIT_OF_MEASURE                ZB_ZCL_METERING_UNIT_OF_MEASURE_DEFAULT_VALUE
#define SP_INIT_METERING_SUMMATION_FORMATTING           ZB_ZCL_METERING_FORMATTING_SET(ZB_TRUE, 6, 2)
#define SP_INIT_METERING_METERING_DEVICE_TYPE           ZB_ZCL_METERING_ELECTRIC_METERING
#define SP_INIT_METERING_INSTANTANEOUS_DEMAND           ZB_ZCL_METERING_INSTANTANEOUS_DEMAND_DEFAULT_VALUE
#define SP_INIT_METERING_DEMAND_FORMATIING              ZB_ZCL_METERING_FORMATTING_SET(ZB_TRUE, 6, 2)
#define SP_INIT_METERING_HISTORICAL_CONSUMPTION_FORMATTING      ZB_ZCL_METERING_FORMATTING_SET(ZB_TRUE, 6, 2)
#define SP_INIT_METERING_MULTIPLIER                     1
#define SP_INIT_METERING_DIVISOR                        10000

/* attributes of ON/Off cluster */
typedef struct sp_device_on_off_attr_s
{
  zb_bool_t on_off;
  zb_bool_t global_scene_ctrl;
  zb_uint16_t on_time;
  zb_uint16_t off_wait_time;
}
sp_device_on_off_attr_t;

/* Groups cluster attributes data */
typedef struct sp_device_groups_attr_s
{
  zb_uint8_t name_support;
}
sp_device_groups_attr_t;

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
  zb_uint8_t param;     // buffer, contain process command (if scheduling process)
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

#ifdef SP_CONTROL4_COMPATIBLE
/* Control4 Network Cluster attributes */
typedef struct sp_device_control4_attr_s
{
  zb_uint8_t  device_type;
  zb_char_t   firmware_version[16];
  zb_uint8_t  reflash_version;
  zb_uint16_t boot_count;
  zb_char_t   product_string[32];
  zb_uint16_t access_point_node_ID;
  zb_ieee_addr_t access_point_long_ID;
  zb_uint8_t  access_point_cost;
  zb_uint8_t  mesh_channel;
}
sp_device_control4_attr_t;

typedef struct sp_device_control4_attr_ext_s
{
  zb_uint8_t  device_type;
  zb_uint16_t announce_window;
  zb_uint16_t mtorr_period;
  zb_char_t   firmware_version[16];
  zb_uint8_t  reflash_version;
  zb_uint16_t boot_count;
  zb_char_t   product_string[32];
  zb_uint16_t access_point_node_ID;
  zb_ieee_addr_t access_point_long_ID;
  zb_uint8_t  access_point_cost;
  zb_uint8_t  mesh_channel;
  zb_int8_t   avg_rssi;
  zb_uint8_t  avg_lqi;
  zb_int8_t   battery_level;
  zb_uint8_t  radio_4_bars;
}
sp_device_control4_attr_ext_t;
#endif /* SP_CONTROL4_COMPATIBLE */

/* Global device context */
typedef struct sp_device_ctx_s
{
  sp_dev_state_t dev_state;
  zb_time_t      button_press_time;    /* time last pressed button */
  zb_time_t      button_release_time;  /* time last released button */
  zb_uint8_t     button_bounce_flag;   /* button bounce flag*/
  zb_uint8_t     button_changed_count;  /* time circle pressed/released duration one series */
  sp_led_state_t led_state;

  zb_uint8_t blink_counter;
  zb_uint8_t join_counter;

  /* Flag that is set if critical error appeared => deferred reset operation */
  zb_bool_t reset_device;

  sp_device_basic_attr_t basic_attr;
  sp_device_identify_attr_t identify_attr;
  sp_device_metering_attr_t metering_attr;
  sp_device_on_off_attr_t on_off_attr;
  sp_device_groups_attr_t groups_attr;
  sp_device_ota_attr_t ota_attr;
  sp_ota_upgrade_ctx_t ota_ctx;
#ifdef SP_CONTROL4_COMPATIBLE
  sp_device_control4_attr_ext_t c4_attr;
  zb_uint8_t is_on_c4_network;
#endif /* SP_CONTROL4_COMPATIBLE */

  zb_uint16_t overcurrent_ma;
  zb_uint16_t overvoltage_dv;
}
sp_device_ctx_t;

/* [AV] Define g_dev_ctx for all the modules */
extern sp_device_ctx_t g_dev_ctx;

/* Handler for specific zcl commands */
zb_uint8_t sp_zcl_cmd_handler(zb_uint8_t param);

/*** App init/deinit ***/
void sp_clusters_attr_init(zb_uint8_t param);
void sp_device_app_init(zb_uint8_t param);
void sp_app_ctx_init();
void sp_critical_error();
zb_ret_t sp_hw_init(zb_uint8_t param);
void sp_platform_init();
void sp_start_device(zb_uint8_t);
void sp_start_join(zb_uint8_t param);
#ifdef SP_ENABLE_NWK_STEERING_BUTTON
void sp_nwk_steering(zb_uint8_t param);
#endif
void sp_retry_join();
void sp_start_bdb_commissioning(zb_uint8_t param);
void sp_write_app_data(zb_uint8_t param);

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
void sp_start_operation_blink(zb_uint8_t param);
void sp_device_interface_cb(zb_uint8_t param);

void sp_com_button_pressed_reset_fd(zb_uint8_t param);
void sp_button_debounce(zb_uint8_t param);
void sp_update_on_off_state(zb_uint8_t is_on);
/*****************************************************************************/

/*** internal part ***/

/* Reporting configuration */
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

/*** Application dataset for persisting into nvram ***/
typedef ZB_PACKED_PRE struct sp_device_nvram_dataset_s
{
  zb_uint48_t curr_summ_delivered;
  zb_uint8_t sp_onoff_state;
#ifdef SP_CONTROL4_COMPATIBLE
  zb_uint16_t boot_count;
  zb_uint8_t is_on_c4_network;
  zb_uint8_t aligned[2];
#else
  zb_uint8_t aligned[5];
#endif /* SP_CONTROL4_COMPATIBLE */
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

/* Persisting data into NVRAM routines */
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

/* #define SP_OTA */
/* #define SP_FLASH_TESTING */
/* #define SP_OTA_SKIP_APP_UPGRADE */
/* #define SP_OTA_SKIP_HASH_CHECK */
/* #define SP_NO_SPI_FLASH */

#ifndef ZB_USE_NVRAM
#error "NVRAM support is needed - define ZB_USE_NVRAM"
#endif

#define SP_ENDPOINT  12  /* Smart Plug device end point */

/* #define SP_DEFAULT_APS_CHANNEL_MASK ZB_TRANSCEIVER_ALL_CHANNELS_MASK */
#define SP_DEFAULT_APS_CHANNEL_MASK (1l<<21)

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

#ifdef SP_CONTROL4_COMPATIBLE
#define SP_CONTROL4_PRODUCT_STRING     "dsr:smart_plug:SPv313p691"
#define SP_CONTROL4_FIRMWARE_VERSION   "00.00.01"
#endif /* SP_CONTROL4_COMPATIBLE */

#define SP_PERMIT_JOIN_DURATION 180 /* seconds */

#endif /* SP_DEVICE_H */
