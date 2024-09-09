/**************************************************************************//**
 * @file     rtc_reg.h
 * @version
 * @brief    RTC Register defined
 *
 * @copyright
 ******************************************************************************/
/** @defgroup RTC_Register RTC
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  RTC_Register header information
*/
#ifndef __RT584_RTC_REG_H__
#define __RT584_RTC_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


//0x18
typedef union rtc_ctl_s
{
    struct rtc_ctl_b
    {
        uint32_t test_mode              : 6;
        uint32_t year400_flag           : 1;
        uint32_t clrpls                 : 1;
        uint32_t enable                 : 1;
        uint32_t reserved0              : 7;
        uint32_t reserved1              : 16;
    } bit;
    uint32_t reg;
} rtc_ctl_t;

typedef struct
{
    __IO  uint32_t   rtc_second;             //0x0
    __IO  uint32_t   rtc_minute;             //0x4
    __IO  uint32_t   rtc_hour;               //0x8
    __IO  uint32_t   rtc_day;                //0xc
    __IO  uint32_t   rtc_month;              //0x10
    __IO  uint32_t   rtc_year;               //0x14
    __IO  uint32_t   rtc_control;            //0x18
    __IO  uint32_t   rtc_clock_div;          //0x1c
    __IO  uint32_t   rtc_alarm_second;       //0x20
    __IO  uint32_t   rtc_alarm_minute;       //0x24
    __IO  uint32_t   rtc_alarm_hour;         //0x28
    __IO  uint32_t   rtc_alarm_day;          //0x2c
    __IO  uint32_t   rtc_alarm_month;        //0x30
    __IO  uint32_t   rtc_alarm_year;         //0x34
    __IO  uint32_t   rtc_int_control;        //0x38
    __IO  uint32_t   rtc_int_status;         //0x3c
    __IO  uint32_t   rtc_int_clear;          //0x40
    __IO  uint32_t   rtc_load;               //0x44
    __IO  uint32_t   rtc_msecond;             //0x48
    __IO  uint32_t   rtc_alarm_msecond;       //0x4c

} RTC_T;

#define  RTC_INT_SEC         (1<<0)
#define  RTC_INT_MIN         (1<<1)
#define  RTC_INT_HOUR        (1<<2)
#define  RTC_INT_DAY         (1<<3)
#define  RTC_INT_MONTH       (1<<4)
#define  RTC_INT_YEAR        (1<<5)
#define  RTC_INT_EVENT       (1<<6)

#define  RTC_CTRL_CLRPLS     (1<<7)
#define  RTC_CTRL_ENABLE     (1<<8)

#define  RTC_LOAD_TIME       (1<<0)
#define  RTC_LOAD_ALARM      (1<<1)
#define  RTC_LOAD_DIVISOR    (1<<2)


/*@}*/ /* end of peripheral_group RTC_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_RTC_REG_H__ */