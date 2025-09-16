/**
 * @file device_api.h
 * @brief
 *  
 * @version 0.1
 * 
 * @date 
 * 
 */

#ifndef __DEVICE_API_H
#define __DEVICE_API_H

#include "EnhancedFlashDataset.h"
#include "zigbee_api.h"

#define LED_BLUE 20

void pwm_ctl_init(void);
void pwm_ctl_set_level(uint8_t duty);

#endif // __DEVICE_API_H
