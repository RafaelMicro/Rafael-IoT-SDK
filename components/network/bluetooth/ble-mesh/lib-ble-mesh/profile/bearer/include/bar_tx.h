#ifndef __BAR_TX_H__
#define __BAR_TX_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/

void bearer_setBLE_AdvParam(ble_adv_param_t *p_parm);

int bearer_setBLE_AdvData(uint8_t *p_buf, uint8_t buf_len);

int bearer_setBLE_AdvEnable(uint8_t host_id);

void bearer_setBLE_AdvDisable(void);

int bearer_setBLE_ScanParam(ble_scan_param_t *p_parm);

int bearer_setBLE_ScanRspData(uint8_t *p_buf, uint8_t buf_len);

void bearer_setBLE_ScanEnable(void);

void bearer_setBLE_ScanDisable(void);

int bearer_setBLE_Address(ble_gap_addr_t *p_addr);

void bearer_setBLE_Disconnect(uint8_t host_id);

bool bearer_gatt_data_req(uint8_t host_id, uint8_t pdu_type, uint8_t *p_br_pdu, uint16_t pdu_len);

#endif /* __BAR_TX_H__ */
