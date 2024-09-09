#ifndef __MMDL_SCHEDULER_SRV_H__
#define __MMDL_SCHEDULER_SRV_H__

#ifdef __cplusplus
extern "C" {
#endif


extern void mmdl_scheduler_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t in_is_broadcast);
extern void mmdl_scheduler_setup_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern void mmdl_generic_power_onoff_ex_cb(ble_mesh_element_param_t *current_p_element, ble_mesh_model_param_t *child_p_model, general_parameter_t p);
extern void mmdl_generic_power_onoff_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t onopowerup_state);
extern void mmdl_generic_power_onoff_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
extern void mmdl_scheduler_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_SCHEDULER_SRV_H__*/
