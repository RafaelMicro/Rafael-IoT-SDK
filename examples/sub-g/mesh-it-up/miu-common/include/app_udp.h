#ifndef __APP_UDP_H
#define __APP_UDP_H

#include <openthread/ip6.h>

uint8_t app_sockInit(otInstance* instance, uint16_t udp_port);
int app_udpSend(otIp6Address dstaddr, uint8_t* p, uint16_t len, bool fromISR);

#endif /* __APP_UDP_H */
