// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_MAIN_H__
#define __PRV_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif




int mesh_provision_init(void);
void mesh_provision_state_init(void);
uint8_t prov_pkt_proc(uint8_t *p_pb_pdu, uint8_t pd_len);
uint8_t ctrl_pdu_proc(uint32_t linkId, uint8_t *p_ctl_pdu, uint8_t pd_len);
void prov_pdu_proc(uint8_t *p_prv_pdu, uint8_t pdu_Len);
uint8_t prov_end_transaction(void);
void tmr_prov_cb(TimerHandle_t cb_param);
void tmr_prov_tx_cb(TimerHandle_t cb_param);
void tmr_prov_ack_cb(TimerHandle_t cb_param);
void prov_prepare_ack(void);
bool prov_check_newpdu(const uint8_t *p_gen_pdu, uint8_t pdu_Len);
void prov_check_ack(uint8_t opcode);
void prov_clean(void);
#ifdef __cplusplus
};
#endif
#endif /* __PRV_MAIN_H__ */
