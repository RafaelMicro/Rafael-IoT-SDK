/** @file sadc.c
 *
 * @brief SAR ADC driver file.
 *
 */


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "sadc.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define SADC_TEST_MODE          1

#if (SADC_TEST_MODE == 1)
#define SADC_TEST_VALUE         0x5AF
#endif

#define SADC_GAIN_AIN          0x00    /* VGA gain selection, [3:0] +3dB/step, [5:4] +6dB/step */
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
    {SADC_CHANNEL_0, SADC_AIN_0, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_1, SADC_AIN_1, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_2, SADC_AIN_2, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_3, SADC_AIN_3, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_4, SADC_AIN_4, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_5, SADC_AIN_5, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_6, SADC_AIN_6, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_7, SADC_AIN_7, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_8, SADC_AIN_8, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
    {SADC_CHANNEL_9, SADC_AIN_9, SADC_AIN_DISABLED, SADC_GAIN_AIN, SADC_PULL_AIN, SADC_TACQ_EDLY_TIME_16US, SADC_TACQ_EDLY_TIME_16US, SADC_BURST_ENABLE, SADC_MONITOR_LOW_THD_DEFAULT, SADC_MONITOR_HIGH_THD_DEFAULT},
};

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
void Sadc_Analog_Init(void)
{
    //    SADC->SADC_ANA_SET0.bit.CFG_AUX_ANA_SET0 = 0x7F708;

    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_CMSEL = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_CMSEL = 5;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_CMSEL = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_CMSEL = 1;

    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_COMP = 3;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_ADC_OUTPUTSTB = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_TEST_MODE = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_VLDO = 3;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_CLK_SEL = 0;

    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_PW = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_PW = 36;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_PW = 0;
    //    SADC->SADC_ANA_SET1.bit.CFG_AUX_PW = 36;

    //    SADC->SADC_ANA_SET1.bit.CFG_EN_CLKAUX = ENABLE;
}


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void Sadc_Register_Int_Callback(sadc_proc_cb sadc_int_callback)
{
    sadc_reg_handler = sadc_int_callback;

    return;
}

void Sadc_Int_Enable(uint32_t int_mask)
{
    SADC->SADC_INT_CLEAR.reg = SADC_INT_CLEAR_ALL;
    SADC->SADC_INT_MASK.reg = int_mask;
    NVIC_EnableIRQ((IRQn_Type)(Sadc_IRQn));
    return;
}

void Sadc_Int_Disable(void)
{
    NVIC_DisableIRQ((IRQn_Type)(Sadc_IRQn));
    SADC->SADC_INT_MASK.reg = SADC_INT_DISABLE_ALL;
    SADC->SADC_INT_CLEAR.reg = SADC_INT_CLEAR_ALL;
    return;
}

void Sadc_Xdma_Config(uint32_t xdma_start_addr,
                      uint16_t xdma_seg_size,
                      uint16_t xdma_blk_size)
{
    /*Reset XDMA*/
    SADC->SADC_WDMA_CTL1.bit.CFG_SADC_WDMA_CTL1 = ENABLE;

    /*Clear XDMA IRQ*/
    SADC->SADC_INT_CLEAR.bit.WDMA = ENABLE;
    SADC->SADC_INT_CLEAR.bit.WDMA_ERROR = ENABLE;

    /*Enable XDMA IRQ*/
    SADC->SADC_INT_MASK.bit.WDMA = 0;
    SADC->SADC_INT_MASK.bit.WDMA_ERROR = 0;


    /*Set XDMA buffer address*/
    SADC->SADC_WDMA_SET1 = xdma_start_addr;

    /*Set XDMA buffer size*/
    SADC->SADC_WDMA_SET0.bit.CFG_SADC_SEG_SIZE = xdma_seg_size;
    SADC->SADC_WDMA_SET0.bit.CFG_SADC_BLK_SIZE = xdma_blk_size;

    /*Start XDMA for memory access*/
    SADC_SET_XDMA_START();

    return;
}

uint32_t Sadc_Resolution_Compensation(sadc_value_t *p_data)
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

void Sadc_Channel_Enable(sadc_channel_config_t *config_channel)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;
    //uint32_t gpio_pull_ctrl_offset = 0;
    uint32_t gpio_pull_ctrl_bit = 0;

    sadc_pnsel_ch = &(SADC->SADC_PNSEL_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);
    sadc_set_ch = &(SADC->SADC_SET_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);
    sadc_thd_ch = &(SADC->SADC_THD_CH0) + (config_channel->ch_sel * SADC_CH_REG_OFFSET);


    sadc_pnsel_ch->bit.CFG_SADC_PSEL_CH = config_channel->pi_sel;
    sadc_pnsel_ch->bit.CFG_SADC_NSEL_CH = config_channel->ni_sel;
    sadc_pnsel_ch->bit.CFG_SADC_GAIN_CH = config_channel->gain;

    sadc_pnsel_ch->bit.CFG_SADC_PULL_CH = config_channel->pull;
    sadc_pnsel_ch->bit.CFG_SADC_TACQ_CH = config_channel->tacq;
    sadc_pnsel_ch->bit.CFG_SADC_EDLY_CH = config_channel->edly;

    sadc_set_ch->bit.CFG_SADC_BURST_CH = config_channel->burst;

    sadc_thd_ch->bit.CFG_SADC_LTHD_CH = config_channel->low_thd;
    sadc_thd_ch->bit.CFG_SADC_HTHD_CH = config_channel->high_thd;

    return;
}

void Sadc_Channel_Disable(sadc_config_channel_t ch_sel)
{
    volatile sadc_pnsel_ch_t *sadc_pnsel_ch;
    volatile sadc_set_ch_t *sadc_set_ch;
    volatile sadc_thd_ch_t *sadc_thd_ch;

    sadc_pnsel_ch = &(SADC->SADC_PNSEL_CH0) + (ch_sel * SADC_CH_REG_OFFSET);
    sadc_set_ch = &(SADC->SADC_SET_CH0) + (ch_sel * SADC_CH_REG_OFFSET);
    sadc_thd_ch = &(SADC->SADC_THD_CH0) + (ch_sel * SADC_CH_REG_OFFSET);

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

    Sadc_Analog_Init();

    SADC_RES_BIT(p_config->sadc_resolution);            /*Set SADC resolution bit*/

    SADC_OVERSAMPLE_RATE(p_config->sadc_oversample);    /*Set SADC oversample rate*/

    if (p_config->sadc_xdma.enable == ENABLE)
    {
        Sadc_Xdma_Config(p_config->sadc_xdma.start_addr, p_config->sadc_xdma.seg_size, p_config->sadc_xdma.blk_size);

        if (p_config->sadc_xdma.blk_size == 0)
        {
            sadc_xdma_single_mode = ENABLE;
        }
    }

    if (sadc_int_callback != NULL)
    {
        Sadc_Register_Int_Callback(sadc_int_callback);
    }
    Sadc_Int_Enable(p_config->sadc_int_mask.reg);

    SADC_SAMPLE_MODE(p_config->sadc_sample_mode);                    /*Sample rate depends on timer rate*/
    if (p_config->sadc_sample_mode == SADC_SAMPLE_TIMER)
    {
        SADC_TIMER_CLOCK(p_config->sadc_timer.timer_clk_src);        /*Timer clock source = system clock*/
        SADC_TIMER_CLOCK_DIV(p_config->sadc_timer.timer_clk_div);    /*Timer rate configuration*/
    }


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

    Sadc_Init(&p_sadc_config, sadc_int_callback);

    Sadc_Enable();       /*Enable SADC*/
}

void Sadc_Disable(void)
{
    SADC_DISABLE();       /*Disable SADC*/
    SADC_LDO_DISABLE();   /*Disable the SADC LDO*/
    SADC_VGA_DISABLE();   /*Disable the SADC VGA*/

    Sadc_Int_Disable();
    Sadc_Register_Int_Callback(NULL);

    return;
}

void Sadc_Enable(void)
{
    SADC->control0.bit.cfg_sadc_ena = ENABLE;

    return;
}

void Sadc_Start(void)
{
    if (sadc_xdma_single_mode == ENABLE)
    {
        SADC_SET_XDMA_START();
    }

    sadc_convert_state = SADC_CONVERT_START;

    SADC_START();        /*Start to trigger SADC*/

    return;
}

sadc_convert_state_t Sadc_Convert_State_Get(void)
{
    return sadc_convert_state;
}

uint32_t Sadc_Channel_Read(sadc_input_ch_t ch)
{
    uint32_t read_status;

    Enter_Critical_Section();

    if (sadc_ch_read_convert_state != SADC_CONVERT_START)
    {
        sadc_ch_read_convert_state = SADC_CONVERT_START;

        Leave_Critical_Section();

        sadc_convert_ch = ch;
        Sadc_Config_Enable(sadc_config_res, sadc_config_os, sadc_config_int_callback);
        Sadc_Channel_Enable(&sadc_ch_init[ch]);
        Sadc_Start();        /*Start to trigger SADC*/

        read_status = STATUS_SUCCESS;
    }
    else
    {
        Leave_Critical_Section();

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
void Sadc_Handler(void)
{
    sadc_cb_t cb;
    
	sadc_reg_handler(&cb);
}

/*
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

            Sadc_Resolution_Compensation(&sadc_value);
            cb.raw.conversion_value = sadc_value;

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
*/