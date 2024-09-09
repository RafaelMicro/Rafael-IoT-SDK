#ifndef __MMDL_SCHEDULER_COMMON_H__
#define __MMDL_SCHEDULER_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MMDL_SCHEDULE_REG_ENTRY_NUM             2

#define MMDL_SCHEDULE_REG_ACTION_TURN_OFF       0x00
#define MMDL_SCHEDULE_REG_ACTION_TURN_ON        0x01
#define MMDL_SCHEDULE_REG_ACTION_SCENE_RECALL   0x02
#define MMDL_SCHEDULE_REG_ACTION_NO_ACTION      0x0F


typedef struct schedule_reg_state_s
{
    /* public variables */
    uint16_t    months;         /**< Scheduled month for the action */
    uint8_t     day;            /**< Scheduled day of the month for the action */
    uint8_t     hour;           /**< Scheduled hour for the action */
    uint8_t     minute;         /**< Scheduled minute for the action */
    uint8_t     second;         /**< Scheduled second for the action */
    uint16_t    days_of_week;   /**< Scheduled days of the week for the action */
    uint8_t     action;         /**< Action to be performed at the scheduled time */
    uint8_t     trans_time;     /**< Transition time for this action */
    uint16_t    scene_number;   /**< Scene number to be used for some actions */
    uint8_t     year;           /**< Scheduled year for the action */

    /* private variables for internal access */
    uint8_t     year_act;       /**< Occurrence year for the action */
    uint8_t     months_act;     /**< Occurrence month for the action */
    uint8_t     day_act;        /**< Occurrence day of the month for the action */
    uint8_t     hour_act;       /**< Occurrence hour for the action */
    uint8_t     minute_act;     /**< Occurrence minute for the action */
    uint8_t     second_act;     /**< Occurrence second for the action */

    uint8_t     Next;           /**< next schedule register in list */
} schedule_reg_state_t;

typedef struct scheduler_state_s
{
    /* public variables */
    uint16_t schedules;
    schedule_reg_state_t schedule_reg_state[MMDL_SCHEDULE_REG_ENTRY_NUM];

    /* private variables for internal access */
    //uint8_t    dummy;
} scheduler_state_t;

typedef struct __attribute__((packed)) scheduler_status_msg_s
{
    uint16_t    schedules;
} scheduler_status_msg_t;

typedef struct scheduler_action_get_msg_s
{
    uint16_t    index;   /**< Scene number to be used for some actions */
} scheduler_action_get_msg_t;

typedef struct __attribute__((packed)) scheduler_action_status_time_reg_s
{
    uint64_t    index:          4;      /**< Index of the Schedule Register entry to set */
    uint64_t    year:           7;      /**< Scheduled year for the action */
    uint64_t    months:         12;     /**< Scheduled month for the action */
    uint64_t    day:            5;      /**< Scheduled day of the month for the action */
    uint64_t    hour:           5;      /**< Scheduled hour for the action */
    uint64_t    minute:         6;      /**< Scheduled minute for the action */
    uint64_t    second:         6;      /**< Scheduled second for the action */
    uint64_t    days_of_week:   7;      /**< Scheduled days of the week for the action */
    uint64_t    action:         4;      /**< Action to be performed at the scheduled time */
    uint64_t    trans_time:     8;      /**< Transition time for this action */
} scheduler_action_status_time_reg_t;

typedef struct __attribute__((packed)) scheduler_action_set_msg_s
{
    scheduler_action_status_time_reg_t  time_reg;
    uint16_t                            scene_number;   /**< Scene number to be used for some actions */
} scheduler_action_set_msg_t;

typedef struct __attribute__((packed)) scheduler_action_status_msg_s
{
    scheduler_action_status_time_reg_t  time_reg;
    uint16_t                            scene_number;   /**< Scene number to be used for some actions */
} scheduler_action_status_msg_t;



#ifdef __cplusplus
};
#endif

#endif /* __MMDL_SCHEDULER_COMMON_H__*/
