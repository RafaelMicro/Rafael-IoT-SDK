#ifndef __MMDL_GEN_POWERUP_LEVEL_COMMON_H__
#define __MMDL_GEN_POWERUP_LEVEL_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_power_level_state_s
{
    /* public variables */
    uint16_t     power_actual_state;
    uint16_t     power_last_state;
    uint16_t     power_default_state;
    uint16_t     power_range_min_state;
    uint16_t     power_range_max_state;
    /* private variables for internal access */
} gen_power_level_state_t;

typedef struct __attribute__((packed)) gen_power_level_set_msg_s
{
    uint16_t     power;
    uint8_t      tid;
    uint8_t      transition_time;
    uint8_t      delay;
} gen_power_level_set_msg_t;

typedef struct __attribute__((packed)) gen_power_default_set_msg_s
{
    uint16_t     power;
} gen_power_default_set_msg_t;

typedef struct __attribute__((packed)) gen_power_range_set_msg_s
{
    uint16_t     range_min;
    uint16_t     range_max;
} gen_power_range_set_msg_t;

typedef struct __attribute__((packed)) gen_power_level_status_msg_s
{
    uint16_t     present_power;
    uint16_t     target_power;
    uint8_t      remaining_time;
} gen_power_level_status_msg_t;

typedef struct __attribute__((packed)) gen_power_last_status_msg_s
{
    uint16_t     present_power;
} gen_power_last_status_msg_t;

typedef struct __attribute__((packed)) gen_power_default_status_msg_s
{
    uint16_t     present_power;
} gen_power_default_status_msg_t;

typedef struct __attribute__((packed)) gen_power_range_status_msg_s
{
    uint8_t      status_code;
    uint16_t     range_min;
    uint16_t     range_max;
} gen_power_range_status_msg_t;

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_POWERUP_LEVEL_COMMON_H__*/
