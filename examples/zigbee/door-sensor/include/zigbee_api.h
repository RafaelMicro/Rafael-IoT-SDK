/**
 * @file zigbee_api.h
 * @brief
 *  
 * @version 0.1
 * 
 * @date 
 * 
 */

#ifndef __ZIGBEE_API_H
#define __ZIGBEE_API_H

#include <stdint.h>
#include <zigbee_platform.h>
#include "fota_define.h"
#include "hosal_gpio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#define DOOR_SENSOR_EP 1

#define ZIGBEE_APP_NOTIFY_ISR(ebit)                                            \
    (g_zb_app_evt_var |= ebit);                                                \
    zb_app_signal()
#define ZIGBEE_APP_NOTIFY(ebit)                                                \
    vPortEnterCritical();                                                      \
    g_zb_app_evt_var |= ebit;                                                  \
    vPortExitCritical();                                                       \
    zb_app_signal()
#define ZIGBEE_APP_GET_NOTIFY(ebit)                                            \
    vPortEnterCritical();                                                      \
    ebit = g_zb_app_evt_var;                                                   \
    g_zb_app_evt_var = ZB_APP_EVENT_NONE;                                      \
    vPortExitCritical()

typedef enum {
    ZB_APP_EVENT_NONE = 0,
    ZB_APP_EVENT_INIT = 0x00000001,
    ZB_APP_EVENT_NOT_JOINED = 0x00000002,
    ZB_APP_EVENT_JOINED = 0x00000004,
    ZB_APP_EVENT_FACTORY_RESET = 0x00000008,


    ZB_APP_EVENT_ALL = 0xffffffff,
} zb_app_event_t;

zb_app_event_t g_zb_app_evt_var;
extern zb_af_device_ctx_t simple_desc_door_sensor_ctx;
void zigbee_app_init(void);
void app_main_loop(void* parameters_ptr);
void zb_app_signal(void);
void zigbee_app_nwk_start(uint32_t channel_mask, uint32_t max_child, uint32_t reset);
void zigbee_start_identify(void);
void zigbee_zcl_set_attrubute(uint8_t ep, uint16_t cluster, uint8_t role, uint16_t attr_id, uint8_t* val);

void reset_attr(void);
uint32_t get_identify_time(void);
void z_get_upgrade_server_addr(uint8_t* in);
uint32_t z_get_file_version(void);
void z_set_file_version(uint32_t ver);
void z_set_OTA_status(uint8_t ver);
void z_set_OTA_downloaded_file_version(uint32_t ver);
void z_set_OTA_downloaded_file_offset(uint32_t ver);
#endif // __ZIGBEE_API_H
