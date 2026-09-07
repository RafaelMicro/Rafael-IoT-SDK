/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

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

#ifdef __cplusplus
extern "C" {
#endif

#include "EnhancedFlashDataset.h"
#include "zigbee_api.h"

#define LED_BLUE 20

void pwm_ctl_init(void);
void pwm_ctl_set_level(uint8_t duty);

#ifdef __cplusplus
}
#endif
#endif // __DEVICE_API_H
