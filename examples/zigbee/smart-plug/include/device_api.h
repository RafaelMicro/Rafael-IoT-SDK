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
#define SMART_PLUG_TRIGGER_PIN 21

void set_led_onoff(uint8_t led, uint8_t on);
void plug_init(void);
void set_plug_status(uint8_t on);
void button_init(void);
#endif // __DEVICE_API_H
