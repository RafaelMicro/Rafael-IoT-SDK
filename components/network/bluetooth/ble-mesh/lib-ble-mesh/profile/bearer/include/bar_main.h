#ifndef __BAR_MAIN_H__
#define __BAR_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_api.h"

typedef enum
{
    BAR_ADV_TX_STATE_IDLE    = 0,
    BAR_ADV_TX_STATE_BUSY,
    BAR_ADV_TX_STATE_COMPLETE,
} e_bar_adv_tx_state;

extern uint8_t g_mtu_size[2];
extern SemaphoreHandle_t g_adv_disabled_sem;

int bar_init(void);
uint8_t bearer_dispatch_handle(mesh_tlv_t *pt_tlv);
void bar_set_device_address(void);
bool bar_is_adv_disabled(void);


void bcn_sbn_process(uint8_t *pBuf, uint16_t Buflen);


#ifdef __cplusplus
};
#endif
#endif /* __BAR_MAIN_H__ */
