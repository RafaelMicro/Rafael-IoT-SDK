// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __BCN_MAIN_H__
#define __BCN_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================


typedef struct __attribute((packed))
{
    uint8_t key_refresh        : 1; /**< Key refresh procedure initiated. */
    uint8_t iv_update          : 1; /**< IV update active. */
    uint8_t _rfu               : 6; /**< Reserved for future use. */
}
net_beacon_sec_flags_t;

/**
 * Secure network broadcast beacon payload.
 */
typedef struct __attribute((packed))
{
    net_beacon_sec_flags_t flags;                           /**< Beacon flags. */
    uint8_t                network_id[MESH_NETID_SIZE]; /**< Network identifier. */
    uint32_t               iv_index;                        /**< Current IV index. */
}
net_beacon_payload_t;

/**
 * Secure Network Broadcast Beacon.
 */
typedef struct __attribute((packed))
{
    net_beacon_payload_t payload;                    /**< Payload of the secure network beacon. */
    uint8_t              cmac[8]; /**< CMAC authentication value. */
}
net_beacon_t;

void bcn_init(void);
void bcn_set_beacon(beacon_set_param_t *pt_bcn_set_param);
void StartPbAdvBeacon(uint32_t interval, uint32_t duration);
void StartSecureBeacon(uint8_t Interval);
void StopSecureBeacon(void);

void StartPbGattBeacon(uint32_t interval, uint32_t duration);

void StartNodIdAdv(uint16_t Keyindex, uint8_t Interval);
void StopNodIdAdv(uint16_t Keyindex, uint8_t Interval);
void bcn_start_proxy_adv(uint8_t Interval);



bool BeaconMatch(const uint8_t *pUuid);
void beacon_packet_in(uint8_t *p_beacon_data, uint32_t data_len);


#ifdef __cplusplus
};
#endif
#endif /* __BCN_MAIN_H__ */
