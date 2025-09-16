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
#define LED_RED 21

void set_led_onoff(uint8_t led, uint8_t on);
void button_init(void);
#endif // __DEVICE_API_H
