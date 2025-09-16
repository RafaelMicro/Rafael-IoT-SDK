#ifndef __MESH_MDL_HANDLER_H__
#define __MESH_MDL_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mmdl_gen_onoff_common.h"
#include "mmdl_gen_onoff_cl.h"
#include "mmdl_gen_onoff_sr.h"

#include "mmdl_gen_level_common.h"
#include "mmdl_gen_level_sr.h"



/* include the lighting related defitions */
#include "mmdl_light_lightness_common.h"
#include "mmdl_light_lightness_sr.h"

#include "mmdl_scene_common.h"
#include "mmdl_scene_sr.h"
#include "mmdl_trsp_common.h"
#include "mmdl_trsp_sr.h"
#include "mmdl_trsp_cl.h"

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** @brief BLE initialization.
 * @details Initial BLE stack and application task.
 *
 * @return none.
 */
void mmdl_init(void);
void app_process_model_msg(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc, ble_mesh_element_param_t *p_element, uint8_t is_broadcast);

void app_process_element_lightness_model_state(uint16_t element_address, uint16_t state);
void app_process_element_raf_trsp_sr_model_state(raf_trsp_cb_params_t *p_raf_trsp_cb_params);
void app_process_element_scene_model_state(uint16_t element_address, uint8_t action, uint32_t *p_scene_state, void **p_extend_model_state_set);
void app_process_element_onoff_model_state(uint16_t element_address, uint16_t state);

extern xQueueHandle app_msg_q;

#ifdef __cplusplus
};
#endif

#endif /* __MESH_MDL_HANDLER_H__*/
