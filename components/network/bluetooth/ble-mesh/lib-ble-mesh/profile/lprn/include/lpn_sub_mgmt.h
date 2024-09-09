#ifndef __MESH_LPN_SUB_MGMT_H__
#define __MESH_LPN_SUB_MGMT_H__

#include <stdint.h>



void mesh_lpn_submgmt_init(void);
int32_t mesh_lpn_submgmt_add(uint16_t address);
int32_t mesh_lpn_submgmt_remove(uint16_t address);
void mesh_lpn_submgmt_data_push(transport_control_packet_t *p_trs_ctrl_pkt);
void mesh_lpn_submgmt_data_clear(void);
uint16_t mesh_lpn_friend_subscription_size_get(void);
void submgmt_friend_subscription_confirm_handle(uint8_t *pData, uint16_t data_len, transport_packet_metadata_t *p_metadata);
void submgmt_lpn_fd_established(void);
void submgmt_lpn_fd_terminated(void);

#endif /* __MESH_LPN_SUB_MGMT_H__ */
