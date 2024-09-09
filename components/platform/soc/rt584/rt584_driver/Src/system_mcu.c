/****************************************************************************
 * @file     system_cm33.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/
#include "sysctrl.h"
#include "system_mcu.h"
#include "flashctl.h"

#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
#include "partition.h"
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#ifndef SET_SYS_CLK
#define SET_SYS_CLK    SEL_CLK_32MHZ
#endif


#if (SET_SYS_CLK == SEL_CLK_RCO1MHZ)
#define XTAL    (1000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_16MHZ)
#define XTAL    (16000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_32MHZ)
#define XTAL    (32000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_48MHZ)
#define XTAL    (48000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_64MHZ)
#define XTAL    (64000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_72MHZ)
#define XTAL    (72000000UL)            /* Oscillator frequency               */           /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_36MHZ)
#define XTAL    (36000000UL)            /* Oscillator frequency               */
#elif (SET_SYS_CLK == SEL_CLK_40MHZ)
#define XTAL    (40000000UL)            /* Oscillator frequency               */
#endif

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
void systemcoreclockupdate (void)
{
    SystemCoreClock = SYSTEM_CLOCK;
    SystemFrequency = SYSTEM_CLOCK;
}

void systempmusetmode(pmu_mode_cfg_t pmu_mode)
{
#if 1//RT584_SHUTTLE_IC==1

    if (pmu_mode == PMU_MODE_DCDC)
    {
        PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 1;
        PMU_CTRL->pmu_en_control.bit.en_dcdc_nm = 1;
        PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 0;
    }
    else if (pmu_mode == PMU_MODE_LDO)
    {
        PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 1;
        PMU_CTRL->pmu_en_control.bit.en_dcdc_nm = 0;
        PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 1;
    }

#endif
}


void rco1m_and_rco32k_init()
{

    //RT584 Shuttle IC
#if 1//RT584_SHUTTLE_IC==1

//20K
//  PMU_CTRL->PMU_OSC32K.bit.TUNE_FINE_RCO_32K = 0;
//  PMU_CTRL->PMU_OSC32K.bit.TUNE_COARSE_RCO_32K = 0;
//  PMU_CTRL->PMU_OSC32K.bit.PW_BUF_RCO_32K = 0;
//  PMU_CTRL->PMU_OSC32K.bit.PW_RCO_32K = 0;
//  PMU_CTRL->PMU_OSC32K.bit.RCO_32K_SEL = 1;
//  SYSCTRL->SYS_CLK_CTRL2.bit.EN_RCO32K_DIV2 = 1;
//	32K
    PMU_CTRL->pmu_osc32k.bit.tune_fine_rco_32k = 88;
    PMU_CTRL->pmu_osc32k.bit.tune_coarse_rco_32k = 3;
    PMU_CTRL->pmu_osc32k.bit.pw_buf_rco_32k = 3;
    PMU_CTRL->pmu_osc32k.bit.pw_rco_32k = 15;
    PMU_CTRL->pmu_osc32k.bit.rco_32k_sel = 1;	
	
    PMU_CTRL->soc_pmu_rco1m.bit.tune_fine_rco_1m = 70;
    PMU_CTRL->soc_pmu_rco1m.bit.tune_coarse_rco_1m = 11;
    PMU_CTRL->soc_pmu_rco1m.bit.pw_rco_1m = 1;
    PMU_CTRL->soc_pmu_rco1m.bit.test_rco_1m = 0;
    PMU_CTRL->soc_pmu_rco1m.bit.en_rco_1m = 1;


#elif (CHIP_MODEL==RT584_MPA) ||  (RT584_FPGA_MPW==1)
#if (SOC_PMU_SECURE_EN == 1)
     PMU_CTRL->soc_pmu_rco1m.bit.en_rco_1m = 1;
#else
   
#endif
   

#endif

#if RT584_SHUTTLE_IC_CHIP_NUMBER_8_BLACK==1
    /* for Real Chip number 8*/
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_COARSE_RCO_1M = 11;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_FINE_RCO_1M = 63;
#endif

}

void systempmuupdatedcdc()
{
#if 1//RT584_SHUTTLE_IC==1

    //32K
    PMU_CTRL->pmu_osc32k.bit.tune_fine_rco_32k = 88;
    PMU_CTRL->pmu_osc32k.bit.tune_coarse_rco_32k = 3;
    PMU_CTRL->pmu_osc32k.bit.pw_buf_rco_32k = 3;
    PMU_CTRL->pmu_osc32k.bit.pw_rco_32k = 15;
    PMU_CTRL->pmu_osc32k.bit.rco_32k_sel = 1;

    //20K (For RTC 1ms interrupt)
    //  PMU_CTRL->PMU_OSC32K.bit.TUNE_FINE_RCO_32K = 0;
    //  PMU_CTRL->PMU_OSC32K.bit.TUNE_COARSE_RCO_32K = 0;
    //  PMU_CTRL->PMU_OSC32K.bit.PW_BUF_RCO_32K = 0;
    //  PMU_CTRL->PMU_OSC32K.bit.PW_RCO_32K = 0;
    //  PMU_CTRL->PMU_OSC32K.bit.RCO_32K_SEL = 1;
    //  SYSCTRL->SYS_CLK_CTRL2.bit.EN_CK_DIV_32K = 1;
    //  SYSCTRL->SYS_CLK_CTRL2.bit.EN_RCO32K_DIV2 = 1;

    PMU_CTRL->soc_pmu_rco1m.bit.tune_fine_rco_1m = 70;
    PMU_CTRL->soc_pmu_rco1m.bit.tune_coarse_rco_1m = 11;
    PMU_CTRL->soc_pmu_rco1m.bit.pw_rco_1m = 1;
    PMU_CTRL->soc_pmu_rco1m.bit.test_rco_1m = 0;
    PMU_CTRL->soc_pmu_rco1m.bit.en_rco_1m = 1;   				//5ua

    //Offset:609c
    PMU_CTRL->pmu_soc_pmu_timing.bit.force_dcdc_soc_pmu =   1;// sub system PMU ModeControl by CM33

    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_ppower_normal    =   0x00;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_en_comp_normal   =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_npower_normal    =   0x06;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_en_zcd_normal    =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_pdrive_normal    =   0x00;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_mg_normal        =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_ndrive_normal    =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_en_cm_normal     =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_pw_normal        =   0x04;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_c_hg_normal      =   0x01;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_pwmf_normal      =   0x0e;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_c_sc_normal      =   0x00;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_os_pn_normal     =   0x00;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_os_normal        =   0x00;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_hg_normal        =   0x03;
    PMU_CTRL->pmu_dcdc_normal.bit.dcdc_dly_normal       =   0x00;
    //Offset:60a8
    PMU_CTRL->pmu_dcdc_reserved.bit.dcdc_pw_dig_normal =    0x0;
    //Offset:60a0
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_ppower_heavy      =   0x0;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_en_comp_heavy     =   0x1;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_npower_heavy      =   0x2;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_en_zcd_heavy      =   0x1;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_pdrive_heavy      =   0x7;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_mg_heavy          =   0x1;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_ndrive_heavy      =   0x2;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_en_cm_heavy       =   0x1;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_pw_heavy          =   0x0;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_c_hg_heavy        =   0x1;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_pwmf_heavy        =   0x9;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_c_sc_heavy        =   0x0;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_os_pn_heavy       =   0x0;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_os_heavy          =   0x0;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_hg_heavy          =   0x3;
    PMU_CTRL->pmu_dcdc_heavy.bit.dcdc_dly_heavy         =   0x0;
    //Offset:60a8
    PMU_CTRL->pmu_dcdc_reserved.bit.dcdc_pw_dig_heavy   =   0x0;
    //Offset:60a4
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_ppower_light      =   0x3;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_en_comp_light     =   0x1;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_npower_light      =   0x3;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_en_zcd_light      =   0x1;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_pdrive_light      =   0x5;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_mg_light          =   0x1;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_ndrive_light      =   0x6;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_en_cm_light       =   0x1;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_pw_light          =   0x5;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_c_hg_light        =   0x1;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_pwmf_light        =   0xf;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_c_sc_light        =   0x0;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_os_pn_light       =   0x0;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_os_light          =   0x0;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_hg_light          =   0x3;
    PMU_CTRL->pmu_dcdc_light.bit.dcdc_dly_light         =   0x0;
    //Offset:60a8
    PMU_CTRL->pmu_dcdc_reserved.bit.dcdc_pw_dig_light   =   0x0;
    //Offset:60ac
    PMU_CTRL->pmu_ldo_ctrl.bit.dcdc_ioc                 =   0x01;
    //Offset:6020
    PMU_CTRL->pmu_soc_pmu_xtal0.bit.xosc_lpf_c          =   0x03;
    PMU_CTRL->pmu_soc_pmu_xtal0.bit.xosc_lpf_c          =   0x01;
    //
    //  //Offset:60b0
    PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 1;
    PMU_CTRL->pmu_en_control.bit.en_dcdc_nm = 1;
    PMU_CTRL->pmu_en_control.bit.en_ldomv_nm = 0;
    //
    //  //Offset:6098
    PMU_CTRL->pmu_core_vosel.bit.sldo_vosel_sp = 10; // (8~10), chip number 9
    //
    //  //Offset:6090
    PMU_CTRL->pmu_dcdc_vosel.bit.dcdc_vosel_normal = 0x9;
    PMU_CTRL->pmu_dcdc_vosel.bit.dcdc_vosel_heavy = 0x17;
    PMU_CTRL->pmu_dcdc_vosel.bit.dcdc_vosel_light = 0x9;

    //Offset:6094
    PMU_CTRL->pmu_ldomv_vosel.bit.ldomv_vosel_normal = 0x09;
    PMU_CTRL->pmu_ldomv_vosel.bit.ldomv_vosel_heavy = 0x17;
    PMU_CTRL->pmu_ldomv_vosel.bit.ldomv_vosel_light = 0x09;

    //Offset:60b8
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 0x09;
    //Offset:6098
    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 0x9;

#endif
}

void rco1m_and_rco32k_calibration()
{
    //RT584 Shuttle IC
#if 1//RT584_SHUTTLE_IC==1

    //RCO10K
    RCO32K_CAL->cal32k_cfg0.bit.cfg_cal32k_target = 128000; //20KHz=204800 , 32KHz = 128000
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_lock_err = 0x20;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_avg_coarse = 1;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_avg_fine = 2;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_avg_lock = 2;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_dly = 0;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_fine_gain = 10;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_lock_gain = 10;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_track_en = 1;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_skip_coarse = 1;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_cal32k_bound_mode = 0;
    RCO32K_CAL->cal32k_cfg1.bit.cfg_32k_rc_sel = 1;
    RCO32K_CAL->cal32k_cfg1.bit.en_ck_cal32k = 1;
    RCO32K_CAL->cal32k_cfg0.bit.cfg_cal32k_en = 1;

    //RCO1M
    RCO1M_CAL->cal1m_cfg0.bit.cfg_cal_target = 0x22b8e;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_lock_err = 0x20;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_avg_coarse = 1;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_avg_fine = 2;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_avg_lock = 2;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_dly = 0;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_fine_gain = 10;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_lock_gain = 10;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_track_en = 1;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_skip_coarse = 1;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_cal_bound_mode = 0;
    RCO1M_CAL->cal1m_cfg1.bit.cfg_tune_rco_sel = 1;
    RCO1M_CAL->cal1m_cfg1.bit.en_ck_cal = 1;
    RCO1M_CAL->cal1m_cfg0.bit.cfg_cal_en = 1;

	PMU_CTRL->pmu_soc_pmu_timing.bit.cfg_mv_settle_time = 1;
	PMU_CTRL->pmu_soc_pmu_timing.bit.cfg_lv_settle_time = 0;
	PMU_CTRL->pmu_soc_pmu_xtal1.bit.cfg_xtal_settle_time = 19;
	PMU_CTRL->pmu_rvd0.bit.cfg_xtal_fast_time = 9;
	PMU_CTRL->pmu_soc_pmu_timing.bit.cfg_pwrx_settle_time = 0;

	//SRAM Deep sleep in sleep mode.
	SYSCTRL->sram_lowpower_0.reg = 0xffffffff;

	//  
    PMU_CTRL->pmu_bg_control.bit.pmu_res_dis = 1;

#elif RT584_FPGA_MPA==1

#if (SOC_PMU_SECURE_EN == 1)
     PMU_CTRL->SOC_PMU_RCO1M.bit.EN_RCO_1M = 1;
#else
   
#endif
   

#endif

#if RT584_SHUTTLE_IC_CHIP_NUMBER_8_BLACK==1
    /* for Real Chip number 8*/
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_COARSE_RCO_1M = 11;
    PMU_CTRL->SOC_PMU_RCO1M.bit.TUNE_FINE_RCO_1M = 63;
#endif
}
/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void systeminit (void)
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

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)

	 
	 //SystemPmuSetMode(PMU_MODE_DCDC);

		
    rco1m_and_rco32k_init();

    systempmuupdatedcdc();

    rco1m_and_rco32k_calibration();

#if (SYSCTRL_SECURE_EN == 1)

#if (SET_SYS_CLK == SEL_CLK_RCO1MHZ)

    change_ahb_system_clk(SYS_RCO1MHZ_CLK);
    /*
      The peripheral clock source cannot exceed the system clock,
      otherwise the MCU will experience an unknown error.
    */
    change_peri_clk(PERCLK_SEL_RCO1M);

#elif (SET_SYS_CLK == SEL_CLK_16MHZ)

    change_ahb_system_clk(SYS_16MHZ_CLK);
    change_peri_clk(PERCLK_SEL_16M);

#elif (SET_SYS_CLK == SEL_CLK_32MHZ)

    change_ahb_system_clk(SYS_32MHZ_CLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_48MHZ)

    change_ahb_system_clk(SYS_48MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_64MHZ)

    change_ahb_system_clk(SYS_64MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_72MHZ)

    change_ahb_system_clk(SYS_72MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_36MHZ)

    change_ahb_system_clk(SYS_36MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_40MHZ)

    change_ahb_system_clk(SYS_40MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#endif

#endif

#if (FLASHCTRL_SECURE_EN == 1)
    /*set flash timing.*/
    flash_timing_init();

    /*enable flash 4 bits mode*/
    flash_enable_qe();
#endif

#else

#if (SYSCTRL_SECURE_EN == 0)

#if (SET_SYS_CLK == SEL_CLK_RCO1MHZ)

    change_ahb_system_clk(SYS_RCO1MHZ_CLK);
    /*
      The peripheral clock source cannot exceed the system clock,
      otherwise the MCU will experience an unknown error.
    */
    change_peri_clk(PERCLK_SEL_RCO1M);

#elif (SET_SYS_CLK == SEL_CLK_16MHZ)

    change_ahb_system_clk(SYS_16MHZ_CLK);
    change_peri_clk(PERCLK_SEL_16M);

#elif (SET_SYS_CLK == SEL_CLK_32MHZ)

    change_ahb_system_clk(SYS_32MHZ_CLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_48MHZ)

    change_ahb_system_clk(SYS_48MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_64MHZ)

    change_ahb_system_clk(SYS_64MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_72MHZ)

    change_ahb_system_clk(SYS_72MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_36MHZ)

    change_ahb_system_clk(SYS_36MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#elif (SET_SYS_CLK == SEL_CLK_40MHZ)

    change_ahb_system_clk(SYS_40MHZ_PLLCLK);
    change_peri_clk(PERCLK_SEL_32M);

#endif

#endif

#if (FLASHCTRL_SECURE_EN == 0)
    /*set flash timing.*/
    flash_timing_init();

    /*enable flash 4 bits mode*/
    flash_enable_qe();
#endif

#endif

    SystemCoreClock = SYSTEM_CLOCK;
}

//#if  (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
///*This interrupt must be secure world.*/

///*Debug used*/
//void Sec_Ctrl_Handler(void)
//{
//    uint32_t status;

//    status = SEC_CTRL->SEC_INT_STATUS.reg;
//    SEC_CTRL->SEC_INT_CLR.reg = status;
//    status = SEC_CTRL->SEC_INT_STATUS.reg;         /*ensure the clear.*/
//}

//#endif

