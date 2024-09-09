// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __MESH_KEYGEN_H__
#define __MESH_KEYGEN_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

typedef struct
{
    /** LPN address */
    uint16_t lpn_address;
    /** Friend address */
    uint16_t friend_address;
    /** LPN counter value */
    uint16_t lpn_counter;
    /** Friend counter value */
    uint16_t friend_counter;
} keygen_friendship_secmat_params_t;

typedef struct
{
    /** Indicates whether the device key or the application is used. */
    bool is_device_key;
    /** Application ID. Calculated and used internally. */
    uint8_t aid;
    /** Application or device key storage. */
    uint8_t key[16];
} application_secmat_t;
/**
 * Nonce types.
 */

typedef struct __attribute((packed))
{
    uint8_t  type;              /**< Nonce type. */
    uint32_t ttl   :  7;        /**< Time to live. */
    uint32_t ctl   :  1;        /**< Control message bit. */
    uint32_t seq   : 24;        /**< Sequence number. */
    uint16_t src;               /**< Source address. */
    uint16_t padding;           /**< Padding zero bytes. */
    uint32_t iv_index;          /**< IV index. */
}
enc_nonce_net_t;

/**
 * Application nonce structure.
 */
typedef struct __attribute((packed))
{
    uint8_t  type;              /**< Nonce type. */
    uint32_t padding : 7;       /**< Padding bits. */
    uint32_t aszmic  : 1;       /**< Application MIC size bit if segmented message, 0 otherwise. */
    uint32_t seq     : 24;      /**< Sequence number. */
    uint16_t src;               /**< Source address. */
    uint16_t dst;               /**< Destination address. */
    uint32_t iv_index;          /**< IV index. */
}
enc_nonce_app_t;

/**
 * Proxy nonce structure.
 */
typedef struct __attribute((packed))
{
    uint8_t type;      /**< Nonce type. */
    uint8_t pad;       /**< Padding. */
    uint32_t seq : 24; /**< Sequence number. */
    uint16_t src;      /**< Source address. */
    uint16_t pad2;     /**< Padding zero bytes. */
    uint32_t iv_index; /**< IV index. */
}
enc_nonce_proxy_t;

/**
 * Device key nonce structure.
 */
typedef enc_nonce_app_t enc_nonce_dev_t;

void keygen_identify_key(uint8_t *pNetKeyPtr, uint8_t *pIdentifyKeyPtr);
uint8_t keygen_appkey_aid(uint8_t *pKeyPtr);

uint32_t keygen_network_secmat(uint8_t *p_netkey, network_secmat_t *p_secmat);
void net_secmat_get_by_nid(uint8_t nid, network_secmat_t **pp_secmat);
void friendship_net_secmat_get_by_nid(uint8_t nid, network_secmat_t **pp_secmat);

void net_secmat_local_get(network_secmat_t **pp_secmat);
void enc_nonce_generate(network_meta_t *p_net_metadata, enc_nonce_t type, uint8_t aszmic, uint8_t *p_nonce);
void keygen_networkID(uint8_t *p_in, uint8_t *p_out);
uint32_t keygen_friendship_secmat(uint8_t *p_netkey, keygen_friendship_secmat_params_t *p_params, network_secmat_t *p_secmat);
uint16_t keygen_virtual_address(uint8_t *puuid);


#ifdef __cplusplus
};
#endif
#endif /* __MESH_KEYGEN_H__ */
