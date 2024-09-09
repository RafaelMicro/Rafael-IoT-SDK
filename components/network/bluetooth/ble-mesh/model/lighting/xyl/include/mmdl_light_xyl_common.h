/**
 * @file mmdl_light_xyl_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __MMDL_LIGHT_XYL_COMMON_H__
#define __MMDL_LIGHT_XYL_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Light CTL model and setup model sharing the same data structure */
typedef struct light_xyl_state_s
{
    /* states for Light CTL model */
    uint16_t    xyL_x_state;
    uint16_t    xyL_y_state;
    uint16_t    xyL_lightness;

    /* states for Light CTL setup model */
    uint16_t    xyL_x_default_state;
    uint16_t    xyL_x_range_min;
    uint16_t    xyL_x_range_max;
    uint16_t    xyL_y_default_state;
    uint16_t    xyL_y_range_min;
    uint16_t    xyL_y_range_max;
    uint16_t    *p_lightness_default;

    /* private variables */
    uint16_t    target_xyL_lightness_state;
    uint16_t    target_xyL_x_state;
    uint16_t    target_xyL_y_state;
    uint8_t     remaining_time;
} light_xyl_state_t;

typedef struct __attribute__((packed)) light_xyL_set_msg_s
{
    uint16_t    xyL_lightness;
    uint16_t    xyL_x;
    uint16_t    xyL_y;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_xyL_set_msg_t;

typedef struct __attribute__((packed)) light_xyL_default_set_msg_s
{
    uint16_t    lightness;
    uint16_t    xyL_x;
    uint16_t    xyL_y;
} light_xyL_default_set_msg_t;

typedef struct __attribute__((packed)) light_xyL_range_set_msg_s
{
    uint16_t    xyL_x_range_min;
    uint16_t    xyL_x_range_max;
    uint16_t    xyL_y_range_min;
    uint16_t    xyL_y_range_max;
} light_xyL_range_set_msg_t;

typedef struct __attribute__((packed)) light_xyL_status_msg_s
{
    uint16_t    xyL_lightness;
    uint16_t    xyL_x;
    uint16_t    xyL_y;
    uint8_t     remaining_time;
} light_xyL_status_msg_t;

typedef struct __attribute__((packed)) light_xyL_target_status_msg_s
{
    uint16_t    target_xyL_lightness;
    uint16_t    target_xyL_x;
    uint16_t    target_xyL_y;
    uint8_t     remaining_time;
} light_xyL_target_status_msg_t;

typedef struct __attribute__((packed)) light_xyL_default_status_msg_s
{
    uint16_t    lightness;
    uint16_t    xyL_x;
    uint16_t    xyL_y;
} light_xyL_default_status_msg_t;

typedef struct __attribute__((packed)) light_xyL_range_status_msg_s
{
    uint8_t     status_code;
    uint16_t    xyL_x_range_min;
    uint16_t    xyL_x_range_max;
    uint16_t    xyL_y_range_min;
    uint16_t    xyL_y_range_max;
} light_xyL_range_status_msg_t;


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_LIGHT_XYL_COMMON_H__*/
