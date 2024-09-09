#ifndef __MMDL_GEN_ONOFF_CL_H__
#define __MMDL_GEN_ONOFF_CL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mmdl_client.h"
#include "mmdl_gen_onoff_common.h"

extern void mmdl_generic_onoff_cl_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
extern void mmdl_generic_onoff_cl_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern void mmdl_generic_onoff_send_set(mmdl_transmit_info_t tx_info, gen_on_off_set_msg_t *p_onoff_set);
extern void mmdl_generic_onoff_send_unack_set(mmdl_transmit_info_t tx_info, gen_on_off_set_msg_t *p_onoff_set);


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_ONOFF_CL_H__*/
