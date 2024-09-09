/**************************************************************************//**
 * @file     bod_comp.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "bod_comp.h"


static bod_comp_proc_cb  bod_notify_cb;

void bod_comp_register_callback(bod_comp_proc_cb bod_comp_callback)
{
    bod_notify_cb = bod_comp_callback;
    return;
}

void bod_comp_ana_init(void)
{
    BOD_COMP->comp_ana_ctrl.bit.bod_ib = 0;
    BOD_COMP->comp_ana_ctrl.bit.bod_hys = 3;
    BOD_COMP->comp_ana_ctrl.bit.bod_div_sel = 15;

    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 9;
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 9;

    return;
}

void bod_comp_open(bod_comp_config_t bod_cfg, bod_comp_proc_cb bod_comp_callback)
{
    bod_comp_ana_init();
    
    BOD_COMP->comp_dig_ctrl0.bit.debounce_en = bod_cfg.debounce_en;
    BOD_COMP->comp_dig_ctrl0.bit.debounce_sel = bod_cfg.debounce_sel;

    BOD_COMP->comp_dig_ctrl0.bit.counter_mode_edge = bod_cfg.counter_mode_edge;
    BOD_COMP->comp_dig_ctrl1.bit.en_intr_counter = bod_cfg.counter_mode_int_en;
    BOD_COMP->comp_dig_ctrl0.bit.counter_trigger_th = bod_cfg.counter_mode_threshold;
    BOD_COMP->comp_dig_ctrl0.bit.counter_mode_en = bod_cfg.counter_mode_en;



    BOD_COMP->comp_dig_ctrl1.bit.comp_settle_time = 14;

    BOD_COMP->comp_dig_ctrl1.bit.clr_intr_falling = 1;
    BOD_COMP->comp_dig_ctrl1.bit.clr_intr_rising = 1;
    BOD_COMP->comp_dig_ctrl1.bit.clr_intr_counter = 1;
    BOD_COMP->comp_dig_ctrl1.bit.clr_counter = 1;

    BOD_COMP->comp_dig_ctrl1.bit.en_intr_rising = bod_cfg.rising_edge_int_en;
    BOD_COMP->comp_dig_ctrl1.bit.en_intr_falling = bod_cfg.falling_edge_int_en;

    SYSCTRL->sys_clk_ctrl2.bit.en_ck32_bodcomp = 1;

    bod_comp_register_callback(bod_comp_callback);
    NVIC_EnableIRQ(Bod_Comp_IRQn);

    return;
}

void bod_comp_normal_start(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_nm = 1;
    return;
}

void bod_comp_normal_stop(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_nm = 0;
    return;
}

void Bod_Comp_Sleep_Start(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_sp = 1;
    return;
}

void Bod_Comp_Sleep_Stop(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_sp = 0;
    return;
}

void Bod_Comp_Deep_Sleep_Start(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_ds = 1;
    return;
}

void Bod_Comp_Deep_Sleep_Stop(void)
{
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_ds = 0;
    return;
}

void Bod_Comp_Setup_Deep_Sleep_Enable_Clock(void)
{
    SYSCTRL->sram_lowpower_3.bit.cfg_ds_rco32k_off = 0;
}

void Bod_Comp_Setup_Deep_Sleep_Disable_Clock(uint8_t wakeup_level)
{
    SYSCTRL->sram_lowpower_3.bit.cfg_ds_rco32k_off = 1;
    
    BOD_COMP->comp_dig_ctrl0.bit.comp_en_nm = 1;
    
    SYSCTRL->sys_clk_ctrl2.bit.en_ck32_bodcomp = 1;

    BOD_COMP->comp_dig_ctrl0.bit.ds_wakeup_pol = wakeup_level;
    BOD_COMP->comp_dig_ctrl0.bit.ds_wakeup_en = 1;
}

uint32_t Get_Bod_Comp_Counter_Count(void)
{
    return BOD_COMP->comp_dig_ctrl2.bit.counter_cnt;
}

void Clear_Bod_Comp_Counter_Count(void)
{
    BOD_COMP->comp_dig_ctrl1.bit.clr_counter = 1;
    return;
}

/**
 * @ingroup BOD_COMP_Driver
 * @brief Bod comparator interrupt
 * @details
 * @return
 */
/*
void Bod_Comp_Handler(void)
{
    uint32_t int_status = 0;
    int_status = BOD_COMP->COMP_DIG_CTRL2.reg & 0x07;

    BOD_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_COUNTER = BOD_COMP->COMP_DIG_CTRL2.bit.STA_INTR_COUNTER;
    BOD_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_FALLING = BOD_COMP->COMP_DIG_CTRL2.bit.STA_INTR_FALLING;
    BOD_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_RISING = BOD_COMP->COMP_DIG_CTRL2.bit.STA_INTR_RISING;

    if (bod_notify_cb != NULL)
    {
        bod_notify_cb(int_status);
    }
}
*/
