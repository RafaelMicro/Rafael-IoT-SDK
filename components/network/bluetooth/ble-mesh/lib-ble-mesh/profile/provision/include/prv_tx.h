// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_TX_H__
#define __PRV_TX_H__

#ifdef __cplusplus
extern "C" {
#endif


void prov_api_send_linkAck(uint32_t linkid);
void prov_api_send_invite(ProvSessionData_t *pSessionData);
void prov_api_send_capabilites(ProvSessionData_t *pSessionData);
void prov_api_send_start(ProvSessionData_t *pSessionData);
void prov_api_send_pub_key(ProvSessionData_t *pSessionData, uint8_t peerType);
void prov_api_send_confirmation(ProvSessionData_t *pSessionData, uint8_t peerType);
void prov_api_send_random(ProvSessionData_t *pSessionData, uint8_t *pRecvConfimration, uint8_t peerType);
bool SendProvisioningPdu(uint8_t *p_prv_pdu, uint8_t pdu_len, uint16_t timeout);
uint8_t prov_start_tx(uint8_t data_len, mesh_tlv_t *p_data, uint8_t retry_cnt);
void prov_triger_tx(void);
void send_link_open(ble_mesh_prov_evt_t *pt_prov_evt);
void send_link_close(uint8_t CloseReson);

#ifdef __cplusplus
};
#endif
#endif /* __PRV_TX_H__ */
