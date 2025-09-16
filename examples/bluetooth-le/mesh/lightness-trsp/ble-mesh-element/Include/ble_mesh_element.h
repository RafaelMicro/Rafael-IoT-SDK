/**************************************************************************//**
 * @file  ble_profile.h
 * @brief Provide the declarations that for BLE Profile subsystem needed.
*****************************************************************************/

#ifndef _BLE_MESH_ELEMENT_H_
#define _BLE_MESH_ELEMENT_H_

#include "mmdl_common.h"
#include "hosal_pwm.h"

#define ELEMENT0_PWN_ID 0
#define ELEMENT1_PWN_ID 1

extern const uint8_t ble_mesh_element_count;

extern void element_info_init(void);
#endif //_BLE_MESH_ELEMENT_H_

