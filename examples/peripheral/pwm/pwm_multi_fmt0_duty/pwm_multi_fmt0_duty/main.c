#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_pwm.h"
#include "uart_stdio.h"


#define GPIO_20    20

static uint32_t tseq_element[10];
static uint32_t rseq_element[10];

int main(void) {

    hosal_pwm_dev_t pwm_dev;
    uint8_t duty = 0, i;

    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal pwm format 0 multi duty demo\r\n");
    printf("[PWM0 config]\r\n");
    printf(" seq_order_1st   :  HOSAL_PWM_SEQ_ORDER_T\r\n");
    printf(" seq_order_2nd   :  HOSAL_PWM_SEQ_ORDER_R\r\n");
    printf(" play_cnt        :  0\r\n");
    printf(" triggered_src   :  HOSAL_PWM_TRIGGER_SRC_SELF\r\n");
    printf(" seq_num         :  HOSAL_PWM_SEQ_NUM_2\r\n");
    printf(" dma_smp_fmt     :  HOSAL_PWM_DMA_SMP_FMT_1\r\n");
    printf(" seq_mode        :  HOSAL_PWM_SEQ_MODE_CONTINUOUS\r\n");
    printf(" clk_div         :  HOSAL_PWM_CLK_DIV_1\r\n");
    printf(" counter_mode    :  HOSAL_PWM_COUNTER_MODE_UP\r\n");
    printf(" frequency       : 16000 (16K)\r\n"); //using format 1 the count end value don't care
    printf(" count_end_val   :  3000\r\n"); //using format 1 the count end value don't care
    printf(" delay_num       :  0 \r\n");
    printf(" repeat_num      :  0 \r\n");
    printf(" element_num     :  10 \r\n");
    printf("[PWM0 Pin]       :  GPIO20 \r\n");

    pwm_dev.config.id = HOSAL_PWM_ID_0;
    pwm_dev.config.seq_order_1st = HOSAL_PWM_SEQ_ORDER_T;
    pwm_dev.config.seq_order_2nd = HOSAL_PWM_SEQ_ORDER_R;
    pwm_dev.config.play_cnt = 0;
    pwm_dev.config.triggered_src = HOSAL_PWM_TRIGGER_SRC_SELF;
    pwm_dev.config.seq_num = HOSAL_PWM_SEQ_NUM_2;
    pwm_dev.config.dma_smp_fmt = HOSAL_PWM_DMA_SMP_FMT_0;
    pwm_dev.config.seq_mode = HOSAL_PWM_SEQ_MODE_CONTINUOUS;
    pwm_dev.config.clk_div = HOSAL_PWM_CLK_DIV_1;
    pwm_dev.config.counter_mode = HOSAL_PWM_COUNTER_MODE_UP;
    pwm_dev.config.count_end_val = 3000; //format 0 vaild

    //Rseq
    pwm_dev.config.rseq.delay_num = 0;
    pwm_dev.config.rseq.repeat_num = 0;
    pwm_dev.config.rseq.element_num = 10;
    pwm_dev.config.rseq.rdma_addr = (uint32_t)rseq_element;

    //Tseq
    pwm_dev.config.tseq.delay_num = 0;
    pwm_dev.config.tseq.repeat_num = 0;
    pwm_dev.config.tseq.element_num = 10;
    pwm_dev.config.tseq.rdma_addr = (uint32_t)tseq_element;

    pwm_dev.config.frequency = 16000;   //16k
    pwm_dev.config.pin_out = GPIO_20;


    hosal_pwm_multi_init(&pwm_dev);

    duty = 10;
    for (i = 0; i < 10; i++) {
        pwm_dev.config.seq_order = HOSAL_PWM_SEQ_ORDER_T;
        hosal_pwm_multi_fmt0_duty(pwm_dev.config.id, &pwm_dev, i, duty, duty);
        duty += 5;
    }

    duty = 10;

    for (i = 0; i < 10; i++) {
        pwm_dev.config.seq_order = HOSAL_PWM_SEQ_ORDER_R;
        hosal_pwm_multi_fmt0_duty(pwm_dev.config.id, &pwm_dev, i, duty, duty);
        duty += 10;
    }

    hosal_pwm_start(pwm_dev.config.id);

    printf("\r\n\r\n");
    printf("hosal pwm format 0 multi duty finish\r\n");

    while (1) {;}
}

