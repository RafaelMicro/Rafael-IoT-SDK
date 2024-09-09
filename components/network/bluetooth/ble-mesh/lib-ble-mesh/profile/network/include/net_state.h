// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __NET_STATE_H__
#define __NET_STATE_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================



#define NETWORK_IVI_MASK     (0x00000001)

typedef struct
{
    uint32_t seqnum;                        /**< Sequence number. */
    uint32_t seqnum_max_available;          /**< The highest sequence number available for use. */
    uint32_t iv_index;                      /**< IV index. */
    bool     isUpdateActive;
    bool     pending;                           /**< An IV update state change has been triggered, awaiting timer. */
    bool     locked;                            /**< Changes to IV index or update state has been locked by an external module. */
    network_state_iv_update_t state;        /**< IV update state. */
    uint16_t timeout_counter;               /**< Counter for IV update procedure timeout, in minutes. */
    uint16_t ivr_timeout_counter;           /**< Counter for IV recovery procedure timeout, in minutes. */
} network_state_t;


uint32_t net_state_iv_index_update(uint32_t iv_index);
void net_state_ivi_flag_update(bool PrvFlag);

uint32_t net_state_rx_iv_index_get(uint8_t ivi);
uint32_t net_state_get_current_ivi(void);

bool net_state_get_ivi_updt_flag(void);
network_state_iv_update_t net_state_iv_update_get(void);
void net_state_iv_updata_state_set(network_state_iv_update_t state);

#ifdef __cplusplus
};
#endif
#endif /* __NET_STATE_H__ */
