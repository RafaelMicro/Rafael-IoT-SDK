/**
 * @file hci_bridge.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _HCI_BRIDGE_H_
#define _HCI_BRIDGE_H_

#include "ble_hci.h"

typedef enum
{
    HIC_INTERFACE_CALLBACK_TYPE_EVENT,
    HIC_INTERFACE_CALLBACK_TYPE_DATA,
} hci_bridge_callback_type_t;


typedef int (*hci_bridge_callback_t)(uint8_t *p_data, uint16_t data_len);

void hci_bridge_init(void);
void hci_bridge_callback_set(hci_bridge_callback_type_t type, hci_bridge_callback_t pfn_callback);
int hci_bridge_message_write(ble_hci_message_t *pmesg);

#endif // 1_HCI_BRIDGE_H_

