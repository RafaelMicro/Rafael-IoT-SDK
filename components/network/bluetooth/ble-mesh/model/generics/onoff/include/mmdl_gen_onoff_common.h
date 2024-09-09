#ifndef __MMDL_GEN_ONOFF_COMMON_H__
#define __MMDL_GEN_ONOFF_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_onoff_model_status_s
{
    uint8_t        present_onoff;
    uint8_t        target_onoff;
    uint8_t        remaining_time;
    uint8_t        delay;
} gen_onoff_model_status_t;

typedef struct gen_on_off_state_s
{
    /* public variables */
    uint8_t     on_off_state;
    /* private variables for internal access */
    uint8_t     target_onoff;
    uint8_t     present_onoff;
} gen_on_off_state_t;

typedef struct __attribute__((packed)) gen_on_off_set_msg_s
{
    uint8_t     on_off;
    uint8_t     tid;
} gen_on_off_set_msg_t;

typedef struct __attribute__((packed)) gen_on_off_status_msg_s
{
    uint8_t     present_onoff;
} gen_on_off_status_msg_t;


typedef void (*model_onoff_user_call_back)(uint16_t element_address, uint32_t state);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_ONOFF_COMMON_H__*/
