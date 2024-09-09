#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_pwm.h"


#define GPIO_20    20

static uint32_t tseq_element[10];
static uint32_t rseq_element[10];

int main(void) {

    hosal_pwm_dev_t pwm_dev;
    uint8_t duty = 0, i;

    printf("hosal pwm format 0 multi duty examples %s %s %c%c", __DATE__, __TIME__, '\r', '\n');

    pwm_dev.config.id = PWM_ID_0;
    pwm_dev.config.seq_order_1st = PWM_SEQ_ORDER_T;
    pwm_dev.config.seq_order_2nd = PWM_SEQ_ORDER_R;
    pwm_dev.config.play_cnt = 0;
    pwm_dev.config.triggered_src = PWM_TRIGGER_SRC_SELF;
    pwm_dev.config.seq_num = PWM_SEQ_NUM_2;
    pwm_dev.config.dma_smp_fmt = PWM_DMA_SMP_FMT_0;
    pwm_dev.config.seq_mode = PWM_SEQ_MODE_CONTINUOUS;
    pwm_dev.config.clk_div = PWM_CLK_DIV_1;
    pwm_dev.config.counter_mode = PWM_COUNTER_MODE_UP;
    pwm_dev.config.count_end_val = 3000; //format 0

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

    pwm_dev.config.frequency = 16000;
    pwm_dev.config.pin_out = 20;


    hosal_pwm_multi_init_ex(&pwm_dev);

    duty = 10;
    for (i = 0; i < 10; i++) {
        pwm_dev.config.seq_order = PWM_SEQ_ORDER_T;
        hosal_pwm_multi_fmt0_duty_ex(pwm_dev.config.id, &pwm_dev, i, duty, duty);
        duty += 5;
    }

    duty = 10;

    for (i = 0; i < 10; i++) {
        pwm_dev.config.seq_order = PWM_SEQ_ORDER_R;
        hosal_pwm_multi_fmt0_duty_ex(pwm_dev.config.id, &pwm_dev, i, duty, duty);
        duty += 10;
    }

    hosal_pwm_start_ex(0);

    while (1) {;}
}

