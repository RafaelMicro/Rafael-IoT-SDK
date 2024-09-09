/**************************************************************************//**
 * @file     sysctrl.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "sysctrl.h"
#include "pin_mux_define.h"

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void delay_us(unsigned int us)
{
    uint32_t  Div = 0, Delay = 0;

    if (SystemCoreClock <= 1000000)
    {
        Delay = us * (SystemCoreClock / 320000);
    }
    else
    {
        Delay = us * (SystemCoreClock / 10000000);
    }
    do
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();

    } while (Delay--);
}

void delay_ms(unsigned int ms)
{
    uint32_t  Div = 0, Delay = 0;

    if (SystemCoreClock <= 1000000)
    {
        Delay = ms * (SystemCoreClock / 320);
    }
    else
    {
        Delay = ms * (SystemCoreClock / 10000);
    }

    do
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();

    } while (Delay--);

}


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/

void pin_set_mode(uint32_t pin_number, uint32_t mode)
{
    uint32_t reg, base, mask_offset, mask;

    /*RT584 mode become 4 bits.. so max mode is 15*/
    if ((pin_number >= 32) || (mode > 15))
    {
        return;     /*Invalid setting mode.*/
    }

#if 1 //(RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

    base = MAP_BASE + (pin_number >> 3) * 4;
    mask_offset = (pin_number & 0x7) << 2;
    mask = 0xF << mask_offset;

    /*pin mux setting is share resource.*/
    reg = *((volatile unsigned int *) base);
    reg = reg & ~mask;
    reg = reg | (mode << mask_offset);

    *((volatile unsigned int *)base) =  reg;

#else
    if ( rt58x_pin_mux_define[pin_number][mode][1] != MODE_PIN_MUX_NULL)
    {
        if ( rt58x_pin_mux_define[pin_number][mode][2] != MODE_PIN_MUX_NULL )
        {
            Pin_Set_Out_Mode_Ex(pin_number, rt58x_pin_mux_define[pin_number][mode][2]);
        }
        if ( rt58x_pin_mux_define[pin_number][mode][3] != MODE_PIN_MUX_NULL )
        {
            Pin_Set_In_Mode_Ex(pin_number, rt58x_pin_mux_define[pin_number][mode][3]);
        }
    }
#endif
    return;
}

#if 1//(RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

#else
void Pin_Set_Out_Mode_Ex(uint32_t pin_number, uint32_t mode)
{
    uint32_t reg, base, reg_shift, enable;
    uint32_t index;

    /*RT584 mode become 4 bits.. so max mode is 15*/
    if ((pin_number >= 32) || (mode & 0x3F) > 0x3F)
    {
        return;     /*Invalid setting mode.*/
    }

    index = (pin_number / 4);
    base  = (pin_number % 4);
    reg_shift = (base << 3);

    SYSCTRL->SYS_GPIO_OMUX[index] = (SYSCTRL->SYS_GPIO_OMUX[index] & ~ (0x3F << reg_shift));
    SYSCTRL->SYS_GPIO_OMUX[index] =  SYSCTRL->SYS_GPIO_OMUX[index] |  (mode << reg_shift);

    return;
}
#endif

#if 1//(RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

#else
void Pin_Set_In_Mode_Ex(uint32_t pin_number, uint32_t mode)
{
    uint32_t reg, base, reg_shift, enable;
    uint32_t index;

    /*RT584 mode become 4 bits.. so max mode is 15*/
    if ((pin_number >= 32) || (mode & 0x3F) > 0x3F)
    {
        return;     /*Invalid setting mode.*/
    }


    index = 0;
    base = 0;
    reg_shift = 0;

    index  = (mode >> 28 & 0x07);
    reg_shift = (mode & 0x18);
    enable = ((mode & 0x18) + 7);

    SYSCTRL->SYS_GPIO_IMUX[index] = (SYSCTRL->SYS_GPIO_IMUX[index] & ~(0x3F << reg_shift));
    SYSCTRL->SYS_GPIO_IMUX[index] = (SYSCTRL->SYS_GPIO_IMUX[index] | (pin_number << reg_shift));
    SYSCTRL->SYS_GPIO_IMUX[index] = (SYSCTRL->SYS_GPIO_IMUX[index] | (1 << enable));

    return;
}
#endif

uint32_t pin_get_mode(uint32_t pin_number)
{
    uint32_t reg, base, mask_offset;

    if (pin_number >= 32)
    {
        return 0xF;     /*Invalid setting mode.*/
    }

#if 1//(RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

    base = MAP_BASE + (pin_number >> 3) * 4;
    mask_offset = (pin_number & 0x7) << 2;

    reg = *((volatile unsigned int *) base);
    reg = reg >> mask_offset;
    reg = reg & 0x7;

#else
    //    if( rt58x_pin_mux_define[pin_number][mode][1] != MODE_PIN_MUX_NULL)
    //    {
    //        if( rt58x_pin_mux_define[pin_number][mode][2] != MODE_PIN_MUX_NULL )
    //        {
    //            Pin_Set_Out_Mode_Ex(pin_number, rt58x_pin_mux_define[pin_number][mode][2]);
    //        }
    //        if( rt58x_pin_mux_define[pin_number][mode][3] != MODE_PIN_MUX_NULL )
    //        {
    //            Pin_Set_In_Mode_Ex(pin_number, rt58x_pin_mux_define[pin_number][mode][3]);
    //        }
    //    }
#endif

    return reg;
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue.
 * so we need critical section protect.
 *
 */
void enable_perclk(uint32_t clock)
{
    if ((clock < UART0_CLK) || (clock > GPIO_32K_CLK))
    {
        return;     /*Invalid setting mode.*/
    }

    SYSCTRL->sys_clk_ctrl.reg |= (1 << clock) ;
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue,
 *  so we need critical section protect.
 *
 */
void disable_perclk(uint32_t clock)
{
    if ((clock < UART0_CLK) || (clock > GPIO_32K_CLK))
    {
        return;     /*Invalid setting mode.*/
    }

    SYSCTRL->sys_clk_ctrl.reg  &= ~(1 << clock);
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue, (in fact, almost impossible)
 *  so we need critical section protect.
 *
 */
void pin_set_pullopt(uint32_t pin_number, uint32_t mode)
{
    uint32_t reg, base, mask_offset, mask;

    if ((pin_number >= 32) || (mode > 7))
    {
        return;     /*Invalid setting mode.*/
    }

    base = PULLOPT_BASE + (pin_number >> 3) * 4;
    mask_offset = (pin_number & 0x7) << 2;
    mask = 0xF << mask_offset;


    /*pin mux setting is share resource.*/
    reg = *((volatile unsigned int *) base);
    reg = reg & ~mask;
    reg = reg | (mode << mask_offset);

    *((volatile unsigned int *)base) =  reg;

    return;
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue, (in fact, almost impossible)
 *  so we need critical section protect.
 *
 */
void pin_set_drvopt(uint32_t pin_number, uint32_t mode)
{
    uint32_t reg, base, mask_offset, mask;

    if ((pin_number >= 32) || (mode > 3))
    {
        return;     /*Invalid setting mode.*/
    }

    base = DRV_BASE + (pin_number >> 4) * 4;
    mask_offset = (pin_number & 0xF) << 1;
    mask = 0x3 << mask_offset;

    /*pin mux setting is share resource.*/
    reg = *((volatile unsigned int *) base);
    reg = reg & ~mask;
    reg = reg | (mode << mask_offset);

    *((volatile unsigned int *)base) =  reg;

    return;
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue, so we need critical section protect.
 * In fact, it is almost impossible to dynamic change open drain.
 *
 */
void enable_pin_opendrain(uint32_t pin_number)
{
    uint32_t base, mask, reg;

    if (pin_number >= 32)
    {
        return;     /*Invalid setting mode.*/
    }

    base = OD_BASE ;
    mask = 1 << pin_number ;

    /*pin mux setting is share resource.*/
    reg = *((volatile unsigned int *) base);
    reg = reg | mask;
    *((volatile unsigned int *)base) =  reg;

    return;
}

/*
 * For multithread OS, and dynamic enable/disable peripheral environment,
 * Set this register has race condition issue, so we need critical section protect.
 * In fact, it is almost impossible to dynamic change open drain.
 *
 */
void disable_pin_opendrain(uint32_t pin_number)
{
    uint32_t base, mask, reg;

    if (pin_number >= 32)
    {
        return;     /*Invalid setting mode.*/
    }

    base = OD_BASE ;
    mask = ~(1 << pin_number);

    /*pin mux setting is share resource.*/
    reg = *((volatile unsigned int *) base);
    reg = reg & mask;
    *((volatile unsigned int *)base) =  reg;

    return;
}

uint32_t change_ahb_system_clk(sys_clk_sel_t sys_clk_mode)
{
    uint32_t i = 0;
    uint32_t pll_clk = 0;

    if (sys_clk_mode > SYS_CLK_MAX)                                         /*Invalid parameter*/
    {
        return STATUS_ERROR;
    }

    SYSCTRL->sys_clk_ctrl.reg = 0;                                      /*Set MCU clock source to default 32MHZ*/

    if ( sys_clk_mode <= SYS_RCO1MHZ_CLK)
    {
        SYSCTRL->sys_clk_ctrl.bit.cfg_bbpll_en = 0;

        if ( (sys_clk_mode) == SYS_RCO1MHZ_CLK )
        {
            PMU_CTRL->soc_pmu_rco1m.bit.en_rco_1m = 1;
        }

        SYSCTRL->sys_clk_ctrl.reg = (SYSCTRL->sys_clk_ctrl.reg & ~MCU_HCLK_SEL_MASK);
        SYSCTRL->sys_clk_ctrl.reg = (SYSCTRL->sys_clk_ctrl.reg & ~MCU_HCLK_SEL_MASK) | (sys_clk_mode << MCU_HCLK_SEL_SHFT);
    }
    else
    {
        pll_clk = (sys_clk_mode & SYS_PLL_CLK_OFFSET);

        SYSCTRL->sys_clk_ctrl.reg = (SYSCTRL->sys_clk_ctrl.reg & ~MCU_BBPLL_CLK_MASK) | (pll_clk << MCU_BBPLL_CLK_SHIFT);
        SYSCTRL->sys_clk_ctrl.reg = (SYSCTRL->sys_clk_ctrl.reg | MCU_BBPLL_ENABLE);      /*config BASEBAND_PLL_ENABLE*/

        /*baseband pll enable wait delay time 200us*/
        for (i = 0; i < PLL_WAIT_PERIOD; i++)
        {
            __NOP();
        }

        SYSCTRL->sys_clk_ctrl.reg = (SYSCTRL->sys_clk_ctrl.reg & ~MCU_HCLK_SEL_MASK) | HCLK_SEL_PLL;
    }


    return STATUS_SUCCESS;
}

uint32_t get_ahb_system_clk(void)
{
    uint32_t clk_mode = 0;

    clk_mode = ((SYSCTRL->sys_clk_ctrl.reg & MCU_HCLK_SEL_MASK) >> MCU_HCLK_SEL_SHFT);

    if (clk_mode == HCLK_SEL_PLL)
    {
        clk_mode = ((SYSCTRL->sys_clk_ctrl.reg & MCU_BBPLL_CLK_MASK) >> MCU_BBPLL_CLK_SHIFT);
        clk_mode = clk_mode + SYS_PLL_CLK_OFFSET;
    }
    else if (clk_mode > 3)
    {
        clk_mode = SYS_CLK_MAX;
    }

    return clk_mode;
}

uint32_t change_peri_clk(perclk_clk_sel_t sys_clk_mode)
{
    if ( sys_clk_mode == PERCLK_SEL_RCO1M )
    {
        PMU_CTRL->soc_pmu_rco1m.bit.en_rco_1m = 1;
    }

    SYSCTRL->sys_clk_ctrl.bit.per_clk_sel = sys_clk_mode;

    return STATUS_SUCCESS;
}

uint32_t get_peri_clk(void)
{
    uint32_t clk_mode;

    clk_mode = SYSCTRL->sys_clk_ctrl.bit.per_clk_sel;

    return clk_mode;
}

void set_slow_clock_source(uint32_t mode)
{
    /*Slow clock selection.*/
    uint32_t  temp;

    if (mode > 3)
    {
        return;    /*Invalid mode*/
    }

    SYSCTRL->sys_clk_ctrl.bit.slow_clk_sel = mode;
}
