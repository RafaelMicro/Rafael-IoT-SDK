/**
 * \file            pwm.c
 * \brief           pwm driver file
 *
 */
/*
 * Copyright (c) Rafael Micro
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
 *
 * Author:          ives.lee
 */
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
uint32_t pwm_int_status[PWM_ID_MAX];
/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef struct {
    pwm_callback_t        pwm_callback;      /**< user application ISR handler. */
} pwm_cb_config_t;

typedef struct {
    pwm_t                       *pwm;                           /* pwm based hardware address   */
    IRQn_Type                   irq_num;                        /* pwm IRQ Number                           */
    pwm_cb_config_t             cb;                             /* pwm callback function table  */
    pwm_phase_t                 phase;                          /* pwm phase    */
    uint32_t                    tseq_addr;
    uint32_t                    rseq_addr;
    uint32_t                    frequency;
    uint32_t                    full_count;
    uint32_t                    int_status;
} pwm_handle_t;

static pwm_handle_t  m_pwm_handle[5] = {
    {
        /*PWM0 instatnce*/
        .pwm = PWM0,                                /* pwm  based hardware address  */
        .irq_num = Pwm0_IRQn,                       /* pwm0 IRQ Number              */
        .cb = NULL,                                 /* pwm  callback function       */
        .phase = PWM_PHASE_POSITIVE,                /* pwm  phase                   */
        .tseq_addr = 0,
        .rseq_addr = 0,
        .frequency = 0,
        .full_count = 0,
        .int_status = 0,
    },

    {
        /*PWM1 instatnce*/
        .pwm = PWM1,                                /* pwm  based hardware address  */
        .irq_num = Pwm1_IRQn,                       /* pwm0 IRQ Number              */
        .cb = NULL,                                 /* pwm  callback function       */
        .phase = PWM_PHASE_POSITIVE,                /* pwm  phase                   */
        .tseq_addr = 0,
        .rseq_addr = 0,
        .frequency = 0,
        .full_count = 0,
        .int_status = 0,
    },

    {
        /*PWM2 instatnce*/
        .pwm = PWM2,                                /* pwm  based hardware address  */
        .irq_num = Pwm2_IRQn,                       /* pwm0 IRQ Number              */
        .cb = NULL,                                 /* pwm  callback function       */
        .phase = PWM_PHASE_POSITIVE,                /* pwm  phase                   */
        .tseq_addr = 0,
        .rseq_addr = 0,
        .frequency = 0,
        .full_count = 0,
        .int_status = 0,
    },

    {
        /*PWM3 instatnce*/
        .pwm = PWM3,                                /* pwm  based hardware address  */
        .irq_num = Pwm3_IRQn,                       /* pwm0 IRQ Number              */
        .cb = NULL,                                 /* pwm  callback function       */
        .phase = PWM_PHASE_POSITIVE,                /* pwm  phase                   */
        .tseq_addr = 0,
        .rseq_addr = 0,
        .frequency = 0,
        .full_count = 0,
        .int_status = 0,
    },

    {
        /*PWM4 instatnce*/
        .pwm = PWM4,                                /* pwm  based hardware address  */
        .irq_num = Pwm4_IRQn,                       /* pwm0 IRQ Number              */
        .cb = NULL,                                 /* pwm  callback function       */
        .phase = PWM_PHASE_POSITIVE,                /* pwm  phase                   */
        .tseq_addr = 0,
        .rseq_addr = 0,
        .frequency = 0,
        .full_count = 0,
        .int_status = 0,
    },
};
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/**
 *@brief pwm initinal
 */
uint32_t pwm_init_fmt1_ex(uint32_t id, uint32_t freqency) {
    pwm_t *pwm = m_pwm_handle[id].pwm;

    if (id >= PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_int_status[id] = 0;

    pwm->pwm_int_clear = PWM_RDMA0_INT_CLR | PWM_RDMA1_INT_CLR | PWM_RDMA0_ERR_INT_CLR | PWM_RDMA1_ERR_INT_CLR | PWM_RSEQ_DONE_INT_CLR | PWM_TSEQ_DONE_INT_CLR | PWM_TRSEQ_DONE_INT_CLR;
    pwm->pwm_ctl1 |= PWM_RESET;
    pwm->pwm_rdma0_ctl1 = PWM_RDMA_RESET;
    pwm->pwm_rdma1_ctl1 = PWM_RDMA_RESET;

    pwm->pwm_set0 = (PWM_SEQ_ORDER_R << PWM_CFG0_SEQ_ORDER_SHFT) |
                    (PWM_SEQ_NUM_1 << PWM_CFG0_SEQ_NUM_SEL_SHFT) |
                    (PWM_SEQ_MODE_CONTINUOUS << PWM_CFG0_SEQ_MODE_SHFT) |
                    (PWM_DMA_SMP_FMT_1 << PWM_CFG0_PWM_DMA_FMT_SHFT) |
                    (PWM_COUNTER_MODE_UP << PWM_CFG0_PWM_CNT_MODE_SHFT) |
                    (PWM_DMA_AUTO_ENABLE << PWM_CFG0_SEQ_DMA_AUTO_SHFT) |
                    (PWM_CLK_DIV_1 << PWM_CFG0_CK_DIV_SHFT) |
                    (PWM_TRIGGER_SRC_SELF << PWM_CFG0_PWM_ENA_TRIG_SHFT);

    //PWM Default Simple Format 1, so counter end value is zero
    pwm->pwm_set1 = PWM_DEFAULT_CNT_END_VAL;
    pwm->pwm_set2 = PWM_DEFAULT_PLAY_CNT;

    //RSEQ
    pwm->pwm_set3 = PWM_DEFAULT_ELEMENT_NUM;
    pwm->pwm_set4 = PWM_DEFAULT_REPEAT_NUM;
    pwm->pwm_set5 = PWM_DEFAULT_DLY_NUM;
    //Simple format 1
    pwm->pwm_rdma0_set0 = ((PWM_DEFAULT_ELEMENT_NUM) | ((PWM_DEFAULT_ELEMENT_NUM >> 1) << 16));
    pwm->pwm_rdma0_set1 = (uint32_t)&m_pwm_handle[id].rseq_addr;

    //TSEQ
    pwm->pwm_set6 = PWM_DEFAULT_ELEMENT_NUM;
    pwm->pwm_set7 = PWM_DEFAULT_REPEAT_NUM;
    pwm->pwm_set8 = PWM_DEFAULT_DLY_NUM;

    //Simple format 1
    pwm->pwm_rdma1_set0 = ((PWM_DEFAULT_ELEMENT_NUM) | ((PWM_DEFAULT_ELEMENT_NUM >> 1) << 16));
    pwm->pwm_rdma1_set1 = (uint32_t)&m_pwm_handle[id].tseq_addr;

    pwm->pwm_rdma0_ctl0 &= ~ PWM_RDMA_ENABLE;
    pwm->pwm_rdma1_ctl0 &= ~ PWM_RDMA_ENABLE;
    //pwm interrupt
    pwm->pwm_int_mask = ~(PWM_RDMA0_INT_MASK_ENABLE | PWM_RSEQ_DONE_INT_MASK_ENABLE | PWM_TRSEQ_DONE_INT_MASK_ENABLE);

    m_pwm_handle[id].phase = PWM_PHASE_POSITIVE;

    if (pwm_set_frequency_ex(id, freqency) != STATUS_SUCCESS) {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}

/**
 *@brief pwm initinal
 */
uint32_t pwm_init_fmt0_ex(uint32_t id, uint32_t freqency, uint32_t count_end_value) {
    pwm_t *pwm = m_pwm_handle[id].pwm;

    if (id >= PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_int_status[id] = 0;

    pwm->pwm_int_clear = PWM_RDMA0_INT_CLR | PWM_RDMA1_INT_CLR | PWM_RDMA0_ERR_INT_CLR | PWM_RDMA1_ERR_INT_CLR | PWM_RSEQ_DONE_INT_CLR | PWM_TSEQ_DONE_INT_CLR | PWM_TRSEQ_DONE_INT_CLR;
    pwm->pwm_ctl1 |= PWM_RESET;
    pwm->pwm_rdma0_ctl1 = PWM_RDMA_RESET;
    pwm->pwm_rdma1_ctl1 = PWM_RDMA_RESET;

    pwm->pwm_set0 = (PWM_SEQ_ORDER_R << PWM_CFG0_SEQ_ORDER_SHFT) |
                    (PWM_SEQ_NUM_1 << PWM_CFG0_SEQ_NUM_SEL_SHFT) |
                    (PWM_SEQ_MODE_CONTINUOUS << PWM_CFG0_SEQ_MODE_SHFT) |
                    (PWM_DMA_SMP_FMT_0 << PWM_CFG0_PWM_DMA_FMT_SHFT) |
                    (PWM_COUNTER_MODE_UP << PWM_CFG0_PWM_CNT_MODE_SHFT) |
                    (PWM_DMA_AUTO_ENABLE << PWM_CFG0_SEQ_DMA_AUTO_SHFT) |
                    (PWM_CLK_DIV_1 << PWM_CFG0_CK_DIV_SHFT) |
                    (PWM_TRIGGER_SRC_SELF << PWM_CFG0_PWM_ENA_TRIG_SHFT);

    //PWM Default Simple Format 1, so counter end value is zero
    pwm->pwm_set1 = count_end_value;
    pwm->pwm_set2 = PWM_DEFAULT_PLAY_CNT;

    //RSEQ
    pwm->pwm_set3 = PWM_FMT0_DEFAULT_ELEMENT_NUM;
    pwm->pwm_set4 = PWM_DEFAULT_REPEAT_NUM;
    pwm->pwm_set5 = PWM_DEFAULT_DLY_NUM;

    //Simple format 0
    pwm->pwm_rdma0_set0 = ((PWM_FMT0_DEFAULT_ELEMENT_NUM >> 1) | ((PWM_FMT0_DEFAULT_ELEMENT_NUM >> 2) << 16));
    pwm->pwm_rdma0_set1 = (uint32_t)&m_pwm_handle[id].rseq_addr;

    //TSEQ
    pwm->pwm_set6 = PWM_FMT0_DEFAULT_ELEMENT_NUM;
    pwm->pwm_set7 = PWM_DEFAULT_REPEAT_NUM;
    pwm->pwm_set8 = PWM_DEFAULT_DLY_NUM;

    //Simple format 0
    pwm->pwm_rdma1_set0 = ((PWM_FMT0_DEFAULT_ELEMENT_NUM >> 1) | ((PWM_FMT0_DEFAULT_ELEMENT_NUM >> 2) << 16));
    pwm->pwm_rdma1_set1 = (uint32_t)&m_pwm_handle[id].tseq_addr;

    pwm->pwm_rdma0_ctl0 &= ~ PWM_RDMA_ENABLE;
    pwm->pwm_rdma1_ctl0 &= ~ PWM_RDMA_ENABLE;
    //pwm interrupt
    pwm->pwm_int_mask = ~(PWM_RDMA0_INT_MASK_ENABLE | PWM_RSEQ_DONE_INT_MASK_ENABLE | PWM_TRSEQ_DONE_INT_MASK_ENABLE);

    m_pwm_handle[id].phase = PWM_PHASE_POSITIVE;

    if (pwm_set_frequency_ex(id, freqency) != STATUS_SUCCESS) {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief get pwm frequency
 */
uint32_t pwm_get_frequency_ex(uint32_t id, uint32_t *get_frequency) {
    *(uint32_t *)&get_frequency = m_pwm_handle[id].frequency;

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm output frequency
 */
uint32_t pwm_set_frequency_ex(uint32_t id, uint32_t freqency) {
    uint32_t pwm_frequency = 0;

    pwm_t *pwm = m_pwm_handle[id].pwm;

    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    pwm_frequency = SystemCoreClock / (1 << pwm_set_cfg.bit.cfg_ck_div);
    pwm_frequency = pwm_frequency / freqency;

    if (pwm_set_cfg.bit.cfg_pwm_dma_fmt == PWM_DMA_SMP_FMT_0) {
        pwm->pwm_set1 = pwm_frequency;
    }

    m_pwm_handle[id].frequency = freqency;

    m_pwm_handle[id].full_count = pwm_frequency;

    if (m_pwm_handle[id].phase == PWM_PHASE_POSITIVE) {
        if (pwm_set_cfg.bit.cfg_pwm_dma_fmt == PWM_DMA_SMP_FMT_0) {
            m_pwm_handle[id].full_count = 0;
        } else {
            m_pwm_handle[id].full_count -= 1;
        }
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm clock divider
 */
uint32_t pwm_clock_divider_ex(pwm_id_t id, pwm_clk_div_t pwm_clk_div) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (id > PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_set_cfg.bit.cfg_ck_div = pwm_clk_div;

    pwm->pwm_set0 = pwm_set_cfg.Reg;

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_get_pahse(uint32_t id, uint32_t *get_phase) {

    *(uint32_t *)&get_phase = m_pwm_handle[id].phase;

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm output phase
 */
uint32_t pwm_set_pahse_ex(uint32_t id, pwm_phase_t phase) {
    m_pwm_handle[id].phase = phase;

    return STATUS_SUCCESS;
}
/**
 *@brief get pwm counter
 */
uint32_t pwm_get_count_ex(uint32_t id, uint32_t *full_count) {
    *(uint32_t *)&full_count = m_pwm_handle[id].full_count;

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm couter mode
 */
uint32_t pwm_set_counter_mode_ex(uint32_t id, pwm_counter_mode_t counter_mode) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (id > PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_set_cfg.bit.cfg_pwm_cnt_mode = counter_mode;

    pwm->pwm_set0 = pwm_set_cfg.Reg;

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm counter end value for format 0
 */
uint32_t pwm_set_counter_end_value_ex(uint32_t id, uint32_t counter_end_value) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    //pwm_set_t pwm_set_cfg;
    //pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if ((id > PWM_ID_MAX) || (counter_end_value < PWM_COUNT_END_VALUE_MIN) || (counter_end_value > PWM_COUNT_END_VALUE_MAX)) {
        return STATUS_INVALID_PARAM;
    }

    pwm->pwm_set1 = counter_end_value;

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm dma format, format1 or format0
 */
uint32_t pwm_set_dma_format_ex(uint32_t id, pwm_dma_smp_fmt_t format) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (id > PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_set_cfg.bit.cfg_pwm_dma_fmt = format;

    pwm->pwm_set0 = pwm_set_cfg.Reg;

    return STATUS_SUCCESS;

}
/**
 *@brief set pwm dma format, format1 or format0
 */
uint32_t pwm_get_dma_element_ex(uint32_t id, uint32_t *get_element) {
    pwm_t *pwm = m_pwm_handle[id].pwm;

    if (id > PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;


    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)get_element = pwm->pwm_set3;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)get_element = pwm->pwm_set6;
        }
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)get_element = pwm->pwm_set3;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)get_element = pwm->pwm_set6;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm format 1 duty
 */
uint32_t pwm_fmt1_duty_ex(uint32_t id, uint8_t duty) {
    uint32_t duty_val = 0;
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    duty_val = ((m_pwm_handle[id].full_count *duty) / PWM_DUTY_MAX_PERCENT);

    if (duty == PWM_DUTY_MAX_PERCENT) {
        if (m_pwm_handle[id].phase == PWM_PHASE_POSITIVE) {
            duty_val += 1;
        }
    }

    if (duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
        m_pwm_handle[id].rseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, duty_val, (m_pwm_handle[id].full_count));
    } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
        m_pwm_handle[id].tseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, duty_val, (m_pwm_handle[id].full_count));
    }

    pwm_start_ex(id);

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm format 0 duty
 */
uint32_t pwm_fmt0_duty_ex(uint32_t id, uint8_t duty) {
    uint32_t duty1, duty2 = 0;
    uint32_t count_end_value = 0;
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;
    count_end_value = pwm->pwm_set1;

    if (duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    duty1 = ((count_end_value *duty) / PWM_DUTY_MAX_PERCENT);
    duty2 = duty1;

    if (duty == PWM_DUTY_MAX_PERCENT) {
        if (m_pwm_handle[id].phase == PWM_PHASE_POSITIVE) {
            duty2 = (duty1 + 1);
        }
    }

    if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
        m_pwm_handle[id].rseq_addr =  PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, duty1, duty2);
    } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
        m_pwm_handle[id].tseq_addr =  PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, duty1, duty2);
    }

    pwm_start_ex(id);

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm format 1 counter
 */
uint32_t pwm_fmt1_count_ex(uint32_t id, uint32_t count) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
        m_pwm_handle[id].rseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, (m_pwm_handle[id].full_count));
    } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
        m_pwm_handle[id].tseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, (m_pwm_handle[id].full_count));
    }

    pwm_start_ex(id);

    return STATUS_SUCCESS;
}
/**
 *@brief set pwm format 0 counter
 */
uint32_t pwm_fmt0_count_ex(uint32_t id, uint32_t count) {
    uint32_t count_end_value = 0;
    uint32_t count1 = 0;
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;
    count_end_value = pwm->pwm_set1;

    if (count > count_end_value) {
        return STATUS_INVALID_PARAM;
    }


    count1 = count;

    if (count == count_end_value) {
        if (m_pwm_handle[id].phase == PWM_PHASE_POSITIVE) {
            count1 += 1;
        }
    }

    if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
        m_pwm_handle[id].rseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, count1);
    } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
        m_pwm_handle[id].tseq_addr =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, count1);
    }

    pwm_start_ex(id);

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_multi_init_ex(pwm_config_t pwm_cfg, uint32_t freqency) {
    pwm_t *pwm = m_pwm_handle[pwm_cfg.id].pwm;

    if (pwm_cfg.id > PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    pwm_int_status[pwm_cfg.id] = 0;

    pwm->pwm_int_clear = PWM_RDMA0_INT_CLR | PWM_RDMA1_INT_CLR | PWM_RDMA0_ERR_INT_CLR | PWM_RDMA1_ERR_INT_CLR | PWM_RSEQ_DONE_INT_CLR | PWM_TSEQ_DONE_INT_CLR | PWM_TRSEQ_DONE_INT_CLR;
    pwm->pwm_ctl1 |= PWM_RESET;
    pwm->pwm_rdma0_ctl1 = PWM_RDMA_RESET;
    pwm->pwm_rdma1_ctl1 = PWM_RDMA_RESET;

    pwm->pwm_set0 = (pwm_cfg.seq_order_1st << PWM_CFG0_SEQ_ORDER_SHFT) |
                    (pwm_cfg.seq_num << PWM_CFG0_SEQ_NUM_SEL_SHFT) |
                    (pwm_cfg.seq_mode << PWM_CFG0_SEQ_MODE_SHFT) |
                    (pwm_cfg.dma_smp_fmt << PWM_CFG0_PWM_DMA_FMT_SHFT) |
                    (pwm_cfg.counter_mode << PWM_CFG0_PWM_CNT_MODE_SHFT) |
                    (PWM_DMA_AUTO_ENABLE << PWM_CFG0_SEQ_DMA_AUTO_SHFT) |
                    (pwm_cfg.clk_div << PWM_CFG0_CK_DIV_SHFT) |
                    (pwm_cfg.triggered_src << PWM_CFG0_PWM_ENA_TRIG_SHFT);

    //PWM Default Simple Format 1
    pwm->pwm_set1 = pwm_cfg.count_end_val;
    pwm->pwm_set2 = pwm_cfg.play_cnt;

    //RSEQ
    pwm->pwm_set3 = pwm_cfg.rseq.element_num;
    pwm->pwm_set4 = pwm_cfg.rseq.repeat_num;
    pwm->pwm_set5 = pwm_cfg.rseq.delay_num;

    //Simple format 1
    pwm->pwm_rdma0_set0 = (pwm_cfg.dma_smp_fmt == PWM_DMA_SMP_FMT_0) ? ((pwm_cfg.rseq.element_num >> 1) | ((pwm_cfg.rseq.element_num >> 2) << 16))
                          : ((pwm_cfg.rseq.element_num)    | ((pwm_cfg.rseq.element_num >> 1) << 16));
    pwm->pwm_rdma0_set1 = (uint32_t)pwm_cfg.rseq.rdma_addr;

    //Save rseq start address
    m_pwm_handle[pwm_cfg.id].tseq_addr = pwm->pwm_rdma0_set1;

    //TSEQ
    pwm->pwm_set6 = pwm_cfg.tseq.element_num;
    pwm->pwm_set7 = pwm_cfg.tseq.repeat_num;
    pwm->pwm_set8 = pwm_cfg.tseq.delay_num;

    //Simple format 1
    pwm->pwm_rdma1_set0 = (pwm_cfg.dma_smp_fmt == PWM_DMA_SMP_FMT_0) ? ((pwm_cfg.rseq.element_num >> 1) | ((pwm_cfg.rseq.element_num >> 2) << 16))
                          : ((pwm_cfg.rseq.element_num)    | ((pwm_cfg.rseq.element_num >> 1) << 16));
    pwm->pwm_rdma1_set1 = (uint32_t)pwm_cfg.tseq.rdma_addr;

    //Save tseq start address
    m_pwm_handle[pwm_cfg.id].rseq_addr = pwm->pwm_rdma1_set1;

    pwm->pwm_rdma0_ctl0 &= ~ PWM_RDMA_ENABLE;
    pwm->pwm_rdma1_ctl0 &= ~ PWM_RDMA_ENABLE;
    //pwm interrupt
    pwm->pwm_int_mask = ~(PWM_RDMA0_INT_MASK_ENABLE  | PWM_RDMA1_INT_MASK_ENABLE  | PWM_RSEQ_DONE_INT_MASK_ENABLE | PWM_TSEQ_DONE_INT_MASK_ENABLE | PWM_TRSEQ_DONE_INT_MASK_ENABLE);

    m_pwm_handle[pwm_cfg.id].phase = PWM_PHASE_POSITIVE;

    if (pwm_set_frequency_ex(pwm_cfg.id, freqency) != STATUS_SUCCESS) {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_multi_fmt1_duty_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint8_t duty) {
    uint32_t duty_val = 0;
    pwm_t *pwm = m_pwm_handle[id].pwm;

    uint32_t *rseq_addr = (uint32_t *)pwm->pwm_rdma0_set1;
    uint32_t *tseq_addr = (uint32_t *)pwm->pwm_rdma1_set1;

    duty_val = ((m_pwm_handle[id].full_count *duty) / PWM_DUTY_MAX_PERCENT);

    if (duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    if (seq_order == PWM_SEQ_ORDER_R) {
        *(rseq_addr + element) =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, duty_val, m_pwm_handle[id].full_count);
    } else if (seq_order == PWM_SEQ_ORDER_T) {
        *(tseq_addr + element) =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, duty_val, m_pwm_handle[id].full_count);
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_multi_fmt0_duty_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint8_t th1_duty, uint8_t th2_duty) {
    uint32_t duty_val1 = 0, duty_val2, count_end_value;

    pwm_t *pwm = m_pwm_handle[id].pwm;
    count_end_value = pwm->pwm_set1;
    uint32_t *rseq_addr = (uint32_t *)pwm->pwm_rdma0_set1;
    uint32_t *tseq_addr = (uint32_t *)pwm->pwm_rdma1_set1;

    if (th1_duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    if (th2_duty > PWM_DUTY_MAX_PERCENT) {
        return STATUS_INVALID_PARAM;
    }

    duty_val1 = ((count_end_value *th1_duty) / PWM_DUTY_MAX_PERCENT);
    duty_val2 = ((count_end_value *th2_duty) / PWM_DUTY_MAX_PERCENT);

    if (th1_duty == PWM_DUTY_MAX_PERCENT) {
        if (m_pwm_handle[id].phase == PWM_PHASE_POSITIVE) {
            duty_val2 = (duty_val1 + 1);
        }
    }

    if (seq_order == PWM_SEQ_ORDER_R) {
        *(rseq_addr + element) =   PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, duty_val1, duty_val2);
    } else if (seq_order == PWM_SEQ_ORDER_T) {
        *(tseq_addr + element) =   PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, duty_val1, duty_val2);
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_multi_fmt1_count_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint32_t count) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    uint32_t *rseq_addr = (uint32_t *)pwm->pwm_rdma0_set1;
    uint32_t *tseq_addr = (uint32_t *)pwm->pwm_rdma1_set1;


    if (count > m_pwm_handle[id].full_count) {
        count = (m_pwm_handle[id].full_count + 1);
    }

    if (seq_order == PWM_SEQ_ORDER_R) {
        rseq_addr[element] =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, (m_pwm_handle[id].full_count));
    } else if (seq_order == PWM_SEQ_ORDER_T) {
        tseq_addr[element] =  PWM_FILL_SAMPLE_DATA_MODE1(m_pwm_handle[id].phase, count, (m_pwm_handle[id].full_count));
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_multi_fmt0_count_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint32_t thd1_Count, uint32_t thd2_count) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    uint32_t *rseq_addr = (uint32_t *)pwm->pwm_rdma0_set1;
    uint32_t *tseq_addr = (uint32_t *)pwm->pwm_rdma1_set1;


    if ((thd1_Count > PWM_COUNT_END_VALUE_MAX)) {
        thd1_Count = PWM_COUNT_END_VALUE_MAX;
    }
    if ((thd2_count > PWM_COUNT_END_VALUE_MAX)) {
        thd2_count = PWM_COUNT_END_VALUE_MAX;
    }

    if ((thd1_Count < PWM_COUNT_END_VALUE_MIN)) {
        thd1_Count = PWM_COUNT_END_VALUE_MIN;
    }

    if ((thd2_count < PWM_COUNT_END_VALUE_MIN)) {
        thd2_count = PWM_COUNT_END_VALUE_MIN;
    }

    if (seq_order == PWM_SEQ_ORDER_R) {
        rseq_addr[element] =  PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, thd1_Count, thd2_count);
    } else if (seq_order == PWM_SEQ_ORDER_T) {
        tseq_addr[element] =  PWM_FILL_SAMPLE_DATA_MODE0(m_pwm_handle[id].phase, thd1_Count, thd2_count);
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_set_repeat_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t repeat_number) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            pwm->pwm_set4 = repeat_number;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            pwm->pwm_set7 = repeat_number;
        }
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            pwm->pwm_set4 = repeat_number;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            pwm->pwm_set7 = repeat_number;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_get_repeat_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t *get_repeat_number) {


    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;


    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)&get_repeat_number = pwm->pwm_set4;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)&get_repeat_number = pwm->pwm_set7;
        }
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)&get_repeat_number = pwm->pwm_set4;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)&get_repeat_number = pwm->pwm_set7;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_set_delay_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t dly_number) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;


    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            pwm->pwm_set5 = dly_number;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            pwm->pwm_set8 = dly_number;
        }
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            pwm->pwm_set5 = dly_number;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            pwm->pwm_set0 = dly_number;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_get_delay_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t *get_delay_number) {

    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;


    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)&get_delay_number = pwm->pwm_set5;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)&get_delay_number = pwm->pwm_set8;
        }
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            *(uint32_t *)&get_delay_number = pwm->pwm_set5;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            *(uint32_t *)&get_delay_number = pwm->pwm_set8;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    return STATUS_SUCCESS;
}


/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_start_ex(uint32_t id) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;

    if ((id) >= PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        pwm->pwm_rdma0_ctl0 |= PWM_RDMA_ENABLE;
        pwm->pwm_rdma1_ctl0 |= PWM_RDMA_ENABLE;
    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            pwm->pwm_rdma0_ctl0 |= PWM_RDMA_ENABLE;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            pwm->pwm_rdma1_ctl0 |= PWM_RDMA_ENABLE;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    NVIC_EnableIRQ(m_pwm_handle[id].irq_num);

    pwm->pwm_ctl0 |= (PWM_ENABLE_PWM | PWM_ENABLE_CLK);

    return STATUS_SUCCESS;
}
/**
 *@brief pwm fomrat 0 struct
 */
uint32_t pwm_stop_ex(uint32_t id) {
    pwm_t *pwm = m_pwm_handle[id].pwm;
    pwm_set_t pwm_set_cfg;
    pwm_set_cfg = *(pwm_set_t *)&pwm->pwm_set0;
    uint32_t *rseq_addr = (uint32_t *)pwm->pwm_rdma0_set1;
    uint32_t *tseq_addr = (uint32_t *)pwm->pwm_rdma1_set1;

    uint32_t rseq_number = pwm->pwm_set3;
    uint32_t tseq_number = pwm->pwm_set6;

    uint32_t i = 0;

    if ((id) >= PWM_ID_MAX) {
        return STATUS_INVALID_PARAM;
    }

    if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_2) {
        for (i = 0; i < rseq_number; i++) {
            *(rseq_addr + i) =  0;
        }

        for (i = 0; i < tseq_number; i++) {
            *(tseq_addr + i) =  0;
        }

        pwm->pwm_rdma0_ctl0 &= ~ PWM_RDMA_ENABLE;
        pwm->pwm_rdma1_ctl0 &= ~ PWM_RDMA_ENABLE;

    } else if (pwm_set_cfg.bit.cfg_seq_two_sel == PWM_SEQ_NUM_1 ) {
        if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_R) {
            for (i = 0; i < rseq_number; i++) {
                *(rseq_addr + i) =  0;
            }

            pwm->pwm_rdma0_ctl0 &= ~ PWM_RDMA_ENABLE;
        } else if (pwm_set_cfg.bit.cfg_seq_order == PWM_SEQ_ORDER_T) {
            for (i = 0; i < tseq_number; i++) {
                *(tseq_addr + i) =  0;
            }

            pwm->pwm_rdma1_ctl0 &= ~ PWM_RDMA_ENABLE;
        }
    } else {
        return STATUS_INVALID_PARAM;
    }

    NVIC_DisableIRQ(m_pwm_handle[id].irq_num);

    pwm->pwm_ctl0 &= ~(PWM_ENABLE_PWM | PWM_ENABLE_CLK);

    return STATUS_SUCCESS;

}


/**
 * @brief Function for handling the PWM0 interrupt.
 * @details Checks for PWM interrupt status, and executes PWM handlers for the corresponding status.
 * @param[in] None
 * @return None
 */
void pwm0_handler(void) {
    uint32_t pwm_int_buf;
    pwm_t *pwm = PWM0;

    pwm_int_buf = pwm->pwm_int_status;
    pwm->pwm_int_clear = pwm_int_buf;


    if (pwm_int_buf & PWM_RDMA0_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_0].int_status |= PWM_RDMA0_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA0_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA0_ERR_INT_MASK_ENABLE;
        m_pwm_handle[PWM_ID_0].int_status |= PWM_RDMA0_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_0].int_status |= PWM_RDMA1_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA1_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_0].int_status |= PWM_RDMA1_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_0].int_status |= PWM_RSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_0].int_status |= PWM_TSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TRSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_0].int_status |= PWM_TRSEQ_DONE_STATUS_INT_MASK;
    }
}


/**
 * @brief Function for handling the PWM1 interrupt.
 * @details Checks for PWM interrupt status, and executes PWM handlers for the corresponding status.
 * @param[in] None
 * @return None
 */
void pwm1_handler(void) {
    uint32_t pwm_int_buf;
    pwm_t *pwm = PWM1;

    pwm_int_buf = pwm->pwm_int_status;
    pwm->pwm_int_clear = pwm_int_buf;

    //pwm_int_status[PWM_ID_1] = pwm_int_buf;

    if (pwm_int_buf & PWM_RDMA0_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_1].int_status |= PWM_RDMA0_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA0_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA0_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_1].int_status |= PWM_RDMA0_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_1].int_status |= PWM_RDMA1_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA1_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_1].int_status |= PWM_RDMA1_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_1].int_status |= PWM_RSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_1].int_status |= PWM_TSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TRSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_1].int_status |= PWM_TRSEQ_DONE_STATUS_INT_MASK;
    }
}


/**
 * @brief Function for handling the PWM2 interrupt.
 * @details Checks for PWM interrupt status, and executes PWM handlers for the corresponding status.
 * @param[in] None
 * @return None
 */
void pwm2_handler(void) {
    uint32_t pwm_int_buf;
    pwm_t *pwm = PWM2;

    pwm_int_buf = pwm->pwm_int_status;
    pwm->pwm_int_clear = pwm_int_buf;

    //pwm_int_status[PWM_ID_2] = pwm_int_buf;

    if (pwm_int_buf & PWM_RDMA0_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_2].int_status |= PWM_RDMA0_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA0_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA0_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_2].int_status |= PWM_RDMA0_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_2].int_status |= PWM_RDMA1_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA1_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_2].int_status |= PWM_RDMA1_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_2].int_status |= PWM_RSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_2].int_status |= PWM_TSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TRSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_2].int_status |= PWM_TRSEQ_DONE_STATUS_INT_MASK;
    }
}


/**
 * @brief Function for handling the PWM3 interrupt.
 * @details Checks for PWM interrupt status, and executes PWM handlers for the corresponding status.
 * @param[in] None
 * @return None
 */
void pwm3_handler(void) {
    uint32_t pwm_int_buf;
    pwm_t *pwm = PWM3;

    pwm_int_buf = pwm->pwm_int_status;
    pwm->pwm_int_clear = pwm_int_buf;

    //pwm_int_status[PWM_ID_3] = pwm_int_buf;

    if (pwm_int_buf & PWM_RDMA0_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_3].int_status |= PWM_RDMA0_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA0_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA0_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_3].int_status |= PWM_RDMA0_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_3].int_status |= PWM_RDMA1_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA1_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_3].int_status |= PWM_RDMA1_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_3].int_status |= PWM_RSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_3].int_status |= PWM_TSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TRSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_3].int_status |= PWM_TRSEQ_DONE_STATUS_INT_MASK;
    }
}


/**
 * @brief Function for handling the PWM4 interrupt.
 * @details Checks for PWM interrupt status, and executes PWM handlers for the corresponding status.
 * @param[in] None
 * @return None
 */
void pwm4_handler(void) {
    uint32_t pwm_int_buf;
    pwm_t *pwm = PWM4;

    pwm_int_buf = pwm->pwm_int_status;
    pwm->pwm_int_clear = pwm_int_buf;

    //pwm_int_status[PWM_ID_4] = pwm_int_buf;

    if (pwm_int_buf & PWM_RDMA0_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_4].int_status |= PWM_RDMA0_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA0_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA0_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_4].int_status |= PWM_RDMA0_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_4].int_status |= PWM_RDMA1_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_RDMA1_STATUS_ERR_INT_MASK) {
        pwm->pwm_int_mask |= PWM_RDMA1_ERR_INT_MASK_ENABLE;

        m_pwm_handle[PWM_ID_4].int_status |= PWM_RDMA1_STATUS_ERR_INT_MASK;
    }

    if (pwm_int_buf & PWM_RSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_4].int_status |= PWM_RSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_4].int_status |= PWM_TSEQ_DONE_STATUS_INT_MASK;
    }

    if (pwm_int_buf & PWM_TRSEQ_DONE_STATUS_INT_MASK) {
        m_pwm_handle[PWM_ID_4].int_status |= PWM_TRSEQ_DONE_STATUS_INT_MASK;
    }
}



