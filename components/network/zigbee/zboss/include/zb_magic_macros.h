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
/* PURPOSE: Some macros for fast calculation

*/
#ifndef MAGIC_MACROSES_H
#define MAGIC_MACROSES_H 1

/*! @cond internals_doc */
/**
   @addtogroup ZB_BASE
   @{
*/

#define MAGIC_ROUND_A_TO_B(a, b) (((a) + ((b)-1U)) & -((zb_int_t)(b)))

#define MAGIC_ROUND_TO_32(a) (((a) + (0x1FU)) & (zb_uint8_t)(0xE0U))

#if 0//ndef ZB_IAR
/* Causing warnings about a sign change. Let's do ont use it. */
#define MAGIC_ROUND_TO_4(a) (((a) + ((4)-1)) & -(4))
#else
#define MAGIC_ROUND_TO_4(a) (((a) + 3U) / 4U * 4U)
#endif

#define MAGIC_ROUND_TO_INT_SIZE(a) (((a) + (sizeof(zb_int_t)-1)) & -((zb_int_t)sizeof(zb_int_t)))

#define MAGIC_TRUNC_TO_4(a) ((a) & (~3U))

#define MAGIC_TRUNC_TO_8(a) ((a) & (~7U))

#define MAGIC_LEAST_SIGNIFICANT_BIT_MASK(x) ((x) & (zb_uint_t)(-((zb_int_t)(x))))

#define MAGIC_LS_ZERO_MASK(x) ((~(x)) & ((x) + 1U))

#define MAGIC_TURN_OFF_LS_1(x) ((x) & ((x) - 1U))

/**
 * Turn off (set to 0) rightmost chain of continuous 1s
 */
#define MAGIC_TURN_OFF_RIGHT_ONES(x)  ((((x) | ((x) - 1U)) + 1U) & (x))


#define MAGIC_ISOLATE_RIGHT_ONES(x)  ((x) ^ MAGIC_TURN_OFF_RIGHT_ONES(x))

/**
 * Branch-free calculation of x = (x == a ? b : a)
 */
#define MAGIC_ALTERNATE(x, a, b) ((a) ^ (b) ^ (x))


#define MAGIC_AVG(a, b) ((a) & (b) + ((a) ^ (b))/2U)

/**
   return -1, 0, 1 if x < y, x == y, x > y
  */
#define MAGIC_CMP(x, y) (((x) > (y)) - ((x) < (y)))

/**
   Return signum of the result: -1 if < 0, 0 if == 0, else 1
 */
#define MAGIC_SIGN(x) (((x) > 0) - ((x) < 0))

/**
   Return 1 if the number is a power of 2, works only if x > 0
 */
#define MAGIC_IS_POWER_OF_TWO(x) ( ((x) & ((x) - 1U)) == 0U )


/*
Following set of macros implements compile-time calculation of # of bits in the
word using following code:

 32-bit recursive reduction using SWAR...
     but first step is mapping 2-bit values
     into sum of 2 1-bit values in sneaky way
  x -= ((x >> 1) & 0x55555555);
  x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
  x = (((x >> 4) + x) & 0x0f0f0f0f);
  x += (x >> 8);
  x += (x >> 16);
  return(x & 0x0000003f);
*/


#define MAGIC_ONE_1(x) ((x) - (((x) >> 1U) & 0x55555555U))
#define MAGIC_ONE_2(x) (((MAGIC_ONE_1(x) >> 2U) & 0x33333333U) + (MAGIC_ONE_1(x) & 0x33333333U))
#define MAGIC_ONE_4(x) (((MAGIC_ONE_2(x) >> 4U) + MAGIC_ONE_2(x)) & 0x0f0f0f0fU)
#define MAGIC_ONE_8(x) (MAGIC_ONE_4(x) + (MAGIC_ONE_4(x) >> 8U))
#define MAGIC_ONE_16(x) (MAGIC_ONE_8(x) + (MAGIC_ONE_8(x) >> 16U))
#define MAGIC_ONE_32(x) (MAGIC_ONE_16(x) & 0x0000003fU)


#define MAGIC_LOG2_1(x) ((x) | ((x) >> 1U))
#define MAGIC_LOG2_2(x) (MAGIC_LOG2_1(x) | (MAGIC_LOG2_1(x) >> 2U))
#define MAGIC_LOG2_4(x) (MAGIC_LOG2_2(x) | (MAGIC_LOG2_2(x) >> 4U))
#define MAGIC_LOG2_8(x) (MAGIC_LOG2_4(x) | (MAGIC_LOG2_4(x) >> 8U))
#define MAGIC_LOG2_16(x) (MAGIC_LOG2_8(x) | (MAGIC_LOG2_8(x) >> 16U))

#define MAGIC_LOG2_32(x) (MAGIC_ONE_32(MAGIC_LOG2_16(x)) - 1U)

/*! @} */
/*! @endcond */

#endif /* MAGIC_MACROSES_H */
