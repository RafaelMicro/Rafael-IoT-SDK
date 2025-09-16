#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_pwm.h"
#include "hosal_dma.h"
#include "gpio.h"
#include "uart_stdio.h"

void pwm_call_back_fun(uint32_t id, uint32_t status) {

    printf("cb id=%d,status=%x\r\n",id ,status);
}

int main(void) {


    hosal_pwm_dev_t pwm_dev;

    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal pwm format 1 single duty demo\r\n");
    printf("[PWM0 config]\r\n");
    printf(" frequency       :  16000 (16K)\r\n"); //using format 1 the count end value don't care
    printf("[PWM0 Pin]       :  GPIO20 \r\n");

    pwm_dev.config.id = HOSAL_PWM_ID_0;
    pwm_dev.config.frequency = 16000;
    pwm_dev.config.pin_out = 20;
    
    hosal_pwm_init_fmt1(&pwm_dev);
    hosal_pwm_ioctl(&pwm_dev,HOSAL_PWM_REGISTER_CALLBACK,pwm_call_back_fun);
    hosal_pwm_fmt1_duty(pwm_dev.config.id,50); /*pwm duty 50%*/
    hosal_pwm_start(pwm_dev.config.id);

    printf("\r\n\r\n");
    printf("hosal pwm format 1 single duty finish\r\n");

    while(1){;}
}

