#ifndef CPC_INTERNEL_H
#define CPC_INTERNEL_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void cpc_drv_trnsmit_complete(void);

uint32_t cpc_drv_uart_push_header(uint8_t* pHeader);

void cpc_drv_uart_push_data(uint8_t* p_data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // CPC_INTERNEL_H
