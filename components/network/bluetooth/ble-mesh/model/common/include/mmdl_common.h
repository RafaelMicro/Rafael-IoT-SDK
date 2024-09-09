
#ifndef MMDL_COMMON_H
#define MMDL_COMMON_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "mesh_api.h"
#include "pib.h"
#include "mmdl_opcodes.h"
#include "mmdl_defs.h"
#include "mmdl_client.h"
#include "FreeRTOS.h"
#include "Queue.h"
#include "util.h"
//#include "mmdl_publish.h"


extern uint8_t                      tid;





void mmdl_evt_msg_cb(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc);
uint32_t search_model(ble_mesh_element_param_t *p_element, uint32_t model_id, ble_mesh_model_param_t **p_model);


uint32_t mmdl_model_binding_key_validate(uint16_t appkey_index, ble_mesh_model_param_t *p_model);
uint32_t mmdl_model_subscribe_address_validate(uint16_t subscribe_address, ble_mesh_model_param_t *p_model);


#ifdef __cplusplus
}
#endif

#endif /* MMDL_COMMON_H */
