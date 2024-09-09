#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_pwm.h"


#define GPIO_20    20

int main(void) {

    hosal_pwm_dev_t pwm_dev;
    uint8_t duty = 0;

    printf("hosal pwm format 0 single duty examples %s %s %c%c",__DATE__,__TIME__,'\r','\n');

  
    pwm_dev.config.id = 0;
    pwm_dev.config.frequency = 16000;//16K
    pwm_dev.config.pin_out = 20;	
    pwm_dev.config.count_end_val = 3000;
	
    hosal_pwm_init_fmt0_ex(&pwm_dev);
	
    hosal_pwm_fmt0_duty_ex(0,50);

    while(1){;}
}

