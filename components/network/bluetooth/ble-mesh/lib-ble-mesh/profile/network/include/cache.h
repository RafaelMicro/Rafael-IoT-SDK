// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_CACHE_H__
#define __MESH_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

void net_cache_init(void);
bool net_cache_entry_exists(uint16_t src_addr, uint32_t sequence_number);
void net_cache_entry_add(uint16_t src, uint32_t seq);
void net_cache_clear(void);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_CACHE_H__ */
