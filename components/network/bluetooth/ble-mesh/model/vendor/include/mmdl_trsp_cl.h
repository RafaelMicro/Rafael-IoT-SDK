#ifndef __MMDL_TRSP_CL_H__
#define __MMDL_TRSP_CL_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "mmdl_client.h"
#include "mmdl_trsp_common.h"

extern void mmdl_rafael_trsp_cl_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern int mmdl_rafael_trsp_send_set(mmdl_transmit_info_t tx_info, raf_trsp_set_msg_t *p_raf_trsp_set_msg);
extern int mmdl_rafael_trsp_send_unack_set(mmdl_transmit_info_t tx_info, raf_trsp_set_msg_t *p_raf_trsp_set_msg);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_TRSP_CL_H__*/
