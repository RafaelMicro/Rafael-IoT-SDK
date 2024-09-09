// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_DATA_H__
#define __PRV_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "timers.h"

extern uint32_t u32_prov_state;

extern prov_session_info provision_info;
extern ProvSessionData_t prov_session_data;

extern TimerHandle_t tmr_prov_tx;
extern TimerHandle_t tmr_prov_ack;
extern TimerHandle_t tmr_prov;
extern uint8_t pu8_peerPubKey[MESH_PRV_PDU_PUB_KEY_PDU_SIZE];
extern uint8_t pu8_peerConfirmation[MESH_PRV_PDU_CONFIRM_PDU_SIZE];

extern SemaphoreHandle_t gt_security_sem;
extern bool gb_authenticated;


#ifdef __cplusplus
};
#endif
#endif /* __PRV_DATA_H__ */
