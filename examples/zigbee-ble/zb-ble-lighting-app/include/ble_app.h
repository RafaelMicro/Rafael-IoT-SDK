#ifndef LIGHTING_APP_BLE_APP_H
#define LIGHTING_APP_BLE_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ble_api.h"

typedef enum {
    QUEUE_TYPE_APP_REQ,
    QUEUE_TYPE_OTHERS,
} app_queue_param_type_t;

typedef enum {
    APP_REQUEST_IDLE,
    APP_REQUEST_ADV_START,
    APP_REQUEST_TRSPS_DATA_SEND,
    APP_REQUEST_FOTA_TIMER_EXPIRY,
} app_request_t;

typedef enum {
    STATE_STANDBY,
    STATE_ADVERTISING,
    STATE_CONNECTED,
} ble_state_t;

typedef struct {
    uint8_t host_id;
    uint16_t app_req;
} app_req_param_t;

typedef struct {
    uint8_t event;
    uint8_t from_isr;
    uint16_t param_type;
    union {
        app_req_param_t app_req;
        ble_tlv_t *pt_tlv;
    } param;
} app_queue_t;

#ifdef __cplusplus
}
#endif

#endif
