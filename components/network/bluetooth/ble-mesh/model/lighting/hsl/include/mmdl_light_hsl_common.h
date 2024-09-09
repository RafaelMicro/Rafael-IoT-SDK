/**
 * @file mmdl_light_hsl_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MMDL_LIGHT_HSL_COMMON_H__
#define __MMDL_LIGHT_HSL_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*define the light hsl state set */
typedef struct light_hsl_state_s
{
    /* public variables */
    uint16_t    hsl_hue_state;
    uint16_t    hsl_saturation_state;
    uint16_t    hsl_lightness_state;
    /* state values for setup server */
    uint16_t    hsl_hue_default_state;
    uint16_t    hsl_saturation_default_state;
    /* the lightness default is sharing with lightness model, use pointer to shaing variables */
    uint16_t    *p_lightness_default;
    uint16_t    hsl_hue_range_min;
    uint16_t    hsl_hue_range_max;
    uint16_t    hsl_saturation_range_min;
    uint16_t    hsl_saturation_range_max;
    /* private variables */
    uint16_t    target_hsl_hue_state;
    uint16_t    target_hsl_saturation_state;
    uint16_t    target_hsl_lightness_state;
    uint8_t     remaining_time;
} light_hsl_state_t;

typedef struct __attribute__((packed)) light_hsl_hue_set_msg_s
{
    uint16_t    hue;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_hsl_hue_set_msg_t;


typedef struct __attribute__((packed)) light_hsl_hue_status_msg_s
{
    uint16_t    present_hue;
    uint16_t    target_hue;
    uint8_t     remaining_time;
} light_hsl_hue_status_msg_t;

typedef struct __attribute__((packed)) light_hsl_sat_set_msg_s
{
    uint16_t    saturation;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_hsl_sat_set_msg_t;


typedef struct __attribute__((packed)) light_hsl_sat_status_msg_s
{
    uint16_t    present_saturation;
    uint16_t    target_saturation;
    uint8_t     remaining_time;
} light_hsl_sat_status_msg_t;

typedef struct __attribute__((packed)) light_hsl_set_msg_s
{
    uint16_t    hsl_lightness;
    uint16_t    hsl_hue;
    uint16_t    hsl_saturation;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_hsl_set_msg_t;

typedef struct __attribute__((packed)) light_hsl_default_set_msg_s
{
    uint16_t    hsl_lightness;
    uint16_t    hsl_hue;
    uint16_t    hsl_saturation;
} light_hsl_default_set_msg_t;

typedef struct __attribute__((packed)) light_hsl_range_set_msg_s
{
    uint16_t    hue_range_min;
    uint16_t    hue_range_max;
    uint16_t    saturation_range_min;
    uint16_t    saturation_range_max;
} light_hsl_range_set_msg_t;

typedef struct __attribute__((packed)) light_hsl_status_msg_s
{
    uint16_t    hsl_lightness;
    uint16_t    hsl_hue;
    uint16_t    hsl_saturation;
    uint8_t     remaining_time;
} light_hsl_status_msg_t, light_hsl_target_status_msg_t;

typedef struct __attribute__((packed)) light_hsl_default_status_msg_s
{
    uint16_t    hsl_lightness;
    uint16_t    hsl_hue;
    uint16_t    hsl_saturation;
} light_hsl_default_status_msg_t;

typedef struct __attribute__((packed)) light_hsl_range_status_msg_s
{
    uint8_t     status_code;
    uint16_t    hue_range_min;
    uint16_t    hue_range_max;
    uint16_t    saturation_range_min;
    uint16_t    saturation_range_max;
} light_hsl_range_status_msg_t;



#ifdef __cplusplus
};
#endif

#endif /* __MMDL_LIGHT_HSL_COMMON_H__*/
