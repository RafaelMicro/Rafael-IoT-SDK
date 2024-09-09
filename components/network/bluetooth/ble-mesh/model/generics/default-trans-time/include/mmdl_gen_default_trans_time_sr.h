#ifndef __MMDL_GEN_DEFAULT_TRANS_TIME_SRV_H__
#define __MMDL_GEN_DEFAULT_TRANS_TIME_SRV_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void mmdl_generic_default_trans_time_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern void mmdl_generic_default_trans_time_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t transition_time);
extern void mmdl_generic_default_trans_time_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_DEFAULT_TRANS_TIME_SRV_H__*/
