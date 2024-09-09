/**
 * @file mmdl_scene_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-27
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __MMDL_SCENE_SR_H__
#define __MMDL_SCENE_SR_H__

#ifdef __cplusplus
extern "C"
{
#endif

extern void scene_info_reset(void);
extern void mmdl_scene_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
extern void mmdl_scene_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_SCENE_SR_H__*/

