/** @file sadc.c
 *
 * @brief SAR ADC driver file.
 *
 */


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "sadc.h"
#include "sysctrl.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define SADC_GAIN_AIN          0x03     /* VGA gain selection, [3:0] +3dB/step, [5:4] +6dB/step */
#define SADC_PULL_AIN          0x00    /* Channel pull selection, [0] P-CH pull high, [1] P-CH pull low, [2] N-CH pull high, [3] N-CH pull low */

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
static sadc_proc_cb   sadc_reg_handler = NULL;
static uint32_t sadc_xdma_single_mode = DISABLE;
static sadc_convert_state_t  sadc_convert_state = SADC_CONVERT_IDLE;
static sadc_convert_state_t  sadc_ch_read_convert_state = SADC_CONVERT_IDLE;
static int32_t sadc_compensation_offset = 0;
static int32_t sadc_temperature_calibration_offset = 0;

static sadc_input_ch_t  sadc_convert_ch = SADC_CH_NC;
static sadc_value_t sadc_ch_value;

static sadc_config_resolution_t sadc_config_res = SADC_RES_12BIT;
static sadc_config_oversample_t sadc_config_os = SADC_OVERSAMPLE_256;
static sadc_proc_cb sadc_config_int_callback = NULL;

sadc_channel_config_t sadc_ch_init[] =
{
    {SADC_CHANNEL_0, SADC_AIN_0, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_1, SADC_AIN_1, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_2, SADC_AIN_2, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_3, SADC_AIN_3, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_4, SADC_AIN_4, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_5, SADC_AIN_5, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_6, SADC_AIN_6, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_7, SADC_AIN_7, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_8, SADC_AIN_8, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_9, SADC_AIN_9, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_4US, SADC_TACQ_EDLY_TIME_2US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
};

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
void sadc_analog_aio_init(void)
{
    SADC->sadc_ana_set0.bit.aux_adc_debug = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mode = 0;
    SADC->sadc_ana_set0.bit.aux_adc_outputstb = 0;
    SADC->sadc_ana_set0.bit.aux_adc_ospn = 0;
    SADC->sadc_ana_set0.bit.aux_adc_clk_sel = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mcap = 3;
    SADC->sadc_ana_set0.bit.aux_adc_mdly = 2;
    SADC->sadc_ana_set0.bit.aux_adc_sel_duty = 2;
    SADC->sadc_ana_set0.bit.aux_adc_os = 0;
    SADC->sadc_ana_set0.bit.aux_adc_br = 15;
    SADC->sadc_ana_set0.bit.aux_adc_pw = 0;
    SADC->sadc_ana_set0.bit.aux_adc_stb_bit = 0;
    SADC->sadc_ana_set0.bit.aux_pw = 4;

    SADC->sadc_ana_set1.bit.aux_vga_cmsel = 0;
    SADC->sadc_ana_set1.bit.aux_vga_comp = 1;
    SADC->sadc_ana_set1.bit.aux_vga_sin = 0;
    SADC->sadc_ana_set1.bit.aux_vga_lout = 0;
    SADC->sadc_ana_set1.bit.aux_vga_sw_vdd = 0;
    SADC->sadc_ana_set1.bit.aux_vga_vldo = 2;
    SADC->sadc_ana_set1.bit.aux_vga_acm = 15;
    SADC->sadc_ana_set1.bit.aux_vga_pw = 31;
    SADC->sadc_ana_set1.bit.aux_dc_adj = 3;
    SADC->sadc_ana_set1.bit.aux_test_mode = 0;
    SADC->sadc_ana_set1.bit.cfg_en_clkaux = ENABLE;
    SADC->sadc_ana_set1.bit.aux_vga_test_aio_en = 0;

    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 9;
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 9;
}

void sadc_analog_vbat_init(void)
{
    SADC->sadc_ana_set0.bit.aux_adc_debug = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mode = 0;
    SADC->sadc_ana_set0.bit.aux_adc_outputstb = 0;
    SADC->sadc_ana_set0.bit.aux_adc_ospn = 0;
    SADC->sadc_ana_set0.bit.aux_adc_clk_sel = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mcap = 3;
    SADC->sadc_ana_set0.bit.aux_adc_mdly = 2;
    SADC->sadc_ana_set0.bit.aux_adc_sel_duty = 2;
    SADC->sadc_ana_set0.bit.aux_adc_os = 0;
    SADC->sadc_ana_set0.bit.aux_adc_br = 15;
    SADC->sadc_ana_set0.bit.aux_adc_pw = 0;
    SADC->sadc_ana_set0.bit.aux_adc_stb_bit = 0;
    SADC->sadc_ana_set0.bit.aux_pw = 4;

    SADC->sadc_ana_set1.bit.aux_vga_cmsel = 0;
    SADC->sadc_ana_set1.bit.aux_vga_comp = 1;
    SADC->sadc_ana_set1.bit.aux_vga_sin = 0;
    SADC->sadc_ana_set1.bit.aux_vga_lout = 0;
    SADC->sadc_ana_set1.bit.aux_vga_sw_vdd = 0;
    SADC->sadc_ana_set1.bit.aux_vga_vldo = 2;
    SADC->sadc_ana_set1.bit.aux_vga_acm = 15;
    SADC->sadc_ana_set1.bit.aux_vga_pw = 31;
    SADC->sadc_ana_set1.bit.aux_dc_adj = 0;
    SADC->sadc_ana_set1.bit.aux_test_mode = 0;
    SADC->sadc_ana_set1.bit.cfg_en_clkaux = enable;
    SADC->sadc_ana_set1.bit.aux_vga_test_aio_en = 0;

    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 9;
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 9;
}

void sadc_analog_temp_init(void)
{
    SADC->sadc_ana_set0.bit.aux_adc_debug = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mode = 0;
    SADC->sadc_ana_set0.bit.aux_adc_outputstb = 0;
    SADC->sadc_ana_set0.bit.aux_adc_ospn = 0;
    SADC->sadc_ana_set0.bit.aux_adc_clk_sel = 0;
    SADC->sadc_ana_set0.bit.aux_adc_mcap = 3;
    SADC->sadc_ana_set0.bit.aux_adc_mdly = 2;
    SADC->sadc_ana_set0.bit.aux_adc_sel_duty = 2;
    SADC->sadc_ana_set0.bit.aux_adc_os = 0;
    SADC->sadc_ana_set0.bit.aux_adc_br = 15;
    SADC->sadc_ana_set0.bit.aux_adc_pw = 0;
    SADC->sadc_ana_set0.bit.aux_adc_stb_bit = 0;
    SADC->sadc_ana_set0.bit.aux_pw = 4;

    SADC->sadc_ana_set1.bit.aux_vga_cmsel = 0;
    SADC->sadc_ana_set1.bit.aux_vga_comp = 1;
    SADC->sadc_ana_set1.bit.aux_vga_sin = 0;
    SADC->sadc_ana_set1.bit.aux_vga_lout = 0;
    SADC->sadc_ana_set1.bit.aux_vga_sw_vdd = 0;
    SADC->sadc_ana_set1.bit.aux_vga_vldo = 2;
    SADC->sadc_ana_set1.bit.aux_vga_acm = 15;
    SADC->sadc_ana_set1.bit.aux_vga_pw = 31;
    SADC->sadc_ana_set1.bit.aux_dc_adj = 3;
    SADC->sadc_ana_set1.bit.aux_test_mode = 0;
    SADC->sadc_ana_set1.bit.cfg_en_clkaux = ENABLE;
    SADC->sadc_ana_set1.bit.aux_vga_test_aio_en = 0;

    PMU_CTRL->pmu_core_vosel.bit.ldodig_vosel = 9;
    PMU_CTRL->pmu_rfldo.bit.ldoana_vtune = 9;

/*
    PMU_CTRL->SOC_TS.bit.TS_VX = 5;
    PMU_CTRL->SOC_TS.bit.TS_S = 4;
    PMU_CTRL->SOC_TS.bit.TS_EN = 1;
    PMU_CTRL->SOC_TS.bit.TS_RST = 0;
    PMU_CTRL->SOC_TS.bit.TS_CLK_EN = 1;
    PMU_CTRL->SOC_TS.bit.TS_CLK_SEL = 1;
*/
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void sadc_register_int_callback(sadc_proc_cb sadc_int_callback)
{
    sadc_reg_handler = sadc_int_callback;

    return;
}

void sadc_int_enable(uint32_t int_mask)
{
    SADC->sadc_int_clear.reg = SADC_INT_CLEAR_ALL;
    SADC->sadc_int_mask.reg = int_mask;
    NVIC_EnableIRQ((IRQn_Type)(Sadc_IRQn));
    return;
}

void sadc_int_disable(void)
{
    NVIC_DisableIRQ((IRQn_Type)(Sadc_IRQn));
    SADC->sadc_int_mask.reg = SADC_INT_DISABLE_ALL;
    SADC->sadc_int_clear.reg = SADC_INT_CLEAR_ALL;
    return;
}

void sadc_xdma_config(uint32_t xdma_start_addr,
                      uint16_t xdma_seg_size,
                      uint16_t xdma_blk_size)
{
    /*Reset XDMA*/
    SADC->sadc_wdma_ctl1.bit.cfg_sadc_wdma_ctl1 = ENABLE;

    /*Clear XDMA IRQ*/
    SADC->sadc_int_clear.bit.wdma = ENABLE;
    SADC->sadc_int_clear.bit.wdma_error = ENABLE;

    /*Enable XDMA IRQ*/
    SADC->sadc_int_mask.bit.wdma = 0;
    SADC->sadc_int_mask.bit.wdma_error = 0;


    /*Set XDMA buffer address*/
    SADC->sadc_wdma_set1 = xdma_start_addr;

    /*Set XDMA buffer size*/
    SADC->sadc_wdma_set0.bit.cfg_sadc_seg_size = xdma_seg_size;
    SADC->sadc_wdma_set0.bit.cfg_sadc_blk_size = xdma_blk_size;

    /*Start XDMA for memory access*/
    SADC_SET_XDMA_START();

    return;
}

uint32_t sadc_resolution_compensation(sadc_value_t *p_data)
{
    uint32_t compensation_bit = 0;

    if (p_data == NULL)
    {
        return STATUS_INVALID_PARAM;
    }

    switch (SADC_GET_RES_BIT())
    {
    case SADC_RES_8BIT:
        compensation_bit = 6;
        break;

    case SADC_RES_10BIT:
        compensation_bit = 4;
        break;

    case SADC_RES_12BIT:
        compensation_bit = 2;
        break;

    case SADC_RES_14BIT:
        break;

    default:
        break;
    }

    (*p_data) >>= compensation_bit;

    return STATUS_SUCCESS;
}

void sadc_channel_enable(sadc_channel_config_t *config_channel)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;
    //uint32_t gpio_pull_ctrl_offset = 0;
    uint32_t gpio_pull_ctrl_bit = 0;


    /*
    sadc_pnsel_ch = &(SADC->SADC_PNSEL_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);
    sadc_set_ch = &(SADC->SADC_SET_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);
    sadc_thd_ch = &(SADC->SADC_THD_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);


    sadc_pnsel_ch->bit.CFG_SADC_PSEL_CH = config_channel->pi_sel;
    sadc_pnsel_ch->bit.CFG_SADC_NSEL_CH = config_channel->ni_sel;
    sadc_pnsel_ch->bit.CFG_SADC_GAIN_CH = config_channel->gain;
    sadc_pnsel_ch->bit.CFG_SADC_PULL_CH = config_channel->pull;
    sadc_pnsel_ch->bit.CFG_SADC_REF_IN_CH  = 1;

    sadc_pnsel_ch->bit.CFG_SADC_TACQ_CH = config_channel->tacq;
    sadc_pnsel_ch->bit.CFG_SADC_EDLY_CH = config_channel->edly;

    sadc_set_ch->bit.CFG_SADC_BURST_CH = config_channel->burst;


    sadc_thd_ch->bit.CFG_SADC_LTHD_CH = config_channel->low_thd;
    sadc_thd_ch->bit.CFG_SADC_HTHD_CH = config_channel->high_thd;
    */

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0);
    sadc_set_ch = &(SADC->sadc_set_ch0);
    sadc_thd_ch = &(SADC->sadc_thd_ch0);


    sadc_pnsel_ch->bit.cfg_sadc_psel_ch = 0;
    sadc_pnsel_ch->bit.cfg_sadc_nsel_ch = 15;
    sadc_pnsel_ch->bit.cfg_sadc_gain_ch = 2;
    sadc_pnsel_ch->bit.cfg_sadc_pull_ch = 0;
    sadc_pnsel_ch->bit.cfg_sadc_ref_in_ch  = 1;
    /*
    sadc_pnsel_ch->bit.CFG_SADC_PSEL_CH = 10;
    sadc_pnsel_ch->bit.CFG_SADC_NSEL_CH = 15;
    sadc_pnsel_ch->bit.CFG_SADC_GAIN_CH = 7;
    sadc_pnsel_ch->bit.CFG_SADC_PULL_CH = 3;
    sadc_pnsel_ch->bit.CFG_SADC_REF_IN_CH  = 1;*/

    return;
}

void sadc_channel_disable(sadc_config_channel_t ch_sel)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0) + (ch_sel * SADC_CH_REG_OFFSET);
    sadc_set_ch = &(SADC->sadc_set_ch0) + (ch_sel * SADC_CH_REG_OFFSET);
    sadc_thd_ch = &(SADC->sadc_thd_ch0) + (ch_sel * SADC_CH_REG_OFFSET);

    sadc_pnsel_ch->reg = SADC_PNSEL_CH_REG_DEFAULT;
    sadc_set_ch->reg = SADC_SET_CH_REG_DEFAULT;
    sadc_thd_ch->reg = SADC_THD_CH_REG_DEFAULT;

    return;
}

void sadc_vbat_enable(void)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;
    //uint32_t gpio_pull_ctrl_offset = 0;
    uint32_t gpio_pull_ctrl_bit = 0;

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0);
    sadc_set_ch = &(SADC->sadc_set_ch0);
    sadc_thd_ch = &(SADC->sadc_thd_ch0);

    sadc_pnsel_ch->bit.cfg_sadc_psel_ch = 10;
    sadc_pnsel_ch->bit.cfg_sadc_nsel_ch = 15;
    sadc_pnsel_ch->bit.cfg_sadc_gain_ch = 7;
    sadc_pnsel_ch->bit.cfg_sadc_pull_ch = 3;
    sadc_pnsel_ch->bit.cfg_sadc_ref_in_ch  = 1;
    return;
}

void sadc_vbat_disable(void)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0);
    sadc_set_ch = &(SADC->sadc_set_ch0);
    sadc_thd_ch = &(SADC->sadc_thd_ch0);

    sadc_pnsel_ch->reg = SADC_PNSEL_CH_REG_DEFAULT;
    sadc_set_ch->reg = SADC_SET_CH_REG_DEFAULT;
    sadc_thd_ch->reg = SADC_THD_CH_REG_DEFAULT;

    return;
}

void Sadc_Temp_Enable(void)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;
    //uint32_t gpio_pull_ctrl_offset = 0;
    uint32_t gpio_pull_ctrl_bit = 0;

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0);
    sadc_set_ch = &(SADC->sadc_set_ch0);
    sadc_thd_ch = &(SADC->sadc_thd_ch0);

    sadc_pnsel_ch->bit.cfg_sadc_psel_ch = 8;
    sadc_pnsel_ch->bit.cfg_sadc_nsel_ch = 8;
    sadc_pnsel_ch->bit.cfg_sadc_gain_ch = 3;
    sadc_pnsel_ch->bit.cfg_sadc_pull_ch = 0;
    sadc_pnsel_ch->bit.cfg_sadc_ref_in_ch  = 0;
    return;
}

void sadc_temp_disable(void)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;

    sadc_pnsel_ch = &(SADC->sadc_pnsel_ch0);
    sadc_set_ch = &(SADC->sadc_set_ch0);
    sadc_thd_ch = &(SADC->sadc_thd_ch0);

    sadc_pnsel_ch->reg = SADC_PNSEL_CH_REG_DEFAULT;
    sadc_set_ch->reg = SADC_SET_CH_REG_DEFAULT;
    sadc_thd_ch->reg = SADC_THD_CH_REG_DEFAULT;

    return;
}

uint32_t Sadc_Init(sadc_config_t *p_config, sadc_proc_cb sadc_int_callback)
{
    if (p_config == NULL)
    {
        return STATUS_INVALID_PARAM;
    }

    sadc_xdma_single_mode = DISABLE;
    sadc_convert_state = SADC_CONVERT_IDLE;

    SADC_RESET();                                       /*Reset SADC*/

    Sadc_Analog_Aio_Init();
    //Sadc_Analog_Vbat_Init();

    SADC_RES_BIT(p_config->sadc_resolution);            /*Set SADC resolution bit*/

    SADC_OVERSAMPLE_RATE(p_config->sadc_oversample);    /*Set SADC oversample rate*/

    if (p_config->sadc_xdma.enable == ENABLE)
    {
        sadc_xdma_config(p_config->sadc_xdma.start_addr, p_config->sadc_xdma.seg_size, p_config->sadc_xdma.blk_size);

        if (p_config->sadc_xdma.blk_size == 0)
        {
            sadc_xdma_single_mode = ENABLE;
        }
    }

    if (sadc_int_callback != NULL)
    {
        sadc_register_int_callback(sadc_int_callback);
    }
    sadc_int_enable(p_config->sadc_int_mask.reg);

    SADC_SAMPLE_MODE(p_config->sadc_sample_mode);                    /*Sample rate depends on timer rate*/
    if (p_config->sadc_sample_mode == SADC_SAMPLE_TIMER)
    {
        SADC_TIMER_CLOCK(p_config->sadc_timer.timer_clk_src);        /*Timer clock source = system clock*/
        SADC_TIMER_CLOCK_DIV(p_config->sadc_timer.timer_clk_div);    /*Timer rate configuration*/
    }

    //for analog test
    SADC->sadc_set1.bit.cfg_sadc_tst = 12;
    SADC->sadc_set1.bit.cfg_sadc_chx_sel = 0;
    SADC->sadc_ctl0.bit.cfg_sadc_ck_free = 1;

#if (SADC_TEST_MODE == 1)
    SADC_TEST_ENABLE();
    SADC_TEST_ADJUST_VALUE(SADC_TEST_VALUE);
#elif (SADC_CALIBRATION_VALUE != 0)
    SADC_TEST_ADJUST_VALUE((uint32_t)SADC_CALIBRATION_VALUE);
#endif

    return STATUS_SUCCESS;
}

void Sadc_Config_Enable(sadc_config_resolution_t res, sadc_config_oversample_t os, sadc_proc_cb sadc_int_callback)
{
    sadc_config_t p_sadc_config;

    //Sadc_Calibration_Init();

    //=== Sadc config backup ===
    sadc_config_res = res;
    sadc_config_os = os;
    sadc_config_int_callback = sadc_int_callback;

    //=== Sadc_Config(&p_sadc_config); start ===
    p_sadc_config.sadc_int_mask.bit.DONE = 1;                         /*Set SADC interrupt mask*/
    p_sadc_config.sadc_int_mask.bit.MODE_DONE = 1;
    p_sadc_config.sadc_int_mask.bit.MONITOR_HIGH = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.MONITOR_LOW = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.VALID = 0;
    p_sadc_config.sadc_int_mask.bit.WDMA = 1;
    p_sadc_config.sadc_int_mask.bit.WDMA_ERROR = 1;

    p_sadc_config.sadc_resolution = res;                              /*Set SADC resolution bit*/

    p_sadc_config.sadc_oversample = os;                               /*Set SADC oversample rate*/

    p_sadc_config.sadc_xdma.enable = DISABLE;
    p_sadc_config.sadc_xdma.start_addr = (uint32_t)&sadc_ch_value;
    p_sadc_config.sadc_xdma.seg_size = 1;
    p_sadc_config.sadc_xdma.blk_size = 0;

    p_sadc_config.sadc_sample_mode = SADC_SAMPLE_START;               /*Sample rate depends on start trigger*/
    //p_sadc_config.sadc_sample_mode = SADC_SAMPLE_TIMER;
    //=== Sadc_Config(&p_sadc_config); end ===

    sadc_init(&p_sadc_config, sadc_int_callback);

    Sadc_Enable();       /*Enable SADC*/
}

uint32_t sadc_vbat_init(sadc_config_t *p_config, sadc_proc_cb sadc_int_callback)
{
    if (p_config == NULL)
    {
        return STATUS_INVALID_PARAM;
    }

    sadc_xdma_single_mode = DISABLE;
    sadc_convert_state = SADC_CONVERT_IDLE;

    SADC_RESET();                                       /*Reset SADC*/

    sadc_analog_vbat_init();

    SADC_RES_BIT(p_config->sadc_resolution);            /*Set SADC resolution bit*/

    SADC_OVERSAMPLE_RATE(p_config->sadc_oversample);    /*Set SADC oversample rate*/

    if (p_config->sadc_xdma.enable == ENABLE)
    {
        sadc_xdma_config(p_config->sadc_xdma.start_addr, p_config->sadc_xdma.seg_size, p_config->sadc_xdma.blk_size);

        if (p_config->sadc_xdma.blk_size == 0)
        {
            sadc_xdma_single_mode = ENABLE;
        }
    }

    if (sadc_int_callback != NULL)
    {
        sadc_register_int_callback(sadc_int_callback);
    }
    sadc_int_enable(p_config->sadc_int_mask.reg);

    SADC_SAMPLE_MODE(p_config->sadc_sample_mode);                    /*Sample rate depends on timer rate*/
    if (p_config->sadc_sample_mode == SADC_SAMPLE_TIMER)
    {
        SADC_TIMER_CLOCK(p_config->sadc_timer.timer_clk_src);        /*Timer clock source = system clock*/
        SADC_TIMER_CLOCK_DIV(p_config->sadc_timer.timer_clk_div);    /*Timer rate configuration*/
    }

    //for analog test
    //SADC->SADC_SET1.bit.CFG_SADC_TST = 12;
    SADC->sadc_set1.bit.cfg_sadc_tst = 8;
    SADC->sadc_set1.bit.cfg_sadc_chx_sel = 0;
    SADC->sadc_ctl0.bit.cfg_sadc_ck_free = 1;

#if (SADC_TEST_MODE == 1)
    SADC_TEST_ENABLE();
    SADC_TEST_ADJUST_VALUE(SADC_TEST_VALUE);
#elif (SADC_CALIBRATION_VALUE != 0)
    SADC_TEST_ADJUST_VALUE((uint32_t)SADC_CALIBRATION_VALUE);
#endif

    return STATUS_SUCCESS;
}

void sadc_vbat_config_enable(sadc_config_resolution_t res, sadc_config_oversample_t os, sadc_proc_cb sadc_int_callback)
{
    sadc_config_t p_sadc_config;

    //Sadc_Calibration_Init();

    //=== Sadc config backup ===
    sadc_config_res = res;
    sadc_config_os = os;
    sadc_config_int_callback = sadc_int_callback;

    //=== Sadc_Config(&p_sadc_config); start ===
    p_sadc_config.sadc_int_mask.bit.DONE = 1;                         /*Set SADC interrupt mask*/
    p_sadc_config.sadc_int_mask.bit.MODE_DONE = 1;
    p_sadc_config.sadc_int_mask.bit.MONITOR_HIGH = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.MONITOR_LOW = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.VALID = 0;
    p_sadc_config.sadc_int_mask.bit.WDMA = 1;
    p_sadc_config.sadc_int_mask.bit.WDMA_ERROR = 1;

    p_sadc_config.sadc_resolution = res;                              /*Set SADC resolution bit*/

    p_sadc_config.sadc_oversample = os;                               /*Set SADC oversample rate*/

    p_sadc_config.sadc_xdma.enable = DISABLE;
    p_sadc_config.sadc_xdma.start_addr = (uint32_t)&sadc_ch_value;
    p_sadc_config.sadc_xdma.seg_size = 1;
    p_sadc_config.sadc_xdma.blk_size = 0;

    p_sadc_config.sadc_sample_mode = SADC_SAMPLE_START;               /*Sample rate depends on start trigger*/
    //p_sadc_config.sadc_sample_mode = SADC_SAMPLE_TIMER;
    //=== Sadc_Config(&p_sadc_config); end ===

    sadc_vbat_init(&p_sadc_config, sadc_int_callback);

    sadc_enable();       /*Enable SADC*/
}


uint32_t sadc_temp_init(sadc_config_t *p_config, sadc_proc_cb sadc_int_callback)
{
    if (p_config == NULL)
    {
        return STATUS_INVALID_PARAM;
    }

    sadc_xdma_single_mode = DISABLE;
    sadc_convert_state = SADC_CONVERT_IDLE;

    SADC_RESET();                                       /*Reset SADC*/

    sadc_analog_temp_init();

    SADC_RES_BIT(p_config->sadc_resolution);            /*Set SADC resolution bit*/

    SADC_OVERSAMPLE_RATE(p_config->sadc_oversample);    /*Set SADC oversample rate*/

    if (p_config->sadc_xdma.enable == ENABLE)
    {
        sadc_xdma_config(p_config->sadc_xdma.start_addr, p_config->sadc_xdma.seg_size, p_config->sadc_xdma.blk_size);

        if (p_config->sadc_xdma.blk_size == 0)
        {
            sadc_xdma_single_mode = ENABLE;
        }
    }

    if (sadc_int_callback != NULL)
    {
        sadc_register_int_callback(sadc_int_callback);
    }
    sadc_int_enable(p_config->sadc_int_mask.reg);

    SADC_SAMPLE_MODE(p_config->sadc_sample_mode);                    /*Sample rate depends on timer rate*/
    if (p_config->sadc_sample_mode == SADC_SAMPLE_TIMER)
    {
        SADC_TIMER_CLOCK(p_config->sadc_timer.timer_clk_src);        /*Timer clock source = system clock*/
        SADC_TIMER_CLOCK_DIV(p_config->sadc_timer.timer_clk_div);    /*Timer rate configuration*/
    }

    //for analog test
    //SADC->SADC_SET1.bit.CFG_SADC_TST = 12;
    SADC->sadc_set1.bit.cfg_sadc_tst = 8;
    SADC->sadc_set1.bit.cfg_sadc_chx_sel = 0;
    SADC->sadc_ctl0.bit.cfg_sadc_ck_free = 1;

#if (SADC_TEST_MODE == 1)
    SADC_TEST_ENABLE();
    SADC_TEST_ADJUST_VALUE(SADC_TEST_VALUE);
#elif (SADC_CALIBRATION_VALUE != 0)
    SADC_TEST_ADJUST_VALUE((uint32_t)SADC_CALIBRATION_VALUE);
#endif

    return STATUS_SUCCESS;
}

void sadc_temp_config_enable(sadc_config_resolution_t res, sadc_config_oversample_t os, sadc_proc_cb sadc_int_callback)
{
    sadc_config_t p_sadc_config;

    //Sadc_Calibration_Init();

    //=== Sadc config backup ===
    sadc_config_res = res;
    sadc_config_os = os;
    sadc_config_int_callback = sadc_int_callback;

    //=== Sadc_Config(&p_sadc_config); start ===
    p_sadc_config.sadc_int_mask.bit.DONE = 1;                         /*Set SADC interrupt mask*/
    p_sadc_config.sadc_int_mask.bit.MODE_DONE = 1;
    p_sadc_config.sadc_int_mask.bit.MONITOR_HIGH = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.MONITOR_LOW = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.VALID = 0;
    p_sadc_config.sadc_int_mask.bit.WDMA = 1;
    p_sadc_config.sadc_int_mask.bit.WDMA_ERROR = 1;

    p_sadc_config.sadc_resolution = res;                              /*Set SADC resolution bit*/

    p_sadc_config.sadc_oversample = os;                               /*Set SADC oversample rate*/

    p_sadc_config.sadc_xdma.enable = DISABLE;
    p_sadc_config.sadc_xdma.start_addr = (uint32_t)&sadc_ch_value;
    p_sadc_config.sadc_xdma.seg_size = 1;
    p_sadc_config.sadc_xdma.blk_size = 0;

    p_sadc_config.sadc_sample_mode = SADC_SAMPLE_START;               /*Sample rate depends on start trigger*/
    //p_sadc_config.sadc_sample_mode = SADC_SAMPLE_TIMER;
    //=== Sadc_Config(&p_sadc_config); end ===

    sadc_temp_init(&p_sadc_config, sadc_int_callback);

    Sadc_Enable();       /*Enable SADC*/
}

void sadc_temp_config_enable2(sadc_config_resolution_t res, sadc_config_oversample_t os, sadc_proc_cb sadc_int_callback)
{
    sadc_config_t p_sadc_config;

    //Sadc_Calibration_Init();

    //=== Sadc config backup ===
    sadc_config_res = res;
    sadc_config_os = os;
    sadc_config_int_callback = sadc_int_callback;

    //=== Sadc_Config(&p_sadc_config); start ===
    p_sadc_config.sadc_int_mask.bit.DONE = 1;                         /*Set SADC interrupt mask*/
    p_sadc_config.sadc_int_mask.bit.MODE_DONE = 1;
    p_sadc_config.sadc_int_mask.bit.MONITOR_HIGH = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.MONITOR_LOW = 0x3FF;
    p_sadc_config.sadc_int_mask.bit.VALID = 0;
    p_sadc_config.sadc_int_mask.bit.WDMA = 1;
    p_sadc_config.sadc_int_mask.bit.WDMA_ERROR = 1;

    p_sadc_config.sadc_resolution = res;                              /*Set SADC resolution bit*/

    p_sadc_config.sadc_oversample = os;                               /*Set SADC oversample rate*/

    p_sadc_config.sadc_xdma.enable = DISABLE;
    p_sadc_config.sadc_xdma.start_addr = (uint32_t)&sadc_ch_value;
    p_sadc_config.sadc_xdma.seg_size = 1;
    p_sadc_config.sadc_xdma.blk_size = 0;

    p_sadc_config.sadc_sample_mode = SADC_SAMPLE_START;               /*Sample rate depends on start trigger*/
    //p_sadc_config.sadc_sample_mode = SADC_SAMPLE_TIMER;
    //=== Sadc_Config(&p_sadc_config); end ===

    sadc_temp_init(&p_sadc_config, sadc_int_callback);

    Sadc_Temp_Enable();
    Sadc_Enable();       /*Enable SADC*/
}

void sadc_disable(void)
{


    sadc_int_disable();
    sadc_register_int_callback(NULL);

    SADC_DISABLE();       /*Disable SADC*/
    SADC_LDO_DISABLE();   /*Disable the SADC LDO*/
    SADC_VGA_DISABLE();   /*Disable the SADC VGA*/

    return;
}

void sadc_enable(void)
{
    SADC_ENABLE();       /*Enable SADC*/
    SADC_LDO_ENABLE();
    SADC_VGA_ENABLE();
    return;
}


void sadc_aio_disable(uint8_t aio_num)
{
    SYSCTRL->gpio_aio_ctrl.bit.gpio_en_aio |= (0x01 << aio_num);
    
    return;
}

void sadc_aio_enable(uint8_t aio_num)
{
    SYSCTRL->gpio_aio_ctrl.bit.gpio_en_aio |= (0x01 << aio_num);
    return;
}

void sadc_start(void)
{
    if (sadc_xdma_single_mode == ENABLE)
    {
        SADC_SET_XDMA_START();
    }

    sadc_convert_state = SADC_CONVERT_START;

    SADC_START();        /*Start to trigger SADC*/

    return;
}

sadc_convert_state_t sadc_convert_state_get(void)
{
    return sadc_convert_state;
}

uint32_t sadc_channel_read(sadc_input_ch_t ch)
{
    uint32_t read_status;

    enter_critical_section();

    if (sadc_ch_read_convert_state != SADC_CONVERT_START)
    {
        sadc_ch_read_convert_state = SADC_CONVERT_START;

        leave_critical_section();

        sadc_convert_ch = ch;
        sadc_config_enable(sadc_config_res, sadc_config_os, sadc_config_int_callback);
        sadc_channel_enable(&sadc_ch_init[ch]);
        Delay_ms(10);
        sadc_start();        /*Start to trigger SADC*/

        read_status = STATUS_SUCCESS;
    }
    else
    {
        leave_critical_section();

        read_status = STATUS_EBUSY;
    }

    return read_status;
}

uint32_t sadc_vbat_read(void)
{
    uint32_t read_status;

    enter_critical_section();

    if (sadc_ch_read_convert_state != SADC_CONVERT_START)
    {
        sadc_ch_read_convert_state = SADC_CONVERT_START;

        leave_critical_section();

        sadc_convert_ch = SADC_CH_VBAT;
        sadc_vbat_config_enable(sadc_config_res, sadc_config_os, sadc_config_int_callback);
        sadc_vbat_enable();
        Delay_ms(10);
        sadc_start();        /*Start to trigger SADC*/

        read_status = STATUS_SUCCESS;
    }
    else
    {
        leave_critical_section();

        read_status = STATUS_EBUSY;
    }

    return read_status;
}

uint32_t sadc_temp_read(void)
{
    uint32_t read_status;

    enter_critical_section();

    if (sadc_ch_read_convert_state != SADC_CONVERT_START)
    {
        sadc_ch_read_convert_state = SADC_CONVERT_START;

        leave_critical_section();

        sadc_convert_ch = SADC_COMP_TEMPERATURE;
        sadc_temp_config_enable(sadc_config_res, sadc_config_os, sadc_config_int_callback);
        sadc_temp_enable();
        Delay_ms(10);
        sadc_start();        /*Start to trigger SADC*/

        read_status = STATUS_SUCCESS;
    }
    else
    {
        leave_critical_section();

        read_status = STATUS_EBUSY;
    }

    return read_status;
}

uint32_t sadc_temp_read2(void)
{
    uint32_t read_status;

    enter_critical_section();

    if (sadc_ch_read_convert_state != SADC_CONVERT_START)
    {
        sadc_ch_read_convert_state = SADC_CONVERT_START;

        leave_critical_section();

        sadc_convert_ch = SADC_COMP_TEMPERATURE;
        Delay_ms(10);
        Sadc_Start();        /*Start to trigger SADC*/

        read_status = STATUS_SUCCESS;
    }
    else
    {
        leave_critical_section();

        read_status = STATUS_EBUSY;
    }

    return read_status;
}

/**
 * @ingroup SADC_Driver
 * @brief SADC interrupt
 * @details
 * @return
 */
#if 1
void Sadc_Handler(void)
{
    sadc_cb_t cb;
    
	sadc_reg_handler(&cb);
}
#else
void Sadc_Handler(void)
{
    sadc_cb_t cb;
    sadc_int_t reg_sadc_int_status;
    sadc_value_t  sadc_value;
    sadc_cal_type_t cal_type;

    reg_sadc_int_status.reg = SADC->SADC_INT_STATUS.reg;
    SADC->SADC_INT_CLEAR.reg = reg_sadc_int_status.reg;

    if (reg_sadc_int_status.reg != 0)
    {
        if (reg_sadc_int_status.bit.DONE == 1)
        {
        }

        if (reg_sadc_int_status.bit.MODE_DONE == 1)
        {
            if (SADC_GET_SAMPLE_MODE() == SADC_SAMPLE_START)
            {
                sadc_convert_state = SADC_CONVERT_DONE;
            }
        }

        if (reg_sadc_int_status.bit.VALID == 1)
        {
            cb.type = SADC_CB_SAMPLE;
            sadc_value = SADC_GET_ADC_VALUE();

            cb.raw.conversion_value = sadc_value;
            cb.raw.calibration_value = SADC->SADC_R1.bit.SADC_I_12B;
            cb.raw.compensation_value = SADC->SADC_R2.bit.SADC_I_SYN;
            Sadc_Resolution_Compensation(&sadc_value);

            cb.data.sample.value = sadc_value;
            cb.data.sample.channel = sadc_convert_ch;

            sadc_reg_handler(&cb);

            sadc_ch_read_convert_state = SADC_CONVERT_DONE;
        }

        if (reg_sadc_int_status.bit.WDMA == 1)
        {
        }

        if (reg_sadc_int_status.bit.WDMA_ERROR == 1)
        {
        }
    }

}
#endif
