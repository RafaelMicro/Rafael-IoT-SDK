#ifndef __BAR_PB_ADV_H__
#define __BAR_PB_ADV_H__

#ifdef __cplusplus
extern "C" {
#endif

void bar_pb_adv_init(void);
void bar_pb_adv_tx_unprv_beacon(void);
void bar_pb_adv_tx_secure_beacon(void);
void bar_pb_adv_start_scan(void);
void bar_pb_adv_stop_scan(void);
void bar_pb_adv_tx_data(uint8_t *p_buf, uint16_t buf_len, uint16_t interval, uint8_t host_id);

#ifdef __cplusplus
};
#endif
#endif /* __BAR_PB_ADV_H__ */
