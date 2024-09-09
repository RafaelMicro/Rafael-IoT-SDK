/**************************************************************************//**
 * @file     aux_comp.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "aux_comp.h"


static aux_comp_proc_cb  aux_notify_cb;

void aux_comp_register_callback(aux_comp_proc_cb aux_comp_callback)
{
    if( aux_comp_callback != NULL)
    {
    aux_notify_cb = aux_comp_callback;
    }
    return;
}

void aux_comp_ana_init(void)
{
    AUX_COMP->comp_ana_ctrl.bit.comp_selref = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_selinput = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_pw = 3;
    AUX_COMP->comp_ana_ctrl.bit.comp_selhys = 2;
    AUX_COMP->comp_ana_ctrl.bit.comp_swdiv = 1;
    AUX_COMP->comp_ana_ctrl.bit.comp_psrr = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_vsel = 15;
    AUX_COMP->comp_ana_ctrl.bit.comp_refsel = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_chsel = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_tc = 0;
    AUX_COMP->comp_ana_ctrl.bit.comp_en_start = 2;

    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 9;
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 9;

    return;
}

void Aux_Comp_Open(aux_comp_config_t aux_cfg, aux_comp_proc_cb aux_comp_callback)
{
    Aux_Comp_Ana_Init();
    
    AUX_COMP->COMP_DIG_CTRL0.bit.DEBOUNCE_EN = aux_cfg.debounce_en;
    AUX_COMP->COMP_DIG_CTRL0.bit.DEBOUNCE_SEL = aux_cfg.debounce_sel;

    AUX_COMP->COMP_DIG_CTRL0.bit.COUNTER_MODE_EDGE = aux_cfg.counter_mode_edge;
    AUX_COMP->COMP_DIG_CTRL1.bit.EN_INTR_COUNTER = aux_cfg.counter_mode_int_en;
    AUX_COMP->COMP_DIG_CTRL0.bit.COUNTER_TRIGGER_TH = aux_cfg.counter_mode_threshold;
    AUX_COMP->COMP_DIG_CTRL0.bit.COUNTER_MODE_EN = aux_cfg.counter_mode_en;

    AUX_COMP->COMP_DIG_CTRL1.bit.COMP_SETTLE_TIME = 14;

    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_FALLING = 1;
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_RISING = 1;
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_COUNTER = 1;
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_COUNTER = 1;

    AUX_COMP->COMP_DIG_CTRL1.bit.EN_INTR_RISING = aux_cfg.rising_edge_int_en;
    AUX_COMP->COMP_DIG_CTRL1.bit.EN_INTR_FALLING = aux_cfg.falling_edge_int_en;


    Aux_Comp_Register_Callback(aux_comp_callback);

    NVIC_EnableIRQ(Aux_Comp_IRQn);

    SYSCTRL->SYS_CLK_CTRL2.bit.EN_CK32_AUXCOMP = 1;
    return;
}

void Aux_Comp_Normal_Start(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_NM = 1;
    return;
}

void Aux_Comp_Normal_Stop(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_NM = 0;
    return;
}

void Aux_Comp_Sleep_Start(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_SP = 1;
    return;
}

void Aux_Comp_Sleep_Stop(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_SP = 0;
    return;
}

void Aux_Comp_Deep_Sleep_Start(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_DS = 1;
    return;
}

void Aux_Comp_Deep_Sleep_Stop(void)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_DS = 0;
    return;
}

void Aux_Comp_Setup_Deep_Sleep_Enable_Clock(void)
{
    SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_DS_RCO32K_OFF = 0;
}

void Aux_Comp_Setup_Deep_Sleep_Disable_Clock(uint8_t wakeup_level)
{
    AUX_COMP->COMP_DIG_CTRL0.bit.COMP_EN_NM = 1;
    
    AUX_COMP->COMP_DIG_CTRL0.bit.DS_WAKEUP_POL = wakeup_level;
    AUX_COMP->COMP_DIG_CTRL0.bit.DS_WAKEUP_EN = 1;

    SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_DS_RCO32K_OFF = 1;
}

uint32_t Get_Aux_Comp_Counter_Count(void)
{
    return AUX_COMP->COMP_DIG_CTRL2.bit.COUNTER_CNT;
}

void Clear_Aux_Comp_Counter_Count(void)
{
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_COUNTER = 1;
    return;
}


/**
 * @ingroup AUX_COMP_Driver
 * @brief Aux comparator interrupt
 * @details
 * @return
 */
/*
void Aux_Comp_Handler(void)
{
    uint32_t int_status = 0;
    int_status = AUX_COMP->COMP_DIG_CTRL2.reg & 0x07;

    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_COUNTER = AUX_COMP->COMP_DIG_CTRL2.bit.STA_INTR_COUNTER;
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_FALLING = AUX_COMP->COMP_DIG_CTRL2.bit.STA_INTR_FALLING;
    AUX_COMP->COMP_DIG_CTRL1.bit.CLR_INTR_RISING = AUX_COMP->COMP_DIG_CTRL2.bit.STA_INTR_RISING;

    if (aux_notify_cb != NULL)
    {
        aux_notify_cb(int_status);
    }
}
*/
