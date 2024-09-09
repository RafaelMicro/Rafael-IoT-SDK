/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: Macros for hash calculation
*/

#ifndef ZB_HASH_H
#define ZB_HASH_H 1

/*! @cond internals_doc */
/*! \addtogroup ZB_BASE */
/*! @{ */

#define ZB_HASH_MAGIC_VAL 5381UL

/* TODO: check is this hash optimal for ints */
#define ZB_HASH_FUNC_STEP(hash_var, v) ((((hash_var) << 5) + (hash_var)) + (v))
#define ZB_4INT_HASH_FUNC(v1, v2, v3, v4) (ZB_HASH_FUNC_STEP(ZB_HASH_FUNC_STEP( ZB_HASH_FUNC_STEP( ZB_HASH_FUNC_STEP(ZB_HASH_MAGIC_VAL, (v1)), (v2)), (v3)), (v4)) & ZB_INT_MASK)
#define ZB_2INT_HASH_FUNC(v1, v2)      (ZB_HASH_FUNC_STEP(ZB_HASH_FUNC_STEP(ZB_HASH_MAGIC_VAL, (v1)), (v2)) & ZB_INT_MASK)

#define ZB_1INT_HASH_FUNC(v1)          (ZB_HASH_FUNC_STEP(ZB_HASH_MAGIC_VAL, (v1)) & ZB_INT_MASK)

zb_uint32_t zb_64bit_hash(zb_uint8_t *data);

/* Calculate Fowler-Noll-Vo hash for random number generator. */
zb_uint32_t zb_get_fnv_hash(zb_uint8_t *buffer);
#define FNV_32_FNV0 ((zb_uint32_t)0x811c9dc5U)
#define FNV_32_PRIME ((zb_uint32_t)0x01000193U)

/*! @} */
/*! @endcond */

#endif /* ZB_HASH_H */
