// ---------------------------------------------------------------------------
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __BAR_TX_FSM_H__
#define __BAR_TX_FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
void bar_tx_queue_init(void);

uint8_t bar_tx_queue_remain_size_get(void);
void bar_adv_interval_change(uint16_t interval);

void bar_adv_tx_fsm_init(void);

void bar_adv_tx_start(mesh_tlv_t *pt_tlv);
void bar_adv_enabled(void);
void bar_adv_disabled(void);
void bar_gatt_adv_enable(void);
void bar_gatt_adv_disable(void);
void bar_gatt_connected(void);
void bar_gatt_disconnected(void);
void bar_adv_tx_continue(void);


uint8_t bar_tx_queue_empty(void);
void bar_adv_tx_continue(void);


#ifdef __cplusplus
};
#endif
#endif /* __BAR_TX_FSM_H__ */
