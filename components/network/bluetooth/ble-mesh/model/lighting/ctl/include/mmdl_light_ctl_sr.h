/**
 * @file mmdl_light_ctl_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __MMDL_LIGHT_CTL_SR_H__
#define __MMDL_LIGHT_CTL_SR_H__

#ifdef __cplusplus
extern "C"
{
#endif


extern void mmdl_light_ctl_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
extern void mmdl_light_ctl_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);
extern void mmdl_light_ctl_ex_cb(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_child_model, general_parameter_t p);
extern void mmdl_light_ctl_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint16_t lightness_level_state);
extern void mmdl_light_ctl_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
extern void mmdl_light_ctl_scene_set(ble_mesh_element_param_t *p_current_element);


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_LIGHT_CTL_SR_H__*/
