/**
 * @file mmdl_publish.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MMDL_PUBLISH_H__
#define __MMDL_PUBLISH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define MMLDL_PUBLISH_UINT  50     //define the resolution of the publish timer

typedef void (*publish_state_fp)(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

extern xQueueHandle mmdl_publish_msg_q;
extern void mmdl_publish_init(void);
extern void mmdl_publish_state_by_msg(uint32_t value);


#ifdef __cplusplus
};
#endif

#endif /* __MMDL_PUBLISH_H__*/
