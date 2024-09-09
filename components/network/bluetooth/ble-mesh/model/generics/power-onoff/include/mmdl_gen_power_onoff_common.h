#ifndef __MMDL_GEN_POWERUP_ONOFF_COMMON_H__
#define __MMDL_GEN_POWERUP_ONOFF_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_onpowerup_state_s
{
    /* public variables */
    uint8_t     on_power_up_state;
    /* private variables for internal access */
    uint8_t     on_off_state;
} gen_onpowerup_state_t;

typedef struct __attribute__((packed)) gen_onpowerup_set_msg_s
{
    uint8_t     on_power_up;
} gen_onpowerup_set_msg_t;

typedef struct __attribute__((packed)) gen_onpowerup_status_msg_s
{
    uint8_t     on_power_up;
} gen_onpowerup_status_msg_t;



#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_POWERUP_ONOFF_COMMON_H__*/
