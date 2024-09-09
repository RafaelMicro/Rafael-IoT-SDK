// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __FRIEDNSHIP_TYPE_H__
#define __FRIEDNSHIP_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** Shortest receive window supported by the Friend node. */
#define MESH_FRIEND_RECEIVE_WINDOW_MIN_MS (1)
/** Longest receive window supported by the Friend node. */
#define MESH_FRIEND_RECEIVE_WINDOW_MAX_MS (255)
/**
 * Default receive window offered by the Friend node.
 * @note You can later change this with @ref mesh_friend_receive_window_set().
 */
#define MESH_FRIEND_RECEIVE_WINDOW_DEFAULT_MS (50)

typedef enum
{
    FRIENDSHIP_RSSI_FACTOR_1_0 = 0,
    FRIENDSHIP_RSSI_FACTOR_1_5 = 1,
    FRIENDSHIP_RSSI_FACTOR_2_0 = 2,
    FRIENDSHIP_RSSI_FACTOR_2_5 = 3
} friendship_rssi_factor_t;

/**
 * Weight factor applied to the offered Receive Window by the Friend node.
 *
 * Available factors are 1.0, 1.5, 2.0, and 2.5. The lower the factor, the less a long offered
 * Receive Window increases the delay of the Friend Offer. See the equation in @ref
 * mesh_lpn_friend_request_t.friend_criteria.
 */
typedef enum
{
    FRIENDSHIP_RECEIVE_WINDOW_FACTOR_1_0 = 0,
    FRIENDSHIP_RECEIVE_WINDOW_FACTOR_1_5 = 1,
    FRIENDSHIP_RECEIVE_WINDOW_FACTOR_2_0 = 2,
    FRIENDSHIP_RECEIVE_WINDOW_FACTOR_2_5 = 3
} friendship_receive_window_factor_t;

/**
 * Minimum size of the Friend Queue.
 *
 * This is the minimum number of Lower Transport PDUs that the Friend node can store.
 */
typedef enum
{
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_PROHIBITED = 0,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_2          = 1,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_4          = 2,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_8          = 3,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_16         = 4,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_32         = 5,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_64         = 6,
    FRIENDSHIP_MIN_FRIEND_QUEUE_SIZE_128        = 7
} mesh_friendship_min_friend_queue_size_t;

typedef struct
{
    uint16_t src;               /**< LPN source address. */
    uint16_t prev_friend_src;   /**< Source of the previous Friend. */
    uint16_t element_count;     /**< Number of elements in the LPN. */
    uint16_t request_count;     /**< Number of Friend Requests sent by the LPN. */
} mesh_friendship_lpn_t;

typedef struct
{
    mesh_friendship_lpn_t lpn;  /**< Low Power node data. */
    uint32_t poll_timeout_ms;   /**< Poll Timeout in milliseconds. */
    uint32_t poll_countdown;    /**< Poll coundown value. */
    uint32_t poll_count;        /**< Number of polls received from the LPN. */
    uint8_t receive_delay_ms;   /**< Delay before the receive window starts, in milliseconds. */
    uint8_t receive_window_ms;  /**< Length of the LPN's Receive Window in milliseconds. */
    int8_t avg_rssi;            /**< Average RSSI of the LPN messages received. */
} mesh_friendship_t;



#ifdef __cplusplus
};
#endif
#endif /* __FRIEDNSHIP_TYPE_H__ */
