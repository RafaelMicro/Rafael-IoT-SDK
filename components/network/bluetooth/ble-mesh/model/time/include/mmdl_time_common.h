#ifndef __MMDL_TIME_COMMON_H__
#define __MMDL_TIME_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TIME_CONV_DAYS_400Y                 146097
#define TIME_CONV_DAYS_100Y                 36524
#define TIME_CONV_DAYS_4Y                   1461
#define TIME_CONV_DAYS_Y                    365
#define TIME_CONV_SECONDS_D                 86400
#define TIME_CONV_SECONDS_H                 3600
#define TIME_CONV_SECONDS_M                 60
#define TIME_CONV_SECONDS_ZONE              900
#define TIME_CONV_SECONDS_UTC_ZONE          3600
#define TIME_DELTA_SECONDS_UTC_TAI_2016     37          //37: TAI & UTC diff. since 2017
#define TIME_OFFSET_ZONE_15MIN              (-64)
#define TIME_OFFSET_UTC_TAI_DELTA           (-255)

#define MMDL_TIME_SRV_TIME_ROLE_NONE                    0x00
#define MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_AUTHORITY     0x01
#define MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_RELAY         0x02
#define MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_CLIENT        0x03


typedef struct time_state_s
{
    /* public variables */
    uint64_t    tai_seconds;            /**< Current TAI time in seconds */
    uint8_t     subsecond;              /**< Sub-second time */
    uint8_t     uncertainty;            /**< Estimated uncertainty in 10-millisecond steps */
    uint8_t     time_authority;         /**< Time Authority */
    uint8_t     time_role;              /**< Time Role */
    int16_t     time_zone_offset;       /**< Current Time Zone Offset */
    int16_t     time_zone_offset_new;   /**< Upcoming Time Zone Offset */
    uint64_t    tai_zone_change;        /**< TAI Seconds time of upcoming offset change */
    int16_t     tai_utc_delta;          /**< Current difference between TAI and UTC in seconds */
    int16_t     tai_utc_delta_new;      /**< Upcoming difference between TAI and UTC in seconds */
    uint64_t    tai_delta_change;       /**< TAI Seconds time of the upcoming TAI-UTC Delat change */

    /* private variables for internal access */
    uint16_t    UTC_year;
} time_state_t;

typedef struct __attribute__((packed)) time_status_msg_sec0_s
{
    uint64_t    dummy:                  8;
    uint64_t    tai_seconds:            40;
    uint64_t    subsecond:              8;
    uint64_t    uncertainty:            8;
} time_status_msg_sec0_t;

typedef struct __attribute__((packed)) time_status_msg_sec1_s
{
    uint32_t    time_authority:         1;
    uint32_t    tai_utc_delta:          15;
    uint32_t    time_zone_offset:       8;
    uint32_t    dummy:                  8;
} time_status_msg_sec1_t;

typedef struct __attribute__((packed)) time_status_msg_s
{
    time_status_msg_sec0_t     msg_sec0;
    time_status_msg_sec1_t     msg_sec1;
} time_status_msg_t;

typedef struct __attribute__((packed)) timezone_set_msg_s
{
    uint64_t    time_zone_offset_new:   8;          /**< Upcoming Time Zone Offset */
    uint64_t    tai_zone_change:        40;         /**< TAI Seconds time of upcoming offset change */
    uint64_t    dummy:                  16;
} timezone_set_msg_t;

typedef struct __attribute__((packed)) timezone_status_msg_s
{
    uint64_t    time_zone_offset:       8;          /**< Current Time Zone Offset */
    uint64_t    time_zone_offset_new:   8;          /**< Upcoming Time Zone Offset */
    uint64_t    tai_zone_change:        40;         /**< TAI Seconds time of upcoming offset change */
    uint64_t    dummy:                  8;
} timezone_status_msg_t;

typedef struct __attribute__((packed)) timedelta_current_param_s
{
    uint16_t    tai_utc_delta:          15;         /**< Current difference between TAI and UTC in seconds */
    uint16_t    padding:                1;          /**< Need always 0b0 */
} timedelta_current_param_t;

typedef struct __attribute__((packed)) timedelta_upcoming_param_s
{
    uint64_t    tai_utc_delta_new:      15;         /**< Upcoming difference between TAI and UTC in seconds */
    uint64_t    padding:                1;          /**< Need always 0b0 */
    uint64_t    tai_delta_change:       40;         /**< TAI Seconds time of the upcoming TAI-UTC Delat change */
    uint64_t    dummy:                  8;
} timedelta_upcoming_param_t;

typedef struct __attribute__((packed)) timedelta_set_msg_s
{
    timedelta_upcoming_param_t  upcoming;
} timedelta_set_msg_t;

typedef struct __attribute__((packed)) timedelta_status_msg_s
{
    timedelta_current_param_t   current;
    timedelta_upcoming_param_t  upcoming;
} timedelta_status_msg_t;

typedef struct __attribute__((packed)) timerole_set_msg_s
{
    uint8_t     time_role;                          /**< Time Role */
} timerole_set_msg_t;

typedef struct __attribute__((packed)) timerole_status_msg_s
{
    uint8_t     time_role;                          /**< Time Role */
} timerole_status_msg_t;


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_TIME_COMMON_H__*/
