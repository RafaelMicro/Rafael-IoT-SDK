/**
 * @file zigbee_data.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

//=============================================================================
//                Include
//=============================================================================
#include "zigbee_platform.h"
#include "zigbee_api.h"
//=============================================================================
//                Private Global Variables
//=============================================================================
#define GW_IN_CLUSTER_NUM  4
#define GW_OUT_CLUSTER_NUM 11

#define GW_DEVICE_VER 0
#define GW_DEVICE_ID  0
/* Basic cluster attributes */
static uint8_t attr_zcl_version = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
static uint8_t attr_power_source = ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;

/* OTA Upgrade server cluster attributes */
static uint8_t query_jitter = 0x64;
static zb_uint32_t current_time = 0x00000000;

/* Identify cluster attributes */
static uint16_t attr_identify_time = 0;

/* Time cluster attributes */
uint32_t g_attr_time = ZB_ZCL_TIME_TIME_DEFAULT_VALUE;
uint8_t g_attr_time_status = 1;
uint32_t g_attr_time_zone = ZB_ZCL_TIME_TIME_ZONE_DEFAULT_VALUE;
uint32_t g_attr_dst_start = ZB_ZCL_TIME_DST_START_DEFAULT_VALUE;
uint32_t g_attr_dst_end = ZB_ZCL_TIME_DST_END_DEFAULT_VALUE;
uint32_t g_attr_dst_shift = ZB_ZCL_TIME_DST_SHIFT_DEFAULT_VALUE;
uint32_t g_attr_standard_time = ZB_ZCL_TIME_STANDARD_TIME_DEFAULT_VALUE;
uint32_t g_attr_local_time = ZB_ZCL_TIME_LOCAL_TIME_DEFAULT_VALUE;
uint32_t g_attr_last_set_time = ZB_ZCL_TIME_LAST_SET_TIME_DEFAULT_VALUE;
uint32_t g_attr_valid_until_time = ZB_ZCL_TIME_VALID_UNTIL_TIME_DEFAULT_VALUE;
//=============================================================================
//                Global Variables
//=============================================================================

//=============================================================================
//                Attribute definitions
//=============================================================================
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(basic_attr_list, &attr_zcl_version,
                                 &attr_power_source);

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &attr_identify_time);

ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST_SERVER(ota_upgrade_attr_list,
                                              &query_jitter, &current_time, 1);

ZB_ZCL_DECLARE_TIME_ATTRIB_LIST(time_attr_list,
                                &g_attr_time, &g_attr_time_status, &g_attr_time_zone,
                                &g_attr_dst_start, &g_attr_dst_end, &g_attr_dst_shift, &g_attr_standard_time,
                                &g_attr_local_time, &g_attr_last_set_time, &g_attr_valid_until_time);
//=============================================================================
//                Cluster definitions
//=============================================================================
zb_zcl_cluster_desc_t g_zigbee_cluster_list[] = {
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                        ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),
                        (ota_upgrade_attr_list), ZB_ZCL_CLUSTER_SERVER_ROLE,
                        ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_BASIC,
                        ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),
                        (basic_attr_list), ZB_ZCL_CLUSTER_SERVER_ROLE,
                        ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IDENTIFY,
                        ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),
                        (identify_attr_list), ZB_ZCL_CLUSTER_SERVER_ROLE,
                        ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_TIME,
                        ZB_ZCL_ARRAY_SIZE(time_attr_list, zb_zcl_attr_t),
                        (time_attr_list), ZB_ZCL_CLUSTER_SERVER_ROLE,
                        ZB_ZCL_MANUF_CODE_INVALID),

    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_BASIC, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_ON_OFF, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_SCENES, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_GROUPS, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IAS_ZONE, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IDENTIFY, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_DOOR_LOCK, 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_CUSTOM , 0, NULL,
                        ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUFACTURE_CODE_CUSTOM_CLUSTER)};
//=============================================================================
//                Simple desc definitions
//=============================================================================
ZB_DECLARE_SIMPLE_DESC(4, 11);

ZB_AF_SIMPLE_DESC_TYPE(GW_IN_CLUSTER_NUM, GW_OUT_CLUSTER_NUM)
simple_desc_gateway = {
    ZIGBEE_DEFAULT_ENDPOINT,                   /* Endpoint */
    ZB_AF_HA_PROFILE_ID, /* Application profile identifier */
    GW_DEVICE_ID,        /* Application device identifier */
    GW_DEVICE_VER,       /* Application device version */
    0,                   /* Reserved */
    GW_IN_CLUSTER_NUM,   /* Application input cluster count */
    GW_OUT_CLUSTER_NUM,  /* Application output cluster count */
    /* Application input and output cluster list */
    {
        ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
        ZB_ZCL_CLUSTER_ID_BASIC,
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        ZB_ZCL_CLUSTER_ID_TIME,
        ZB_ZCL_CLUSTER_ID_BASIC,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
        ZB_ZCL_CLUSTER_ID_COLOR_CONTROL,
        ZB_ZCL_CLUSTER_ID_SCENES,
        ZB_ZCL_CLUSTER_ID_GROUPS,
        ZB_ZCL_CLUSTER_ID_IAS_ZONE,
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
        ZB_ZCL_CLUSTER_ID_DOOR_LOCK,
        ZB_ZCL_CLUSTER_ID_CUSTOM,
    }};

ZB_AF_DECLARE_ENDPOINT_DESC(
    simple_desc_gateway_ep, ZIGBEE_DEFAULT_ENDPOINT, ZB_AF_HA_PROFILE_ID, 0, NULL,
    ZB_ZCL_ARRAY_SIZE(g_zigbee_cluster_list, zb_zcl_cluster_desc_t),
    g_zigbee_cluster_list, (zb_af_simple_desc_1_1_t*)&simple_desc_gateway, 0,
    NULL, 0, NULL);

ZBOSS_DECLARE_DEVICE_CTX_1_EP(simple_desc_gateway_ctx, simple_desc_gateway_ep);

void set_gw_time(uint32_t time, uint8_t sync_externally) {
    if (sync_externally)
        g_attr_last_set_time = time;
    g_attr_time = time;
    g_attr_standard_time = time;
    g_attr_local_time = time;
}

uint32_t get_gw_time(void) {
    return g_attr_time;
}