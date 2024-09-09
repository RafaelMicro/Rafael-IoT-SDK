/**************************************************************************//**
 * @file     bod_comp_reg.h
 * @version  V1.00
 * @brief    BOD Comparator register definition header file
 *
 * @copyright
 *****************************************************************************/
/** @defgroup BOD_COMP_Register BOD_COMP
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  BOD_Comp_Register header information
*/
#ifndef BOD_COMP_REG_H
#define BOD_COMP_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

//0x00
typedef union bod_comp_ana_ctl_s
{
    struct bod_comp_ana_ctl_b
    {
        uint32_t reserved1              : 2;
        uint32_t bod_ib                 : 2;
        uint32_t bod_hys                : 2;
        uint32_t reserved2              : 2;
        uint32_t bod_div_sel            : 4;
        uint32_t reserved3              : 4;
        uint32_t reserved4              : 16;
    } bit;
    uint32_t reg;
} bod_comp_ana_ctl_t;

//0x04
typedef union bod_comp_dig_ctl0_s
{
    struct bod_comp_dig_ctl0_b
    {
        uint32_t comp_en_nm         : 1;
        uint32_t comp_en_sp         : 1;
        uint32_t comp_en_ds         : 1;
        uint32_t reserved1          : 1;
        uint32_t debounce_en        : 1;
        uint32_t reserved2          : 1;
        uint32_t debounce_sel       : 2;
        
        uint32_t counter_mode_en    : 1;
        uint32_t reserved3          : 1;
        uint32_t counter_mode_edge  : 2;
        uint32_t ds_wakeup_en       : 1;
        uint32_t ds_wakeup_pol      : 1;
        uint32_t reserved4          : 2;
        
        uint32_t counter_trigger_th : 16;
    } bit;
    uint32_t reg;
} bod_comp_dig_ctl0_t;

//0x08
typedef union bod_comp_dig_ctl1_s
{
    struct bod_comp_dig_ctl1_b
    {
        uint32_t en_intr_rising         : 1;
        uint32_t en_intr_falling        : 1;
        uint32_t en_intr_counter        : 1;
        uint32_t reserved1              : 5;
        
        uint32_t clr_intr_rising        : 1;
        uint32_t clr_intr_falling       : 1;
        uint32_t clr_intr_counter       : 1;
        uint32_t clr_counter            : 1;
        uint32_t reserved2              : 4;
        
        uint32_t comp_settle_time       : 4;
        uint32_t reserved3              : 12;
    } bit;
    uint32_t reg;
} bod_comp_dig_ctl1_t;

//0x0c
typedef union bod_comp_dig_ctl2_s
{
    struct bod_comp_dig_ctl2_b
    {
        uint32_t sta_intr_rising        : 1;
        uint32_t sta_intr_falling       : 1;
        uint32_t sta_intr_counter       : 1;
        uint32_t reserved1              : 5;
        
        uint32_t comp_out               : 1;
        uint32_t reserved2              : 7;
        uint32_t counter_cnt            : 16;
    } bit;
    uint32_t reg;
} bod_comp_dig_ctl2_t;

typedef struct
{
    __IO bod_comp_ana_ctl_t       comp_ana_ctrl;       /*0x00*/
    __IO bod_comp_dig_ctl0_t      comp_dig_ctrl0;      /*0x04*/
    __IO bod_comp_dig_ctl1_t      comp_dig_ctrl1;      /*0x08*/
    __IO bod_comp_dig_ctl2_t      comp_dig_ctrl2 ;     /*0x0c*/
} BOD_COMP_CTL_T;

/*@}*/ /* end of peripheral_group BOD_COMP_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_BOD_COMP_REG_H__ */
