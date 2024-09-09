#ifndef __MMDL_GEN_POWERUP_LEVEL_SRV_H__
#define __MMDL_GEN_POWERUP_LEVEL_SRV_H__

#ifdef __cplusplus
extern "C" {
#endif


extern void mmdl_generic_power_level_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern void mmdl_generic_power_level_scene_set(ble_mesh_element_param_t *p_current_element);

extern void mmdl_generic_power_level_ex_cb(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_child_model, general_parameter_t p);
extern void mmdl_generic_power_level_update_extend(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_parent_model, ble_mesh_model_param_t *p_child_model);
extern void mmdl_generic_power_level_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t onopowerup_state);
extern void mmdl_generic_power_level_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_GEN_POWERUP_ONOFF_SRV_H__*/
