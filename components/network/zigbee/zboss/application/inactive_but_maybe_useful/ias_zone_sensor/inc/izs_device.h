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
/* PURPOSE: General IAS zone device declaration
*/

#ifndef IZS_DEVICE_H
#define IZS_DEVICE_H 1

#include "zboss_api.h"
#include "zb_ringbuffer.h"
#include "izs_avg_val.h"
#include "izs_hal.h"
#include "izs_config.h"

/* Common constants. */
#define IZS_RESTART_JOIN_AFTER_LEAVE_DELAY (ZB_MILLISECONDS_TO_BEACON_INTERVAL(5000))
#define IZS_JOIN_RETRY_DELAY(_cnt) (ZB_MILLISECONDS_TO_BEACON_INTERVAL((zb_time_t)(5000) * ((_cnt) + 1)))
#define IZS_NOT_ENROLLED_TIMEOUT (2 * ZB_TIME_ONE_SECOND)
#define IZS_WAIT_MATCH_IAS_ZONE (10 * ZB_TIME_ONE_SECOND)

#define IZS_FIRST_JOIN_ATTEMPT 0
#define IZS_RETRY_JOIN_ATTEMPT 1
/* try to join not more than 20 times, go sleep after join retry limit is exceeded */
/* take into accoun the first join attempt */
#define IZS_JOIN_LIMIT 19


/* Retry backoff constants */
#define IZS_RETRY_BACKOFF_FIRST_TIMEOUT 1 /* seconds */
#define IZS_RETRY_BACKOFF_MAX_TIMEOUT   30*60 /* 30 minutes */
#define IZS_RETRY_BACKOFF_MAX_COUNT (11)

/* IAS Zone cluster attributes data */
#define IZS_INIT_ZONE_STATUS        (ZB_ZCL_IAS_ZONE_ZONE_STATUS_RESTORE)
#define IZS_INIT_ZONE_ID            ZB_ZCL_IAS_ZONEID_ID_DEF_VALUE
#define IZS_INIT_CIE_SHORT_ADDR     0xffff
#define IZS_INIT_CIE_ENDPOINT       0x0

/* Poll Control cluster attributes data */
#define FAST_POLLING_DURING_COMMISSIONING
#define IZS_DEVICE_CHECKIN_INTERVAL           (27*60*4) /* 27 min units of 250 ms */
#define IZS_DEVICE_LONG_POLL_INTERVAL         5*60*4  /* 5 min in units of 250 ms */

#define IZS_DEVICE_SHORT_POLL_INTERVAL         2	    /* 1/2 sec in units of 250 ms */
#define IZS_DEVICE_MIN_CHECKIN_INTERVAL        0x50  /* 80 quarterseconds == 20 sec */
#define IZS_DEVICE_MIN_POLL_CONTROL_INTERVAL   0x01	/* 1 quarterseconds == 250 msec */
#define IZS_DEVICE_MIN_LONG_POLL_INTERVAL      4

#define IZS_DEVICE_TURBO_POLL_AFTER_JOIN_DURATION      (60 /* seconds */)
#define IZS_DEVICE_TURBO_POLL_AFTER_CIE_ADDR_DURATION  (/* 3 minutes = */ 180 /*, unit = seconds */)
#define IZS_DEVICE_TURBO_POLL_AFTER_ENROLL_DURATION    (/* 3 minutes = */ 180 /*, unit = seconds */)
#define IZS_DEVICE_TURBO_POLL_ENROLL_REQ_DURATION      (/* 3 minutes = */ 180 /*, unit = seconds */)

#define IZS_DEVICE_COMMISSIONING_POLL_RATE 2000

/* Battery attribute variables */
#define IZS_INIT_BATTERY_VOLTAGE        0x0
#define IZS_INIT_BATTERY_QUANTITY 2
#define IZS_INIT_BATTERY_ALARM_MASK 		0
#define IZS_INIT_BATTERY_ALARM_STATE		0

/*** Parameters for battery measurement ***/
#define IZS_BATTERY_MONITORING_ARRAY_SIZE 16

#define IZS_BATTERY_VOLTAGE_INVALID_THRESHOLD 0
#define IZS_BATTERY_VOLTAGE_60_DAYS_THRESHOLD 2500  /* scale - 1mV*/
#define IZS_BATTERY_VOLTAGE_60_DAYS_THRESHOLD_PERCENTAGE 30

#define IZS_BATTERY_DEFECT_DELTA 300 /* 300 mV */

#define IZS_BATTERY_THRESHOLD_DELTA 100 /*+/- about threshold (100 mV), used to take into account hysteresis */

#ifdef IAS_ACE_APP
#define IAS_ACE_ZONE_TABLE_LIST_SIZE 3

#define IAS_ACE_ZONE_PANEL_STATUS_LIST_SIZE 5

#endif
/*************************************************************************/

/* attributes of Basic cluster */
typedef struct izs_device_basic_attr_s
{
  zb_uint8_t zcl_version;
  zb_uint8_t app_version;
  zb_uint8_t stack_version;
  zb_uint8_t hw_version;
  zb_char_t mf_name[32];
  zb_char_t model_id[16];
  zb_char_t date_code[10];
  zb_uint8_t power_source;
  zb_char_t location_id[5];
  zb_uint8_t ph_env;
  zb_char_t sw_build_id[3];
}
izs_device_basic_attr_t;

/* attributes of Identify cluster */
typedef struct izs_device_identify_attr_s
{
  zb_uint16_t identify_time;
}
izs_device_identify_attr_t;


/*************************************************/

/* Poll Control cluster attributes data */
typedef struct izs_device_poll_control_attr_s
{
  zb_uint32_t checkin_interval;
  zb_uint32_t long_poll_interval;
  zb_uint16_t short_poll_interval;
  zb_uint16_t fast_poll_timeout;
  zb_uint32_t checkin_interval_min;
  zb_uint32_t long_poll_interval_min;
  zb_uint16_t fast_poll_timeout_max;
}
izs_device_poll_control_attr_t;

/* OTA Upgrade client cluster attributes data */
typedef struct izs_device_ota_attr_s
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
izs_device_ota_attr_t;

typedef struct izs_device_power_config_attr_s
{
  zb_uint8_t voltage;
  zb_uint8_t size;
  zb_uint8_t quantity;
  zb_uint8_t rated_voltage;
  zb_uint8_t alarm_mask;
  zb_uint8_t voltage_min_threshold;
  zb_uint8_t remaining;
  zb_uint8_t threshold1;
  zb_uint8_t threshold2;
  zb_uint8_t threshold3;
  zb_uint8_t min_threshold;
  zb_uint8_t percent_threshold1;
  zb_uint8_t percent_threshold2;
  zb_uint8_t percent_threshold3;
  zb_uint32_t alarm_state;
}
izs_device_power_config_attr_t;

typedef struct zb_ota_upgrade_ctx_s
{
  zb_uint32_t total_image_size;
  zb_uint32_t addr_to_erase;
  zb_uint32_t address;          /* Supposed to be constant value, init
                                 * on OTA Upgrade start  */
  void       *flash_dev;
  zb_uint32_t fw_version;
  zb_uint8_t  param;
} zb_ota_upgrade_ctx_t;

/*-----------------------------------------------------------------*/

typedef enum izs_device_state_e
{
  IZS_STATE_APP_NOT_INIT = 0,
  IZS_STATE_HW_INIT,                   /* app just started up, H/W is initialized */
  IZS_STATE_IDLE,                      /* device is idle (after leave) */
  IZS_STATE_START_JOIN,                /* join is started */
  IZS_STATE_STARTUP_COMPLETE,          /* startup complete with SUCCESS status */
  IZS_STATE_REJOIN_BACKOFF,            /* rejoin backoff is in progress */
  IZS_STATE_RESET_TO_DEFAULT,          /* reset to default */
  IZS_STATE_NO_NWK_SLEEP,              /* join failed, go sleep */
  IZS_STATE_SENSOR_NORMAL              /* Sensor App normal processing */
}
izs_device_state_t;


void izs_set_device_state(izs_device_state_t new_state);
#define IZS_SET_DEVICE_STATE(new_state) izs_set_device_state(new_state)
#define IZS_GET_DEVICE_STATE() (g_device_ctx.device_state)

#define IZS_DEVICE_CIE_ADDR() (g_device_ctx.zone_attr.cie_addr)
#define IZS_DEVICE_ZONE_STATE() (g_device_ctx.zone_attr.zone_state)

/* Check if device is enrolled or not */
#define IZS_DEVICE_IS_ENROLLED() (g_device_ctx.zone_attr.zone_state == ZB_ZCL_IAS_ZONE_ZONESTATE_ENROLLED)

/* IZS_DEVICE_GET_ZONE_STATUS must return the current sone status, rather than the zone status
   that is being transfered to the remote - chances are that the remote delays the APS ack
   . . . during that delay, new status changes may occur (that get reflected in the IAS Zone Queue) */
#define IZS_DEVICE_GET_ZONE_STATUS() (g_device_ctx.current_zone_state)


/****************** IAS Zone Queue declaration ***************************/
#define IZS_IAS_ZONE_QUEUE_SIZE 6

typedef struct izs_ias_zone_info_s
{
  zb_uint16_t ias_status;
  zb_uint16_t timestamp;        // qsec
}
izs_ias_zone_info_t;

ZB_RING_BUFFER_DECLARE(izs_ias_zone_queue, izs_ias_zone_info_t, IZS_IAS_ZONE_QUEUE_SIZE);

typedef struct izs_ias_zone_attr_s
{
  zb_uint16_t zone_type;
  zb_uint16_t zone_status;
  zb_uint8_t zone_id;
  zb_uint8_t zone_state;
  zb_ieee_addr_t cie_addr;
  zb_uint16_t cie_short_addr;
  zb_uint8_t cie_ep;
  zb_uint8_t number_of_zone_sens_levels_supported;
  zb_uint8_t current_zone_sens_level;
}
izs_ias_zone_attr_t;

#ifdef IAS_ACE_APP
typedef struct izs_ias_ace_attr_s
{
  zb_zcl_ias_ace_zone_table_t ias_ace_zone_table_list[IAS_ACE_ZONE_TABLE_LIST_SIZE];
  zb_uint8_t ias_ace_zone_table_length;
}
izs_ias_ace_attr_t;
#endif

/*** Production config data ***/
typedef ZB_PACKED_PRE struct izs_production_config_t
{
  zb_char_t manuf_name[16];
  zb_char_t model_id[16];
}
ZB_PACKED_STRUCT izs_production_config_t;

typedef struct izs_device_ctx_s
{
  izs_device_state_t device_state;
  zb_uint8_t join_counter;

  izs_ias_zone_attr_t zone_attr;
  izs_device_basic_attr_t basic_attr;
  izs_device_identify_attr_t identify_attr;
  izs_device_poll_control_attr_t poll_control_attr;
  izs_device_ota_attr_t ota_attr;
  izs_device_power_config_attr_t pwr_cfg_attr;

  zb_ota_upgrade_ctx_t ota_ctx;

  izs_ias_zone_queue_t ias_zone_queue;
  zb_bufid_t out_buf; /* buffer is used for sending notifications */
  zb_uint16_t current_zone_state;
  zb_uint16_t retry_backoff_cnt; /* Retry backoff counter */

  /* This flag is used on HW detector fails. Resets only on device restart. */
  zb_bool_t detector_trouble;
  zb_bool_t battery_low;
  zb_bool_t enroll_req_generated;

#ifdef IAS_ACE_APP
  izs_ias_ace_attr_t ias_ace_attr;
#endif

  zb_bitfield_t check_in_started:1; /* flag to check if check in is already started or not  */

}
izs_device_ctx_t;

/*** Battery definitions  ***/
IZS_DECLARE_AVG_DATA_TYPE(izs_voltage_data_t, IZS_BATTERY_MONITORING_ARRAY_SIZE)

extern izs_device_ctx_t g_device_ctx;

#ifdef IAS_ACE_APP
extern zb_zcl_ias_ace_panel_status_changed_t g_ias_ace_zone_panel_status;
extern zb_uint8_t g_ias_ace_zone_table_list_bypassed_status[IAS_ACE_ZONE_TABLE_LIST_SIZE];
#endif


/*** App init/deinit ***/
void izs_basic_reset_to_defaults(zb_uint8_t param);
void izs_device_app_init(zb_uint8_t param);
void izs_app_ctx_init();
void izs_critical_error();

/*** General callbacks ***/
void izs_identify_notification(zb_uint8_t param);
void izs_device_interface_cb(zb_uint8_t param);

/*** startup sequence routines ***/
void izs_joined_cont(zb_uint8_t param);
void izs_device_hw_init(zb_uint8_t param);
void izs_start_join(zb_uint8_t param);
void izs_retry_join();
void izs_full_reset_to_defaults(zb_uint8_t param);
void izs_go_on_guard(zb_uint8_t param);

/*** Notification send routines ***/
void izs_send_notification();
void izs_ias_zone_notification_cb(zb_uint8_t param);
void izs_resend_notification(zb_uint8_t param);
void izs_send_enroll_req(zb_uint8_t param);

/*** IAS Zone Status update routines ***/
void izs_update_ias_zone_status(zb_uint16_t status_bit, zb_uint8_t status_bit_value);
void izs_read_sensor_status(zb_uint8_t param);

/*** Retry backoff ***/
void izs_retry_backoff(void);

/*** Queue routines ***/
void izs_ias_zone_queue_init();
void izs_ias_zone_queue_put(zb_uint16_t new_zone_state);
izs_ias_zone_info_t* izs_ias_zone_get_element_from_queue();
void izs_ias_zone_release_element_from_queue();
zb_bool_t izs_ias_zone_queue_is_empty();

/*** Battery API ***/
void izs_read_loaded_battery_voltage(zb_uint8_t param);
void izs_read_unloaded_battery_voltage(zb_uint8_t param);
void izs_set_battery_voltage(zb_uint16_t voltage);
void izs_check_battery_defect(zb_uint16_t avg_loaded, zb_uint16_t avg_unloaded);
void izs_measure_batteries();

/*** Measurement & Reporting API ***/
void izs_check_in_cb(zb_uint8_t param);
void izs_configure_power_config_default_reporting(zb_uint8_t param);

/*** ZB network API ***/
void izs_find_ias_zone_client(zb_uint8_t param, zb_callback_t cb);
void izs_leave_nwk(zb_uint8_t param);
void izs_leave_nwk_cb(zb_uint8_t param);

/*** OTA specific API ***/
#ifdef IZS_OTA
void izs_check_and_get_ota_server(zb_uint8_t param);


#ifdef ZB_USE_OSIF_OTA_ROUTINES
zb_uint8_t izs_ota_upgrade_start(zb_uint32_t image_size,
                                 zb_uint32_t image_version);
zb_ret_t izs_ota_upgrade_write_next_portion(zb_uint8_t *ptr, zb_uint32_t off, zb_uint8_t len);
zb_uint8_t izs_ota_upgrade_check_fw(zb_uint8_t param);
void izs_ota_upgrade_mark_fw_ok();
void izs_ota_upgrade_abort();
#endif /* ZB_USE_OSIF_OTA_ROUTINES */
#endif /* IZS_OTA */

zb_uint16_t izs_get_current_time_qsec();
zb_uint16_t izs_calc_time_delta_qsec(zb_uint16_t t);

void izs_device_interface_cb(zb_uint8_t param);

#endif  /* IZS_DEVICE_H */
