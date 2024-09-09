/**
 * \file            hosal_pwm.c
 * \brief           Hosal PWM driver file
 *          
 */
/*
 * Copyright (c) year FirstName LASTNAME
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
#include "stdio.h"
#include <stdint.h>
#include "mcu.h"
#include "hosal_pwm.h"



int hosal_pwm_pin_conifg(uint32_t id, uint32_t pin_number)
{
    int status = STATUS_SUCCESS;

    if((pin_number<8) || (pin_number>23) || (pin_number==16) || (pin_number==17))
    {
        return STATUS_INVALID_PARAM;
    }

    if(id==0)
    {     
            if (pin_number == 8)
            {
                pin_set_mode(pin_number, MODE_PWM);
            }
            else
            {
                pin_set_mode(pin_number, MODE_PWM0);
            }
    }
    else if(id==1)
    {        
        if (pin_number == 9)
        {
           pin_set_mode(pin_number, MODE_PWM);
        }
        else
        {
           pin_set_mode(pin_number, MODE_PWM1);
        }

    }
     else if(id==2)
    {
        if (pin_number == 14)
        {
           pin_set_mode(pin_number, MODE_PWM);
        }
        else
        {
           pin_set_mode(pin_number, MODE_PWM2);
        }


    }   
    else if(id==3)
    {         
        if (pin_number == 15)
        {
           pin_set_mode(pin_number, MODE_PWM);
        }
        else
        {
           pin_set_mode(pin_number, MODE_PWM3);
        }

    }
    else if(id==4)
    {
       pin_set_mode(pin_number, MODE_PWM4);
    }       
    
	    return status;
}


int hosal_pwm_init_fmt1_ex(hosal_pwm_dev_t* dev)
{
    int status = STATUS_SUCCESS;

    hosal_pwm_pin_conifg(dev->config.id,dev->config.pin_out);

	status = pwm_init_fmt1_ex(dev->config.id,dev->config.frequency);

	return status;
}

int hosal_pwm_init_fmt0_ex(hosal_pwm_dev_t* dev)
{
    int status = STATUS_SUCCESS;

    hosal_pwm_pin_conifg(dev->config.id,dev->config.pin_out);

	status = pwm_init_fmt0_ex(dev->config.id,dev->config.frequency,dev->config.count_end_val);

	return status;
}

int hosal_pwm_fmt1_duty_ex(uint32_t id, uint8_t duty)
{
    int status = STATUS_SUCCESS;

    pwm_fmt1_duty_ex(id,duty);

    return status;
}

int hosal_pwm_fmt0_duty_ex(uint32_t id, uint8_t duty)
{
    int status = STATUS_SUCCESS;

    pwm_fmt0_duty_ex(id, duty);

    return status;
	
}

int hosal_pwm_fmt1_count_ex(uint32_t id, uint32_t count)
{
    int status = STATUS_SUCCESS;

    hosal_pwm_fmt1_count_ex(id, count);

    return status;
    
}


int hosal_pwm_fmt0_count_ex(uint32_t id, uint32_t count)
{
    int status = STATUS_SUCCESS;

    pwm_fmt0_count_ex(id, count);

    return status;
    
}


int hosal_pwm_multi_init_ex(hosal_pwm_dev_t *pwm_dev)
{
    int status = STATUS_SUCCESS;

	hosal_pwm_pin_conifg(pwm_dev->config.id,pwm_dev->config.pin_out);
	
    pwm_multi_init_ex(pwm_dev->config, pwm_dev->config.frequency);

    return status;

}


int hosal_pwm_multi_fmt1_duty_ex(uint32_t id, hosal_pwm_dev_t  *pwm_dev,uint32_t element,uint8_t duty)
{
    int status = STATUS_SUCCESS;

	if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_T )
	{
	   pwm_multi_fmt1_duty_ex(id, pwm_dev->config.seq_order, element,duty);
	}
	else if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_R)
	{
	   pwm_multi_fmt1_duty_ex(id, pwm_dev->config.seq_order, element,duty);
	}
    else
	{
		status = STATUS_INVALID_PARAM;
	}		
   
    return status;
}


int hosal_pwm_multi_fmt0_duty_ex(uint32_t id, hosal_pwm_dev_t  *pwm_dev,uint32_t element,uint8_t thd1_duty,uint8_t thd2_duty)
{
    int status = STATUS_SUCCESS;

	if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_T )
	{
	  pwm_multi_fmt0_duty_ex(id, pwm_dev->config.seq_order, element,thd1_duty,thd2_duty);
	}
	else if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_R)
	{
	  pwm_multi_fmt0_duty_ex(id, pwm_dev->config.seq_order, element,thd1_duty,thd2_duty);
	}
    else
	{
		status = STATUS_INVALID_PARAM;
	}		
       

    return status;
}


int hosal_pwm_multi_fmt1_count_ex(uint32_t id, hosal_pwm_dev_t  *pwm_dev, uint32_t element,uint32_t count)
{
    int status = STATUS_SUCCESS;

	if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_T )
	{
	  pwm_multi_fmt1_count_ex( id, pwm_dev->config.seq_order, element, count);
	}
	else if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_R)
	{
	  pwm_multi_fmt1_count_ex( id, pwm_dev->config.seq_order, element, count);
	}
    else
	{
	   status = STATUS_INVALID_PARAM;
	}
	
    return status;
}


int hosal_pwm_multi_fmt0_count_ex(uint32_t id, hosal_pwm_dev_t  *pwm_dev, uint32_t element,uint32_t thd1_Count,uint32_t thd2_count)
{

    int status = STATUS_SUCCESS;

	if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_T )
	{
	  pwm_multi_fmt0_count_ex( id, pwm_dev->config.seq_order, element,thd1_Count,thd2_count);

	}
	else if(pwm_dev->config.seq_order==PWM_SEQ_ORDER_R)
	{
	  pwm_multi_fmt0_count_ex( id, pwm_dev->config.seq_order, element,thd1_Count,thd2_count);

	}
    else
	{
	   status = STATUS_INVALID_PARAM;
	}
	
    return status;
}


int hosal_pwm_start_ex(uint32_t id)
{
    int status = STATUS_SUCCESS;
	
	status = pwm_start_ex(id);

	return (int)status;
}

int hosal_pwm_sotp_ex(uint32_t id)
{
    int status = STATUS_SUCCESS;
	
	status = pwm_stop_ex(id);

	return (int)status;
}


int hosal_pwm_ioctl(hosal_pwm_dev_t* dev, int ctl, void* p_arg)
{
    int status = STATUS_SUCCESS;

    switch (ctl) {

                    case HOSAL_PWM_GET_FRQUENCY: 

                    status = pwm_get_frequency_ex(dev->config.id, p_arg);

                    break;

                    case HOSAL_PWM_SET_FRQUENCY: 


                    status =  pwm_set_frequency_ex(dev->config.id, dev->config.frequency); 

                    break;

                    case HOSAL_PWM_SET_CLOCK_DIVIDER: 

                        status =  pwm_clock_divider_ex(dev->config.id, dev->config.clk_div);
                        
                    break;

                    case HOSAL_PWM_GET_PHASE: 

                        status =  pwm_get_pahse(dev->config.id,p_arg);

                    break;

                    case HOSAL_PWM_SET_PHASE: 

                    status =  pwm_set_pahse_ex(dev->config.id, dev->config.phase);
                    
                    break;

                    case HOSAL_PWM_GET_COUNT: 

                    status =  pwm_get_count_ex(dev->config.id,p_arg);

                    break;
                    case HOSAL_PWM_SET_COUNT_MODE: 

                    status =  pwm_set_counter_mode_ex(dev->config.id,dev->config.counter_mode);

                    break;

                    case HOSAL_PWM_SET_COUNT_END_VALUE: 

                    status =  pwm_set_counter_end_value_ex(dev->config.id, dev->config.count_end_val);
                    
                    break;

                    case HOSAL_PWM_SET_DMA_FORMAT: 

                    status =  pwm_set_dma_format_ex(dev->config.id, dev->config.dma_smp_fmt);

                    break;
	
                    case HOSAL_PWM_SET_REPEAT_NUMBER:

                    status = pwm_set_repeat_number_ex(dev->config.id, dev->config.seq_order , *(uint32_t*)p_arg);

                    break;

                    case HOSAL_PWM_GET_REPEAT_NUMBER: 

                    status =  pwm_get_repeat_number_ex(dev->config.id,dev->config.seq_order,p_arg);

                    break;
                    
                    case HOSAL_PWM_SET_DELAY_NUMBER: 

                    status =  pwm_set_delay_number_ex(dev->config.id, dev->config.seq_order, *(uint32_t*)p_arg);

                    break;

                    case HOSAL_PWM_GET_DELAY_NUMBER: 

                    status =  pwm_get_delay_number_ex(dev->config.id,dev->config.seq_order ,p_arg);

                    break;


             default: return -1;
    }

	return (int)status;
}








