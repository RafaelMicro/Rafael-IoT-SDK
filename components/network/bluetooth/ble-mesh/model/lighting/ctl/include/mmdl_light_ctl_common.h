/**
 * @file mmdl_light_ctl_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __MMDL_LIGHT_CTL_COMMON_H__
#define __MMDL_LIGHT_CTL_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Light CTL model and setup model sharing the same data structure */
typedef struct light_ctl_state_s
{
    /* states for Light CTL model */
    uint16_t    ctl_lightness_state;
    uint16_t    ctl_temperature_state;
    uint16_t    ctl_delta_uv_state;


    /* states for Light CTL setup model */
    uint16_t    ctl_temperature_default_state;
    uint16_t    ctl_delta_uv_default_state;
    uint16_t    ctl_temperature_range_min;
    uint16_t    ctl_temperature_range_max;
    uint16_t    *p_lightness_default;

    /* private variables */
    uint16_t    target_ctl_lightness_state;
    uint16_t    target_ctl_temperature_state;
    uint16_t    target_ctl_delta_uv_state;
    uint8_t     remaining_time;
} light_ctl_state_t;

typedef struct __attribute__((packed)) light_ctl_set_msg_s
{
    uint16_t    ctl_lightness;
    uint16_t    ctl_temperature;
    uint16_t    ctl_delta_uv;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_ctl_set_msg_t;

typedef struct __attribute__((packed)) light_ctl_temperature_set_msg_s
{
    uint16_t    ctl_temperature;
    uint16_t    ctl_delta_uv;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} light_ctl_temperature_set_msg_t;

typedef struct __attribute__((packed)) light_ctl_temperature_range_set_msg_s
{
    uint16_t    range_min;
    uint16_t    range_max;
} light_ctl_temperature_range_set_msg_t;

typedef struct __attribute__((packed)) light_ctl_default_set_msg_s
{
    uint16_t    lightness;
    uint16_t    temperature;
    uint16_t    delta_uv;
} light_ctl_default_set_msg_t;

typedef struct __attribute__((packed)) light_ctl_status_msg_s
{
    uint16_t    present_ctl_lightness;
    uint16_t    present_ctl_temperature;
    uint16_t    target_ctl_lightness;
    uint16_t    target_ctl_temperature;
    uint8_t     remaining_time;
} light_ctl_status_msg_t;

typedef struct __attribute__((packed)) light_ctl_temperature_range_status_msg_s
{
    uint8_t     status_code;
    uint16_t    range_min;
    uint16_t    range_max;
} light_ctl_temperature_range_status_msg_t;

typedef struct __attribute__((packed)) light_ctl_default_status_msg_s
{
    uint16_t    lightness;
    uint16_t    temperature;
    uint16_t    delta_uv;
} light_ctl_default_status_msg_t;


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_LIGHT_CTL_COMMON_H__*/
