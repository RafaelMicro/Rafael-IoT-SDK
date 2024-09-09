#ifndef __MMDL_GEN_LEVEL_COMMON_H__
#define __MMDL_GEN_LEVEL_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif



typedef struct gen_level_model_status_s
{
    int16_t        present_level;
    int16_t        target_level;
    uint8_t         remaining_time;
    uint8_t         delay;
} gen_level_model_status_t;

typedef struct gen_level_state_s
{
    /* public variables */
    int16_t    level_state;
    /* private variables for internal access */
    int16_t    target_level;
    int16_t    current_level;
    int32_t    delta_level;
    int16_t    move_delta_level;
    uint8_t    transition_time;
    uint8_t    delay;
    uint8_t    remaining_time;
} gen_level_state_t;

typedef struct __attribute__((packed)) gen_level_set_msg_s
{
    uint16_t    level;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} gen_level_set_msg_t;

typedef struct __attribute__((packed)) gen_level_delta_set_msg_s
{
    int32_t     delta_level;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} gen_level_delta_set_msg_t;

typedef struct __attribute__((packed)) gen_level_move_set_msg_s
{
    int16_t     delta_level;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} gen_level_move_set_msg_t;


typedef struct __attribute__((packed)) gen_level_status_msg_s
{
    int16_t     present_level;
    int16_t     target_level;
    uint8_t     remaining_time;
} gen_level_status_msg_t;

typedef void (*model_level_user_call_back)(uint16_t element_address, uint16_t state);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_LEVEL_COMMON_H__*/
