#ifndef __MMDL_GEN_DEFAULT_TRANS_TIME_COMMON_H__
#define __MMDL_GEN_DEFAULT_TRANS_TIME_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_default_trans_time_state_s
{
    uint8_t     default_trans_num_of_step: 6;
    uint8_t     default_trans_step_resoultion: 2;
} gen_default_trans_time_state_t;

typedef struct __attribute__((packed)) gen_default_trans_time_set_msg_s
{
    uint8_t  transition_time;
} gen_default_trans_time_set_msg_t;

typedef struct __attribute__((packed)) gen_default_trans_time_status_msg_s
{
    uint8_t  transition_time;
} gen_default_trans_time_status_msg_t;


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_DEFAULT_TRANS_TIME_COMMON_H__*/
