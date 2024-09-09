#ifndef __MMDL_GEN_BATTERY_COMMON_H__
#define __MMDL_GEN_BATTERY_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_battery_state_s
{
    uint32_t  battery_level: 8;
    uint32_t  battery_time_to_discharge: 24;
    uint32_t  battery_time_to_charge: 24;
    uint32_t  battery_flag: 8;
} gen_battery_state_t;

typedef struct __attribute__((packed)) gen_battery_status_msg_s
{
    uint32_t  level: 8;
    uint32_t  time_to_discharge: 24;
    uint32_t  time_to_charge: 24;
    uint32_t  flag: 8;
} gen_battery_status_msg_t;

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_BATTERY_COMMON_H__*/
