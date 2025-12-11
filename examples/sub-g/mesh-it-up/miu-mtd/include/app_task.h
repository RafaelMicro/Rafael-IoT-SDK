#ifndef __APP_TASK_H
#define __APP_TASK_H

#include <miu_port.h>
#include <openthread/thread.h>

typedef enum {
    APP_EVENT_RETRY_JOIN_NOW,
    APP_EVENT_JOIN_SUCCESS,
    APP_EVENT_JOIN_FAILED,
    APP_EVENT_CHANGE_ROLE,
#if CONFIG_APP_TASK_CENTRAL_ENABLE
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

void app_join_response_handler(uint16_t panid, uint8_t* net_key);

#endif /* __APP_NET_MGM_H */
