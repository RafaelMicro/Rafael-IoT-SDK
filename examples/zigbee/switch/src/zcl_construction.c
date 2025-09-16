/**
 * Copyright (c) 2021 All Rights Reserved.
 */
/** @file zigbee_data.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/12/09
 * @license
 * @description
 */

//=============================================================================
//                Include
//=============================================================================
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "zigbee_platform.h"
#include "zigbee_api.h"

#define HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID 0x0103
//=============================================================================
//                Private Global Variables
//=============================================================================
/* Basic cluster attributes */
static const uint8_t attr_zcl_version = ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
static const uint8_t attr_app_version = 0;
static const uint8_t attr_stack_version = 0;
static const uint8_t attr_hw_version = 0;
static const char attr_mf_name[] = {11,  'R', 'a', 'f', 'a', 'e',
                                    'l', 'M', 'i', 'c', 'r', 'o'};
static const char attr_model_id[] = {9,   'L', 'i', 'g', 'h',
                                     't', '0', '0', '0', '1'};
static const char attr_date_code[] = {8,   '2', '0', '2', '2',
                                      '0', '1', '2', '1'};
static const char attr_sw_build_id[] = {8,   '2', '0', '2', '2',
                                        '0', '1', '2', '1'};
static const uint8_t attr_power_source =
    ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;

static uint8_t attr_location_id[] = {16,  ' ', ' ', ' ', ' ', ' ',
                                     ' ', ' ', ' ', ' ', ' ', ' ',
                                     ' ', ' ', ' ', ' ', ' '};
static uint8_t attr_ph_env = 0;

static uint8_t attr_device_class =
    ZB_ZCL_BASIC_GENERIC_DEVICE_CLASS_DEFAULT_VALUE;
static uint8_t attr_device_type =
    ZB_ZCL_BASIC_GENERIC_DEVICE_TYPE_DEFAULT_VALUE;
static const char attr_product_code[] = ZB_ZCL_BASIC_PRODUCT_CODE_DEFAULT_VALUE;
static const char attr_product_url[] = ZB_ZCL_BASIC_PRODUCT_URL_DEFAULT_VALUE;

/* OTA Upgrade client cluster attributes */
uint16_t raf_image_type = 321;
/* UpgradeServerID attribute */
uint8_t upgrade_server[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
/* FileOffset attribute */
uint32_t file_offset = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE;
/* CurrentFileVersion attribute (custom data) */
uint32_t file_version = 0x01010101;
/* CurrentZigbeeStackVersion attribute */
uint16_t stack_version = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO;
/* DownloadedFileVersion attribute */
uint32_t downloaded_file_ver =
    ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE;
/* DownloadedZigbeeStackVersion attribute */
uint16_t downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE;
/* ImageUpgradeStatus attribute */
uint8_t image_status = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE;
/* Manufacturer ID attribute (custom data) */
uint16_t manufacturer = 123;
/* Image Type ID attribute (custom data) */
uint16_t image_type = 0xFFFF;
/* MinimumBlockReque attribute */
uint16_t min_block_reque = 0;
/* Image Stamp attribute */
uint16_t image_stamp = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE;
/* Server short address attribute */
uint16_t server_addr;
/* Server endpoint attribute */
uint8_t server_ep;

/*! Define a default global trust center link key */
uint8_t ZB_STANDARD_TC_KEY[] = {0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C,
                                0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39};
uint8_t install_code_key[18] = {0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93,
                                0x97, 0x23, 0xA5, 0xC6, 0x39, 0xB2,
                                0x69, 0x16, 0xD5, 0x05, 0xC3, 0xB5};
//=============================================================================
//                Global Variables
//=============================================================================

uint16_t g_attr_identify_time = 0;

/* Group cluster attributes data */
uint8_t g_attr_name_support = 0;


zb_zcl_reporting_info_t rep_ctx[8];
//=============================================================================
//                Attribute definitions
//=============================================================================
ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &attr_zcl_version,
                                     &attr_app_version,
                                     &attr_stack_version,
                                     &attr_hw_version,
                                     &attr_mf_name,
                                     &attr_model_id,
                                     &attr_date_code,
                                     &attr_power_source,
                                     &attr_location_id,
                                     &attr_ph_env,
                                     &attr_sw_build_id,
                                     &attr_device_class,
                                     &attr_device_type,
                                     &attr_product_code,
                                     &attr_product_url);

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list,
                                    &g_attr_identify_time);

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list,
                                  &g_attr_name_support);

ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST(ota_upgrade_cli_attr_list,
                                       &upgrade_server,
                                       &file_offset,
                                       &file_version,
                                       &stack_version,
                                       &downloaded_file_ver,
                                       &downloaded_stack_ver,
                                       &image_status,
                                       &manufacturer,
                                       &image_type,
                                       &min_block_reque,
                                       &image_stamp,
                                       &server_addr,
                                       &server_ep,
                                       0x0101,
                                       64,
                                       ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF);

zb_zcl_cluster_desc_t g_zigbee_cluster_list[] =
{
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_BASIC,
        ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),
        (basic_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),
        (identify_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_GROUPS,
        ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),
        (groups_attr_list),
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        0,
        NULL,
        ZB_ZCL_CLUSTER_CLIENT_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),
    ZB_ZCL_CLUSTER_DESC(
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        0,
        NULL,
        ZB_ZCL_CLUSTER_CLIENT_ROLE,
        ZB_ZCL_MANUF_CODE_INVALID
    ),

};

zb_zcl_cluster_desc_t g_zigbee_cluster_ota_list[] =
{
    ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
                        ZB_ZCL_ARRAY_SIZE(ota_upgrade_cli_attr_list, zb_zcl_attr_t),
                        (ota_upgrade_cli_attr_list),
                        ZB_ZCL_CLUSTER_CLIENT_ROLE,
                        ZB_ZCL_MANUF_CODE_INVALID
                       )
};
//=============================================================================
//                Simple desc definitions
//=============================================================================
ZB_DECLARE_SIMPLE_DESC(3, 2);
ZB_DECLARE_SIMPLE_DESC(0, 1);


ZB_AF_SIMPLE_DESC_TYPE(3, 2) simple_desc_switch_ep =
{
    SWITCH_EP,                                  /* Endpoint */
    ZB_AF_HA_PROFILE_ID,                           /* Application profile identifier */
    HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID,              /* Application device identifier */
    1,                                              /* Application device version */
    0,                                  /* Reserved */
    3,                  /* Application input cluster count */
    2,                 /* Application output cluster count */
    /* Application input and output cluster list */
    {
        ZB_ZCL_CLUSTER_ID_BASIC,
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
        ZB_ZCL_CLUSTER_ID_GROUPS,
        ZB_ZCL_CLUSTER_ID_ON_OFF,
        ZB_ZCL_CLUSTER_ID_IDENTIFY,
    }
};

ZB_AF_SIMPLE_DESC_TYPE(0, 1) simple_desc_switch_ota =
{
    10,                                              /* Endpoint */
    ZB_AF_HA_PROFILE_ID,                            /* Application profile identifier */
    HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID,              /* Application device identifier */
    1,                                              /* Application device version */
    0,                                              /* Reserved */
    0,                                              /* Application input cluster count */
    1,                                              /* Application output cluster count */
    /* Application input and output cluster list */
    {
        ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,
    }
};
ZB_AF_DECLARE_ENDPOINT_DESC(
    switch_ep,
    SWITCH_EP,
    ZB_AF_HA_PROFILE_ID,
    0,
    NULL,
    ZB_ZCL_ARRAY_SIZE(g_zigbee_cluster_list, zb_zcl_cluster_desc_t),
    g_zigbee_cluster_list,
    (zb_af_simple_desc_1_1_t *)&simple_desc_switch_ep,
    8,
    rep_ctx,
    1,
    NULL);

ZB_AF_DECLARE_ENDPOINT_DESC(
    switch_ota_ep,
    10,
    ZB_AF_HA_PROFILE_ID,
    0,
    NULL,
    ZB_ZCL_ARRAY_SIZE(g_zigbee_cluster_ota_list, zb_zcl_cluster_desc_t),
    g_zigbee_cluster_ota_list,
    (zb_af_simple_desc_1_1_t *)&simple_desc_switch_ota,
    0,
    NULL,
    0,
    NULL);

zb_af_endpoint_desc_t *ep_list_switch[] =
{
    &switch_ep,
    &switch_ota_ep
};

//zb_af_device_ctx_t simple_desc_switch_ctx =
//{
//    2,
//    ep_list_switch
//}

ZBOSS_DECLARE_DEVICE_CTX_2_EP(simple_desc_switch_ctx, switch_ep, switch_ota_ep);

uint32_t get_identify_time(void)
{
    return g_attr_identify_time;
}

void reset_attr(void)
{
    g_attr_identify_time = 0;
    g_attr_name_support = 0;
}

void z_get_upgrade_server_addr(uint8_t* in) 
{
    memcpy(in, upgrade_server, 8);
}

uint32_t z_get_file_version(void) 
{ 
    return file_version; 
}

void z_set_file_version(uint32_t ver) 
{ 
    file_version = ver; 
}

void z_set_OTA_status(uint8_t ver) 
{ 
    image_status = ver; 
}

void z_set_OTA_downloaded_file_version(uint32_t ver) 
{
    downloaded_file_ver = ver;
}

void z_set_OTA_downloaded_file_offset(uint32_t ver) 
{
    file_offset = ver;
}
