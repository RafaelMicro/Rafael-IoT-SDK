// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __FRIEDNSHIP_QUEUE_H__
#define __FRIEDNSHIP_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "util_queue.h"
#include "transport.h"
#include "mesh_def.h"
#include "pkt_mesh.h"

typedef struct
{
    uint8_t length;      /**< Length of the packet. */
    uint16_t set_id;     /**< Packet ID. Packets with the same ID are deleted at the same time on overflow. */

    /** Network metadata for the packet. */
    struct
    {
        uint8_t ttl;
        bool control_packet;
        uint16_t src;
        uint16_t dst;
        uint32_t seqnum;
        uint32_t iv_index;
    } net_metadata;

    /** Packet buffer itself. */
    packet_mesh_trs_packet_t packet;
    /** Queue node used for keeping track of the packet. */
    queue_elem_t queue_elem;
} friend_packet_t;

typedef struct
{
    uint16_t src;     /**< Source address of the SAR session. */
    uint16_t set_id;  /**< Set ID of this SAR session. All SAR packets get the same set_id. */
    queue_t segments; /**< Queue for all segments in the SAR session. */
} friend_queue_sar_session_t;

/** Friend queue statistics. */
typedef struct
{
    struct
    {
        uint32_t free;      /**< Number of free packets at this moment. */
        uint32_t min_free;  /**< Lowest number of free packets the queue has ever seen. */
        uint32_t sent;      /**< Number of packets that have been passed to the user. */
        uint32_t discarded; /**< Number of packets that have been discarded to make room for newer packets. */
    } packets;
    struct
    {
        uint32_t successful; /**< Number of SAR sessions that have been added to the queue. */
        uint32_t failed;     /**< Number of SAR sessions that have failed before they were added to the queue. */
        uint32_t pending;    /**< Number of pending SAR sessions at this moment. */
    } sar_sessions;
} friend_queue_stats_t;

/** Queue instance. */
typedef struct
{
    friend_packet_t buffer[MESH_FRIEND_QUEUE_SIZE]; /**< Packet buffer in which all packets are allocated. */
    queue_t committed_packets;                      /**< Queue of committed packets. */
    queue_t free_packets;                           /**< Queue of free packets. */
    friend_queue_sar_session_t sar_sessions[TRANSPORT_SAR_SESSIONS_MAX]; /**< Active SAR sessions. */
    uint16_t next_set_id;                           /**< Set ID counter. */
    bool user_has_packet;                           /**< Flag indicating that the user has gotten the oldest packet returned in a
                                                         call to @ref friend_queue_packet_get() since the previous call to
                                                         @ref friend_queue_packet_release(). */
#if FRIEND_DEBUG
    friend_queue_stats_t stats; /**< Statistics for this queue instance. */
#endif
} friend_queue_t;

/**
 * Initializes a Friend Queue instance.
 *
 * @param[in,out] p_queue     Friend Queue instance to initialize.
 */
void friend_queue_init(friend_queue_t *p_queue);

/**
 * Get a packet from the Friend Queue.
 *
 * Gets the oldest packet from the queue. The packet will not be removed from the queue, but the
 * packet may be freed automatically between calls to make space for new packets.
 *
 * @note The returned packet pointer is only valid until the next friend queue API call, and should
 * never be kept across contexts.
 *
 * @param[in,out] p_queue     Queue to pop from.
 *
 * @returns Pointer to the first packet in the queue, or NULL if no packets are pending.
 */
const friend_packet_t *friend_queue_packet_get(friend_queue_t *p_queue);

/**
 * Releases the packet returned from the previous @ref friend_queue_packet_get() call.
 *
 * Frees the memory associated with the packet that was last returned from @ref friend_queue_packet_get()
 * if it hasn't already been freed by internal housekeeping.
 *
 * @param[in,out] p_queue Queue to release packet from.
 */
void friend_queue_packet_release(friend_queue_t *p_queue);

/**
 * Pushes a packet to the Friend Queue.
 *
 * The packet will be added to the Friend Queue according to a set of rules enforced by the mesh specification.
 * A packet that is pushed can be removed by overflow before it is removed from the queue.
 *
 * @param[in,out]     p_queue     Queue to push to.
 * @param[in]         p_packet    Packet to push.
 * @param[in]         length      Length of the packet.
 * @param[in]         p_metadata  Transport metadata tied to the packet.
 * @param[in]         role        Role of this device for the packet.
 */
void friend_queue_packet_push(friend_queue_t *p_queue,
                              packet_mesh_trs_packet_t *p_packet,
                              uint8_t length,
                              transport_packet_metadata_t *p_metadata);

/**
 * Notify the Friend Queue that a SAR transaction is complete.
 *
 * @param[in,out]     p_queue     Queue to notify.
 * @param[in]         src         Source address of the SAR transaction.
 * @param[in]         success     Flag indicating whether the transaction is successful.
 */
void friend_queue_sar_complete(friend_queue_t *p_queue, uint16_t src, bool success);

/**
 * Check whether a packet with the specified SeqAuth is in the queue.
 *
 * @param[in,out]     p_queue     Queue to check.
 * @param[in]         src         Source address of the SAR transaction.
 * @param[in]         seqauth     SeqAuth to check.
 *
 * @retval true The packet is in the queue.
 * @retval false The packet is not in the queue.
 */
bool friend_queue_sar_exists(friend_queue_t *p_queue, uint16_t src, uint64_t seqauth);

/**
 * Checks whether the Friend Queue is empty.
 *
 * @param[in] p_queue Queue to check.
 *
 * @retval @c true   The queue is empty.
 * @retval @c false  The queue is not empty.
 */
bool friend_queue_is_empty(friend_queue_t *p_queue);

/**
 * Clears the Friend Queue to an empty state.
 *
 * @warning All data present in the queue will be lost.
 */
void friend_queue_clear(friend_queue_t *p_queue);

/**
 * Calculates and returns committed packets for LPN.
 *
 * @param[in] p_queue Queue to check.
 *
 * @retval @c Number of committed packets.
 */
uint32_t friend_queue_packet_counter_get(friend_queue_t *p_queue);


#ifdef __cplusplus
};
#endif
#endif /* __FRIEDNSHIP_QUEUE_H__ */
