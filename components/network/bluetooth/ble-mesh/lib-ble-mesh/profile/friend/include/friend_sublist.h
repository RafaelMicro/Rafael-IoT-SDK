// ---------------------------------------------------------------------------
// Copyright (c) 2021
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __FRIEDNSHIP_SUBLIST_H__
#define __FRIEDNSHIP_SUBLIST_H__

#ifdef __cplusplus
extern "C" {
#endif


/** Structure of statistics for the Friend Subscription List. */
typedef struct
{
    uint32_t lookups;    /**< Number of lookups performed on the subscription list. */
    uint32_t hits;       /**< Number of hits in the lookups. */
    uint32_t removed;    /**< Number of removed entries. */
    uint32_t max_count;  /**< Highest number of entries. */
    uint32_t curr_count; /**< Current number of entries. */
} friend_sublist_stats_t;

/** Friend Subscription List. */
typedef struct
{
    uint16_t addrs[MESH_FRIEND_SUBLIST_SIZE]; /**< 16-bit raw mesh address array. */

#if FRIEND_DEBUG
    friend_sublist_stats_t stats; /**< Statistics for the subscription list instance. */
#endif
} friend_sublist_t;

/**
 * Initializes the Friend Subscription List.
 *
 * @param[in,out] p_sublist             Pointer to the Friend Subscription List.
 */
void friend_sublist_init(friend_sublist_t *p_sublist);

/**
 * Adds the given address to the Friend Subscription List.
 *
 * @param[in,out] p_sublist             Pointer to the Friend Subscription List.
 * @param[in] address                   16-bit raw mesh address.
 *
 * @retval  NRF_SUCCESS                 Address has been added.
 * @retval  NRF_ERROR_NO_MEM            The list is full.
 * @retval  NRF_ERROR_INVALID_PARAM     The given address is not a group or a virtual address.
 */
uint32_t friend_sublist_add(friend_sublist_t *p_sublist, uint16_t address);

/**
 * Removes the given address from the Friend Subscription List.
 *
 * @param[in,out] p_sublist             Pointer to the Friend Subscription List.
 * @param[in] address                   16-bit raw mesh address.
 *
 * @retval  NRF_SUCCESS                 Address has been removed.
 * @retval  NRF_ERROR_NOT_FOUND         The given address is not on the list.
 * @retval  NRF_ERROR_INVALID_PARAM     The given address is not a group or a virtual address.
 */
uint32_t friend_sublist_remove(friend_sublist_t *p_sublist, uint16_t address);

/**
 * Checks whether the Friend Subscription List contains the given address or not.
 *
 * @param[in] p_sublist                 Pointer to the Friend Subscription List.
 * @param[in] address                   16-bit raw mesh address.
 *
 * @retval  NRF_SUCCESS                 The list contains the given address.
 * @retval  NRF_ERROR_NOT_FOUND         The given address is not on the list.
 * @retval  NRF_ERROR_INVALID_PARAM     The given address is not a group or a virtual address.
 */
uint32_t friend_sublist_contains(const friend_sublist_t *p_sublist, uint16_t address);

/**
 * Clears the Friend Subscription List.
 *
 * @param[in,out] p_sublist Pointer to the Friend Subscription List.
 */
void friend_sublist_clear(friend_sublist_t *p_sublist);

#ifdef __cplusplus
};
#endif
#endif /* __FRIEDNSHIP_SUBLIST_H__ */
