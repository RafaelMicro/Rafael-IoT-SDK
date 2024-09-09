 /**
  ******************************************************************************
  * @file    system_cm33.c
  * @author  
  * @brief   system cm33 file
  ******************************************************************************
  * @attention
  * Copyright (c) 2024 Rafael Micro.
  *
  * Permission is hereby granted, free of charge, to any person
  * obtaining a copy of this software and associated documentation
  * files (the "Software"), to deal in the Software without restriction,
  * including without limitation the rights to use, copy, modify, merge,
  * publish, distribute, sublicense, and/or sell copies of the Software,
  * and to permit persons to whom the Software is furnished to do so,
  * subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be
  * included in all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
  * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  * OTHER DEALINGS IN THE SOFTWARE.
  *
  * This file is part of library_name.
  */
#include "system_cm33.h"
#include "flashctl.h"
#include "sysctrl.h"


#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
#include "partition.h"
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define  XTAL            (32000000UL)     /* Oscillator frequency */

#define  SYSTEM_CLOCK    (XTAL)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[64];

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */
uint32_t SystemFrequency = SYSTEM_CLOCK;
/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
 /**
 * @brief    system core clock update
 * @param    None
 * @return   None
 */
void SystemCoreClockUpdate (void)
{
    SystemCoreClock = SYSTEM_CLOCK;
    SystemFrequency = SYSTEM_CLOCK;
}


 /**
 * @brief    rco1m and rco32 calibration initinal
 * @param    None
 * @return   None
 */
void Rco1m_And_Rco32k_init()
{
    //RT584 Shuttle IC
#if RT584_SHUTTLE_IC==1

    PMU_CTRL->PMU_OSC32K.bit.TUNE_FINE_RCO_32K = 88;
    PMU_CTRL->PMU_OSC32K.bit.TUNE_COARSE_RCO_32K = 3;
    PMU_CTRL->PMU_OSC32K.bit.PW_BUF_RCO_32K = 3;
    PMU_CTRL->PMU_OSC32K.bit.PW_RCO_32K = 15;
    PMU_CTRL->PMU_OSC32K.bit.RCO_32K_SEL = 1;
    SYSCTRL->SYS_CLK_CTRL2.bit.EN_CK_DIV_32K = 1;
    SYSCTRL->SYS_CLK_CTRL2.bit.EN_RCO32K_DIV2 = 1;

    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_FINE_RCO_1M = 70;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_COARSE_RCO_1M = 11;
    PMU_CTRL->SOC_PMU_RCO1M.bit.PW_RCO_1M = 1;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TEST_RCO_1M = 0;
    PMU_CTRL->SOC_PMU_RCO1M.bit.EN_RCO_1M = 1;

#endif
}
 /**
 * @brief    system pmu dcddc mode
 * @param    None
 * @return   None
 */
void SystemPmuUpdateDcdc()
{
#if RT584_SHUTTLE_IC==1
    //32K
    //    PMU_CTRL->PMU_OSC32K.bit.TUNE_FINE_RCO_32K = 88;
    //    PMU_CTRL->PMU_OSC32K.bit.TUNE_COARSE_RCO_32K = 3;
    //    PMU_CTRL->PMU_OSC32K.bit.PW_BUF_RCO_32K = 3;
    //    PMU_CTRL->PMU_OSC32K.bit.PW_RCO_32K = 15;
    //    PMU_CTRL->PMU_OSC32K.bit.RCO_32K_SEL = 1;

    //20K (For RTC 1ms interrupt)
    PMU_CTRL->PMU_OSC32K.bit.TUNE_FINE_RCO_32K = 0;
    PMU_CTRL->PMU_OSC32K.bit.TUNE_COARSE_RCO_32K = 0;
    PMU_CTRL->PMU_OSC32K.bit.PW_BUF_RCO_32K = 0;
    PMU_CTRL->PMU_OSC32K.bit.PW_RCO_32K = 0;
    PMU_CTRL->PMU_OSC32K.bit.RCO_32K_SEL = 1;
    SYSCTRL->SYS_CLK_CTRL2.bit.EN_CK_DIV_32K = 1;
    SYSCTRL->SYS_CLK_CTRL2.bit.EN_RCO32K_DIV2 = 1;

    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_FINE_RCO_1M = 70;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_COARSE_RCO_1M = 11;
    PMU_CTRL->SOC_PMU_RCO1M.bit.PW_RCO_1M = 1;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TEST_RCO_1M = 0;
    PMU_CTRL->SOC_PMU_RCO1M.bit.EN_RCO_1M = 0;

    //Offset:609C
    PMU_CTRL->PMU_SOC_PMU_TIMING.bit.FORCE_DCDC_SOC_PMU =   1;// Sub System PMU ModeControl by CM33

    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_PPOWER_NORMAL    =   0x00;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_EN_COMP_NORMAL   =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_NPOWER_NORMAL    =   0x06;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_EN_ZCD_NORMAL    =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_PDRIVE_NORMAL    =   0x00;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_MG_NORMAL        =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_NDRIVE_NORMAL    =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_EN_CM_NORMAL     =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_PW_NORMAL        =   0x04;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_C_HG_NORMAL      =   0x01;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_PWMF_NORMAL      =   0x0E;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_C_SC_NORMAL      =   0x00;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_OS_PN_NORMAL     =   0x00;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_OS_NORMAL        =   0x00;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_HG_NORMAL        =   0x03;
    PMU_CTRL->PMU_DCDC_NORMAL.bit.DCDC_DLY_NORMAL       =   0x00;
    //Offset:60A8
    PMU_CTRL->PMU_DCDC_RESERVED.bit.DCDC_PW_DIG_NORMAL =    0x0;
    //Offset:60A0
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PPOWER_HEAVY      =   0x0;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_EN_COMP_HEAVY     =   0x1;

#if SUPPORT_SUBG_14DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_NPOWER_HEAVY      =   0x2;
#elif SUPPORT_SUBG_0DBM==1 || SUPPORT_SUBG_20DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_NPOWER_HEAVY      =   0x0;
#else
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_NPOWER_HEAVY      =   0x2;        //default 14dbm
#endif

    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_EN_ZCD_HEAVY      =   0x1;

#if SUPPORT_SUBG_14DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PDRIVE_HEAVY      =   0x7;
#elif SUPPORT_SUBG_0DBM==1 || SUPPORT_SUBG_20DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PDRIVE_HEAVY      =   0x6;
#else
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PDRIVE_HEAVY      =   0x7;        //default 14dbm
#endif

    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_MG_HEAVY          =   0x1;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_NDRIVE_HEAVY      =   0x2;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_EN_CM_HEAVY       =   0x1;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PW_HEAVY          =   0x0;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_C_HG_HEAVY        =   0x1;

#if SUPPORT_SUBG_14DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PWMF_HEAVY        =   0x09;
#elif SUPPORT_SUBG_0DBM==1 || SUPPORT_SUBG_20DBM==1
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PWMF_HEAVY        =   0x08;
#else
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_PWMF_HEAVY        =   0x09;       //default 14dbm
#endif

    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_C_SC_HEAVY        =   0x0;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_OS_PN_HEAVY       =   0x0;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_OS_HEAVY          =   0x0;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_HG_HEAVY          =   0x3;
    PMU_CTRL->PMU_DCDC_HEAVY.bit.DCDC_DLY_HEAVY         =   0x0;
    //Offset:60A8
    PMU_CTRL->PMU_DCDC_RESERVED.bit.DCDC_PW_DIG_HEAVY   =   0x0;
    //Offset:60A4
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_PPOWER_LIGHT      =   0x3;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_EN_COMP_LIGHT     =   0x1;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_NPOWER_LIGHT      =   0x3;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_EN_ZCD_LIGHT      =   0x1;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_PDRIVE_LIGHT      =   0x5;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_MG_LIGHT          =   0x1;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_NDRIVE_LIGHT      =   0x6;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_EN_CM_LIGHT       =   0x1;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_PW_LIGHT          =   0x5;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_C_HG_LIGHT        =   0x1;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_PWMF_LIGHT        =   0xF;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_C_SC_LIGHT        =   0x0;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_OS_PN_LIGHT       =   0x0;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_OS_LIGHT          =   0x0;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_HG_LIGHT          =   0x3;
    PMU_CTRL->PMU_DCDC_LIGHT.bit.DCDC_DLY_LIGHT         =   0x0;
    //Offset:60A8
    PMU_CTRL->PMU_DCDC_RESERVED.bit.DCDC_PW_DIG_LIGHT   =   0x0;
    //Offset:60AC
    PMU_CTRL->PMU_LDO_CTRL.bit.DCDC_IOC                 =   0x01;
		
    //Offset:60AC
    PMU_CTRL->PMU_LDO_CTRL.bit.DCDC_IOC                 =   0x01;
	PMU_CTRL->PMU_LDO_CTRL.bit.LDODIG_SIN				=   0x00;
	PMU_CTRL->PMU_LDO_CTRL.bit.LDODIG_LOUT				=   0x01;
	PMU_CTRL->PMU_LDO_CTRL.bit.LDODIG_IOC_NM			=   0x01; 
		
	PMU_CTRL->PMU_LDO_CTRL.bit.LDOMV_SIN				=   0x00;
	PMU_CTRL->PMU_LDO_CTRL.bit.LDOMV_LOUT				=   0x01; 
	PMU_CTRL->PMU_LDO_CTRL.bit.LDOMV_IOC_NM				=   0x01;
	//Offset:60B8
		
	PMU_CTRL->PMU_RFLDO.bit.LDOANA_LOUT					=   0x01;
	PMU_CTRL->PMU_RFLDO.bit.LDOANA_IOC_NM				=   0x01;
		
    //Offset:6020
    PMU_CTRL->PMU_SOC_PMU_XTAL0.bit.XOSC_LPF_C          =   0x03;
    PMU_CTRL->PMU_SOC_PMU_XTAL0.bit.XOSC_LPF_R          =   0x01;
    //
    //  //Offset:60B0
    PMU_CTRL->PMU_EN_CONTROL.bit.EN_LDOMV_NM = 1;
    PMU_CTRL->PMU_EN_CONTROL.bit.EN_DCDC_NM = 1;
    PMU_CTRL->PMU_EN_CONTROL.bit.EN_LDOMV_NM = 0;
    //
    //  //Offset:6098
    PMU_CTRL->PMU_CORE_VOSEL.bit.SLDO_VOSEL_SP = 10; // (8~10), chip number 9
    //
    //  //Offset:6090
    PMU_CTRL->PMU_DCDC_VOSEL.bit.DCDC_VOSEL_NORMAL = 0x9;

#if SUPPORT_SUBG_14DBM==1
    PMU_CTRL->PMU_DCDC_VOSEL.bit.DCDC_VOSEL_HEAVY = 0x17;
#elif SUPPORT_SUBG_0DBM==1 || SUPPORT_SUBG_20DBM==1
    PMU_CTRL->PMU_DCDC_VOSEL.bit.DCDC_VOSEL_HEAVY = 0x09;
#else
    PMU_CTRL->PMU_DCDC_VOSEL.bit.DCDC_VOSEL_HEAVY = 0x17;               //default 14dbm
#endif


    PMU_CTRL->PMU_DCDC_VOSEL.bit.DCDC_VOSEL_LIGHT = 0x9;

    //Offset:6094
    PMU_CTRL->PMU_LDOMV_VOSEL.bit.LDOMV_VOSEL_NORMAL = 0x09;

#if SUPPORT_SUBG_14DBM==1
    PMU_CTRL->PMU_LDOMV_VOSEL.bit.LDOMV_VOSEL_HEAVY = 0x17;
#elif SUPPORT_SUBG_0DBM==1 || SUPPORT_SUBG_20DBM==1
    PMU_CTRL->PMU_LDOMV_VOSEL.bit.LDOMV_VOSEL_HEAVY = 0x09;
#else
    PMU_CTRL->PMU_LDOMV_VOSEL.bit.LDOMV_VOSEL_HEAVY = 0x17;             //default 14dbm
#endif


    PMU_CTRL->PMU_LDOMV_VOSEL.bit.LDOMV_VOSEL_LIGHT = 0x09;

    //Offset:60B8
    PMU_CTRL->PMU_RFLDO.bit.LDOANA_VTUNE = 0x09;
    //Offset:6098
    PMU_CTRL->PMU_CORE_VOSEL.bit.LDODIG_VOSEL = 0x9;

    PMU_CTRL->PMU_SOC_PMU_XTAL1.bit.XOSC_CAP_INI = 0xf;
#endif
}
/**
 * @brief    Rco1m_And_Rco32k_Calibration
 * @param    None
 * @return   None
 */
void Rco1m_And_Rco32k_Calibration()
{
    //RT584 Shuttle IC
#if RT584_SHUTTLE_IC==1


    //RCO10K
    RCO32K_CAL->CAL32K_CFG0.bit.CFG_CAL32K_TARGET = 204800; //20KHz=204800 , 32KHz = 128000
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_LOCK_ERR = 0x20;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_AVG_COARSE = 1;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_AVG_FINE = 2;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_AVG_LOCK = 2;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_DLY = 0;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_FINE_GAIN = 10;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_LOCK_GAIN = 10;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_TRACK_EN = 1;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_SKIP_COARSE = 1;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_CAL32K_BOUND_MODE = 0;
    RCO32K_CAL->CAL32K_CFG1.bit.CFG_32K_RC_SEL = 1;
    RCO32K_CAL->CAL32K_CFG1.bit.EN_CK_CAL32K = 1;
    RCO32K_CAL->CAL32K_CFG0.bit.CFG_CAL32K_EN = 1;
    //RCO1M
    RCO1M_CAL->CAL1M_CFG0.bit.CFG_CAL_TARGET = 0x22B8E;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_LOCK_ERR = 0x20;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_AVG_COARSE = 1;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_AVG_FINE = 2;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_AVG_LOCK = 2;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_DLY = 0;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_FINE_GAIN = 10;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_LOCK_GAIN = 10;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_TRACK_EN = 1;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_SKIP_COARSE = 1;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_CAL_BOUND_MODE = 0;
    RCO1M_CAL->CAL1M_CFG1.bit.CFG_TUNE_RCO_SEL = 1;
    RCO1M_CAL->CAL1M_CFG1.bit.EN_CK_CAL = 1;
    RCO1M_CAL->CAL1M_CFG0.bit.CFG_CAL_EN = 1;

    //Slow Clock 32K to 10k
    PMU_CTRL->PMU_SOC_PMU_TIMING.bit.CFG_MV_SETTLE_TIME = 1;
    PMU_CTRL->PMU_SOC_PMU_TIMING.bit.CFG_LV_SETTLE_TIME = 0;
    PMU_CTRL->PMU_SOC_PMU_XTAL1.bit.CFG_XTAL_SETTLE_TIME = 19;
    PMU_CTRL->PMU_RVD0.bit.CFG_XTAL_FAST_TIME = 9;
    PMU_CTRL->PMU_SOC_PMU_TIMING.bit.CFG_PWRX_SETTLE_TIME = 0;

    //SRAM Deep Sleep in Sleep mode.
    SYSCTRL->SRAM_LOWPOWER_0.reg = 0xFFFFFFFF;

    //low power
    PMU_CTRL->PMU_BG_CONTROL.bit.PMU_RES_DIS = 1;
#endif
}
/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
/**
 * @brief    Bootloader System initialization function
 * @param    None
 * @return   None
 */
void SystemInit_Bootloader (void)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    //  uint32_t blk_cfg, blk_max, blk_size, blk_cnt;
#endif

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE[0]);
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    /* Coprocessor Access Control Register. It's banked for secure state and non-seure state */
    SCB->CPACR |= ((3U << 10U * 2U) |         /* enable CP10 Full Access */
                   (3U << 11U * 2U)  );       /* enable CP11 Full Access */

    /*Notice: CPACR Secure state address 0xE000ED88.  CPACR_NS is 0xE002ED88
     *   Secure software can also define whether non-secure software
     *   can access ecah of the coprocessor using a register called NSACR
     *   Non-secure Access Control Register.
     */

#endif


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)

    /* Enable BusFault, UsageFault, MemManageFault and SecureFault to ease diagnostic */
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk  |
                   SCB_SHCSR_BUSFAULTENA_Msk  |
                   SCB_SHCSR_MEMFAULTENA_Msk  |
                   SCB_SHCSR_SECUREFAULTENA_Msk);

    /* BFSR register setting to enable precise errors */
    SCB->CFSR |= SCB_CFSR_PRECISERR_Msk;
#endif

    Rco1m_And_Rco32k_init();

    SystemPmuUpdateDcdc();

    Rco1m_And_Rco32k_Calibration();

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#if (SYSCTRL_SECURE_EN == 1)
    SYSCTRL->SYS_CLK_CTRL.reg = 0;
    SYSCTRL->SYS_CLK_CTRL.bit.HCLK_SEL = HCLK_SEL_32M;
    SYSCTRL->SYS_CLK_CTRL.bit.PER_CLK_SEL = PERCLK_SEL_32M;
#endif

#if (FLASHCTRL_SECURE_EN == 1)
    /*set flash timing.*/
    Flash_Timing_Init();

    /*enable flash 4 bits mode*/
    Flash_Enable_Qe();
#endif
#else
#if (SYSCTRL_SECURE_EN == 0)
    SYSCTRL->SYS_CLK_CTRL.reg = 0;
    SYSCTRL->SYS_CLK_CTRL.bit.HCLK_SEL = HCLK_SEL_32M;
    SYSCTRL->SYS_CLK_CTRL.bit.PER_CLK_SEL = PERCLK_SEL_32M;
#endif

#if (FLASHCTRL_SECURE_EN == 0)
    /*set flash timing.*/
    Flash_Timing_Init();

    /*enable flash 4 bits mode*/
    Flash_Enable_Qe();
#endif
#endif

    SystemCoreClock = SYSTEM_CLOCK;
}

/**
 * @brief    System initialization function
 * @param    None
 * @return   None
 */
void SystemInit (void)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    //  uint32_t blk_cfg, blk_max, blk_size, blk_cnt;
#endif

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE[0]);
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    /* Coprocessor Access Control Register. It's banked for secure state and non-seure state */
    SCB->CPACR |= ((3U << 10U * 2U) |         /* enable CP10 Full Access */
                   (3U << 11U * 2U)  );       /* enable CP11 Full Access */

    /*Notice: CPACR Secure state address 0xE000ED88.  CPACR_NS is 0xE002ED88
     *   Secure software can also define whether non-secure software
     *   can access ecah of the coprocessor using a register called NSACR
     *   Non-secure Access Control Register.
     */

#endif


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)

    /* Enable BusFault, UsageFault, MemManageFault and SecureFault to ease diagnostic */
    SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk  |
                   SCB_SHCSR_BUSFAULTENA_Msk  |
                   SCB_SHCSR_MEMFAULTENA_Msk  |
                   SCB_SHCSR_SECUREFAULTENA_Msk);

    /* BFSR register setting to enable precise errors */
    SCB->CFSR |= SCB_CFSR_PRECISERR_Msk;

    TZ_SAU_Setup();
#endif

    Rco1m_And_Rco32k_init();

    SystemPmuUpdateDcdc();

    Rco1m_And_Rco32k_Calibration();

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#if (SYSCTRL_SECURE_EN == 1)
    SYSCTRL->SYS_CLK_CTRL.reg = 0;
    SYSCTRL->SYS_CLK_CTRL.bit.HCLK_SEL = HCLK_SEL_32M;
    SYSCTRL->SYS_CLK_CTRL.bit.PER_CLK_SEL = PERCLK_SEL_32M;
#endif

#if (FLASHCTRL_SECURE_EN == 1)
    /*set flash timing.*/
    Flash_Timing_Init();

    /*enable flash 4 bits mode*/
    Flash_Enable_Qe();
#endif
#else
#if (SYSCTRL_SECURE_EN == 0)
    SYSCTRL->SYS_CLK_CTRL.reg = 0;
    SYSCTRL->SYS_CLK_CTRL.bit.HCLK_SEL = HCLK_SEL_32M;
    SYSCTRL->SYS_CLK_CTRL.bit.PER_CLK_SEL = PERCLK_SEL_32M;
#endif

#if (FLASHCTRL_SECURE_EN == 0)
    /*set flash timing.*/
    Flash_Timing_Init();

    /*enable flash 4 bits mode*/
    Flash_Enable_Qe();
#endif
#endif

    SystemCoreClock = SYSTEM_CLOCK;
}

#if  (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/*This interrupt must be secure world.*/

/*Debug used*/
void Sec_Ctrl_Handler(void)
{
    uint32_t status;

    status = SEC_CTRL->SEC_INT_STATUS.reg;
    SEC_CTRL->SEC_INT_CLR.reg = status;
    status = SEC_CTRL->SEC_INT_STATUS.reg;         /*ensure the clear.*/
}

#endif

