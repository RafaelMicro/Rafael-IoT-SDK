// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef __MESH_LPN_H__
#define __MESH_LPN_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

/**
 * Minimum Friend Request timeout (in milliseconds).
 *
 * @note The timeout starts counting from the time the Friend Request is sent. The device does not start
 * scanning until 100 milliseconds have passed from the Friend Request.
 * See @tagMeshSp section 3.6.6.4.1.
 */
#define MESH_LPN_FRIEND_REQUEST_TIMEOUT_MIN_MS 100
/** Maximum Friend Request timeout (in milliseconds). */
#define MESH_LPN_FRIEND_REQUEST_TIMEOUT_MAX_MS 1000

/** Minimum supported poll timeout (in milliseconds). */
#define MESH_LPN_POLL_TIMEOUT_MIN_MS 1000
/** Maximum supported poll timeout (in milliseconds). This translates to slightly less than
 * 96 hours (0x34BBFF * 100 ms). The valid range is defined in Table 3.26, @tagMeshSp section 3.6.5.3 */
#define MESH_LPN_POLL_TIMEOUT_MAX_MS 345599900

/* See @tagMeshSp section 3.6.5.3 Friend Request */
/** Minimum allowed Receive Delay (in milliseconds). */
#define MESH_LPN_RECEIVE_DELAY_MIN_MS 10
/** Maximum allowed Receive Delay (in milliseconds). */
#define MESH_LPN_RECEIVE_DELAY_MAX_MS 255

/** The number of times the LPN will retry polling the Friend before regarding the friendship as
 * terminated. */
#ifndef MESH_LPN_POLL_RETRY_COUNT
#define MESH_LPN_POLL_RETRY_COUNT 5
#endif

/** The number of times the LPN will retry the friend request procedure until @ref NRF_MESH_EVT_LPN_FRIEND_REQUEST_TIMEOUT. */
#ifndef MESH_LPN_FRIEND_REQUEST_RETRY_COUNT
#define MESH_LPN_FRIEND_REQUEST_RETRY_COUNT 5
#endif

/** The minimum interval between two individual consecutive polls.  */
#ifndef MESH_LPN_POLL_SEPARATION_INTERVAL_MS
#define MESH_LPN_POLL_SEPARATION_INTERVAL_MS 50
#endif


typedef struct
{
    uint8_t rssi_factor : 2;
    uint8_t receive_window_factor : 2;
    uint8_t friend_queue_size_min_log : 3;
} friend_criteria_t;

typedef struct
{
    uint32_t receive_delay_ms;
    uint32_t poll_timeout_ms;
    friend_criteria_t friend_criteria;
} mesh_lpn_friend_request_t;

typedef struct
{
    uint16_t src;
    struct
    {
        uint16_t friend_counter;
        uint8_t receive_window_ms;
        uint8_t friend_queue_size;
        uint8_t subscription_list_size;
        int8_t measured_rssi;
    } offer;
    network_secmat_t *p_net;
} mesh_lpn_offer_t;

void mesh_lpn_init(void);
int32_t mesh_lpn_friend_request(mesh_lpn_friend_request_t friend_params, uint32_t request_timeout_ms);
uint32_t mesh_lpn_friend_offer_handler(uint8_t *pData, uint16_t data_len, transport_packet_metadata_t *p_metadata);
int32_t mesh_lpn_friend_accept(mesh_lpn_offer_t *p_friend_offer, transport_packet_metadata_t *pt_metadat);
int32_t mesh_lpn_friend_poll(uint32_t delay_ms);
int32_t mesh_lpn_poll_interval_set(uint32_t poll_interval_ms);
uint32_t mesh_lpn_friendship_terminate(void);
uint32_t mesh_lpn_friend_update_handler(uint8_t *pData, uint16_t data_len, transport_packet_metadata_t *p_metadata);
bool mesh_lpn_is_in_friendship(void);
void mesh_lpn_rx_notify(network_meta_t *p_net_metadata);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_LPN_H__*/
