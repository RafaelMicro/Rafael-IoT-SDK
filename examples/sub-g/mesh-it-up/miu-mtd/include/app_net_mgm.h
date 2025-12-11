#ifndef __APP_NET_MGM_H
#define __APP_NET_MGM_H

#include <miu_port.h>
#include <openthread/thread.h>

extern uint8_t enroll_send_try;
extern uint8_t enroll_send_time;
extern bool enroll_done;

void net_mgm_init(otInstance* instance);

int net_mgm_enroll_req_send(otInstance* instance);
void net_mgm_enroll_req_received_done(int status, uint16_t comission_time);

#endif /* __APP_NET_MGM_H */
