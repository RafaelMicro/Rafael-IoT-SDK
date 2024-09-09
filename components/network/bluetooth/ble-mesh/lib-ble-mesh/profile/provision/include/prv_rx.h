// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __PRV_RX_H__
#define __PRV_RX_H__

#ifdef __cplusplus
extern "C" {
#endif


bool prov_chk_rx_mask(uint8_t segN);
void prov_rx_transcation(uint8_t *pGenPdu, uint8_t pduLen);
void prov_rx_continue(uint8_t *pu8_Pdu, uint8_t u8_pduLen);

#ifdef __cplusplus
};
#endif
#endif /* __PRV_RX_H__ */
