#ifndef __APP_MAC_RAW_H
#define __APP_MAC_RAW_H

#include <miu_port.h>

uint8_t app_macRawInit(otInstance* instance);
int app_macRawSend(uint8_t* dstaddr, uint8_t* data, uint16_t len,
                   uint16_t panid, uint8_t channel, bool fromISR);

#endif /* __APP_MAC_RAW_H */
