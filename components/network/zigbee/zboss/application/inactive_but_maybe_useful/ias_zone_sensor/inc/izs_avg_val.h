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
/* PURPOSE: General IAS zone device: average value calculation routines
*/

#ifndef IZS_AVG_VAL_H
#define IZS_AVG_VAL_H 1

#include "zboss_api.h"

#define IZS_DECLARE_AVG_DATA_TYPE(avg_type_name, array_size) \
typedef struct avg_type_name##_s                           \
{                                                          \
  zb_uint8_t count;                                        \
  zb_uint8_t index;                                        \
  zb_uint16_t avg_val;                                     \
  zb_bool_t avg_valid;                                     \
  zb_uint8_t data_size;                                    \
  zb_uint16_t data_val[array_size];                        \
}                                                          \
avg_type_name;


#define IZS_DECLARE_AVG_SIGNED_DATA_TYPE(avg_type_name, array_size) \
typedef struct avg_type_name##_s                           \
{                                                          \
  zb_uint8_t count;                                        \
  zb_uint8_t index;                                        \
  zb_int16_t avg_val;                                      \
  zb_bool_t avg_valid;                                     \
  zb_uint8_t data_size;                                    \
  zb_int16_t data_val[array_size];                         \
}                                                          \
avg_type_name;


#define IZS_DECLARE_AVG_DATA_VAR(avg_type_name, var_name) \
avg_type_name var_name =                                \
{                                                       \
  .index = 0,                                           \
  .count = 0,                                           \
  .avg_valid = ZB_FALSE,                                \
  .data_size = sizeof( ((avg_type_name*)NULL)->data_val)/sizeof(zb_uint16_t) \
}


#define IZS_DECLARE_AVG_SIGNED_DATA_VAR(avg_type_name, var_name) \
avg_type_name var_name =                                \
{                                                       \
  .index = 0,                                           \
  .count = 0,                                           \
  .avg_valid = ZB_FALSE,                                \
  .data_size = sizeof( ((avg_type_name*)NULL)->data_val)/sizeof(zb_int16_t) \
}


#define IZS_AVG_VAL_ARG(a) ((izs_avg_data_t*)(a))

/* Basic type - used in API functions */
IZS_DECLARE_AVG_DATA_TYPE(izs_avg_data_t, 1)

void izs_avg_val_put_value(izs_avg_data_t *avg_data, zb_int16_t value);
void izs_avg_val_calc_avg(izs_avg_data_t *avg_data);

#define IZS_AVG_VAL_PUT_VALUE(avg_data_ptr, value) \
  izs_avg_val_put_value((izs_avg_data_t*)avg_data_ptr, value)


void izs_avg_val_put_signed_value(izs_avg_data_t *avg_data, zb_int16_t value);
void izs_avg_val_calc_signed_avg(izs_avg_data_t *avg_data);

#define IZS_AVG_VAL_PUT_SIGNED_VALUE(avg_data_ptr, value) \
  izs_avg_val_put_signed_value((izs_avg_data_t*)avg_data_ptr, value)


#endif /* IZS_AVG_VAL_H */
