#ifndef __APP_TASK_H
#define __APP_TASK_H

#include <miu_port.h>
#include <openthread/thread.h>

typedef enum {
    APP_EVENT_CHANGE_ROLE,
    APP_EVENT_MAC_JOIN_RESPONSE_SEND,
#if CONFIG_APP_TASK_CENTRAL_ENABLE
    APP_EVENT_NET_MGM_ENROLL_UPDATE_SEND,
    APP_EVENT_NET_MGM_ENROLL_REQ_SEND,
    APP_EVENT_NET_MGM_ENROLL_REQ_SEND_TRYOUT,
#endif
} app_event_id_t;

typedef struct {
    app_event_id_t id;
    void* data;
} app_event_t;

void app_common_init();
void app_task(void);
void app_evt_single(void* data, app_event_id_t id);

void app_set_provisioning_mode(bool enable, uint32_t provision_time);
uint16_t app_provision_get_remain_time();

void app_join_request_handler();

#endif /* __APP_NET_MGM_H */
