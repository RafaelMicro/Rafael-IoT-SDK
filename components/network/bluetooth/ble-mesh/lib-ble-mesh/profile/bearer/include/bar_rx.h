#ifndef __BAR_RX_H__
#define __BAR_RX_H__

#ifdef __cplusplus
extern "C" {
#endif

void bar_idc_handler(bar_indication_t *p_bar_idc, uint8_t from_gatt);

#endif /* __BAR_RX_H__ */
