/**
 * \file            pwm.h
 * \brief           pwm header file
 */
/*
 * Copyright (c) 2024 Rafal Micro
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
 *
 * Author:
 */
#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C"
{
#endif
/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * \brief PWM ID mapping.
 */
typedef enum {
    PWM_ID_0,
    PWM_ID_1,
    PWM_ID_2,
    PWM_ID_3,
    PWM_ID_4,
    PWM_ID_MAX,
} pwm_id_t;

/**
 * \brief PWM Clock division table.
 */
typedef enum {
    PWM_CLK_DIV_1 = 0,
    PWM_CLK_DIV_2,
    PWM_CLK_DIV_4,
    PWM_CLK_DIV_8,
    PWM_CLK_DIV_16,
    PWM_CLK_DIV_32,
    PWM_CLK_DIV_64,
    PWM_CLK_DIV_128,
    PWM_CLK_DIV_256,
} pwm_clk_div_t;

/**
 * \brief PWM Sequence order table.
 * Order_0: S0  /  Order_1: S1  /  Order_2: S0S1  /  Order_3: S1S0
 */
typedef enum {
    PWM_SEQ_ORDER_R = 0,
    PWM_SEQ_ORDER_T,
    PWM_SEQ_ORDER_MAX,
}   pwm_seq_order_t;

/**
 * \brief PWM sequence selection table.
 */
typedef enum {
    PWM_SEQ_NUM_1,
    PWM_SEQ_NUM_2,
}   pwm_seq_num_t;

/**
 * \brief PWM sequence playmode table.
 */
typedef enum {
    PWM_SEQ_MODE_NONCONTINUOUS,
    PWM_SEQ_MODE_CONTINUOUS,
}   pwm_seq_mode_t;

/**
 * \brief PWM trigger source table.
 */
typedef enum {
    PWM_TRIGGER_SRC_PWM0 = 0,
    PWM_TRIGGER_SRC_PWM1,
    PWM_TRIGGER_SRC_PWM2,
    PWM_TRIGGER_SRC_PWM3,
    PWM_TRIGGER_SRC_PWM4,
    PWM_TRIGGER_SRC_SELF = 5,
}   pwm_trigger_src_t;

/**
 * \brief PWM DMA sample format table.
 */
typedef enum {
    PWM_DMA_SMP_FMT_0 = 0,
    PWM_DMA_SMP_FMT_1,
}   pwm_dma_smp_fmt_t;

/**
 * \brief PWM counter mode table.
 * UP: Up mode / UD: Up-Down mode
 */
typedef enum {
    PWM_COUNTER_MODE_UP = 0,
    PWM_COUNTER_MODE_UD,
} pwm_counter_mode_t;

/**
 * \brief PWM DMA auto table.
 */
typedef enum {
    PWM_DMA_AUTO_DISABLE = 0,
    PWM_DMA_AUTO_ENABLE,
} pwm_dma_auto_t;

/**
 * \brief PWM Phase table.
 */
typedef enum {
    PWM_PHASE_POSITIVE = 0,
    PWM_PHASE_NEGATIVE,
} pwm_phase_t;
/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define PWM_ENABLE_PWM                  (0x01UL << PWM_CFG0_PWM_ENA_SHFT)
#define PWM_ENABLE_CLK                  (0x01UL << PWM_CFG0_CK_ENA_SHFT)
#define PWM_RESET                       (0x01UL << PWM_CFG0_PWM_RST_SHFT)
#define PWM_RDMA_ENABLE                 (0x01UL << PWM_CFG0_PWM_RDMA0_CTL0_SHFT)
#define PWM_RDMA_RESET                  (0x01UL << PWM_CFG0_PWM_RDMA0_CTL1_SHFT)

#define PWM_RDMA0_INT_CLR               (0x01UL << PWM_RDMA0_INT_CLR_SHFT)
#define PWM_RDMA0_ERR_INT_CLR           (0x01UL << PWM_RDMA0_ERR_INT_CLR_SHFT)
#define PWM_RDMA1_INT_CLR               (0x01UL << PWM_RDMA1_INT_CLR_SHFT)
#define PWM_RDMA1_ERR_INT_CLR           (0x01UL << PWM_RDMA1_ERR_INT_CLR_SHFT)
#define PWM_RSEQ_DONE_INT_CLR           (0x01UL << PWM_RSEQ_DONE_INT_CLR_SHFT)
#define PWM_TSEQ_DONE_INT_CLR           (0x01UL << PWM_TSEQ_DONE_INT_CLR_SHFT)
#define PWM_TRSEQ_DONE_INT_CLR          (0x01UL << PWM_TRSEQ_DONE_INT_CLR_SHFT)

#define PWM_RDMA0_INT_MASK_ENABLE       (0x01UL << PWM_RDMA0_INT_MASK_SHFT)
#define PWM_RDMA0_ERR_INT_MASK_ENABLE   (0x01UL << PWM_RDMA0_ERR_INT_MASK_SHFT)
#define PWM_RDMA1_INT_MASK_ENABLE       (0x01UL << PWM_RDMA1_INT_MASK_SHFT)
#define PWM_RDMA1_ERR_INT_MASK_ENABLE   (0x01UL << PWM_RDMA1_ERR_INT_MASK_SHFT)
#define PWM_RSEQ_DONE_INT_MASK_ENABLE   (0x01UL << PWM_RSEQ_DONE_INT_MASK_SHFT)
#define PWM_TSEQ_DONE_INT_MASK_ENABLE   (0x01UL << PWM_TSEQ_DONE_INT_MASK_SHFT)
#define PWM_TRSEQ_DONE_INT_MASK_ENABLE  (0x01UL << PWM_TRSEQ_DONE_INT_MASK_SHFT)

#define PWM_MAX_NUMBER                                  5
#define PWM_DUTY_MAX_PERCENT                            100


//PWM default Paramater
#define PWM_DEFAULT_CNT_END_VAL     0                     /*Count end Value using format0*/
#define PWM_DEFAULT_ELEMENT_NUM     1                    /*genrator Pulse number*/
#define PWM_FMT0_DEFAULT_ELEMENT_NUM     2                    /*genrator Pulse number*/
#define PWM_DEFAULT_REPEAT_NUM      0                     /*Repeat Pulse number*/
#define PWM_DEFAULT_DLY_NUM         0                     /*Pulse delay number*/
#define PWM_DEFAULT_PLAY_CNT        0                                           /*0:is infinite*/

#define PWM_RDMA0_SET_DEFAULT           0
#define PWM_RDMA1_SET_DEFAULT           0


#define PWM_COUNT_END_VALUE_MAX         32767
#define PWM_COUNT_END_VALUE_MIN         3
/**
 *\brief pwm set0 struct
 */
typedef union pwm_set_s_ {
    struct pwm_set_b_ {
        uint32_t    cfg_seq_order           : 1;
        uint32_t    cfg_seq_two_sel         : 1;
        uint32_t    cfg_seq_mode            : 1;
        uint32_t    cfg_pwm_dma_fmt         : 1;
        uint32_t    cfg_pwm_cnt_mode        : 1;
        uint32_t    cfg_pwm_cnt_trig        : 1;
        uint32_t    cfg_dma_auto            : 1;
        uint32_t    RVD1                    : 1;
        uint32_t    cfg_ck_div              : 4;
        uint32_t    cfg_pwm_ena_trig        : 3;
        uint32_t    RVD2                    : 1;
        uint32_t    RVD3                    : 4;
        uint32_t    RVD4                    : 12;
    } bit;
    uint32_t Reg;
} pwm_set_t;

/**
 *\brief pwm fomrat 1 struct
 */
typedef union pwm_fmt1_s {
    struct pwm_fmt1_b {
        uint32_t    cunt_end                : 16;
        uint32_t    phase                   : 1;
        uint32_t    Thd                     : 15;
    } bit;
    uint32_t Reg;
} pwm_fmt1_t;

/**
 *\brief pwm fomrat 0 struct
 */
typedef union pwm_fmt0_s {
    struct pwm_fmt0_b {
        uint32_t    phase1              : 1;
        uint32_t    Thd1                : 15;
        uint32_t    phase2              : 1;
        uint32_t    Thd2                : 15;
    } bit;
    uint32_t Reg;
} pwm_fmt0_t;


/**
 *\brief pwm interrupt struct
 */
typedef union pwm_int_s {
    struct pwm_int_b {
        uint32_t    rdma0_int           : 1;
        uint32_t    rdma0_err_int       : 1;
        uint32_t    rdma1_int           : 1;
        uint32_t    rdma1_err_int       : 1;
        uint32_t    rseq_done_int       : 1;
        uint32_t    tseq_done_int       : 1;
        uint32_t    rtseq_done_int      : 1;
        uint32_t    reserved            : 25;
    } bit;
    uint32_t Reg;
} pwm_int_t;
/**
 * \brief Convert THD_Value / End_Value / PHA_Value into a 32-bit data
 * Mode0: val1=THD1, val2=THD2
 * Mode1: val0=PHA, val1=THD, val2=end
 */
#define PWM_FILL_SAMPLE_DATA_MODE0(val0,val1,val2)  ((val0 << 31) | (val2 << 16) | (val0 << 15) | (val1))
#define PWM_FILL_SAMPLE_DATA_MODE1(val0,val1,val2)  ((val2 << 16) | (val0 << 15) | (val1))

/**
 * \brief pwm data mode struct
 */
typedef struct {
    uint32_t    rdma_addr;                 /**< xDMA start address configurations for PWM sequence controller. */
    uint16_t    element_num;               /**< Element number configurations for PWM sequence controller. */
    uint16_t    repeat_num;                /**< Repeat number configurations of each element for PWM sequence controller. */
    uint16_t    delay_num;                 /**< Delay number configurations after PWM sequence is play finish for PWM sequence controller. */
} pwm_data_mode1_t;
/**
 * \brief pwm interrupt call back function typedef.
 *
 */
typedef int (*pwm_callback_t)(void *p_arg);

/**
 * \brief Structure for each RDMA configurations
 */
typedef struct {
    uint32_t    rdma_addr;                 /**< xDMA start address configurations for PWM sequence controller. */
    uint16_t    element_num;               /**< Element number configurations for PWM sequence controller. */
    uint16_t    repeat_num;                /**< Repeat number configurations of each element for PWM sequence controller. */
    uint16_t    delay_num;                 /**< Delay number configurations after PWM sequence is play finish for PWM sequence controller. */
} pwm_seq_para_t;


/**
* \brief Structure for each PWM configurations
*/
typedef struct {
    pwm_seq_para_t        pwm_seq0;            /**< Handle of PWM sequence controller configurations for R-SEQ. */
    pwm_seq_para_t        pwm_seq1;            /**< Handle of PWM sequence controller configurations for T-SEQ. */
    uint16_t              pwm_play_cnt;        /**< PWM play amount configuration. */
    uint16_t              pwm_count_end_val;   /**< PWM counter end value configuration. */
    pwm_seq_order_t       pwm_seq_order;       /**< PWM sequence play order configuration. */
    pwm_trigger_src_t     pwm_triggered_src;   /**< PWM play trigger source configuration. */
    pwm_seq_num_t         pwm_seq_num;         /**< PWM sequence number configuration. */
    pwm_id_t              pwm_id;              /**< PWM ID designation. */
    pwm_clk_div_t         pwm_clk_div;         /**< PWM gated clock divider value configuration. */
    pwm_counter_mode_t    pwm_counter_mode;    /**< PWM counter mode configuration. */
    pwm_dma_smp_fmt_t     pwm_dma_smp_fmt;     /**< PWM DMA sample format configuration. */
    pwm_seq_mode_t        pwm_seq_mode;        /**< PWM sequence play mode configuration. */
} pwm_seq_para_head_t;



/**
* \brief Structure for each PWM configurations
*/
typedef struct {

    pwm_id_t              id;              /**< PWM ID designation.                          */
    pwm_seq_num_t         seq_num;         /**< PWM sequence number configuration.           */
    pwm_clk_div_t         clk_div;         /**< PWM gated clock divider value configuration. */
    pwm_phase_t           phase;
    pwm_counter_mode_t    counter_mode;    /**< PWM counter mode configuration.              */
    pwm_dma_smp_fmt_t     dma_smp_fmt;     /**< PWM DMA sample format configuration.         */
    pwm_seq_mode_t        seq_mode;        /**< PWM sequence play mode configuration.        */
    pwm_trigger_src_t     triggered_src;   /**< PWM play trigger source configuration.       */
    pwm_seq_order_t       seq_order_1st;   /**< PWM sequence play order configuration.       */
    pwm_seq_order_t       seq_order_2nd;   /**< PWM sequence play order configuration.       */
    uint16_t              play_cnt;        /**< PWM play amount configuration.               */
    uint16_t              count_end_val;   /**< PWM counter end value configuration.         */
    pwm_seq_para_t        rseq;            /**< Handle of PWM sequence controller configurations for R-SEQ. */
    pwm_seq_para_t        tseq;            /**< Handle of PWM sequence controller configurations for T-SEQ. */
    uint32_t              frequency;
    uint32_t              pin_out;
    pwm_seq_order_t       seq_order;       /**< PWM sequence play order configuration. */
} pwm_config_t;



/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/**
 * \brief Function to initinal formation 1 function
 * \param[in] id
 * \param[in] freqency  pwm frequency
 * \retval STATUS_SUCCESS config pwm irq,clock,rdma registers is vaild
 * \retval STATUS_INVALID_PARAM config pwm irq,clock,rdma registers is invaild
 */
uint32_t pwm_init_fmt1_ex(uint32_t id, uint32_t freqency);
/**
 * \brief Function to initinal formation 0 function
 * \param[in] id
 * \param[in] freqency  pwm frequency
 * \param[in] count_end_value  pwm counter end value
 * \retval STATUS_SUCCESS config pwm irq,clock,rdma registers is vaild
 * \retval STATUS_INVALID_PARAM config pwm irq,clock,rdma registers is invaild
 */
uint32_t pwm_init_fmt0_ex(uint32_t id, uint32_t freqency, uint32_t count_end_value);
/**
 * \brief get pmw freqnecy
 * \param[in] id        pwm 
 * \param[in] freqency  pwm frequency
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_frequency_ex(uint32_t id, uint32_t *get_frequency);
/**
 * \brief set pmw freqnecy
 * \param[in] id        pwm 
 * \param[in] freqency  pwm frequency
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_frequency_ex(uint32_t id, uint32_t freqency);
/**
 * \brief set pmw clock divider
 * \param[in] id        pwm 
 * \param[in] pwm_clk_div  pwm clock dividere enum
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_clock_divider_ex(pwm_id_t id, pwm_clk_div_t pwm_clk_div);
/**
 * \brief get pmw phase
 * \param[in] id        pwm 
 * \param[in] get_phase  pwm phase
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_pahse(uint32_t id, uint32_t *get_phase);
/**
 * \brief set pmw phase
 * \param[in] id        pwm 
 * \param[in] pwm_phase_t  pwm phase
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_pahse_ex(uint32_t id, pwm_phase_t phase);
/**
 * \brief get pmw count
 * \param[in] id        pwm 
 * \param[in] full_count  pwm full count
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_count_ex(uint32_t id, uint32_t *full_count);
/**
 * \brief set pmw count
 * \param[in] id        pwm 
 * \param[in] counter_mode  pwm counter mode
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_counter_mode_ex(uint32_t id, pwm_counter_mode_t counter_mode);
/**
 * \brief set pmw count end value
 * \param[in] id        pwm 
 * \param[in] counter_end_value  pwm counter end value
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_counter_end_value_ex(uint32_t id, uint32_t counter_end_value);
/**
 * \brief set pmw dma format mode
 * \param[in] id        pwm 
 * \param[in] format  pwm dma format mode
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_dma_format_ex(uint32_t id, pwm_dma_smp_fmt_t format);
/**
 * \brief set pmw format 1 duty
 * \param[in] id        pwm 
 * \param[in] duty  pwm duty
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_fmt1_duty_ex(uint32_t id, uint8_t duty);
/**
 * \brief set pmw format 0 duty
 * \param[in] id        pwm 
 * \param[in] duty  pwm duty
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_fmt0_duty_ex(uint32_t id, uint8_t duty);
/**
 * \brief set pmw format 1 counter
 * \param[in] id        pwm 
 * \param[in] count  pwm count value
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_fmt1_count_ex(uint32_t id, uint32_t count);
/**
 * \brief set pmw format 0 counter
 * \param[in] id        pwm 
 * \param[in] count  pwm count value
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_fmt0_count_ex(uint32_t id, uint32_t count);
/**
 * \brief pmw multi element initinal counter
 * \param[in] pwm_cfg        pwm config paramater
 * \param[in] freqency  pwm freqency
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_multi_init_ex(pwm_config_t pwm_cfg, uint32_t freqency);
/**
 * \brief pmw multi element format 1 duty
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] element           pwm element
 * \param[in] duty              pwm duty value
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_multi_fmt1_duty_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint8_t duty);
/**
 * \brief pmw multi element format 0 duty
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] element           pwm element
 * \param[in] thd1_duty         pwm duty value
 * \param[in] thd2_duty         pwm duty value
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_multi_fmt0_duty_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint8_t thd1_duty, uint8_t thd2_duty);
/**
 * \brief pmw multi element format 1 count
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] element           pwm element
 * \param[in] count              pwm count
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_multi_fmt1_count_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint32_t count);
/**
 * \brief pmw multi element format 0 count
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] element           pwm element
 * \param[in] thd1_Count        pwm count
 * \param[in] thd2_count        pwm count
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_multi_fmt0_count_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t element, uint32_t thd1_Count, uint32_t thd2_count);
/**
 * \brief pmw set repeat number value
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] repeat_number     pwm repeat number
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_repeat_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t repeat_number);
/**
 * \brief pmw get repeat number value
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] repeat_number     pwm repeat number
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_repeat_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t *get_repeat_number);
/**
 * \brief pmw set delay number value
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] dly_number     pwm delay number
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_delay_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t dly_number);
/**
 * \brief pmw get delay number value
 * \param[in] id                pwm id
 * \param[in] seq_order         pwm sequence order
 * \param[in] get_delay_number     pwm delay number
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_delay_number_ex(uint32_t id, pwm_seq_order_t seq_order, uint32_t *get_delay_number);
/**
 * \brief pmw set dma element number value
 * \param[in] id                pwm id
 * \param[in] element           pwm element value 
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_set_dma_element_ex(uint32_t id, uint32_t element);
/**
 * \brief pmw get dma element number value
 * \param[in] id                pwm id
 * \param[in] get_element           pwm element value 
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_get_dma_element_ex(uint32_t id, uint32_t *get_element);
/**
 * \brief pmw start function value
 * \param[in] id                pwm id
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_start_ex(uint32_t id);
/**
 * \brief pmw stop function value
 * \param[in] id                pwm id
 * \retval STATUS_SUCCESS
 * \retval STATUS_INVALID_PARAM
 */
uint32_t pwm_stop_ex(uint32_t id);


#ifdef __cplusplus
}
#endif

#endif


