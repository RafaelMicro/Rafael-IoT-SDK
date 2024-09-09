/***********************************************************************************************************************
 * @file     RCO1M_reg.h
 * @version
 * @brief    software interrupt register defined
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup RCO1M_Register RCO1M
*  @ingroup  peripheral_group
*  @{
*  @brief  RCO1M_Register header information
*/
#ifndef RCO1M_REG_H
#define RCO1M_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef union rco1m_cfg0_s
{
    struct rco1m_cfg0_b
    {
        uint32_t cfg_cal_target : 18;
        uint32_t reserved1      : 6;
        uint32_t cfg_cal_en     : 1;
        uint32_t reserved2      : 7;
    } bit;
    uint32_t reg;
} rco1m_cfg0_t;


typedef union rco1m_cfg1_s
{
    struct rco1m_cfg1_b
    {
        uint32_t    cfg_cal_lock_err    : 8;
        uint32_t    cfg_cal_avg_coarse  : 2;
        uint32_t    cfg_cal_avg_fine    : 2;
        uint32_t    cfg_cal_avg_lock    : 2;
        uint32_t    cfg_cal_dly         : 2;
        uint32_t    cfg_cal_fine_gain   : 4;
        uint32_t    cfg_cal_lock_gain   : 4;
        uint32_t    cfg_cal_track_en    : 1;
        uint32_t    cfg_cal_skip_coarse : 1;
        uint32_t    cfg_cal_bound_mode  : 1;
        uint32_t    cfg_tune_rco_sel    : 1;
        uint32_t    en_ck_cal           : 1;
        uint32_t    reserved1           : 3;
    } bit;
    uint32_t reg;
} rco1m_cfg1_t;


typedef union rco1m_result0_s
{
    struct rco1m_result0_b
    {
        uint32_t    est_rco_result          : 18;
        uint32_t    reserved1               : 5;
        uint32_t    est_rco_result_valid    : 1;
        uint32_t    reserved2               : 3;
        uint32_t    cal_busy                : 1;
        uint32_t    cal_lock                : 1;
        uint32_t    cal_timeout             : 1;
        uint32_t    reserved3               : 1;
    } bit;
    uint32_t reg;
} rco1m_result0_t;

typedef union rco1m_result1_s
{
    struct rco1m_result1_b
    {
        uint32_t    tune_fine_rco   : 7;
        uint32_t    reserved1       : 9;
        uint32_t    tune_coarse_rco : 4;
        uint32_t    reserved2       : 12;
    } bit;
    uint32_t reg;
} rco1m_result1_t;



typedef struct
{
    __IO  rco1m_cfg0_t      cal1m_cfg0;         /*offset:0x00*/
    __IO  rco1m_cfg1_t      cal1m_cfg1;         /*offset:0x04*/
    __I   rco1m_result0_t  cal1m_result0;       /*offset:0x08*/
    __I   rco1m_result1_t  cal1m_result1;       /*offset:0x0C*/
} RCO1M_CAL_T;


/*@}*/ /* end of peripheral_group SWI_Register */
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* end of _RT584_SWI_REG_H */

