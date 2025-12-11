#ifndef __APP_NET_MGM_H
#define __APP_NET_MGM_H

#include <miu_port.h>
#include <openthread/thread.h>

#define NET_MGM_NODE_SURVIVAL_TIME 300 //5 minutes, at least 2 minutes

extern uint16_t enroll_update_timeout;
extern uint8_t enroll_send_try;
extern uint8_t enroll_send_time;
extern bool enroll_done;

typedef struct {
    uint8_t used;
    uint8_t role;
    uint16_t parent;
    uint16_t rloc;
    uint8_t extaddr[OT_EXT_ADDRESS_SIZE];
    int8_t rssi;
    uint32_t version;
    uint16_t survivaltime;
} net_mgm_node_table_t;

bool net_mgm_check_leader_pin_state();
void net_mgm_node_table_num();
void net_mgm_node_table_display();
void net_mgm_init(otInstance* instance);

int net_mgm_enroll_update_send(otInstance* instance);
int net_mgm_enroll_req_send(otInstance* instance);
int net_mgm_node_table_add(net_mgm_node_table_t* net_mgm_node_info);
void net_mgm_enroll_req_received_done(int status, uint16_t comission_time);
void net_mgm_node_survivaltime_update(uint8_t role, uint16_t parent_rloc,
                                      uint16_t self_rloc, int8_t rssi);

#endif /* __APP_NET_MGM_H */
