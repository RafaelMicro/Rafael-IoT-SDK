// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __FRIEND_H__
#define __FRIEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t mesh_friend_init(void);

void friend_poll_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);
void friend_request_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);

void friend_clear_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);
void friend_clear_confirm_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);
void friend_sublist_add_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);


void friend_sublist_remove_handle(uint8_t *p_data, uint16_t data_len, transport_packet_metadata_t *p_metadata);

void mesh_friend_enable(void);

void friend_packet_in(packet_mesh_trs_packet_t *p_packet, uint8_t length, transport_packet_metadata_t *p_metadata);

bool friend_needs_packet(transport_packet_metadata_t *p_metadata);
void friend_sar_complete(uint16_t src, uint32_t seqzero, bool success);
bool friend_sar_exists(uint16_t src, uint64_t seqauth);
bool friend_friendship_established(uint16_t src);
bool friend_need_subscribe_address(uint16_t dst);

bool friend_device_key_get(uint16_t src, uint8_t *p_device_key);
bool friend_device_key_set(uint16_t src, uint8_t *p_device_key);

#ifdef __cplusplus
};
#endif
#endif /* __FRIEND_H__ */
