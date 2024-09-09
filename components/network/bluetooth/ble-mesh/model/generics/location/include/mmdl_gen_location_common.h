#ifndef __MMDL_GEN_LOCATION_COMMON_H__
#define __MMDL_GEN_LOCATION_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gen_location_state_s
{
    uint32_t     global_latitude;
    uint32_t     global_longitude;
    uint16_t     global_altitude;
    uint16_t     local_north;
    uint16_t     local_east;
    uint16_t     local_altitude;
    uint8_t      floor_number;
    uint16_t     uncertainty;
} gen_location_state_t;


typedef struct __attribute__((packed)) gen_location_global_set_msg_s
{
    uint32_t     global_latitude;
    uint32_t     global_longitude;
    uint16_t     global_altitude;
} gen_location_global_set_msg_t;

typedef struct __attribute__((packed)) gen_location_local_set_msg_s
{
    uint16_t     local_north;
    uint16_t     local_east;
    uint16_t     local_altitude;
    uint8_t      floor_number;
    uint16_t     uncertainty;
} gen_location_local_set_msg_t;

typedef struct __attribute__((packed)) gen_location_global_status_msg_s
{
    uint32_t     global_latitude;
    uint32_t     global_longitude;
    uint16_t     global_altitude;
} gen_location_global_status_msg_t;

typedef struct __attribute__((packed)) gen_location_local_status_msg_s
{
    uint16_t     local_north;
    uint16_t     local_east;
    uint16_t     local_altitude;
    uint8_t      floor_number;
    uint16_t     uncertainty;
} gen_location_local_status_msg_t;

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_LOCATION_COMMON_H__*/
